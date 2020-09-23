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

#include <string>
#include <unistd.h>

#include "HiwonderBusServo.hpp"
#include "UnitTest.hpp"

constexpr static uint8_t id=1;

UNIT_TEST(test_have_root_privileges)
{
	ASSERT_EQ( getuid(), 0 );
}

UNIT_TEST(message_ensure_servo_id_is_1)
{
	std::cerr << "Please, ensure servo is connected with ID="<< 
	    static_cast<int>(id) << std::endl;
}

UNIT_TEST(test_can_create_servo_object)
{
	bool throwed=false;
	try
	{
		HiwonderRpi::HiwonderBusServo(id);
	}catch(...)
	{
		throwed=true;
	}
	ASSERT(!throwed);
}

UNIT_TEST(moveTimeWrite_and_posRead_matches)
{
	constexpr uint16_t devPos = 20;
	
	HiwonderRpi::HiwonderBusServo servo(id);
	
	servo.moveTimeWrite(200);
	delay(3000);
	auto res = servo.posRead();
	ASSERT(abs(res-200)<devPos);
	
	servo.moveTimeWrite(800);
	delay(3000);
	res = servo.posRead();
	ASSERT(abs(res-800)<devPos);
}

UNIT_TEST(movetimewrite_and_movetimeread_matches)
{
	HiwonderRpi::HiwonderBusServo servo(id);
	
	servo.moveTimeWrite(200);
	auto res = servo.moveTimeRead();
	ASSERT_EQ(res.position, 200);
	ASSERT_EQ(res.time, 0);
	
	servo.moveTimeWrite(800, 300);
	res = servo.moveTimeRead();
	ASSERT_EQ(res.position, 800);
	ASSERT_EQ(res.time, 300);
}

UNIT_TEST(vinRead_return_value_close_standard_voltages)
{
	// Agree, this is not very precise check, but anything better idea?
	constexpr uint16_t MaxDev = 500;
	HiwonderRpi::HiwonderBusServo servo(id);
	
	auto res = servo.vinRead();
	ASSERT(abs(5000-res)<MaxDev || abs(9000-res)<MaxDev || abs(12000-res)<MaxDev);
}


// TODO: Those operations don't work yet..
/*UNIT_TEST(moveTimeWaitWrite_and_moveTimeWaitRead_matches)
{
	HiwonderRpi::HiwonderBusServo servo(id);
	
	servo.moveTimeWaitWrite(423);
	auto res = servo.moveTimeWaitRead();
	ASSERT_EQ(res.position, 423);
	ASSERT_EQ(res.time, 0);
	
	servo.moveTimeWrite(865, 567);
	res = servo.moveTimeRead();
	ASSERT_EQ(res.position, 865);
	ASSERT_EQ(res.time, 567);
}

UNIT_TEST(waiting_move_start_and_stop)
{
	constexpr uint16_t devPos = 20;
	HiwonderRpi::HiwonderBusServo servo(id);
	
	// set initial position
	servo.moveTimeWrite(200);
	delay(3000);
	ASSERT(abs(servo.posRead()-200)<devPos);
	// send waiting move
	servo.moveTimeWaitWrite(500,1000);
	delay(100);
	ASSERT(abs(servo.posRead()-200)<devPos); // Did not started moving
	// send start
	servo.moveStart();
	delay(500);
	ASSERT(abs(servo.posRead()-350)<devPos); // Servo moved to half travel
	servo.moveStop();
	delay(250);
	ASSERT(abs(servo.posRead()-350)<devPos); // Servo stopped moving

}

// NOT WORKING?
UNIT_TEST(stop_command_apply)
{
	constexpr uint16_t devPos = 20;
	HiwonderRpi::HiwonderBusServo servo(id);
	
	// set initial position
	servo.moveTimeWrite(0);
	delay(3000);
	ASSERT(abs(servo.posRead()-0)<devPos);
	
	// send long move
	servo.moveTimeWrite(1000, 2000);
	delay(1000);
	std::cout << (int)servo.posRead() << std::endl;
	ASSERT(abs(servo.posRead()-500)<4*devPos); // Did started moving
	
	servo.moveStop();
	delay(500);
	ASSERT(abs(servo.posRead()-500)<4*devPos); // Servo stopped moving

}*/


UNIT_TEST(idWrite_and_idRead)
{
	{
		HiwonderRpi::HiwonderBusServo servo(id);
		
		auto res = servo.idRead();
		ASSERT_EQ((int)id, (int)res);
		
		servo.idWrite(42);
		delay(100);
	}
	{
		HiwonderRpi::HiwonderBusServo servo(42);
		auto res = servo.idRead();
		ASSERT_EQ((int)42, (int)res);
		
		servo.idWrite(id);
		delay(100);
	}
	{
		HiwonderRpi::HiwonderBusServo servo(id);
		auto res = servo.idRead();
		ASSERT_EQ((int)id, (int)res);
	}
}

UNIT_TEST(angle_offset_adjust)
{
	HiwonderRpi::HiwonderBusServo servo(id);
	
	// ensure zero at begining
	servo.angleOffsetAdjust(0);
	
	auto res = servo.angleOffsetRead();
	ASSERT_EQ((int)res, 0);
	
	servo.angleOffsetAdjust(100);
	res = servo.angleOffsetRead();
	ASSERT_EQ((int)res, 100);
	delay(500);
	
	servo.angleOffsetAdjust(-100);
	res = servo.angleOffsetRead();
	ASSERT_EQ((int)res, -100);
	delay(500);
	
	servo.angleOffsetAdjust(0);
}

UNIT_TEST(angleLimitWrite_effectively_limit_angle)
{
	HiwonderRpi::HiwonderBusServo servo(id);
	
	constexpr uint16_t devPos = 20;
	
	servo.angleLimitWrite(200,500);
	
	servo.moveTimeWrite(0);
	delay(3000);
	ASSERT(abs(servo.posRead()-200)<devPos);
	
	servo.moveTimeWrite(1000);
	delay(3000);
	ASSERT(abs(servo.posRead()-500)<devPos);
	
	servo.angleLimitWrite(0,1000);
}

UNIT_TEST(angleLimitRead_return_same_angleLimitWrite)
{
	HiwonderRpi::HiwonderBusServo servo(id);
	
	servo.angleLimitWrite(200,500);
	
	auto limits = servo.angleLimitRead();
	ASSERT_EQ(limits.minLimit, 200);
	ASSERT_EQ(limits.maxLimit, 500);
	
	servo.angleLimitWrite(0,1000);
	
	limits = servo.angleLimitRead();
	ASSERT_EQ(limits.minLimit, 0);
	ASSERT_EQ(limits.maxLimit, 1000);
}

UNIT_TEST(angleLimitWrite_out_of_limits_is_adjusted)
{
	HiwonderRpi::HiwonderBusServo servo(id);
	
	servo.angleLimitWrite(-200,2000);
	
	auto limits = servo.angleLimitRead();
	ASSERT_EQ(limits.minLimit, 0);
	ASSERT_EQ(limits.maxLimit, 1000);
	
	servo.angleLimitWrite(1500,500);
	
	limits = servo.angleLimitRead();
	ASSERT_EQ(limits.minLimit, 999);
	ASSERT_EQ(limits.maxLimit, 1000);
	
	servo.angleLimitWrite(500,300);
	
	limits = servo.angleLimitRead();
	ASSERT_EQ(limits.minLimit, 500);
	ASSERT_EQ(limits.maxLimit, 501);
	
	servo.angleLimitWrite(0,1000);
	
	limits = servo.angleLimitRead();
	ASSERT_EQ(limits.minLimit, 0);
	ASSERT_EQ(limits.maxLimit, 1000);
}

UNIT_TEST(vinLimit_read_and_write_match)
{
	HiwonderRpi::HiwonderBusServo servo(id);
	
	servo.vinLimitWrite(5000,6000);
	
	auto limits = servo.vinLimitRead();
	ASSERT_EQ(limits.minLimit, 5000);
	ASSERT_EQ(limits.maxLimit, 6000);
	
	servo.vinLimitWrite(7000,11000);
	
	limits = servo.vinLimitRead();
	ASSERT_EQ(limits.minLimit, 7000);
	ASSERT_EQ(limits.maxLimit, 11000);
}

UNIT_TEST(vinLimitWrite_out_of_limits_is_adjusted)
{
	HiwonderRpi::HiwonderBusServo servo(id);
	
	servo.vinLimitWrite(3000,13000);
	
	auto limits = servo.vinLimitRead();
	ASSERT_EQ(limits.minLimit, 4500);
	ASSERT_EQ(limits.maxLimit, 12000);
	
	servo.vinLimitWrite(13000,7000);
	
	limits = servo.vinLimitRead();
	ASSERT_EQ(limits.minLimit, 11999);
	ASSERT_EQ(limits.maxLimit, 12000);
	
	servo.vinLimitWrite(7000,5000);
	
	limits = servo.vinLimitRead();
	ASSERT_EQ(limits.minLimit, 7000);
	ASSERT_EQ(limits.maxLimit, 7001);
	
	servo.vinLimitWrite(4500,12000);
	
	limits = servo.vinLimitRead();
	ASSERT_EQ(limits.minLimit, 4500);
	ASSERT_EQ(limits.maxLimit, 12000);
}

UNIT_TEST(tempLimit_read_and_write_match)
{
	HiwonderRpi::HiwonderBusServo servo(id);
	
	servo.tempMaxLimitWrite(70);
	ASSERT_EQ((int)servo.tempMaxLimitRead(), 70);
	
	servo.tempMaxLimitWrite(90);
	ASSERT_EQ((int)servo.tempMaxLimitRead(), 90);
}

UNIT_TEST(tempMaxLimitWrite_out_of_limits_is_adjusted)
{
	HiwonderRpi::HiwonderBusServo servo(id);
	
	servo.tempMaxLimitWrite(20);
	ASSERT_EQ((int)servo.tempMaxLimitRead(), 50);
	
	servo.tempMaxLimitWrite(120);
	ASSERT_EQ((int)servo.tempMaxLimitRead(), 100);
	
	servo.tempMaxLimitWrite();
	ASSERT_EQ((int)servo.tempMaxLimitRead(), 85);
}
