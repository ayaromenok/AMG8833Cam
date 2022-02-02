#!/bin/sh
#sudo apt update
#sudo apt-get build-dep qt5-default -y
#sudo apt-get install qt5-default  libqt5serialport5-dev -y
#sudo apt install libopencv-dev 
mkdir bin
cd bin
qmake ../AMG8833Cam.pro
make -j 4