#ifndef Filters_h
#define Filters_h
#include <opencv2/opencv.hpp>
#include <vector>
#include <algorithm>

class Filters {
    public:
    static void averageFilter(const cv::Mat&, cv::Mat&, int);
    static void maxFilter(const cv::Mat&, cv::Mat&, int);
    static void minFilter(const cv::Mat&, cv::Mat&, int);
    static void medianFilter(const cv::Mat&, cv::Mat&, int);
    private:
    static int median(std::vector<int>&);
};

#endif
