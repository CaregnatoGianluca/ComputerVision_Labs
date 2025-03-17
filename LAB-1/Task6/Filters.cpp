#include "Filters.h"

void Filters::averageFilter(const cv::Mat& input, cv::Mat& output, int kernelSize){

    //let's initialize the output matrix
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
                    //since we can have some problems with the borders we check if the neighbor exists
                    if((x + i >= 0) && (x + i < input.cols) && (y + j >= 0) && (y + j < input.rows)){ 
                        counter++;
                        sum += input.at<uchar>(y + i, x + j);
                    }
                }
            }

            output.at<uchar>(y,x) = sum / counter; //if we are not on a border, counter is equal to kernelSize*kernelSize

        }
    }
}
void Filters::maxFilter(const cv::Mat& input, cv::Mat& output, int kernelSize){

    //let's initialize the output matrix
    output = cv::Mat::zeros(input.size(), input.type());

    int max; //they will be used to compute the average

    //Now we want to create a loop that modify one by one all pixels in the image
    for(int x = 0; x < input.cols; x++){
        for(int y = 0; y < input.rows; y++){
            max = 0;
            //We are doing a max filter so we must search the max value of the neighborhood
            for(int i = -kernelSize/2; i <= kernelSize/2; i++){
                for(int j = -kernelSize/2; j <= kernelSize/2; j++){
                    //since we can have some problems with the borders we check if the neighbor exists
                    if((x + i >= 0) && (x + i < input.cols) && (y + j >= 0) && (y + j < input.rows)){ 
                        if(input.at<uchar>(y + i, x + j) > max)
                            max = input.at<uchar>(y + i, x + j);
                    }
                }
            }

            output.at<uchar>(y,x) = max; //if we are not on a border, counter is equal to kernelSize*kernelSize

        }
    }
}
void Filters::minFilter(const cv::Mat& input, cv::Mat& output, int kernelSize){

    //let's initialize the output matrix
    output = cv::Mat::zeros(input.size(), input.type());

    int min; //they will be used to compute the average

    //Now we want to create a loop that modify one by one all pixels in the image
    for(int x = 0; x < input.cols; x++){
        for(int y = 0; y < input.rows; y++){
            min = 255;
            //We are doing a min filter so we must search the min value of the neighborhood
            for(int i = -kernelSize/2; i <= kernelSize/2; i++){
                for(int j = -kernelSize/2; j <= kernelSize/2; j++){
                    //since we can have some problems with the borders we check if the neighbor exists
                    if((x + i >= 0) && (x + i < input.cols) && (y + j >= 0) && (y + j < input.rows)){ 
                        if(input.at<uchar>(y + i, x + j) < min)
                            min = input.at<uchar>(y + i, x + j);
                    }
                }
            }

            output.at<uchar>(y,x) = min; //if we are not on a border, counter is equal to kernelSize*kernelSize

        }
    }
}