# HiwonderRPI
Raspberry PI library to control Hiwonder UART servos


How to use:

1) Make sure you have git, g++, cmake and wiringPi installed:

sudo apt-get install git g++ cmake wiringpi

2) Make sure UART is activated on your RPI

$ sudo raspi-config
    5 Interfacing Options
    P6 Serial
    login shell to be accessible over serial? <NO>
    Serial Port Hardware? <YES>
    <Finish>

3) Download the project

$ mkdir hiwonder_rpi
$ cd hiwonder_rpi
$ git clone https://github.com/Escain/HiwonderRPI
$ cd ..

4) Configure the project

$ mkdir build_hiwonder_rpi
$ cd build_hiwonder_rpi
$ cmake ../hiwonder_rpi

5) Build the project:

$ make

6) Test something

$ sudo ./hiwonder
