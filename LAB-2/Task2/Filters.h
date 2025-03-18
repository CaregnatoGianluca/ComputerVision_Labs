#ifndef Filters_h
#define Filters_h
#include <opencv2/opencv.hpp>
//#include <opencv2/highgui.hpp>

class Filters{
    public:
    void averageFilter(const cv::Mat&, cv::Mat&, int);
    void maxFilter(const cv::Mat&, cv::Mat&, int);
    void minFilter(const cv::Mat&, cv::Mat&, int);

};


#endif