#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <filesystem> // C++17 for listing files
#include <algorithm>  // For std::transform and std::sort
#include <stdexcept>  // For std::runtime_error



int main() {

    const int PATTERN_COLS = 7;
    const int PATTERN_ROWS = 5;
    const float SQUARE_SIZE = 25.0f;

    const std::string IMAGE_DIR_PATH = "../../calibration_images/";
    const std::string TEST_IMAGE_PATH = "calibration_images/img01.jpg"; // Ensure this image exists

    cv::Size pattern_size(PATTERN_COLS, PATTERN_ROWS);

    // 1. Prepare object points
    std::vector<cv::Point3f> objp;
    for (int i = 0; i < PATTERN_ROWS; ++i) {
        for (int j = 0; j < PATTERN_COLS; ++j) {
            objp.push_back(cv::Point3f(j * SQUARE_SIZE, i * SQUARE_SIZE, 0.0f));
        }
    }

    std::vector<std::vector<cv::Point3f>> object_points_vec;
    std::vector<std::vector<cv::Point2f>> image_points_vec;
    std::vector<std::string> image_paths;

    // --- Using C++17 std::filesystem to list image files ---
    try {
        if (!std::filesystem::exists(IMAGE_DIR_PATH)) {
            throw std::runtime_error("Image directory does not exist: " + IMAGE_DIR_PATH);
        }
        if (!std::filesystem::is_directory(IMAGE_DIR_PATH)) {
            throw std::runtime_error("Specified image path is not a directory: " + IMAGE_DIR_PATH);
        }

        for (const auto& entry : std::filesystem::directory_iterator(IMAGE_DIR_PATH)) {
            if (entry.is_regular_file()) {
                std::string path_string = entry.path().string();
                std::string ext = entry.path().extension().string();
                // Convert extension to lower case for case-insensitive comparison
                std::transform(ext.begin(), ext.end(), ext.begin(),
                               [](unsigned char c){ return std::tolower(c); });

                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".tif" || ext == ".tiff") {
                    image_paths.push_back(path_string);
                }
            }
        }
        // Sort paths for consistent order, which can be helpful for reproducibility
        std::sort(image_paths.begin(), image_paths.end());

    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return -1;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }


    if (image_paths.empty()) {
        std::cerr << "Error: No compatible images found in directory: " << IMAGE_DIR_PATH << std::endl;
        return -1;
    }

    std::cout << "Found " << image_paths.size() << " images for calibration using std::filesystem." << std::endl;

    cv::Mat gray;
    cv::Size image_size;
    int_fast16_t images_used = 0;

    // 2. Load images and find chessboard corners
    for (const std::string& img_path : image_paths) {
        cv::Mat img = cv::imread(img_path);
        if (img.empty()) {
            std::cerr << "Warning: Could not read image: " << img_path << std::endl;
            continue;
        }

        if (image_size.empty()) {
            image_size = img.size();
        } else if (image_size != img.size()) {
            std::cerr << "Warning: Image " << img_path << " has a different size ("
                      << img.size() << ") than expected (" << image_size << "). Skipping." << std::endl;
            continue;
        }

        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        std::vector<cv::Point2f> corners;
        bool pattern_found = cv::findChessboardCorners(gray, pattern_size, corners,
                                                       cv::CALIB_CB_ADAPTIVE_THRESH |
                                                       cv::CALIB_CB_NORMALIZE_IMAGE |
                                                       cv::CALIB_CB_FAST_CHECK);

        if (pattern_found) {
            std::cout << "Chessboard corners found in: " << img_path << std::endl;
            object_points_vec.push_back(objp);
            cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
                             cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 30, 0.001));
            image_points_vec.push_back(corners);
            images_used++;

            cv::Mat img_copy_for_drawing = img.clone();
            cv::drawChessboardCorners(img_copy_for_drawing, pattern_size, cv::Mat(corners), pattern_found);
            cv::imshow("Found Corners", img_copy_for_drawing);
            cv::waitKey(0);
        } else {
            std::cout << "Chessboard corners NOT found in: " << img_path << std::endl;
        }
    }
    if (images_used > 0) { cv::destroyWindow("Found Corners"); }


    if (images_used < 5) {
        std::cerr << "Error: Not enough views with detected patterns (" << images_used << "). Need at least 5-10. Calibration might fail or be inaccurate." << std::endl;
        if (images_used == 0) return -1;
    }

    // 3. Calibrate the camera
    cv::Mat camera_matrix, dist_coeffs;
    std::vector<cv::Mat> rvecs, tvecs;
    int calibration_flags = 0; //cv::CALIB_FIX_ASPECT_RATIO; // Add more flags if needed

    std::cout << "\nStarting camera calibration..." << std::endl;
    double rms_error = cv::calibrateCamera(object_points_vec, image_points_vec, image_size,
                                           camera_matrix, dist_coeffs, rvecs, tvecs,
                                           calibration_flags);

    if (rms_error <= 0) {
        std::cerr << "Error: Camera calibration failed or produced non-positive RMS error: " << rms_error << std::endl;
        return -1;
    }

    std::cout << "Calibration successful!" << std::endl;
    std::cout << "Camera Matrix (fx, fy, cx, cy): \n" << camera_matrix << std::endl;
    std::cout << "Distortion Coefficients (k1, k2, p1, p2, [k3, [k4, k5, k6]]): \n" << dist_coeffs << std::endl;
    std::cout << "RMS re-projection error: " << rms_error << " pixels" << std::endl;

    // 4. Compute and describe mean reprojection error
    double total_error_sq = 0;
    int total_points = 0;
    for (size_t i = 0; i < object_points_vec.size(); ++i) {
        if (i < rvecs.size() && i < tvecs.size()) {
            std::vector<cv::Point2f> projected_points;
            cv::projectPoints(object_points_vec[i], rvecs[i], tvecs[i], camera_matrix, dist_coeffs, projected_points);
            if (projected_points.size() == image_points_vec[i].size()) {
                double view_error_sq = 0;
                for(size_t j=0; j < projected_points.size(); ++j) {
                    cv::Point2f diff = image_points_vec[i][j] - projected_points[j];
                    view_error_sq += diff.x * diff.x + diff.y * diff.y;
                }
                total_error_sq += view_error_sq;
                total_points += object_points_vec[i].size();
            }
        }
    }
    if (total_points > 0) {
        double mean_reprojection_error_manual = std::sqrt(total_error_sq / total_points);
        std::cout << "Mean Reprojection Error (calculated manually, should be identical to RMS): " << mean_reprojection_error_manual << " pixels" << std::endl;
    }
    std::cout << "\n--- Report on Mean Reprojection Error ---" << std::endl;
    std::cout << "The Mean Reprojection Error (MRE), reported as RMS error by cv::calibrateCamera, measures the average distance between observed image corners and re-projected 3D pattern corners using estimated camera parameters. A lower RMS error (ideally < 0.5 pixels) indicates a more accurate calibration." << std::endl;
    std::cout << "It's calculated as sqrt( (sum of squared Euclidean distances between detected and reprojected points) / (total number of points) )." << std::endl;
    std::cout << "-----------------------------------------" << std::endl;

    // 5. Undistort and rectify a new image
    cv::Mat original_img = cv::imread(TEST_IMAGE_PATH);
    if (original_img.empty()) {
        std::cerr << "Error: Could not read test image: " << TEST_IMAGE_PATH << std::endl;
        return -1;
    }
    if (image_size != original_img.size()) {
        std::cout << "Warning: Test image " << TEST_IMAGE_PATH << " has a different size. Resizing." << std::endl;
        cv::resize(original_img, original_img, image_size);
    }

    cv::Mat undistorted_img;
    cv::Mat map1, map2;
    cv::Mat new_camera_matrix = cv::getOptimalNewCameraMatrix(camera_matrix, dist_coeffs, image_size, 1.0, image_size, nullptr, false);
    cv::initUndistortRectifyMap(camera_matrix, dist_coeffs, cv::Mat(), new_camera_matrix, image_size, CV_16SC2, map1, map2);
    cv::remap(original_img, undistorted_img, map1, map2, cv::INTER_LINEAR);

    // 6. Show distorted and undistorted images
    cv::Mat comparison_img;
    if (original_img.empty() || undistorted_img.empty()) {
        std::cerr << "Error: Original or undistorted image is empty before concatenation." << std::endl;
        return -1;
    }
    try {
        cv::hconcat(original_img, undistorted_img, comparison_img);
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV exception during hconcat: " << e.what() << std::endl;
        cv::imshow("Original (Distorted)", original_img);
        cv::imshow("Undistorted", undistorted_img);
        cv::waitKey(0);
        return -1;
    }

    cv::namedWindow("Original (Left) vs Undistorted (Right)", cv::WINDOW_AUTOSIZE);
    cv::imshow("Original (Left) vs Undistorted (Right)", comparison_img);
    std::cout << "\nDisplaying original vs undistorted image. Press any key to exit." << std::endl;
    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}