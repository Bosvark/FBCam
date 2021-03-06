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
	bytes_per_pixel = 0;
	x_bytes_per_pixel = 0;
	y_bytes_per_pixel = 0;

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

	bytes_per_pixel = vinfo.bits_per_pixel / 8;
	x_bytes_per_pixel = vinfo.xres * bytes_per_pixel;
	y_bytes_per_pixel = vinfo.yres * bytes_per_pixel;

	// Figure out the size of the screen in bytes
	int screensize = vinfo.xres * vinfo.yres * bytes_per_pixel;

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

int FrameBuffer::put(unsigned int location, unsigned char value)
{
	*(fbp + location) = value;
	return 0;
}

int FrameBuffer::put(unsigned int location, unsigned short value)
{
	*(fbp + location) = (char)value;
	*(fbp + location + 1) = (char)(value >> 8);
	return 0;
}

int FrameBuffer::put_pixel(int x, int y, unsigned short value)
{
	if((x > (int)this->vinfo.xres) || (y > (int)this->vinfo.yres))
		return -1;

	unsigned int location = (y * this->finfo.line_length) + (x * bytes_per_pixel);

	*(fbp + location) = (char)value;
	*(fbp + location + 1) = (char)(value >> 8);

	return 0;
}

}
