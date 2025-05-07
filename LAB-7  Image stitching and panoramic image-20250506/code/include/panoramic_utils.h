#ifndef LAB5__PANORAMIC__UTILS__H
#define LAB5__PANORAMIC__UTILS__H

#include <opencv2/core.hpp>

cv::Mat cylindricalProj(const cv::Mat& image, const double angle);
cv::Mat imageLoader(const std::string& imagePath);
std::vector<cv::Mat> imagesLoader(const std::string& directoryPath);
	

#endif // LAB5__PANORAMIC__UTILS__H