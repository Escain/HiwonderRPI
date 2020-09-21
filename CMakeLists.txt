#
# This file is part of HiwonderRPI library
# 
# HiwonderRPI is free software: you can redistribute it and/or modify 
# it under ther terms of the GNU General Public License as published by 
# the Free Software Foundation, either version 3 of the License, or 
# (at your option) any later version.
# 
# HiwonderRPI is distributed in the hope that it will be useful, 
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License 
# along with HiwonderRPI. If not, see <https://www.gnu.org/licenses/>.
# 
# Author: Adrian Maire escain (at) gmail.com
#

# Define minimal CMake version
cmake_minimum_required( VERSION 3.0 )

# Project name
project ( "HiwonderRPI" )
# C++17
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

include_directories("src")

# Command-line example
add_executable("hiwonder" examples/HiwonderCommand.cpp)
target_link_libraries("hiwonder" "wiringPi")
