
#include <iostream>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "File name shall be provided\n";
        return 0;
    }

    cv::Mat img = cv::imread(argv[1]);
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
    cv::namedWindow("Modified Image1");
    cv::imshow("Modified Image1", img);

    cv::Mat img2 = cv::imread(argv[1]);

    for (int row = 0; row < img2.rows; ++row) {
        for (int col = 0; col < img2.cols; ++col) {
            img2.at<cv::Vec3b>(row, col)[1] = 0; // Blue channel = 0
        }
    }
    cv::namedWindow("Modified Image2");
    cv::imshow("Modified Image2", img2);

    cv::Mat img3 = cv::imread(argv[1]);

    for (int row = 0; row < img3.rows; ++row) {
        for (int col = 0; col < img3.cols; ++col) {
            img3.at<cv::Vec3b>(row, col)[2] = 0; // Blue channel = 0
        }
    }
    cv::namedWindow("Modified Image3");
    cv::imshow("Modified Image3", img3);



    char test = cv::waitKey(0);
    std::cout << "Value of waitKey: " << test << "\n";

    return 0;
}
