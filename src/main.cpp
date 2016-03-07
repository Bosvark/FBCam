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

int scale=1;
int frame_offset_x=0;
int frame_offset_y=0;

int display_frame(cv::Mat &frame, fb::FrameBuffer &fb)
{
	int x_scaled=0;
	int y_scaled=0;

	for(int y=0; y<frame.rows; y++)
	{
		y_scaled = (y * scale) + frame_offset_y;

		for(int x=0; x<frame.cols; x++)
		{
			cv::Vec3b intensity = frame.at<cv::Vec3b>(y, x);

			uchar b = intensity.val[0]>>3;	// Divide by 8
			uchar g = intensity.val[1]>>3;	// Divide by 8
			uchar r = intensity.val[2]>>3;	// Divide by 8

			unsigned short int t = r<<11 | g << 5 | b;

			x_scaled = (x * scale) + frame_offset_x;

			fb.put_pixel(x_scaled  , y_scaled, t);

			if(scale > 1){
				fb.put_pixel(x_scaled+1, y_scaled, t);

				fb.put_pixel(x_scaled  , y_scaled+1, t);
				fb.put_pixel(x_scaled+1, y_scaled+1, t);
			}
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

	return 0;
}

