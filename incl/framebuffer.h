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

	bool isOpen(){return (fbh>0?true:false);}
	int height(){return this->vinfo.yres;}
	int width(){return this->vinfo.xres;}
	int bps(){return this->vinfo.bits_per_pixel;}

private:
	int fbh;
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
	char *fbp;
};

}

#endif // _FB_H_
