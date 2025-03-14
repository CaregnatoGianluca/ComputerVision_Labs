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
	


    if (img.channels() == 3) {

        std::cout << "OK\n";

        std::vector<cv::Mat> channels(3);
        cv::split(img, channels); //BRG

        cv::Mat bluechannel = channels[0];
        cv::Mat greenchannel = channels[1];
        cv::Mat redchannel = channels[2];

        cv::namedWindow("Example 2");
        cv::namedWindow("Example 3");
        cv::namedWindow("Example 4");

        cv::imshow("Example 2", bluechannel);
        cv::imshow("Example 3", greenchannel);
        cv::imshow("Example 4", redchannel);
    }
	
	char test = cv::waitKey(0);

	return 0;
}
