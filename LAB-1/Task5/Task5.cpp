#include <iostream>
#include <opencv2/highgui.hpp>

int main(int argc, char** argv){

    cv::Mat VerticalGradient(256, 256, CV_8UC1);
    cv::Mat HorizontalGradient(256, 256, CV_8UC1);

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


    cv::Mat Chess1(300, 300, CV_8UC1);
    cv::Mat Chess2(300, 300, CV_8UC1);

    for(int x = 0; x < 300; x++){
        for(int y = 0; y < 300; y++){
            Chess1.at<uchar>(x,y) = 255*((x/20 + y/20) % 2);
            Chess2.at<uchar>(x,y) = 255*((x/50 + y/50) % 2);
        }
    }

    cv::namedWindow("Chess1");
    cv::namedWindow("Chess2");

    cv::imshow("Chess1",Chess1);
    cv::imshow("Chess2", Chess2);

    cv::imwrite("Chess1.jpg", Chess1);
    cv::imwrite("Chess2.jpg", Chess2);

    cv::waitKey(0);
    return 0;
}