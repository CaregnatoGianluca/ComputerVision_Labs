#include "../include/panoramic_utils.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

int main(){

    std::string imagepath1 = "../Images/dataset_dolomites/dolomites";
    std::string imagepath2 = "../Images/dataset_kitchen/kitchen";
    std::string imagepath3 = "../Images/dataset_lab/data";
    std::string imagepath4 = "../Images/dataset_lab_19_automatic";
    std::string imagepath5 = "../Images/dataset_lab_19_manual";


    std::vector<cv::Mat> dolomites = imagesLoader(imagepath1);
    std::vector<cv::Mat> kitchen = imagesLoader(imagepath2);
    std::vector<cv::Mat> lab = imagesLoader(imagepath3);
    std::vector<cv::Mat> lab_19_automatic = imagesLoader(imagepath4);
    std::vector<cv::Mat> lab_19_manual = imagesLoader(imagepath5);

    cv::namedWindow("Dolomites");
    cv::imshow("Dolomites", dolomites[0]);
    cv::Mat dolomites_cylindrical = cylindricalProj(dolomites[0], 360);
    cv::waitKey(0);
}