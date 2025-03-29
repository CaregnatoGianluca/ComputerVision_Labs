#include <opencv2/opencv.hpp>
#include <iostream>
#include <string> 

void plotHistogram(const cv::Mat& gray_image, int num_bins, const std::string& window_name) {

    //Calculate Histogram
    cv::Mat hist;
    int histSize = num_bins;
    float range[] = { 0, 256 }; // Upper bound is exclusive
    const float* histRange[] = { range };
    bool uniform = true, accumulate = false;
    int channels[] = {0};

    cv::calcHist(&gray_image, 1, channels, cv::Mat(), hist, 1, &histSize, histRange, uniform, accumulate);

    //Create Image to Display Histogram
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255, 255, 255)); // White background

    //Normalize Histogram to Fit Image Height
    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    //Draw Histogram Lines
    for (int i = 1; i < histSize; i++) {
        cv::line(histImage,
                 cv::Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
                 cv::Point(bin_w * i, hist_h - cvRound(hist.at<float>(i))),
                 cv::Scalar(0, 0, 0), // Black line
                 2, cv::LINE_AA); // Thickness 2, anti-aliased
    }

    cv::imshow(window_name, histImage);

}


int main(int argc, char** argv) {


    if (argc < 2) {
        std::cout << "provide the image file name!\n";
        return -1; // Return -1 usually indicates an error occurred
    }

    cv::Mat img = cv::imread(argv[1]); 

    if (img.empty()) {
        std::cout << "Maybe the file name '" << argv[1] << "' is wrong\n";
        return -1;
    }

    // Convert the image to grayscale
    cv::Mat gray; // Create a Mat object to hold the grayscale version
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY); // Do the conversion
    std::cout << "Converted image to grayscale." << std::endl;

    // Show the grayscale image
    cv::imshow("GrayGarden", gray); //
    std::cout << "Displaying grayscale image." << std::endl;

    // Try saving the grayscale image
    if (!cv::imwrite("GrayGarden.jpg", gray)) 
         std::cerr << "Warning: Couldn't save GrayGarden.jpg for some reason." << std::endl;

    std::cout << "Calculating and displaying histogram with 256 bins" << std::endl;
    plotHistogram(gray, 256, "Histogram (256 Bins)"); // Full detail histogram

    std::cout << "Calculating and displaying histogram with 64 bins" << std::endl;
    plotHistogram(gray, 64,  "Histogram (64 Bins)");  // Less detail

    std::cout << "Calculating and displaying histogram with 16 bins" << std::endl;
    plotHistogram(gray, 16,  "Histogram (16 Bins)");  // Even less detail (more blocky)


    cv::waitKey(0);
    return 0;
}