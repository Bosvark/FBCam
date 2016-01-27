#ifndef _FB_H_
#define _FB_H_

#include <string>
#include <linux/fb.h>

namespace fb
{
class FrameBuffer
{
public:
	FrameBuffer(std::string device);
	~FrameBuffer();

	int put(unsigned int location, unsigned char value);
	int put(unsigned int location, unsigned short value);
	bool isOpen(){return (fbh>0?true:false);}
	unsigned int height(){return this->vinfo.yres;}
	unsigned int width(){return this->vinfo.xres;}
	unsigned int bps(){return this->vinfo.bits_per_pixel;}
	unsigned int xoffset(){return this->vinfo.xoffset;}
	unsigned int yoffset(){return this->vinfo.yoffset;}
	unsigned int line_length(){return this->finfo.line_length;}

private:
	int fbh;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	char *fbp;
};

}

#endif // _FB_H_
