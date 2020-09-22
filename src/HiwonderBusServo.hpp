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

#include <array>
#include <cstdint>
#include <exception>
#include <functional>

#include <wiringPi.h>
#include <wiringSerial.h>

namespace HiwonderRpi
{

// This class is header-only, for ease of usage.

/// This class represent a Hiwonder servo, and implement
/// basic UART communication from a Raspberry PI.
class HiwonderBusServo
{
	using Buffer = std::array<uint8_t,10>;
	
public:
	/// Constructor, accept the servo ID. 
	/// Id=254 is the broadcast ID
	HiwonderBusServo( uint8_t id=254 );
	HiwonderBusServo( const HiwonderBusServo&& );
	/// Servo object can not be copied (UART access is unique)
	HiwonderBusServo( const HiwonderBusServo& ) = delete;
	HiwonderBusServo& operator=( const HiwonderBusServo&) = delete;
	
	virtual ~HiwonderBusServo();

	/// Immediately start moving the servo to the given position
	///     trying to reach target position in the given time (ms)
	/// @arg position: target absolute position in multiples of 0.24deg
	/// @arg time: time to reach the target position in ms (if too short, max-speed is used)
	void moveTimeWrite( int16_t position, uint16_t time=0);
	
	/// Read the values set by moveTimeWrite
	struct MoveTime
	{
		uint16_t position;
		uint16_t time;
	};
	MoveTime moveTimeRead() const;
	
	///
	void moveTimeWaitWrite( int16_t position, uint16_t time=0);
	///
	MoveTime moveTimeWaitRead() const;
	///
	void moveStart();
	///
	void moveStop();
	
	/// Read the input voltage to the servo, in mV
	uint16_t vinRead() const;
	
	/// Read the current servo position in multiple of 0.24 deg (1000 = 240deg)
	uint16_t posRead() const;
	
	/// Set the ID of the servo to <newId>
	///     If the current servo ID is unknown, use broadcast constructor
	/// @arg newId: the new ID to be set.
	void idWrite(uint8_t newId);

private:
	
	/// Message prefix/frame header
	constexpr static uint8_t FrameHeader = 0x55;
	/// Used to pre-fill buffers before real data is set in
	constexpr static uint8_t _pholder = 0;
	
	/// return the lower byte of an uint16_t
	inline static uint8_t getLowByte( const uint16_t in);
	
	/// return the higher byte of an uint16_t
	inline static uint8_t getHighByte( const uint16_t in);
	
	/// Return the checksum for a given message
	inline static uint8_t checksum(const Buffer& buf);

	/// Send a buffer of data to the servo
	inline void sendBuf(const Buffer& buf) const;
	
	/// Get a message from the servo (this function is blocking).
	/// @throw runtime_error if the message does not arrive until timeout (< 1 ms)
	/// Timeout is a busy loop, avoiding long waiting of re-scheduling
	inline const Buffer& getMessage() const;
	
	/// Basic check on a message: 
	///    - If the checksum match
	///    - If the size of the message is the expected (expect at pos 3)
	///    - If the commandId is the expected
	/// Return false in case of error
	inline static bool checkMessage( const Buffer& buf, uint8_t commandId, size_t expectedSize);
	
	/// Set all variable elements in buf (Id, and checksum), and send the request, 
	///     then it read the result, check it validity and return the buffer.
	/// Used internally to reuse common code between all the xxxxREAD commmands
	/// @arg buf: Buffer of the request (id, and checksum are computed internally)
	/// @arg replySize: expected size of the reply (for checks).
	inline const Buffer& genericRead( Buffer& buf, uint8_t replySize ) const;

	// Access to the device
	int fd = -1;
	// Id of the servo
	int id = 1;

	
};




//*********************************************************
//                   IMPLEMENTATION
//*********************************************************

uint8_t HiwonderBusServo::getLowByte( const uint16_t in)
{
	return static_cast<uint8_t>(in);
}

uint8_t HiwonderBusServo::getHighByte( const uint16_t in)
{
	return static_cast<uint8_t>(in>>8);
}

uint8_t HiwonderBusServo::checksum(const Buffer& buf)
{
	uint16_t temp = 0;
	for (size_t i=2; i<buf[3]+2; ++i)
	{
		temp += buf[i];
	}
	temp = ~temp;
	return getLowByte(temp);
}

void HiwonderBusServo::sendBuf(const Buffer& buf) const
{
	for (size_t i=0; i< buf[3]+3; ++i)
	{
		serialPutchar(fd, buf[i]);
	}
}
	
const HiwonderBusServo::Buffer& HiwonderBusServo::getMessage() const
{
	static Buffer res;
	
	constexpr static size_t MaxBusyLoop = 5000;
	
	// To avoid timeout (too long), poll until we get enough bytes
	for(size_t i=0; i<MaxBusyLoop && serialDataAvail(fd)<4; ++i) continue; //noop

	
	if (serialDataAvail(fd)<4)
	{
		res[3]=res[2]=0;
		throw std::runtime_error("Unable to retrieve message header from servo");
		return res;
	}
	
	res[0] = serialGetchar(fd); //frame header 1
	res[1] = serialGetchar(fd); //frame header 2
	res[2] = serialGetchar(fd); //servo id
	res[3] = serialGetchar(fd); //size
	
	for(size_t i=0; i<MaxBusyLoop && serialDataAvail(fd)<res[3]-1; ++i) continue; //noop
	
	if (serialDataAvail(fd)<res[3]-1)
	{
		res[3]=res[2]=0;
		throw std::runtime_error("Unable to retrieve message content from servo");
		return res;
	}
	
	for (size_t i=0; i<res[3]-1; ++i)
	{
		res[i+4] = serialGetchar(fd);
	}
	
	return res;
}
	
	
bool HiwonderBusServo::checkMessage( const Buffer& buf, uint8_t commandId, size_t expectedSize)
{
	if (buf[3] != expectedSize || 
			buf[4] != commandId || 
			buf[expectedSize+2] != checksum(buf))
	{
		return false;
	}
	return true;
}

HiwonderBusServo::HiwonderBusServo(uint8_t id): id(id)
{
	fd = serialOpen("/dev/ttyAMA0", 115200);
	auto setupResult = wiringPiSetup();
	if (0>fd || -1==setupResult)
	{
		throw std::runtime_error("Unable to setup UART device.");
	}
}

HiwonderBusServo::~HiwonderBusServo()
{
	serialClose(fd);
}

const HiwonderBusServo::Buffer& HiwonderBusServo::genericRead( Buffer& buf, uint8_t replySize ) const
{
	buf[2] = id;
	buf[buf[3]+2] = checksum(buf);
	
	serialFlush(fd);
	sendBuf(buf);
	
	// Read result
	const Buffer& res= getMessage();
	
	if (!checkMessage(res, buf[4], replySize))
	{
		throw std::runtime_error("Corrupted message received");
	}
	
	return res;
}

void HiwonderBusServo::moveTimeWrite( int16_t position, uint16_t time)
{
	constexpr static uint8_t MoveTimeWriteId = 1;
	constexpr static uint8_t MoveTimeWriteSize = 7;
	
	static Buffer buf
	{
		FrameHeader, 
		FrameHeader,
		_pholder,
		MoveTimeWriteSize,
		MoveTimeWriteId,
		_pholder,
		_pholder,
		_pholder,
		_pholder,
		_pholder
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

HiwonderBusServo::MoveTime HiwonderBusServo::moveTimeRead() const
{
	constexpr static uint8_t MoveTimeReadId = 2;
	constexpr static uint8_t MoveTimeReadSize = 3;
	constexpr static uint8_t MoveTimeReplySize = 7;
	
	static Buffer buf
	{
		FrameHeader, 
		FrameHeader,
		_pholder,
		MoveTimeReadSize,
		MoveTimeReadId,
		_pholder
	};
	
	const Buffer& resultBuf = genericRead(buf, MoveTimeReplySize);
	
	MoveTime result;
	result.position = resultBuf[5]+(resultBuf[6]<<8);
	result.time = resultBuf[7]+(resultBuf[8]<<8);
	return result;
}

void HiwonderBusServo::moveTimeWaitWrite( int16_t position, uint16_t time)
{
	constexpr static uint8_t MoveTimeWaitWriteId = 7;
	constexpr static uint8_t MoveTimeWaitWriteSize = 7;
	
	static Buffer buf
	{
		FrameHeader, 
		FrameHeader,
		_pholder,
		MoveTimeWaitWriteSize,
		MoveTimeWaitWriteId,
		_pholder,
		_pholder,
		_pholder,
		_pholder,
		_pholder
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

HiwonderBusServo::MoveTime HiwonderBusServo::moveTimeWaitRead() const
{
	constexpr static uint8_t MoveTimeWaitReadId = 8;
	constexpr static uint8_t MoveTimeWaitReadSize = 3;
	constexpr static uint8_t MoveTimeWaitReplySize = 7;
	
	static Buffer buf
	{
		FrameHeader, 
		FrameHeader,
		_pholder,
		MoveTimeWaitReadSize,
		MoveTimeWaitReadId,
		_pholder
	};
	
	const Buffer& resultBuf = genericRead(buf, MoveTimeWaitReplySize);
	
	MoveTime result;
	result.position = resultBuf[5]+(resultBuf[6]<<8);
	result.time = resultBuf[7]+(resultBuf[8]<<8);
	return result;
}

void HiwonderBusServo::moveStart()
{
	constexpr static uint8_t MoveStartId = 11;
	constexpr static uint8_t MoveStartSize = 3;
	
	static Buffer buf
	{
		FrameHeader, 
		FrameHeader,
		_pholder,
		MoveStartSize,
		MoveStartId,
		_pholder
	};
	buf[5] = checksum(buf);
	
	sendBuf(buf);
}

void HiwonderBusServo::moveStop()
{
	constexpr static uint8_t MoveStopId = 12;
	constexpr static uint8_t MoveStopSize = 3;
	
	static Buffer buf
	{
		FrameHeader, 
		FrameHeader,
		_pholder,
		MoveStopSize,
		MoveStopId,
		_pholder
	};
	buf[5] = checksum(buf);
	
	sendBuf(buf);
}
	
uint16_t HiwonderBusServo::vinRead() const
{
	constexpr static uint8_t VInReadId = 27;
	constexpr static uint8_t VInReadSize = 3;
	constexpr static uint8_t VInReplySize = 5;
	
	static Buffer buf
	{
		FrameHeader, 
		FrameHeader,
		_pholder,
		VInReadSize,
		VInReadId,
		_pholder
	};
	
	const Buffer& resultBuf = genericRead(buf, VInReplySize);
	
	return resultBuf[5]+(resultBuf[6]<<8);
}

uint16_t HiwonderBusServo::posRead() const
{
	constexpr static uint8_t posReadId = 28;
	constexpr static uint8_t posReadSize = 3;
	constexpr static uint8_t posReplySize = 5;
	
	static Buffer buf
	{
		FrameHeader, 
		FrameHeader,
		_pholder,
		posReadSize,
		posReadId,
		_pholder
	};
	
	const Buffer& resultBuf = genericRead(buf, posReplySize);
	
	return resultBuf[5]+(resultBuf[6]<<8);
}

void HiwonderBusServo::idWrite(uint8_t newId)
{
	constexpr static uint8_t IdWrite = 13;
	constexpr static uint8_t IdWriteSize = 4;
	
	static Buffer buf
	{
		FrameHeader, 
		FrameHeader,
		_pholder,
		IdWriteSize,
		IdWrite,
		_pholder,
		_pholder
	};
	
	buf[2] = id;
	buf[5] = newId;
	buf[6] = checksum(buf);
	
	sendBuf(buf);
}

}
#endif //HIWONDER_RPI



