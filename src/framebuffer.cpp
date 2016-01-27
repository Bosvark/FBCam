#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include "framebuffer.h"

namespace fb
{

FrameBuffer::FrameBuffer(std::string device)
{
	fbp = NULL;
	fbh = 0;

	memset(&vinfo, 0, sizeof(struct fb_var_screeninfo));
	memset(&finfo, 0, sizeof(struct fb_fix_screeninfo));

	if(device.empty())
		return;

	fbh = open(device.c_str(), O_RDWR);

	if(fbh < 0)
		return;

	// Get fixed screen information
	if (ioctl(fbh, FBIOGET_FSCREENINFO, &finfo) == -1) {
		perror("Error reading fixed information");
		return;
	}

	// Get variable screen information
	if (ioctl(fbh, FBIOGET_VSCREENINFO, &vinfo) == -1) {
		perror("Error reading variable information");
		return;
	}

	// Figure out the size of the screen in bytes
	int screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	// Map the device to memory
	fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbh, 0);

	if (fbp == NULL) {
		perror("Error: failed to map framebuffer device to memory");
		return;
	}
}

FrameBuffer::~FrameBuffer()
{
	if(this->isOpen()){
		int screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
		munmap(fbp, screensize);	// Unmap the allocated memory

		close(fbh);

		fbp = NULL;
		fbh = 0;

		memset(&vinfo, 0, sizeof(struct fb_var_screeninfo));
		memset(&finfo, 0, sizeof(struct fb_fix_screeninfo));
	}
}

}
