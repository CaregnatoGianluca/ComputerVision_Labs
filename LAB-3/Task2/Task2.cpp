#include <opencv2/opencv.hpp>
#include <iostream>

void click(int event, int x, int y, int, void* userdata);

int main(int argc, char** argv){

    cv::Mat img = cv::imread("Robocup.jpg");
    if (img.empty()) {
        std::cout << "File name is wrong or the file does not exist\n";
        return 0;
    }
    
    
    cv::namedWindow("Image");
    cv::setMouseCallback("Image", click, &img);
    cv::imshow("Image",img);
    cv::waitKey(0);
}

void click(int event, int x, int y, int, void* userdata){
    if (event == cv::EVENT_LBUTTONDOWN) {  // Left mouse button click
        cv::Mat* img = reinterpret_cast<cv::Mat*>(userdata);
        cv::Vec3b pixel;

        int blue = 0, green = 0, red = 0;
        int count = 0;  // To store the number of valid pixels

        for(int i = x - 4; i <= x + 4; i++){
            for(int j = y - 4; j <= y + 4; j++){
                // Correct bounds check
                if(i >= 0 && i < img->cols && j >= 0 && j < img->rows){  
                    pixel = img->at<cv::Vec3b>(j, i);
                    blue += pixel[0];
                    green += pixel[1];
                    red += pixel[2];
                    count++;  // Increment the count of valid pixels
                }
            }
        }

        if (count > 0) {  // Avoid division by zero
            std::cout << "Clicked at (" << x << ", " << y << ") - Mean BGR: ("
                      << blue / count << ", "
                      << green / count << ", "
                      << red / count << ")"
                      << std::endl;
        }
    }
}
