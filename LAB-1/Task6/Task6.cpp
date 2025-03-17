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
    
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::Mat output1, output2, output3, output4, output5;

    //Average filter:
    Filters filter;
    int kernelSize = 5;
    filter.averageFilter(gray, output1, kernelSize);


    //cv::blur(gray, output2, cv::Size(kernelSize, kernelSize));
    cv::Sobel(gray, output2, CV_64F, 1, 0, kernelSize);
    cv::Sobel(gray, output3, CV_64F, 0, 1, kernelSize);


    //Max filter
    filter.maxFilter(gray, output4, kernelSize);
    //cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));
    //cv::dilate(gray, output5, kernel);

    //Min filter
    filter.minFilter(gray, output5, kernelSize);

    cv::imshow("Original image", img);
	cv::imshow("Gray image", gray);
	cv::imshow("Average filter", output1);
    cv::imshow("Sobel vertical", output2);
    cv::imshow("Sobel horizontal", output3);
    cv::imshow("Max Filter", output4);
    cv::imshow("Min Filter", output5);
    
    cv::waitKey(0);

    return 0;
}

