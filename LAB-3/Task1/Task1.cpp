#include <opencv2/opencv.hpp>
#include <iostream>

void click(int event, int x, int y, int, void* userdata);

int main(int argc, char** argv){

    cv::Mat img = cv::imread("Robocup.jpg");
    if (img.empty()) {
        std::cout << "File name is wrong or the file does not exist\n";
        return 0;
    }
    
    
    cv::namedWindow("Image");
    cv::setMouseCallback("Image", click, &img);
    cv::imshow("Image",img);
    cv::waitKey(0);
}

void click(int event, int x, int y, int, void* userdata){
	
	if (event == cv::EVENT_LBUTTONDOWN) {  // Left mouse button click
        cv::Mat* img = reinterpret_cast<cv::Mat*>(userdata);
        cv::Vec3b pixel = img->at<cv::Vec3b>(y, x);

        std::cout << "Clicked at (" << x << ", " << y << ") - BGR: ("
             << (int)pixel[0] << ", "  // Blue
             << (int)pixel[1] << ", "  //Green
             << (int)pixel[2] << ")"  //red
             << std::endl;
    }
}
