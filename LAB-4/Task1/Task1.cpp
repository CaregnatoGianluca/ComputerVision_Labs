#include <opencv2/opencv.hpp>
#include <iostream>

// Callback function to update the Canny edge detection
void on_trackbar(int, void* userdata);

int main(int argc, char** argv) {
    std::string filename = "street_scene.png";

    cv::Mat original_image = cv::imread(filename); // Load the original BGR image
    if (original_image.empty()) {
        std::cout << "File name '" << filename << "' is wrong or the file does not exist\n";
        return -1;
    }

    cv::Mat gray_image;
    cv::cvtColor(original_image, gray_image, cv::COLOR_BGR2GRAY); // Convert to grayscale

    int low_threshold = 50;
    int high_threshold = 150;
    int kernel_size = 3;

    // Create a window to display the Canny edge detection
    cv::namedWindow("Canny Edge Detection", cv::WINDOW_AUTOSIZE);

    // Bundle parameters into a tuple for the callback
    auto params = std::make_tuple(&gray_image, &low_threshold, &high_threshold, &kernel_size);

    // Create trackbars to control the Canny parameters
    cv::createTrackbar("Low Threshold", "Canny Edge Detection", nullptr, 255, on_trackbar, &params);
    cv::createTrackbar("High Threshold", "Canny Edge Detection", nullptr, 255, on_trackbar, &params);
    cv::createTrackbar("Kernel Size", "Canny Edge Detection", nullptr, 7, on_trackbar, &params);

    // Set initial trackbar positions
    cv::setTrackbarPos("Low Threshold", "Canny Edge Detection", low_threshold);
    cv::setTrackbarPos("High Threshold", "Canny Edge Detection", high_threshold);
    cv::setTrackbarPos("Kernel Size", "Canny Edge Detection", kernel_size);

    // Initial call to display the Canny edge detection
    on_trackbar(0, &params);

    cv::imshow("Original Image", original_image);

    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}

void on_trackbar(int, void* userdata) {
    auto params = static_cast<std::tuple<cv::Mat*, int*, int*, int*>*>(userdata);
    cv::Mat* gray_image = std::get<0>(*params);
    int* low_threshold = std::get<1>(*params);
    int* high_threshold = std::get<2>(*params);
    int* kernel_size = std::get<3>(*params);

    // Fetch updated trackbar values
    *low_threshold = cv::getTrackbarPos("Low Threshold", "Canny Edge Detection");
    *high_threshold = cv::getTrackbarPos("High Threshold", "Canny Edge Detection");
    *kernel_size = cv::getTrackbarPos("Kernel Size", "Canny Edge Detection");

    // Ensure kernel size is odd and at least 3
    *kernel_size = std::max(3, *kernel_size | 1);

    cv::Mat edges;
    cv::Canny(*gray_image, edges, *low_threshold, *high_threshold, *kernel_size);
    cv::imshow("Canny Edge Detection", edges);
}