

#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "File name shall be provided\n";
        return 0;
    }

    cv::Mat img = cv::imread(argv[1]).clone();
    if (img.empty()) {
        std::cout << "File name is wrong or the file does not exist\n";
        return 0;
    }

    std::cout << "Number of channels: " << img.channels() << "\n";
    int rows = img.rows, cols = img.cols;
    std::cout << "Matrix dimension: rows: " << rows << " cols: " << cols << "\n";

    if (img.channels() != 3) {
        std::cout << "The image is not a 3-channel (BGR) image.\n";
        return 0;
    }

    for (int row = 0; row < img.rows; ++row) {
        for (int col = 0; col < img.cols; ++col) {
            img.at<cv::Vec3b>(row, col)[0] = 0; // Blue channel = 0
        }
    }
    cv::namedWindow("Modified Image");
    cv::imshow("Modified Image", img);

    cv::Mat img = cv::imread(argv[1]).clone();

    for (int row = 0; row < img.rows; ++row) {
        for (int col = 0; col < img.cols; ++col) {
            img.at<cv::Vec3b>(row, col)[1] = 0; // Blue channel = 0
        }
    }
    cv::namedWindow("Modified Image");
    cv::imshow("Modified Image", img);

    cv::Mat img = cv::imread(argv[1]).clone();

    for (int row = 0; row < img.rows; ++row) {
        for (int col = 0; col < img.cols; ++col) {
            img.at<cv::Vec3b>(row, col)[2] = 0; // Blue channel = 0
        }
    }
    cv::namedWindow("Modified Image");
    cv::imshow("Modified Image", img);



    char test = cv::waitKey(0);
    std::cout << "Value of waitKey: " << test << "\n";

    return 0;
}
