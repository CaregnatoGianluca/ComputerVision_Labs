#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
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
    cv::HoughLines(edges, lines, 1, CV_PI / 180, 100); 

    // Fixed angles: 139° and 40°
    int angle1_deg = 139, angle2_deg = 40;

    while (true) {
        cv::Mat display = src.clone();
        // Convert fixed angles from degrees to radians
        double a1_rad = angle1_deg * CV_PI / 180.0;
        double a2_rad = angle2_deg * CV_PI / 180.0;
        const double tolerance = 5 * CV_PI / 180.0; // 5 degree tolerance

        // Draw lines matching the selected angles
        for (const auto& line : lines) {
            float rho = line[0], theta = line[1];
            if (std::abs(theta - a1_rad) < tolerance || std::abs(theta - a2_rad) < tolerance) {
                double a = cos(theta), b = sin(theta);
                double x0 = a * rho, y0 = b * rho;
                cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
                cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
                cv::line(display, pt1, pt2, cv::Scalar(0, 0, 255), 2);
            }
        }

        // Select one line for each angle
        bool found1 = false, found2 = false;
        cv::Vec2f selLine1, selLine2;
        for (const auto& line : lines) {
            float rho = line[0], theta = line[1];
            if (!found1 && std::abs(theta - a1_rad) < tolerance) {
                selLine1 = line;
                found1 = true;
            }
            if (!found2 && std::abs(theta - a2_rad) < tolerance) {
                selLine2 = line;
                found2 = true;
            }
            if (found1 && found2)
                break;
        }

        if (found1 && found2) {
            // Compute intersections of each selected line with the bottom edge
            int y_bottom = display.rows - 1;
            double rho1 = selLine1[0], theta1 = selLine1[1];
            double rho2 = selLine2[0], theta2 = selLine2[1];
            cv::Point ptBottom1, ptBottom2;
            if (std::abs(std::cos(theta1)) > 1e-6)
                ptBottom1.x = cvRound((rho1 - y_bottom * std::sin(theta1)) / std::cos(theta1));
            else
                ptBottom1.x = 0;
            ptBottom1.y = y_bottom;
            if (std::abs(std::cos(theta2)) > 1e-6)
                ptBottom2.x = cvRound((rho2 - y_bottom * std::sin(theta2)) / std::cos(theta2));
            else
                ptBottom2.x = 0;
            ptBottom2.y = y_bottom;

            // Compute the intersection of the two selected lines
            double a1 = std::cos(theta1), b1 = std::sin(theta1), c1 = rho1;
            double a2 = std::cos(theta2), b2 = std::sin(theta2), c2 = rho2;
            double det = a1 * b2 - a2 * b1;
            if (std::abs(det) > 1e-6) {
                int x_inter = cvRound((c1 * b2 - c2 * b1) / det);
                int y_inter = cvRound((a1 * c2 - a2 * c1) / det);
                cv::Point ptIntersection(x_inter, y_inter);

                // Create triangle from bottom intersections and line intersection
                std::vector<cv::Point> triangle = {ptBottom1, ptBottom2, ptIntersection};
                // Fill triangle with red color (BGR: 0,0,255)
                cv::fillConvexPoly(display, triangle, cv::Scalar(0, 0, 255));
                // Optionally, draw triangle outline
                cv::polylines(display, triangle, true, cv::Scalar(0, 0, 255), 2);
            }
        }

        cv::imshow("Selected Lines", display);
        if (cv::waitKey(30) == 27) break; // exit if ESC is pressed
    }
    return 0;
}
