#include <iostream>
#include <opencv2/highgui.hpp>

int main(int argc, char** argv){

    cv::Mat VerticalGradient(256,256,CV_8UC1);
    cv::Mat HorizontalGradient(256,256,CV_8UC1);

    for(int x = 0; x < 256; x++){
        for(int y = 0; y < 256; y++){
            VerticalGradient.at<uchar>(x,y) = y;
            HorizontalGradient.at<uchar>(x,y) = x;
        }
    }

    cv::namedWindow("VerticalGradient");
    cv::namedWindow("HorizontalGradient");

    cv::imshow("VerticalGradient",VerticalGradient);
    cv::imshow("HorizontalGradient", HorizontalGradient);

    cv::imwrite("vertical_gradient.jpg", VerticalGradient);
    cv::imwrite("horizontal_gradient.jpg", HorizontalGradient);

    cv::waitKey(0);
}