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

#include "device.hpp"

#include <cstdio>
#include <cstring>

Device* Device::m_device = NULL;

Device::Device()
{
    m_device = this;
    m_fbfd = -1;
    m_kdfd = -1;
    m_framebuffer = NULL;
    m_backbuffer = NULL;
    m_page_size = 0;
    m_cur_page = 1;
    m_has_orig_vinfo = false;
    m_fbiopan_supported = true;
    
    memset(&m_vinfo, 0, sizeof(fb_var_screeninfo));
    memset(&m_orig_vinfo, 0, sizeof(fb_var_screeninfo));
    memset(&m_finfo, 0, sizeof(fb_fix_screeninfo));
}

Device::~Device()
{
    if (m_kdfd >= 0) 
    {
        ioctl(m_kdfd, KDSETMODE, KD_TEXT);
        close(m_kdfd);
    }
    
    if (!m_fbiopan_supported)
    {
        delete[] m_backbuffer;
    }

    if (m_framebuffer != NULL)
    {
        munmap(m_framebuffer, m_finfo.smem_len);
    }    
    
    if (m_has_orig_vinfo)
    {
        ioctl(m_fbfd, FBIOPUT_VSCREENINFO, &m_orig_vinfo);
    }
    
    if (m_fbfd >= 0)
    {
        close(m_fbfd);
    }
}

bool Device::init()
{
    const char* fb_paths[] = {"/dev/fb0",
                              "/dev/graphics/fb0",
                              "/mnt/dev/graphics/fb0"};
                                           
    for (const char* fb_path : fb_paths)
    {
        m_fbfd = open(fb_path, O_RDWR);
        
        if (m_fbfd != -1)
            break;
    }

    if (m_fbfd == -1) 
    {
        printf("Error: Cannot open framebuffer device.\n");
        return false;
    }

    int err = ioctl(m_fbfd, FBIOGET_VSCREENINFO, &m_vinfo);

    if (err != 0) 
    {
        printf("Error: Cannot read variable screen info.\n");
        return false;
    }

    memcpy(&m_orig_vinfo, &m_vinfo, sizeof(struct fb_var_screeninfo));
    m_has_orig_vinfo = true;
    
    m_vinfo.bits_per_pixel = 32;
    
    err = ioctl(m_fbfd, FBIOPUT_VSCREENINFO, &m_vinfo);
    
    if (err != 0) 
    {
        printf("Error: Cannot set 32bit mode.\n");
        return false;
    }
    
    m_vinfo.xres_virtual = m_vinfo.xres;
    m_vinfo.yres_virtual = m_vinfo.yres * 2;
    
    err = ioctl(m_fbfd, FBIOPUT_VSCREENINFO, &m_vinfo);
    
    if (err != 0) 
    {
        printf("Warning: Page flipping not supported.\n");
        m_fbiopan_supported = false;
        
        m_vinfo.xres_virtual = m_vinfo.xres;
        m_vinfo.yres_virtual = m_vinfo.yres;
        
        err = ioctl(m_fbfd, FBIOPUT_VSCREENINFO, &m_vinfo);
    }
    
    if (err != 0) 
    {
        printf("Error: Cannot set variable screen info.\n");
        return false;
    }
    
    err = ioctl(m_fbfd, FBIOGET_FSCREENINFO, &m_finfo);
    
    if (err != 0) 
    {
        printf("Error: Cannot read fixed screen info.\n");
        return false;
    }

    m_page_size = m_finfo.line_length * m_vinfo.yres;

    m_framebuffer = (uint8_t*)mmap(0, m_finfo.smem_len, PROT_READ | PROT_WRITE, 
                                   MAP_SHARED, m_fbfd, 0);
                                   
    if ((intptr_t)m_framebuffer == -1) 
    {
        printf("Error: Cannot map framebuffer device.\n");
        return false;
    }
            
    if (m_fbiopan_supported)
    {                      
        m_backbuffer = m_framebuffer + m_page_size;
    }
    else
    {
        m_backbuffer = new uint8_t[m_finfo.smem_len];
    }

    int m_kdfd = open("/dev/tty", O_WRONLY);
    
    if (m_kdfd >= 0) 
    {
        ioctl(m_kdfd, KDSETMODE, KD_GRAPHICS);
    }
    
    return true;
}

void Device::clearScreen(uint8_t color) 
{
    memset(m_backbuffer, color, m_page_size);
}

void Device::drawPixel(int x, int y, uint8_t color[3])
{
    int pix_offset = x * 4 + y * m_finfo.line_length;
    
    m_backbuffer[pix_offset + 0] = color[2];
    m_backbuffer[pix_offset + 1] = color[1];
    m_backbuffer[pix_offset + 2] = color[0];
    m_backbuffer[pix_offset + 3] = 255;
}

void Device::sleep(unsigned int time_ms)
{
    struct timespec ts;
    ts.tv_sec = (time_t)(time_ms / 1000);
    ts.tv_nsec = (long)(time_ms % 1000) * 1000000;

    nanosleep(&ts, NULL);
}

void Device::swapBuffers()
{
    if (m_fbiopan_supported)
    {
        m_vinfo.yoffset = m_cur_page * m_vinfo.yres;
        ioctl(m_fbfd, FBIOPAN_DISPLAY, &m_vinfo);
        
        m_cur_page = (m_cur_page + 1) % 2;
        m_backbuffer = m_framebuffer + m_cur_page * m_page_size;
    }
    else
    {
        memcpy(m_framebuffer, m_backbuffer, m_finfo.smem_len);
        
        m_vinfo.activate = FB_ACTIVATE_VBL;
        ioctl(m_fbfd, FBIOPUT_VSCREENINFO, &m_vinfo);
    }
    
    uint32_t dummy = 0;
    ioctl(m_fbfd, FBIO_WAITFORVSYNC, &dummy);
}
