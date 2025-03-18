#ifndef Filters_h
#define Filters_h
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>


class Filters{
    public:
    void averageFilter(const cv::Mat&, cv::Mat&, int);
    void maxFilter(const cv::Mat&, cv::Mat&, int);
    void minFilter(const cv::Mat&, cv::Mat&, int);
    void medianFilter(const cv::Mat&, cv::Mat&, int);
    private:
    int median(std::vector<int>&);

};


#endif