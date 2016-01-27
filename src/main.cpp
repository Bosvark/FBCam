#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "framebuffer.h"

#define VERSION_MAJOR	1
#define VERSION_MINOR	0
#define APP_NAME		"FBCam"

int main(int argc, char *argv[])
{
	std::cout << APP_NAME << " " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;

	fb::FrameBuffer fb = fb::FrameBuffer("/dev/fb0");

	if(!fb.isOpen()){
		perror("Failed to open framebuffer device\n");
		exit(1);
	}

	std::cout << "Framebuffer width: " << fb.width() << std::endl;

#if 0	// Opencv test code
	cv::VideoCapture cap(0);

	if(!cap.isOpened()){
		std::cout << "Cannot open video stream" << std::endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	std::cout << "Frame size:" << dWidth << "x" << dHeight << std::endl;

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
#endif

	return 0;
}
