#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

void averageFilter(const cv::Mat&, cv::Mat&, int);

int main(int argc, char** argv){

    if (argc < 2) {
        std::cout << "File name shall be provided\n";
        return 0;
    }

    cv::Mat img = cv::imread(argv[1]);
    if (img.empty()) {
        std::cout << "File name is wrong or the file does not exist\n";
        return 0;
    }
    
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

    cv::Mat output1;
    cv::Mat output2;
    cv::Mat output3;
    averageFilter(gray, output1, 3);
    averageFilter(gray, output2, 5);
    averageFilter(gray, output3, 7);


    cv::imshow("Original image", img);
	cv::imshow("Gray image", gray);
	cv::imshow("Average filter1", output1);
    cv::imshow("Average filter2", output2);
    cv::imshow("Average filter3", output3);



	cv::waitKey(0);

    return 0;
}

void averageFilter(const cv::Mat& input, cv::Mat& output, int kernelSize){

    //let's initializate the output matrix
    output = cv::Mat::zeros(input.size(), input.type());

    int sum, counter; //they will be used to compute the average

    //Now we want to create a loop that modify one by one all pixels in the image
    for(int x = 0; x < input.cols; x++){
        for(int y = 0; y < input.rows; y++){
            sum = 0;
            counter = 0;
            //We are doing a average filter so we must compute the average of the neighborhood
            for(int i = -kernelSize/2; i <= kernelSize/2; i++){
                for(int j = -kernelSize/2; j <= kernelSize/2; j++){
                    //since we can have some problems with the borders we check if the neighbourg exists
                    if((x + i >= 0) && (x + i < input.cols) && (y + j >= 0) && (y + j < input.rows)){ 
                        counter++;
                        sum += input.at<uchar>(y + i, x + j);
                    }
                }
            }

            output.at<uchar>(y,x) = sum / counter; //if we are not in a border couter is equal to kernelSize*kernelSize

        }
    }
}