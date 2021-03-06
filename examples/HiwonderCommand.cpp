/*
 * This file is part of HiwonderRPI library
 * 
 * HiwonderRPI is free software: you can redistribute it and/or modify 
 * it under ther terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version.
 * 
 * HiwonderRPI is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License 
 * along with HiwonderRPI. If not, see <https://www.gnu.org/licenses/>.
 * 
 * Author: Adrian Maire escain (at) gmail.com
 */

#include <iostream>
#include <vector>
#include "HiwonderBusServo.hpp"


// Some raspian OS still don't have C++17 -> no std::optional
// This is a very incomplete std::optional implementation
#if __has_include(<optional>)
#include <optional>
#else
namespace std
{
	template <typename T>
	struct optional
	{
		bool set=false;
		T value;
		optional() = default;
		optional( bool opt ){}
		optional( T t): set(true), value(t){}
		operator bool() const {return set;}
		const T& operator*() const {return value;}
	};
	
	static constexpr bool nullopt = false;
}
#endif


/// Parse and check an argument for a servo ID
///@arg str: input string
///@arg pos: argument position, for error message
///@return servo id
std::optional<uint8_t> getServoId(const std::string& str, int pos)
{
	uint8_t id=0;
	try
	{
		id = std::stoi(str);
	}
	catch(...)
	{
		std::cout << "Error, argument " << pos << " expected "
		    "to be a valid servo id" << std::endl;
		return std::nullopt;
	}
	return id;
}


/// Parse and check an argument for a servo angle
///@arg str: input string
///@arg pos: argument position, for error message
///@return servo angle
std::optional<uint16_t> getServoAngle(const std::string& str, int pos)
{
	uint16_t angle=0;
	try
	{
		angle = std::stoi(str);
	}
	catch(...)
	{
		std::cout << "Error, argument " << pos << " expected "
		    "to be a valid angle" << std::endl;
		return std::nullopt;
	}
	
	if (angle <0 || angle >1000)
	{
		std::cout << "Error, angle " << pos << " must be in the"
		    "range [0-1000]" << std::endl;
		return std::nullopt;
	}
	
	return angle;
}

/// Print the command help message
void printHelp()
{
	std::cout << 
	"Very minimal command to manage Hiwonder servo. \n"
	" ./hiwonder [command] arg1 arg2 ...\n"
	"\n"
	"Command list:\n"
	" - set_middle <id>: Set the servo with id=<id> to it middle position (500)\n"
	" - move <id> <angle>: Set the servo with id=<id> to it position=<angle> in 0s\n"
	" - read_voltage <id>: Return the input voltage for the servo with id=<id>\n"
	" - read_position <id>: Return the current position of the servo with id=<id>" << std::endl;
}

bool checkArguments( int num, int exp, const std::string& name )
{
	if (num!=exp+2)
	{
		std::cout << "Error: " << name << " command expect " << exp << " arguments" << std::endl;
		printHelp();
		return false;
	}
	return true;
}


/// main function
auto main(int num, char* args[]) ->int
{
	if (1 >= num)
	{
		printHelp();
		return 0;
	}
	
	std::vector<std::string> argsStr;
	for (int i=0; i< num; ++i) argsStr.push_back( args[i]);
	const auto& command = argsStr[1];

	if (command=="set_middle")
	{
		if (!checkArguments(num, 1, "set_middle")) return 1;
		
		auto idOpt = getServoId(argsStr[2],1);
		if (!idOpt) return 1;
		
		HiwonderRpi::HiwonderBusServo servo(*idOpt);
		
		servo.moveTimeWrite( 500, 0);
		delay(1500);
	}
	else if (command == "move")
	{
		if (!checkArguments(num, 2, "move")) return 1;
		
		auto idOpt = getServoId(argsStr[2],1);
		if (!idOpt) return 1;
		auto angleOpt = getServoAngle(argsStr[3],2);
		if (!angleOpt) return 1;
		
		HiwonderRpi::HiwonderBusServo servo(*idOpt);

		servo.moveTimeWrite( *angleOpt, 0);
		delay(3000);
	}
	else if (command == "read_voltage")
	{
		if (!checkArguments(num, 1, "read_voltage")) return 1;
		
		auto idOpt = getServoId(argsStr[2],1);
		if (!idOpt) return 1;
		
		HiwonderRpi::HiwonderBusServo servo(*idOpt);
		std::cout << "    " << static_cast<float>(servo.vinRead())/1000.0f << "V" << std::endl;
	}
	else if (command == "read_position")
	{
		if (!checkArguments(num, 1, "read_position")) return 1;
		
		auto idOpt = getServoId(argsStr[2],1);
		if (!idOpt) return 1;
		
		HiwonderRpi::HiwonderBusServo servo(*idOpt);
		std::cout << "    " << static_cast<float>(servo.posRead())*0.24f << "º" << std::endl;
	}
	else if (command == "demo")
	{
		std::cout <<  "Demoing..." <<  std::endl;
		//demo(fd);
	}
	else
	{
		printHelp();
	}
}
