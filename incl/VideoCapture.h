#include <opencv2/opencv.hpp>
#include <cstddef>
#include <thread>
#include <mutex>

namespace vc{

class fbcVideoCapture
{
public:
	fbcVideoCapture(){thread_running=false; perform_capture=false; frame_captured=false; vcDevice=NULL;}
	fbcVideoCapture(cv::VideoCapture *device);
	~fbcVideoCapture();

	std::thread StartCapturing(void);
	void StopCapturing(void);

	bool isFrameAvailable(void);
	void FrameRefresh(void);
	bool FetchFrame(cv::Mat &captured_frame);
	cv::Mat& FetchFrame(void);

private:
	cv::VideoCapture *vcDevice;
	cv::Mat frame;

	bool perform_capture;
	bool thread_running;
	bool frame_captured;

	void thread_run(void);
};

}
