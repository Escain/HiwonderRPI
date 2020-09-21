# HiwonderRPI
Raspberry PI library to control Hiwonder UART servos


How to use:

1) Make sure you have git, g++, cmake and wiringPi installed:

sudo apt-get install git g++ cmake wiringpi

2) Download the project

mkdir hiwonder_rpi
cd hiwonder_rpi
git clone https://github.com/Escain/HiwonderRPI
cd ..

3) Configure the project

mkdir build_hiwonder_rpi
cd build_hiwonder_rpi
cmake ../hiwonder_rpi

4) Build the project:

make

5) Test something

./hiwonder
