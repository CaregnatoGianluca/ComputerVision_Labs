#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

int main() {
    // Load image
    cv::Mat src = cv::imread("street_scene.png");
    if (src.empty()) {
        std::cerr << "Error: Could not load image!" << std::endl;
        return -1;
    }
    
    // Convert to grayscale and apply Canny edge detector
    cv::Mat gray, edges;
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, 250, 250, 3);
    cv::imshow("Canny Edge Detection", edges);

    // Detect lines using HoughLines
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(edges, lines, 1, CV_PI / 180, 150); // Adjust threshold as necessary
    
    // Filter for near-horizontal lines (theta near pi/2)
    std::vector<int> y_coords;
    const float thetaTolerance = 0.1f; // tolerance in radians
    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0];
        float theta = lines[i][1];
        if (std::fabs(theta - CV_PI / 2) < thetaTolerance) {
            // With theta approx pi/2, the line equation is: y = rho.
            int y = cvRound(rho);
            // Only include candidates within the image boundaries.
            if (y >= 0 && y < src.rows)
                y_coords.push_back(y);
        }
    }
    
    if (y_coords.size() < 2) {
        std::cerr << "Error: Less than two horizontal white lines detected!" << std::endl;
        cv::imshow("Edges", edges);
        cv::waitKey(0);
        return -1;
    }
    
    // Sort the y-coordinates
    std::sort(y_coords.begin(), y_coords.end());
    int topY = y_coords.front();
    int bottomY = y_coords.back();
    
    // Draw a filled red rectangle from topY to bottomY covering full image width.
    cv::Mat overlay = src.clone();
    cv::rectangle(overlay, cv::Point(0, topY), cv::Point(src.cols - 1, bottomY), cv::Scalar(0, 0, 255), cv::FILLED);
    
    // Blend the overlay with the original image
    double alpha = 0.4;
    cv::addWeighted(overlay, alpha, src, 1.0 - alpha, 0.0, src);
    
    cv::imshow("Detected Horizontal Lines & Red Rectangle", src);
    cv::waitKey(0);
    cv::imwrite("output_red_rectangle.jpg", src);
    cv::destroyAllWindows();
    return 0;
}
