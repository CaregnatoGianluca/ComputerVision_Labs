#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>

void averageFilterG(const cv::Mat&, cv::Mat&, int);

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

    cv::Mat output;
    averageFilterG(gray, output, 3);


	cv::imshow("Example 1", gray);
	cv::imshow("Example 2", output);



	cv::waitKey(0);

    return 0;
}

void averageFilterG(cv::Mat& input, cv::Mat& output, int kernelSize){

    //let's initializate the output matrix
    output = cv::Mat::zeros(input.size(), input.type());

    int sum, counter; //they will be used to compute the average

    //Now we want to create a loop that modify one by one all pixels in the image
    for(int x = 0; x < input.cols; x++){
        for(int y = 0; y < input.rows; y++){
            sum, counter = 0;
            //We are doing a average filter so we must compute the average of the neighborhood
            for(int i = -kernelSize/2; i < kernelSize/2; i++){
                for(int j = -kernelSize/2; j < kernelSize/2; j++){
                    //since we can have some problems with the borders we check if the neighbourg exists
                    if((x + i >= 0) && (x + i < input.cols) && (y + j >= 0) && (y + j < input.rows)){ 
                        counter++;
                        sum += input.at<uchar>(x + i, y + j);
                    }
                }
            }

            output.at<uchar>(x,y) = sum / counter; //if we are not in a border couter is equal to kernelSize*kernelSize

        }
    }
}