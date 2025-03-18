#include <opencv2/opencv.hpp>
#include "Filters.h"
#include <iostream>



int main(int argc, char** argv){

    if (argc < 2) {
        std::cout << "File name shall be provided\n";
        return 0;
    }

    cv::Mat img = cv::imread(argv[1]);
    if (img.empty()) {
        std::cout << "File name is wrong or the file does not exist\n";
        return 0;
    }
    
    Filters filter;
    int kernelSize = 3;
    
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::Mat output1, output2;

    //Max filter
    filter.maxFilter(gray, output1, kernelSize);

    //Min filter
    filter.minFilter(gray, output2, kernelSize);

    cv::imshow("Original image", img);
	cv::imshow("Gray image", gray);
    cv::imshow("Max Filter", output1);
    cv::imshow("Min Filter", output2);
    
    cv::waitKey(0);

    return 0;
}
