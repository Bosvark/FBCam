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

	std::cout << "Framebuffer width : " << fb.width() << std::endl;
	std::cout << "Framebuffer height: " << fb.height() << std::endl;
	std::cout << "Bits per Pixel    : " << fb.bps() << std::endl;

	unsigned int x = 0, y = 0;
    long int location = 0;

	// Figure out where in memory to put the pixel
	for (y = 5; y < fb.height()-5; y++){
		for (x = 5; x < fb.width()-5; x++) {
			location = (x+fb.xoffset()) * (fb.bps()/8) + (y+fb.yoffset()) * fb.line_length();

			if (fb.bps() == 32) {
				fb.put(location, (unsigned char)100);					// Some blue
				fb.put(location+1, (unsigned char)(15+(x-100)/2));		// A little green
				fb.put(location+2, (unsigned char)(200-(y-100)/5));		// A lot of red
				fb.put(location+3, (unsigned char)0);      				// No transparency
			}else{ //assume 16bpp
				int b = 10;
				int g = (x-100)/6;										// A little green
				int r = 31-(y-100)/16;									// A lot of red
				unsigned short int t = r<<11 | g << 5 | b;
				fb.put(location, t);
			}
		}
	}

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
