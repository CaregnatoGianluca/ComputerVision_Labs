#include "../include/stitcher.h"
#include "../include/panoramic_utils.h" // For cylindricalProj, ensure this path is correct


#include <opencv2/imgproc.hpp>    // For warpPerspective, cvtColor, threshold
#include <opencv2/calib3d.hpp>    // For findHomography, perspectiveTransform
#include <opencv2/features2d.hpp> // For ORB, BFMatcher, KeyPoint, DMatch etc.
#include <opencv2/highgui.hpp>    // For imshow, waitKey, destroyAllWindows

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept> // For std::runtime_error
#include <limits>    // For std::numeric_limits
#include <iomanip>   // <<--- ADDED for std::setprecision

// Constructor - ORB ONLY
Stitcher::Stitcher(double min_distance_ratio, double ransac_reproj_thresh)
    : min_distance_ratio_(min_distance_ratio), ransac_reproj_thresh_(ransac_reproj_thresh) {
    feature_detector_ = cv::ORB::create();
    if (!feature_detector_) {
        throw std::runtime_error("Failed to create ORB feature detector.");
    }
    matcher_ = cv::BFMatcher::create(cv::NORM_HAMMING, false);
    if (!matcher_) {
         throw std::runtime_error("Failed to create BFMatcher.");
    }
    std::cout << "Using ORB detector and BFMatcher (NORM_HAMMING)." << std::endl;
}

// Stitch method - WITH DEBUG OUTPUT
cv::Mat Stitcher::stitch(const std::vector<cv::Mat>& base_images, const std::string& dataset_identifier) {
    // --- Initial checks and FoV setup (same as before) ---
    if (base_images.empty()) return cv::Mat();
    double fov_angle_deg = (dataset_identifier == "dolomites") ? 27.0 : 33.0;
    std::cout << "Stitching " << base_images.size() << " images for " << dataset_identifier << " (half FoV: " << fov_angle_deg << ")" << std::endl;
    if (base_images.size() == 1) return cylindricalProj(base_images[0], fov_angle_deg);

    std::vector<ImageFeatures> all_image_features(base_images.size());

    // 1. Project images and extract features (same as before)
    std::cout << "Step 1: Projecting images and extracting ORB features..." << std::endl;
    for (size_t i = 0; i < base_images.size(); ++i) {
         all_image_features[i].image_projected = cylindricalProj(base_images[i], fov_angle_deg);
         if (all_image_features[i].image_projected.empty()) return cv::Mat(); // Error check
         feature_detector_->detectAndCompute(all_image_features[i].image_projected, cv::noArray(),
                                           all_image_features[i].keypoints, all_image_features[i].descriptors);
         std::cout << "  Image " << i + 1 << ": " << all_image_features[i].keypoints.size() << " keypoints." << std::endl;
         // Add error checks if needed
    }

    // 2. Match features and estimate relative homographies
    std::cout << "Step 2: Matching features and estimating relative translations..." << std::endl;
    std::vector<cv::Mat> relative_homographies;

    for (size_t i = 0; i < all_image_features.size() - 1; ++i) {
        const auto& features1 = all_image_features[i];
        const auto& features2 = all_image_features[i + 1];

        // Skip if no features/descriptors
        if (features1.keypoints.empty() || features2.keypoints.empty() ||
            features1.descriptors.empty() || features2.descriptors.empty()) {
            std::cerr << "  Skipping pair " << i+1 << "-" << i+2 << " due to missing features/descriptors." << std::endl;
            relative_homographies.push_back(cv::Mat::eye(3, 3, CV_64F)); // Use identity
            continue;
        }

        // Match and filter (same as before)
        std::vector<cv::DMatch> all_raw_matches, good_matches;
        matcher_->match(features1.descriptors, features2.descriptors, all_raw_matches);
        if(all_raw_matches.empty()) {
             std::cerr << "  No raw matches for pair " << i+1 << "-" << i+2 << std::endl;
             relative_homographies.push_back(cv::Mat::eye(3, 3, CV_64F)); // Use identity
             continue;
        }
        double min_dist = std::numeric_limits<double>::max();
        for(const auto& match : all_raw_matches) min_dist = std::min(min_dist, (double)match.distance);
        double distance_threshold = min_distance_ratio_ * min_dist;
        for(const auto& match : all_raw_matches) {
            if (match.distance < distance_threshold) good_matches.push_back(match);
        }
        std::cout << "  Pair " << i + 1 << "-" << i + 2 << ": Raw=" << all_raw_matches.size()
                  << ", MinDist=" << min_dist << ", Thresh=" << distance_threshold
                  << ", Good=" << good_matches.size() << std::endl;

        // --- Visualization (same as before) ---
        if (!good_matches.empty()) {
            cv::Mat img_matches;
            cv::drawMatches(features1.image_projected, features1.keypoints,
                            features2.image_projected, features2.keypoints,
                            good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
                            std::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
            std::string window_title = "Matches " + std::to_string(i + 1) + "-" + std::to_string(i + 2);
            cv::imshow(window_title, img_matches);
            std::cout << "  Showing matches for pair " << i + 1 << "-" << i + 2 << ". Press any key..." << std::endl;
            cv::waitKey(0);
            cv::destroyWindow(window_title);
        }

        // Estimate Homography
        cv::Mat H; // H_i_to_i+1
        if (good_matches.size() >= 4) {
            std::vector<cv::Point2f> points1, points2;
            for (const auto& match : good_matches) {
                points1.push_back(features1.keypoints[match.queryIdx].pt);
                points2.push_back(features2.keypoints[match.trainIdx].pt);
            }
             // *** Add mask output from findHomography ***
            std::vector<unsigned char> ransac_mask;
            H = cv::findHomography(points1, points2, cv::RANSAC, ransac_reproj_thresh_, ransac_mask);

            // *** DEBUG: Count RANSAC inliers ***
            int inlier_count = 0;
            for(unsigned char mask_val : ransac_mask) {
                if(mask_val) inlier_count++;
            }
            std::cout << "  findHomography RANSAC inliers: " << inlier_count << "/" << good_matches.size() << std::endl;

            if (H.empty() || inlier_count < 4) { // Check if H is valid and enough inliers support it
                 std::cerr << "  findHomography failed or insufficient inliers for pair " << i + 1 << "-" << i + 2 << ". Using identity." << std::endl;
                 H = cv::Mat::eye(3, 3, CV_64F);
            }
        } else {
            std::cerr << "  Not enough good matches (" << good_matches.size() << ") for pair " << i + 1 << "-" << i + 2 << ". Using identity." << std::endl;
            H = cv::Mat::eye(3, 3, CV_64F);
        }

        // *** DEBUG: Print Relative Homography ***
        std::cout << "  Relative H[" << i << "] (Image " << i+1 << " -> " << i+2 << "):\n" << H << std::endl;

        relative_homographies.push_back(H);
    }

    // 3. Accumulate homographies
    std::cout << "Step 3: Accumulating transformations..." << std::endl;
    std::vector<cv::Mat> absolute_homographies_to_pano; // M_i: maps points from image i -> panorama
    cv::Mat current_M = cv::Mat::eye(3, 3, CV_64F); // M_0
    absolute_homographies_to_pano.push_back(current_M.clone());
    std::cout << "  Absolute M[0] (Image 0 -> Pano):\n" << current_M << std::endl; // DEBUG M_0

    for (size_t i = 0; i < relative_homographies.size(); ++i) {
        cv::Mat H_i_to_i_plus_1 = relative_homographies[i];
        cv::Mat H_i_plus_1_to_i; // Inverse: maps points from image i+1 -> image i

        // Check determinant before inverting
        if (cv::determinant(H_i_to_i_plus_1) != 0) {
             H_i_plus_1_to_i = H_i_to_i_plus_1.inv();
        } else {
             std::cerr << "  Warning: Relative H[" << i << "] is singular! Using identity for inverse." << std::endl;
             H_i_plus_1_to_i = cv::Mat::eye(3,3, CV_64F);
        }

        // Accumulate: M_{i+1} = M_i * H_{i+1}_to_i = M_i * (H_i_to_i+1)^-1
        current_M = current_M * H_i_plus_1_to_i;
        absolute_homographies_to_pano.push_back(current_M.clone());

        // *** DEBUG: Print Accumulated Homography ***
        std::cout << "  Absolute M[" << i+1 << "] (Image " << i+1 << " -> Pano):\n" << current_M << std::endl;
    }


    // 4. Determine panorama canvas size
    std::cout << "Step 4: Determining panorama canvas size..." << std::endl;
    std::vector<cv::Point2f> all_transformed_corners;
    for (size_t i = 0; i < all_image_features.size(); ++i) {
        const auto& img_feat = all_image_features[i];
        float w = img_feat.image_projected.cols;
        float h = img_feat.image_projected.rows;
        std::vector<cv::Point2f> corners_orig = {{0,0}, {w,0}, {w,h}, {0,h}};
        std::vector<cv::Point2f> corners_transformed;

        // Check if the absolute homography is valid before transforming corners
        if(absolute_homographies_to_pano[i].empty() || absolute_homographies_to_pano[i].rows != 3 || absolute_homographies_to_pano[i].cols != 3) {
            std::cerr << "  Error: Invalid absolute homography M[" << i << "] for corner transformation. Skipping." << std::endl;
            continue;
        }

        cv::perspectiveTransform(corners_orig, corners_transformed, absolute_homographies_to_pano[i]);

        // *** DEBUG: Print transformed corners ***
        std::cout << "  Transformed corners for Image " << i << ": [";
        for(const auto& pt : corners_transformed) {
            std::cout << "(" << std::fixed << std::setprecision(1) << pt.x << "," << pt.y << ") ";
        }
        std::cout << "]" << std::endl;

        all_transformed_corners.insert(all_transformed_corners.end(), corners_transformed.begin(), corners_transformed.end());
    }

    // Check if we have any corners to calculate ROI
    if(all_transformed_corners.empty()) {
        std::cerr << "Error: No valid transformed corners found. Cannot determine panorama size." << std::endl;
        return cv::Mat();
    }

    cv::Rect panorama_roi = cv::boundingRect(all_transformed_corners);
    // *** DEBUG: Print Panorama ROI ***
    std::cout << "  Calculated Panorama ROI: " << panorama_roi << std::endl;

    // Check if ROI is valid
    if (panorama_roi.width <= 0 || panorama_roi.height <= 0) {
         std::cerr << "Error: Invalid panorama ROI calculated (width or height <= 0)." << std::endl;
         return cv::Mat();
    }


    cv::Mat T_offset_to_canvas = cv::Mat::eye(3, 3, CV_64F);
    T_offset_to_canvas.at<double>(0,2) = -panorama_roi.x;
    T_offset_to_canvas.at<double>(1,2) = -panorama_roi.y;
    int pano_type = all_image_features[0].image_projected.type();
    cv::Mat panorama(panorama_roi.height, panorama_roi.width, pano_type);
    panorama.setTo(cv::Scalar(0));

    // 5. Warp images onto the panorama canvas
    std::cout << "Step 5: Warping images onto panorama canvas..." << std::endl;
    for (size_t i = 0; i < all_image_features.size(); ++i) {

        // Check if absolute homography is valid
         if(absolute_homographies_to_pano[i].empty() || absolute_homographies_to_pano[i].rows != 3 || absolute_homographies_to_pano[i].cols != 3) {
            std::cerr << "  Skipping warp for Image " << i << " due to invalid absolute homography." << std::endl;
            continue;
        }

        cv::Mat H_final_warp = T_offset_to_canvas * absolute_homographies_to_pano[i];

         // *** DEBUG: Print Final Warp Homography ***
        // std::cout << "  Final Warp H for Image " << i << ":\n" << H_final_warp << std::endl; // Optional: Can be very verbose

        cv::Mat warped_image;
        cv::warpPerspective(all_image_features[i].image_projected, warped_image, H_final_warp,
                            panorama.size(), cv::INTER_LINEAR, cv::BORDER_TRANSPARENT);

        // --- Masking and Copying (same as before) ---
        cv::Mat mask;
        if (warped_image.channels() == 1) {
            cv::threshold(warped_image, mask, 0, 255, cv::THRESH_BINARY);
        } else {
             std::vector<cv::Mat> channels;
             cv::split(warped_image, channels);
             if (!channels.empty()) { // Check if split was successful
                 cv::threshold(channels[0], mask, 0, 255, cv::THRESH_BINARY);
             } else { // Fallback if split fails
                 mask = cv::Mat::ones(warped_image.size(), CV_8U) * 255;
                 std::cerr << "Warning: Could not split channels for mask creation on image " << i << ". Using default mask." << std::endl;
             }
        }
        warped_image.copyTo(panorama, mask);
    }

    std::cout << "Stitching complete using ORB." << std::endl;
    return panorama;
}