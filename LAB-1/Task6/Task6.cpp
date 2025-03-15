//#include <opencv2/highgui.hpp>
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

    cv::Mat output1, output2, output3;

    Filters filter;

    filter.averageFilter(gray, output1, 5);
    cv::avgFilter
    cv::blur(gray, output2, cv::Size(5, 5));


    cv::imshow("Original image", img);
	cv::imshow("Gray image", gray);
	cv::imshow("Average filter1", output1);
    cv::imshow("Average filter2", output2);
    //cv::imshow("Average filter3", output3);

	cv::waitKey(0);

    return 0;
}

