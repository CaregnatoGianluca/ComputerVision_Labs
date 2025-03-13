#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
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

    cv::namedWindow("Example 1");
	cv::imshow("Example 1", gray);
	cv::waitKey(0);

    return 0;
    

    return 0;
}