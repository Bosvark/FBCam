# Makefile - build script */

APPNAME    = fbcam
SOURCEDIR  = ./src
INCLUDEDIR = ./incl
OUTPUTDIR  = ./out

OPENCV_INCS = $(shell pkg-config --cflags opencv)
OPENCV_LIBS = $(shell pkg-config --libs opencv)

# build environment
GNUPREFIX     = arm-linux-gnueabihf-

######################################
# common makefile definition section #
######################################

# source files
SOURCES_ASM := $(wildcard *.S)
SOURCES_C   := $(wildcard *.c)
SOURCES_CPP := $(wildcard *.cpp)

# object files
OBJS=\
     $(OUTPUTDIR)/frame.o           \
     $(OUTPUTDIR)/framebuffer.o     \
     $(OUTPUTDIR)/main.o

# Build flags
DEPENDFLAGS := -MD -MP

INCLUDES    += -I$(INCLUDEDIR)
INCLUDES    += 

LIBS        +=  

BASEFLAGS   += -O0 -g -fpic -pedantic -pedantic-errors
WARNFLAGS   += -Wall
#WARNFLAGS   += -Werror
ASFLAGS     += $(INCLUDES) $(DEPENDFLAGS) -D__ASSEMBLY__
CFLAGS      += $(INCLUDES) $(DEPENDFLAGS) $(BASEFLAGS) $(WARNFLAGS) -std=c99
CXXFLAGS    += $(INCLUDES) $(DEPENDFLAGS) $(BASEFLAGS) $(WARNFLAGS)

.PHONY: all

all: $(OBJS)
	$(info ********************************* Linking *********************************)
	$(GNUPREFIX)g++ -o $(OUTPUTDIR)/$(APPNAME) $(OBJS) $(LIBS) 
	chmod +x $(OUTPUTDIR)/$(APPNAME)

.PHONY: clean

clean:
	rm -f $(OUTPUTDIR)/$(APPNAME)
	rm -f $(OUTPUTDIR)/test 
	rm -f $(OUTPUTDIR)/*.d
	rm -f $(OBJS)

# C++
%.cpp %.o:
	$(info ********************************* Compiling C++ *********************************)
	$(GNUPREFIX)g++ $(CXXFLAGS) -c $< -o $@

# C.
%.c %.o:
	$(info ********************************* Compiling C *********************************)
	$(GNUPREFIX)gcc $(CFLAGS) -c $< -o $@

# AS.
%.s %.o:
	$(GNUPREFIX)gcc $(ASFLAGS) -c $< -o $@

.PHONY:$(OBJDIR)

$(OUTPUTDIR)/frame.o         : $(SOURCEDIR)/frame.cpp $(INCLUDEDIR)/frame.h
$(OUTPUTDIR)/framebuffer.o   : $(SOURCEDIR)/framebuffer.cpp $(INCLUDEDIR)/framebuffer.h
$(OUTPUTDIR)/main.o          : $(SOURCEDIR)/main.cpp
