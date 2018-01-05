APP_NAME="simple_fbdev"
SOURCES="main.cpp device.cpp squares_demo.cpp"

g++ -g -O3 -std=gnu++0x -Wall -I./ $SOURCES -o $APP_NAME
