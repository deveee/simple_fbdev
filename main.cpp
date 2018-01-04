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

#include <cstdio>

#include "device.hpp"
#include "squares_demo.hpp"


int main(int argc, char* argv[])
{
    Device* device = new Device();
    bool success = device->init();
    
    if (!success)
    {
        printf("Error: Couldn't initialize device.\n");
        return -1;
    }
    
    SquaresDemo* squares_demo = new SquaresDemo();
    success = squares_demo->init();
    
    if (!success)
    {
        printf("Error: Couldn't initialize squares demo.\n");
        return -1;
    }
    
    squares_demo->run();
    
    delete squares_demo;
    delete device;
        
    return 0;
}
