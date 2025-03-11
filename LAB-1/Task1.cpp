#include <iostream>
#include <opencv2/highgui.hpp>

int main(int argc, char** argv){
	
	if(argc < 2){
		std::cout << "File name shall be provided\n";
		return 0;
	}
	if(cv::imread(argv[1]).empty()){
		std::cout << "File name is wrong or the file does not exists";
		return 0;
	}
	
	cv::Mat img = cv::imread(argv[1]);
	cv::namedWindow("Example 1");
	cv::imshow("Example 1", img);
	cv::waitKey(0);
	
	
	
	return 0;
}
