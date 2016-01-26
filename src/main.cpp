#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#define VERSION_MAJOR	1
#define VERSION_MINOR	0
#define APP_NAME		"FBCam"

int main(int argc, char *argv[])
{
	std::cout << APP_NAME << " " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;

	cv::VideoCapture cap(0);

	if(!cap.isOpened()){
		std::cout << "Cannot open video stream" << std::endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	std::cout << "Frame sixe:" << dWidth << "x" << dHeight << std::endl;

	cv::namedWindow("MyVideo", CV_WINDOW_AUTOSIZE);

	while(1){
		cv::Mat frame;

		bool bSuccess = cap.read(frame);

		if(!bSuccess){
			std::cout << "Cannot read frame from video stream" << std::endl;
			break;
		}

		cv::imshow("MyVideo", frame);

		if(cv::waitKey(30) == 27){
			std::cout << "Escape key pressed" << std::endl;
			break;
		}
	}

	return 0;
}
