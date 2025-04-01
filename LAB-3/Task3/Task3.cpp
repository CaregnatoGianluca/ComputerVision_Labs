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
        cv::Vec3b pixel = img->at<cv::Vec3b>(y, x);
		int tolerance = 25;
		bool select;
        
        for(int i = 0; i < input.cols; i++){
			for(int j = 0; j < input.rows; j++){
				select = false;
				if(pixel[0] - tolerance <= img->at<cv::Vec3b>(j, i)[0] && pixel[0] + tolerance <= img->at<cv::Vec3b>(j, i)[0]){
					if(pixel[1] - tolerance <= img->at<cv::Vec3b>(j, i)[1] && pixel[1] + tolerance <= img->at<cv::Vec3b>(j, i)[1]){
						if(pixel[2] - tolerance <= img->at<cv::Vec3b>(j, i)[2] && pixel[2] + tolerance <= img->at<cv::Vec3b>(j, i)[2]){
							img->at<cv::Vec3b>(j, i) = cv::Vec3b(255, 255, 255);
							select = true;
							}
						}
					}
				}
				if(!select)
					img->at<cv::Vec3b>(j, i) = cv::Vec3b(0, 0, 0);
				
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
