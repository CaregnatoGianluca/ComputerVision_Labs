
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
	
	std::cout << "Number of channels: " << img.channels() << "\n";
	
	cv::namedWindow("Example 1");
	cv::imshow("Example 1", img);
	char test = cv::waitKey(0);
	std::cout << "Value of waitKey: " << test << "\n";
	
	
	return 0;
}
