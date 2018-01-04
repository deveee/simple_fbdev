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

#ifndef SQUARES_DEMO_HPP
#define SQUARES_DEMO_HPP

#include <vector>

struct Square
{
    int x;
    int y;
    int dx;
    int dy;
    int size;
    uint8_t color[3];
};

class SquaresDemo
{
private:
    std::vector<Square> m_squares;

    void drawSquare(Square& square);
    void updatePosition(Square& square);
        
public:
    SquaresDemo();
    ~SquaresDemo();
    
    bool init();
    void run();
};

#endif
