#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>

void plotHistogram(const cv::Mat& gray_image, int num_bins, const std::string& window_name) {
    if (gray_image.empty() || gray_image.channels() != 1) {
        std::cerr << "Problem: Image needs to be grayscale for histogram.\n";
        return;
    }

    cv::Mat hist;
    int histSize = num_bins;
    float range[] = { 0, 256 };
    const float* histRange[] = { range };
    bool uniform = true, accumulate = false;
    int channels[] = {0};

    cv::calcHist(&gray_image, 1, channels, cv::Mat(), hist, 1, &histSize, histRange, uniform, accumulate);

    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(255, 255, 255));

    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    for (int i = 1; i < histSize; i++) {
        cv::line(histImage,
                 cv::Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
                 cv::Point(bin_w * i, hist_h - cvRound(hist.at<float>(i))),
                 cv::Scalar(0, 0, 0),
                 2, cv::LINE_AA);
    }

    cv::imshow(window_name, histImage);
}


int main(int argc, char** argv) {

    if (argc < 2) {
        std::cout << "provide the image file name!\n";
        return -1;
    }

    cv::Mat img = cv::imread(argv[1]);

    if (img.empty()) {
        std::cout << "Maybe the file name '" << argv[1] << "' is wrong\n";
        return -1;
    }

    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    std::cout << "Converted image to grayscale." << std::endl;

    cv::imshow("Original GrayGarden", gray);
    std::cout << "Displaying original grayscale image." << std::endl;

    if (!cv::imwrite("GrayGarden.jpg", gray))
         std::cerr << "Warning: Couldn't save GrayGarden.jpg for some reason." << std::endl;

    plotHistogram(gray, 256, "Original Histogram (256 Bins)"); //Calculating and displaying original histogram with 256 bins

    plotHistogram(gray, 64,  "Original Histogram (64 Bins)"); //Calculating and displaying original histogram with 64 bins

    plotHistogram(gray, 16,  "Original Histogram (16 Bins)"); //Calculating and displaying original histogram with 16 bins

    cv::Mat equalized_gray;
    cv::equalizeHist(gray, equalized_gray); //Applied histogram equalization

    cv::imshow("Equalized GrayGarden", equalized_gray);

    if (!cv::imwrite("GrayGarden_equalized.jpg", equalized_gray))
        std::cout << "Warning: Could not save equalized image.\n";


    plotHistogram(equalized_gray, 256, "Equalized Histogram (256 Bins)");


    cv::waitKey(0);

    return 0;
}