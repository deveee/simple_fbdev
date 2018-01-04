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
#include "squares_demo.hpp"

#define FRAMES_COUNT 500
#define ELEMENTS_COUNT 10

SquaresDemo::SquaresDemo()
{
}

SquaresDemo::~SquaresDemo()
{
}

bool SquaresDemo::init()
{ 
    Device* device = Device::getDevice();
    
    for (int i = 0; i < ELEMENTS_COUNT; i++) 
    {
        Square square;
        square.size = device->getHeight() / 10;
        square.x = rand() % (device->getWidth() - square.size);
        square.y = rand() % (device->getHeight() - square.size);
        square.dx = (rand() % 10) + 1;
        square.dy = (rand() % 10) + 1;
        square.color[0] = rand() % 256;
        square.color[1] = rand() % 256;
        square.color[2] = rand() % 256;
        
        m_squares.push_back(square);
    }
    
    return true;
}

void SquaresDemo::drawSquare(Square& square)
{
    Device* device = Device::getDevice();

    for (int i = 0; i < square.size; i++) 
    {
        for (int j = 0; j < square.size; j++) 
        {
            device->drawPixel(square.x + j, square.y + i, square.color);
        }
    }
}

void SquaresDemo::updatePosition(Square& square)
{
    Device* device = Device::getDevice();

    if ((square.x + square.dx < 0) || 
        (square.x + square.dx > device->getWidth() - square.size)) 
    {
        square.dx = -square.dx;
    }

    if ((square.y + square.dy < 0) || 
        (square.y + square.dy > device->getHeight() - square.size)) 
    {
        square.dy = -square.dy;
    }

    square.x += square.dx;
    square.y += square.dy;
}

void SquaresDemo::run()
{
    for (int i = 0; i < FRAMES_COUNT; i++) 
    {
        Device* device = Device::getDevice();
        device->clearScreen(100);
        
        for (Square& square : m_squares) 
        {
            drawSquare(square);
            updatePosition(square);
        }
        
        device->swapBuffers();
        device->sleep(15);
    }
}
