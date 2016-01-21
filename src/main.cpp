#include <iostream>

#define VERSION_MAJOR	1
#define VERSION_MINOR	0
#define APP_NAME		"FBCam"

int main(int argc, char *argv[])
{
	std::cout << APP_NAME << " " << VERSION_MAJOR << "." << VERSION_MINOR << std::endl;
	return 0;
}
