    #include <opencv2/opencv.hpp>
    #include <opencv2/calib3d.hpp>
    #include <opencv2/highgui.hpp>
    #include <opencv2/imgproc.hpp>
    #include <iostream>
    #include <vector>
    #include <string>
    #include <filesystem> // C++17 for listing files and creating directories
    #include <algorithm>  // For std::transform and std::sort
    #include <stdexcept>  // For std::runtime_error
    #include <fstream>    // For file output (ofstream)
    #include <iomanip>    // For std::setw, std::fixed, std::setprecision (optional formatting)

    int main() {


        int dataset_type = 0;

        int PATTERN_COLS;
        int PATTERN_ROWS;
        float SQUARE_SIZE;
        std::string IMAGE_DIR_PATH;
        std::string TEST_IMAGE_PATH;
        std::string OUTPUT_DIR_PATH;

        if (dataset_type == 0) {
            PATTERN_COLS = 10;
            PATTERN_ROWS = 7;
            SQUARE_SIZE = 1.5f;
            IMAGE_DIR_PATH = "../calibration_images/";
            TEST_IMAGE_PATH = "../calibration_images/img1.jpeg";
            OUTPUT_DIR_PATH = "../Output/"; // Make sure this path is valid from where you run the exe
        } else if (dataset_type == 1) {
            PATTERN_COLS = 6;
            PATTERN_ROWS = 5;
            SQUARE_SIZE = 11.0f;
            IMAGE_DIR_PATH = "../data_lab_calibration/data/checkerboard_images/";
            TEST_IMAGE_PATH = "../data_lab_calibration/data/test_image.png"; // Corrected path
            OUTPUT_DIR_PATH = "../Output1/"; // Make sure this path is valid
        } else {
            std::cerr << "Error: Invalid dataset_type: " << dataset_type << std::endl;
            return -1;
        }
        

        // --- Create Output Directory ---
        
        try {
            if (!std::filesystem::exists(OUTPUT_DIR_PATH)) {
                std::filesystem::create_directories(OUTPUT_DIR_PATH);
                std::cout << "Created directory: " << OUTPUT_DIR_PATH << std::endl;
            }
        } catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Error creating output directory: " << e.what() << std::endl;
            return -1;
        }

        cv::Size pattern_size(PATTERN_COLS, PATTERN_ROWS);
        std::cout << "Using pattern size (inner corners): " << pattern_size.width << "x" << pattern_size.height << std::endl;

        // 1. Prepare object points (3D points of corners in an ideal pattern coordinate system)
        std::vector<cv::Point3f> objp;
        for (int i = 0; i < PATTERN_ROWS; ++i) {
            for (int j = 0; j < PATTERN_COLS; ++j) {
                objp.push_back(cv::Point3f(j * SQUARE_SIZE, i * SQUARE_SIZE, 0.0f));
            }
        }

        std::vector<std::vector<cv::Point3f>> object_points_vec;
        std::vector<std::vector<cv::Point2f>> image_points_vec;
        std::vector<std::string> image_paths;

        // --- Populate image_paths using std::filesystem ---
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
                    std::transform(ext.begin(), ext.end(), ext.begin(),
                                [](unsigned char c){ return std::tolower(c); });
                    if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp" || ext == ".tif" || ext == ".tiff") {
                        image_paths.push_back(path_string);
                    }
                }
            }
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
        std::cout << "Found " << image_paths.size() << " images for calibration." << std::endl;

        cv::Mat gray;
        cv::Size image_size; // Will be set by the first valid image
        int_fast16_t images_used_for_calibration = 0; // Counter for images where corners were found

        // 2. Load images and find chessboard corners
        int image_process_counter = 0;
        for (const std::string& img_path : image_paths) {
            image_process_counter++;
            cv::Mat img = cv::imread(img_path);
            if (img.empty()) {
                std::cerr << "Warning: Could not read image: " << img_path << std::endl;
                continue;
            }

            if (image_size.empty()) { // Set image_size from the first successfully loaded image
                image_size = img.size();
                std::cout << "Using image size: " << image_size.width << "x" << image_size.height << std::endl;
            } else if (image_size != img.size()) {
                std::cout << "Warning: Image " << img_path << " has a different size ("
                        << img.size() << ") than expected (" << image_size << "). Resizing." << std::endl;
                cv::resize(img, img, image_size); // Resize if necessary
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
                
                cv::Mat img_copy_for_drawing = img.clone();
                cv::drawChessboardCorners(img_copy_for_drawing, pattern_size, cv::Mat(corners), pattern_found);
                
                // --- Save image with detected corners ---
                std::string output_filename = OUTPUT_DIR_PATH + "corners_detected_" + std::to_string(images_used_for_calibration) + ".png";
                if (cv::imwrite(output_filename, img_copy_for_drawing)) {
                    std::cout << "Saved: " << output_filename << std::endl;
                } else {
                    std::cerr << "Error: Could not save " << output_filename << std::endl;
                }
                images_used_for_calibration++; // Increment only if corners are found and used
            } else {
                std::cout << "Chessboard corners NOT found in: " << img_path << std::endl;
            }
        }

        if (images_used_for_calibration < 5) {
            std::cerr << "Error: Not enough views with detected patterns (" << images_used_for_calibration << "). Need at least 5-10 for good calibration." << std::endl;
            if (images_used_for_calibration == 0) return -1;
        }

        // 3. Calibrate the camera
        cv::Mat camera_matrix, dist_coeffs;
        std::vector<cv::Mat> rvecs, tvecs;
        int calibration_flags = 0;

        std::cout << "\nStarting camera calibration with " << images_used_for_calibration << " valid views..." << std::endl;
        if (image_size.empty()){ // Should have been set if images_used_for_calibration > 0
            std::cerr << "Error: image_size is not set. Calibration cannot proceed." << std::endl;
            return -1;
        }

        double rms_error = cv::calibrateCamera(object_points_vec, image_points_vec, image_size,
                                            camera_matrix, dist_coeffs, rvecs, tvecs,
                                            calibration_flags);

        if (rms_error <= 0) {
            std::cerr << "Error: Camera calibration failed or produced non-positive RMS error: " << rms_error << std::endl;
            return -1;
        }

        std::cout << "Calibration successful!" << std::endl;
        std::cout << "RMS re-projection error: " << rms_error << " pixels" << std::endl;

        // --- Prepare to write results to text file ---
        std::ofstream results_file;
        std::string results_filepath = OUTPUT_DIR_PATH + "calibration_results.txt";
        results_file.open(results_filepath);

        if (!results_file.is_open()) {
            std::cerr << "Error: Could not open " << results_filepath << " for writing." << std::endl;
        } else {
            results_file << std::fixed << std::setprecision(5); // Set precision for floating point numbers

            results_file << "Calibration Results" << std::endl;
            results_file << "-------------------" << std::endl;
            results_file << "Number of images successfully used: " << images_used_for_calibration << std::endl;
            results_file << "Image resolution: " << image_size.width << "x" << image_size.height << std::endl;
            results_file << "Pattern (inner corners): " << PATTERN_COLS << "x" << PATTERN_ROWS << std::endl;
            results_file << "Square size: " << SQUARE_SIZE << " (units)" << std::endl;
            results_file << "\nRMS re-projection error: " << rms_error << " pixels" << std::endl;
            
            results_file << "\nCamera Matrix (K):" << std::endl;
            results_file << camera_matrix << std::endl;

            results_file << "\nDistortion Coefficients (k1, k2, p1, p2, [k3, k4, k5, k6]):" << std::endl;
            results_file << dist_coeffs << std::endl;

            // 4. Compute and describe mean reprojection error (manual calculation for understanding)
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
                results_file << "\nMean Reprojection Error (calculated manually for verification, should match RMS): "
                            << mean_reprojection_error_manual << " pixels" << std::endl;
            }

            results_file << "\n\n--- Report on Mean Reprojection Error ---" << std::endl;
            results_file << "The Mean Reprojection Error (MRE), reported as RMS error by cv::calibrateCamera, measures the average distance between observed image corners and re-projected 3D pattern corners using estimated camera parameters. A lower RMS error (ideally < 0.5 pixels) indicates a more accurate calibration." << std::endl;
            results_file << "It's calculated as sqrt( (sum of squared Euclidean distances between detected and reprojected points) / (total number of points) )." << std::endl;
            results_file << "-----------------------------------------" << std::endl;

            results_file.close();
            std::cout << "Calibration results saved to: " << results_filepath << std::endl;
        }


        // 5. Undistort and rectify a new image (and save it)
        cv::Mat original_test_img = cv::imread(TEST_IMAGE_PATH);
        if (original_test_img.empty()) {
            std::cerr << "Warning: Could not read test image: " << TEST_IMAGE_PATH << ". Skipping undistortion test." << std::endl;
        } else {
            if (image_size != original_test_img.size() && !image_size.empty()) {
                std::cout << "Warning: Test image " << TEST_IMAGE_PATH << " has a different size. Resizing." << std::endl;
                cv::resize(original_test_img, original_test_img, image_size);
            }

            cv::Mat undistorted_test_img;
            cv::Mat map1, map2;
            cv::Mat new_camera_matrix = cv::getOptimalNewCameraMatrix(camera_matrix, dist_coeffs, image_size, 1.0, image_size, nullptr, false);
            cv::initUndistortRectifyMap(camera_matrix, dist_coeffs, cv::Mat(), new_camera_matrix, image_size, CV_16SC2, map1, map2);
            cv::remap(original_test_img, undistorted_test_img, map1, map2, cv::INTER_LINEAR);

            // 6. Save distorted and undistorted test images side-by-side
            cv::Mat comparison_img;
            if (!original_test_img.empty() && !undistorted_test_img.empty()) {
                try {
                    cv::hconcat(original_test_img, undistorted_test_img, comparison_img);
                    std::string comparison_filename = OUTPUT_DIR_PATH + "test_image_comparison.png";
                    if (cv::imwrite(comparison_filename, comparison_img)) {
                        std::cout << "Saved test image comparison: " << comparison_filename << std::endl;
                    } else {
                        std::cerr << "Error: Could not save " << comparison_filename << std::endl;
                    }
                } catch (const cv::Exception& e) {
                    std::cerr << "OpenCV exception during hconcat for test image: " << e.what() << std::endl;
                    // Optionally save them separately if hconcat fails
                    cv::imwrite(OUTPUT_DIR_PATH + "test_image_original.png", original_test_img);
                    cv::imwrite(OUTPUT_DIR_PATH + "test_image_undistorted.png", undistorted_test_img);
                }
            } else {
                std::cerr << "Warning: Original or undistorted test image is empty. Cannot save comparison." << std::endl;
            }
        }

        std::cout << "\nProcessing complete. Check the '" << OUTPUT_DIR_PATH << "' folder." << std::endl;
        // cv::destroyAllWindows(); // Not strictly needed as no windows were kept open persistently

        return 0;
    }