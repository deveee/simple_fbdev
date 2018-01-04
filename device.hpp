//    Simple fbdev - Simple demo that uses linux framebuffer device
//    Copyright (C) 2017-2018 Dawid Gan <deveee@gmail.com>
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include <cstdlib>
#include <cstdint>

class Device
{
private:
    static Device* m_device;
    
    int m_fbfd;
    int m_kdfd;
    int m_page_size;
    int m_cur_page;
    bool m_has_orig_vinfo;
    bool m_fbiopan_supported;
    uint8_t* m_framebuffer;
    uint8_t* m_backbuffer;
    struct fb_var_screeninfo m_vinfo;
    struct fb_var_screeninfo m_orig_vinfo;
    struct fb_fix_screeninfo m_finfo;

public:
    Device();
    ~Device();
    
    bool init();
    void clearScreen(uint8_t color);
    void drawPixel(int x, int y, uint8_t color[3]);
    void sleep(unsigned int time_ms);
    void swapBuffers();
    int getWidth() {return m_vinfo.xres;}
    int getHeight() {return m_vinfo.yres;}
    
    static Device* getDevice() {return m_device;}
};

#endif
