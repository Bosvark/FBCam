#include <unistd.h>
#include "VideoCapture.h"

namespace vc{

fbcVideoCapture::fbcVideoCapture(cv::VideoCapture *device)
{
	if(device == NULL)
		return;

	vcDevice = device;

	perform_capture = false;
	thread_running = false;
}

fbcVideoCapture::~fbcVideoCapture()
{
	perform_capture = false;
	while(thread_running) usleep(0);
}

std::thread fbcVideoCapture::StartCapturing(void)
{
	frame_captured = false;
	perform_capture = true;
	return std::thread ([this] { thread_run(); });
}

void fbcVideoCapture::StopCapturing(void)
{
	perform_capture = false;

	while(thread_running) usleep(0);
}

bool fbcVideoCapture::isFrameAvailable(void)
{
	return frame_captured;
}

void fbcVideoCapture::FrameRefresh(void)
{
	frame.release();
	frame_captured = false;
}

bool fbcVideoCapture::FetchFrame(cv::Mat &captured_frame)
{
	if(frame_captured){
		captured_frame =  frame.clone();
		frame_captured = false;
		return true;
	}

	return false;
}

cv::Mat& fbcVideoCapture::FetchFrame(void)
{
	frame_captured = false;
	return frame;
}

//
// Private member functions
//
void fbcVideoCapture::thread_run(void)
{
	if(thread_running)
		return;

	thread_running = true;

	std::cout << "Capture thread started." << std::endl;

	while(perform_capture){
/*
		if(frame_captured){
			usleep(0);
			continue;
		}
*/
		if(vcDevice->read(frame)){
			if(!frame.empty())
				frame_captured = true;
		}else{
			frame_captured = false;
//			perform_capture = false;
		}
	}

	thread_running = false;

	std::cout << std::endl << "Capture thread exited." << std::endl;
}

}
