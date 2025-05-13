#ifndef STITCHER_H
#define STITCHER_H

#include <vector>
#include <string>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp> // For cv::Ptr<cv::Feature2D>, cv::Ptr<cv::DescriptorMatcher>

class Stitcher {
public:
    // 'min_distance_ratio' is the 'ratio' multiplier for min_distance filter as per prompt.
    // 'ransac_reproj_thresh' is for cv::findHomography's RANSAC.
    Stitcher(double min_distance_ratio = 3.0, double ransac_reproj_thresh = 5.0);

    // Stitches a vector of base images.
    // 'dataset_identifier' is used to determine FoV (e.g., "dolomites" or other values like "kitchen", "lab").
    cv::Mat stitch(const std::vector<cv::Mat>& base_images, const std::string& dataset_identifier);

private:
    cv::Ptr<cv::Feature2D> feature_detector_;
    cv::Ptr<cv::DescriptorMatcher> matcher_;
    double min_distance_ratio_;
    double ransac_reproj_thresh_;

    // Structure to hold features and projected image for each input image
    struct ImageFeatures {
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        cv::Mat image_projected; // Cylindrically projected image (grayscale as per panoramic_utils)
    };
};

#endif // STITCHER_H