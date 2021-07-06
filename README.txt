HiwonderRPI
===========


HiwonderRPI is a C++ library to control Hiwonder UART servos with a Raspberry PI. 
Even if it uses some of the modern C++ features, it is designed with performance and usability in mind.

Install & use
-------------

1) Make sure you have git, g++, cmake and wiringPi installed:

$ sudo apt-get install git g++ cmake wiringpi

2) Make sure UART is activated on your RPI

$ sudo raspi-config
    5 Interfacing Options
    P6 Serial
    login shell to be accessible over serial? <NO>
    Serial Port Hardware? <YES>
    <Finish>

3) Download the project

$ git clone https://github.com/Escain/HiwonderRPI ./hiwonder_rpi
$ cd ..

4) Configure the project

$ mkdir build_hiwonder_rpi
$ cd build_hiwonder_rpi
$ cmake ../hiwonder_rpi

5) Build the project:

$ make

6) Test something

$ sudo ./hiwonder


Feedback & Suggestions
----------------------

For any feedback or suggestion:

https://github.com/Escain/HiwonderRPI/discussions

Issues
------

If you found a bug or issue, feel free to open an issue at https://github.com/Escain/HiwonderRPI/issues

If you are not sure it is an issue, feel free to open a feedback/discussion: https://github.com/Escain/HiwonderRPI/discussions

Questions
---------

Please, prefer to post questions at https://www.stackoverflow.com or https://electronics.stackexchange.com and use the tag HiwonderRPI.
This make them available for other users as well.
