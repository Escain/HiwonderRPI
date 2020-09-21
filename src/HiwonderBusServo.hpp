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

#ifndef HIWONDER_RPI
#define HIWONDER_RPI


#include <cstdint>
#include <functional>

#include <wiringPi.h>
#include <wiringSerial.h>

namespace HiwonderRpi
{

// This class is header-only, for ease of usage.

/// This class represent a Hiwonder servo, and implement
/// basic RPI commands to communicate with it
class HiwonderBusServo
{
	constexpr static uint8_t FrameHeader = 0x55;
	constexpr static uint8_t Placeholder = 0;

	inline uint8_t getLowByte( const uint16_t in)
	{
		return static_cast<uint8_t>(in);
	}
	inline uint8_t getHighByte( const uint16_t in)
	{
		return static_cast<uint8_t>(in>>8);
	}

	inline uint8_t checksum(uint8_t buf[])
	{
		uint8_t i;
		uint16_t temp = 0;
		for (i=2; i<buf[3] +2; ++i)
		{
			temp += buf[i];
		}
		temp = ~temp;
		i=getLowByte(temp);
		return i;
	}

	inline void sendBuf(uint8_t buf[])
	{
		for (uint8_t i=0; i< buf[3]+3; ++i)
		{
			serialPutchar(fd, buf[i]);
		}
	}
	
	inline const uint8_t* getMessage(int expected=8)
	{
		static uint8_t res[10];
		
		for(size_t i=0; i<10 && serialDataAvail(fd) != expected; ++i)
		{
			delay(30);
		}
		if (serialDataAvail(fd)!=expected)
		{
			std::cout << "error in reception " << serialDataAvail(fd) << std::endl;
			res[3]=0;
			return res;
		}
		
		int i=0;
		res[0] = serialGetchar(fd); //frame header 1
		res[1] = serialGetchar(fd); //frame header 2
		res[2] = serialGetchar(fd); //servo id
		res[3] = serialGetchar(fd); //size
		for (size_t i=0; i<res[3]-1; ++i)
		{
			res[i+4] = serialGetchar(fd);
		}
		
		return res;
	}

	int fd = 0;
	int id = 1;

public:
	
	HiwonderBusServo(uint8_t id=1): id(id)
	{
		fd = serialOpen("/dev/ttyAMA0", 115200);
		wiringPiSetup();
	}
	
	virtual ~HiwonderBusServo()
	{
		serialClose(fd);
	}

	void moveTimeWrite( int16_t position, uint16_t time=0)
	{
		constexpr static uint8_t MoveTimeWriteId = 1;
		constexpr static uint8_t MoveTimeWriteSize = 7;
		
		static uint8_t buf[]
		{
			FrameHeader, 
			FrameHeader,
			Placeholder,
			MoveTimeWriteSize,
			MoveTimeWriteId,
			Placeholder,
			Placeholder,
			Placeholder,
			Placeholder,
			Placeholder,
			Placeholder
		};
		
		if (position<0) position=0;
		if (position>1000) position=1000;
		
		buf[2] = id;
		buf[5] = getLowByte(position);
		buf[6] = getHighByte(position);
		buf[7] = getLowByte(time);
		buf[8] = getHighByte(time);
		buf[9] = checksum(buf);
		
		sendBuf(buf);
	}

	void vInRead(std::function<void(uint16_t val)>callback, 
	    bool block=false)
	{
		constexpr static uint8_t VInReadId = 27;
		constexpr static uint8_t VInReadSize = 3;
		
		static uint8_t buf[]
		{
			FrameHeader, 
			FrameHeader,
			Placeholder,
			VInReadSize,
			VInReadId,
			Placeholder
		};
		
		buf[2] = id;
		buf[5] = checksum(buf);
		
		serialFlush(fd);
		sendBuf(buf);
		
		// Read result
		const uint8_t* res= getMessage();
		for (size_t i=0; i<res[3]+3; ++i)
		{
			std::cout << " " << static_cast<int>(res[i]);
		}
		std::cout << std::endl;
	}

	void servoSetId(uint8_t newId)
	{
		constexpr static uint8_t IdWrite = 13;
		constexpr static uint8_t IdWriteSize = 4;
		
		static uint8_t buf[]
		{
			FrameHeader, 
			FrameHeader,
			Placeholder,
			IdWriteSize,
			IdWrite,
			Placeholder,
			Placeholder
		};
		
		buf[2] = id;
		buf[5] = newId;
		buf[6] = checksum(buf);
		
		sendBuf(buf);
	}
	
};

}
#endif //HIWONDER_RPI



