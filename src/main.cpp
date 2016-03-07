#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
//#include <opencv2/core/types.hpp>
#include "framebuffer.h"

#define VERSION_MAJOR	1
#define VERSION_MINOR	0
#define APP_NAME		"FBCam"

char cspace[255];

int info(void)
{
	fb::FrameBuffer fb = fb::FrameBuffer("/dev/fb0");

	if(!fb.isOpen()){
		perror("Failed to open framebuffer device\n");
		return -1;
	}

	cv::VideoCapture cap(0);

	if(!cap.isOpened()){
		std::cout << "Cannot open video stream" << std::endl;
		return -2;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	std::cout << "Framebuffer width : " << fb.width() << std::endl;
	std::cout << "Framebuffer height: " << fb.height() << std::endl;
	std::cout << "Bits per Pixel    : " << fb.bps() << std::endl;
	std::cout << "Line length       : " << fb.line_length() << std::endl;

	std::cout << std::endl;

	std::cout << "Video size:" << dWidth << "x" << dHeight << std::endl;

	return 0;
}

int scale=2;
int frame_offset_x=0;
int frame_offset_y=0;

int display_frame(cv::Mat &frame, fb::FrameBuffer &fb)
{
	int x_scaled=0;
	int y_scaled=0;

	for(int y=0; y<frame.rows; y++)
	{
		for(int x=0; x<frame.cols; x++)
		{
			cv::Vec3b intensity = frame.at<cv::Vec3b>(y, x);

			uchar b = intensity.val[0]>>3;	// Divide by 8
			uchar g = intensity.val[1]>>3;	// Divide by 8
			uchar r = intensity.val[2]>>3;	// Divide by 8

			unsigned short int t = r<<11 | g << 5 | b;

			x_scaled = x * scale;
			y_scaled = y * scale;

			fb.put_pixel((x_scaled)+frame_offset_x, (y_scaled)+frame_offset_y, t);
			fb.put_pixel((x_scaled+1)+frame_offset_x, (y_scaled)+frame_offset_y, t);

			fb.put_pixel((x_scaled)+frame_offset_x, (y_scaled+1)+frame_offset_y, t);
			fb.put_pixel((x_scaled+1)+frame_offset_x, (y_scaled+1)+frame_offset_y, t);
		}
	}

	return 0;
}


int main(int argc, char *argv[])
{
	std::cout << APP_NAME << " " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;

	if(info() < 0)
		return -1;

	fb::FrameBuffer fb = fb::FrameBuffer("/dev/fb0");

	if(!fb.isOpen()){
		perror("Failed to open framebuffer device\n");
		return -1;
	}

	cv::VideoCapture cap(0);

	if(!cap.isOpened()){
		std::cout << "Cannot open video stream" << std::endl;
		return -2;
	}

	// Calculate color space reduction lookup table
	for (int i=0; i<255; i++){
		cspace[i] = (char)(i/8);
	}

	char display_frame_info_flag=1;
	unsigned int fps=0;
	double tick = (double)cv::getTickCount();
	double tick_freq = cv::getTickFrequency();
	double current_tick = 0;

	while(1){
		cv::Mat frame;

		bool bSuccess = cap.read(frame);

		if(!bSuccess){
			std::cout << "Cannot read frame from video stream" << std::endl;
			break;
		}

		if(display_frame_info_flag){
			display_frame_info_flag = 0;

			std::cout << "Frame Channels:" << frame.channels() << std::endl;
			std::cout << "Color space:" << frame.type() << std::endl;
			std::cout << "Step:" << frame.step1() << std::endl;
			std::cout << "Continues:" << frame.isContinuous() << std::endl;

			switch(frame.depth())
			{
				case CV_8U: std::cout << "Depth: CV_8U" << std::endl; break;
				case CV_8S:  std::cout << "Depth: CV_8S" << std::endl; break;
				case CV_16U: std::cout << "Depth: CV_16U" << std::endl; break;
				case CV_16S: std::cout << "Depth: CV_16S" << std::endl; break;
				case CV_32S: std::cout << "Depth: CV_32S" << std::endl; break;
				case CV_32F: std::cout << "Depth: CV_32F" << std::endl; break;
				case CV_64F: std::cout << "Depth: CV_64F" << std::endl; break;
			}

			frame_offset_x=((fb.width() - (frame.cols*scale))/2);
			frame_offset_y=((fb.height() - (frame.rows*scale))/2);

		}

		display_frame(frame, fb);

//		std::cout << "Fps:" << cap.get(CV_CAP_PROP_FPS) << "         " << "\r";

		current_tick = ((double)cv::getTickCount() - tick)/tick_freq;

		if(current_tick >= 1.0){
			std::cout << "Fps:" << fps << "         " << "\r" << std::flush;
			fps = 0;
			tick = (double)cv::getTickCount();
		}else{
			fps++;
		}

	}

#if 0
	fb::FrameBuffer fb = fb::FrameBuffer("/dev/fb0");

	if(!fb.isOpen()){
		perror("Failed to open framebuffer device\n");
		return 1;
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
#endif
#if 0	// Opencv test code
	cv::VideoCapture cap(0);

	if(!cap.isOpened()){
		std::cout << "Cannot open video stream" << std::endl;
		return -1;
	}

//	cv::Size refS = cv::Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH), (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));

//	int dWidth = refS.width;
//	int dHeight = refS.height;

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

