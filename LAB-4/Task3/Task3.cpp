#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Load image
    cv::Mat src = cv::imread("street_scene.png");
    if (src.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }
    
    // Apply Gaussian Blur for smoothing to reduce noise
    cv::Mat blurred;
    cv::GaussianBlur(src, blurred, cv::Size(5, 5), 1.5);
    
    // Convert to grayscale and apply Canny edge detector on the blurred image
    cv::Mat gray, edges;
    cv::cvtColor(blurred, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, 250, 250, 3);
    cv::imshow("Canny Edge Detection", edges);

    // Detect lines using HoughLines
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 180, 100); 
    
    // Create a window for interactive parameter tuning
    cv::namedWindow("Detected Road Signs", cv::WINDOW_AUTOSIZE);

    int dp_slider = 15;          // dp = dp_slider / 10.0 (e.g., 10 -> 1.0)
    int minDist_slider = gray.rows / 8;
    int param1_slider = 150;     // Canny high threshold (lower threshold is half of this)
    int param2_slider = 26;      // Accumulator threshold for circle centers
    int minRadius_slider = 7;
    int maxRadius_slider = 15;    // 0 means no maximum radius

    // Create trackbars to adjust the parameters
    cv::createTrackbar("dp x0.1", "Detected Road Signs", &dp_slider, 20);
    cv::createTrackbar("minDist", "Detected Road Signs", &minDist_slider, gray.rows);
    cv::createTrackbar("Canny Threshold", "Detected Road Signs", &param1_slider, 300);
    cv::createTrackbar("Accumulator Thresh", "Detected Road Signs", &param2_slider, 100);
    cv::createTrackbar("minRadius", "Detected Road Signs", &minRadius_slider, 100);
    cv::createTrackbar("maxRadius", "Detected Road Signs", &maxRadius_slider, 150);

    while (true) {
        // Clone the original image for drawing
        cv::Mat temp = src.clone();

        // Read trackbar positions
        double dp = dp_slider / 10.0;
        int minDist = minDist_slider;
        int param1 = param1_slider;
        int param2 = param2_slider;
        int minRadius = minRadius_slider;
        int maxRadius = maxRadius_slider;

        // Detect circles using HoughCircles with current parameters
        std::vector<cv::Vec3f> circles;
        cv::HoughCircles(edges, circles, cv::HOUGH_GRADIENT, dp, minDist, param1, param2, minRadius, maxRadius);

        // Draw the detected circles on the image
        for (size_t i = 0; i < circles.size(); i++) {
            cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
            int radius = cvRound(circles[i][2]);
            cv::circle(temp, center, 3, cv::Scalar(0, 255, 0), -1);
            cv::circle(temp, center, radius, cv::Scalar(0, 0, 255), 3);
        }

        // Display the updated image
        cv::imshow("Detected Road Signs", temp);

        // Exit loop if 'q' or ESC is pressed
        char key = (char)cv::waitKey(30);
        if (key == 27 || key == 'q') break;
    }
    
    // Apply Hough Circle Transform to detect circles (road signs)
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(edges, circles, cv::HOUGH_GRADIENT, 1, gray.rows / 16, 100, 30, 0, 0);

    // Draw the detected circles on the original image
    for (size_t i = 0; i < circles.size(); i++) {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        // Draw the circle center
        cv::circle(src, center, 3, cv::Scalar(0, 255, 0), -1);
        // Draw the circle outline
        cv::circle(src, center, radius, cv::Scalar(0, 0, 255), 3);
    }

    // Display the result
    cv::imshow("Detected Road Signs", src);
    cv::waitKey(0);
    return 0;
}