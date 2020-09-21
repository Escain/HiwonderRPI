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


// WORK IN PROGRESS

#ifndef ADVANCED_MOTION
#define ADVANCED_MOTION


namespace HiwonderRpi
{

/*	float bellFunc(float min, float max, float pos)
	{
		auto K = (max-min)/2;
		auto A = 6.0f*K / (4.0f * K*K*K);
		auto p = pos-K-min;
		auto res = 1.0f/3.0f*A*(2.0f*K - p)*(K+p)*(K+p)+min;

		return res;
	}

struct Move
{
	int init;
	int end;
	int time;
	int id;
	
	mutable float iP;
	mutable float fP;
	mutable float sDiv;
	mutable float posInc;
	mutable float curJ;
	Move(int init, int end, int time, int id): init(init), end(end), time(time), id(id){}
};

void smoothMove(int fd, const std::vector<Move>& moves)
{
	
	static int msPerDiv = 50;
	for (const auto& m: moves)
	{
		//float t=time;
		m.iP = m.init<=m.end?m.init:m.end;
		m.fP = m.end>=m.init?m.end:m.init;
		m.sDiv = m.time/msPerDiv;
		m.posInc = (m.end-m.init)/m.sDiv;
		m.curJ = 0.0f;
	}
	
	bool cont=true;
	while (cont)
	{
		cont = false;
		for (const auto& m: moves)
		{
			if (m.curJ>=m.sDiv) continue;
			
			float lPos = m.init+m.posInc*m.curJ;
			float fPos = bellFunc(m.iP, m.fP, lPos);
			serialMove(fd, m.id, fPos, m.time/m.sDiv);
			m.curJ+=1.0f;
			cont=true;
		}
		delay(msPerDiv);
	}
}

void demo(int fd)
{
	constexpr int minM=450;
	constexpr int maxM=550;
	constexpr int minM2=330;
	constexpr int maxM2=670;
	constexpr int timeM=1000;
	

	{
		serialMove( fd, 1, minM2, 1000);
		serialMove( fd, 2, maxM2, 1000);
		delay(1000);
	}
	
	delay(1000);
	
	
	for(int i=0; i<100; ++i)
	{
		{   // go right
			std::vector<Move> moves;
			moves.emplace_back(minM2, maxM, timeM, 1);
			moves.emplace_back(maxM2, maxM, timeM, 2);
			smoothMove(fd, moves);
		}
		{ //go front
			std::vector<Move> moves;
			moves.emplace_back(maxM, maxM2, timeM, 1);
			moves.emplace_back(maxM, minM2, timeM, 2);
			smoothMove(fd, moves);
		}
		{ // go left
			std::vector<Move> moves;
			moves.emplace_back(maxM2, minM, timeM, 1);
			moves.emplace_back(minM2, minM, timeM, 2);
			smoothMove(fd, moves);
		}
		{ // go back
			std::vector<Move> moves;
			moves.emplace_back(minM, minM2, timeM, 1);
			moves.emplace_back(minM, maxM2, timeM, 2);
			smoothMove(fd, moves);
		}
		


		{
			std::vector<Move> moves;
			moves.emplace_back(minM2, maxM2, timeM, 1);
			moves.emplace_back(maxM2, minM2, timeM, 2);
			smoothMove(fd, moves);
		}
		{
			std::vector<Move> moves;
			moves.emplace_back(maxM2, minM2, timeM, 1);
			moves.emplace_back(minM2, maxM2, timeM, 2);
			smoothMove(fd, moves);
		}
		{
			std::vector<Move> moves;
			moves.emplace_back(minM2, maxM2, timeM, 1);
			moves.emplace_back(maxM2, minM2, timeM, 2);
			smoothMove(fd, moves);
		}
		{
			std::vector<Move> moves;
			moves.emplace_back(maxM2, minM, timeM, 1);
			moves.emplace_back(minM2, minM, timeM, 2);
			smoothMove(fd, moves);
		}
		
		
		

		{
			std::vector<Move> moves;
			moves.emplace_back(minM, maxM, timeM, 1);
			moves.emplace_back(minM, maxM, timeM, 2);
			smoothMove(fd, moves);
		}
		{
			std::vector<Move> moves;
			moves.emplace_back(maxM, minM, timeM, 1);
			moves.emplace_back(maxM, minM, timeM, 2);
			smoothMove(fd, moves);
		}
		{
			std::vector<Move> moves;
			moves.emplace_back(minM, maxM, timeM, 1);
			moves.emplace_back(minM, maxM, timeM, 2);
			smoothMove(fd, moves);
		}
		{
			std::vector<Move> moves;
			moves.emplace_back(maxM, minM2, timeM, 1);
			moves.emplace_back(maxM, maxM2, timeM, 2);
			smoothMove(fd, moves);
		}
		
		

		
		//digitalWrite(5, LOW);
		//digitalWrite(4, HIGH); 
		//serialPutchar(fd, '\x55');
		//serialPutchar(fd, '\x55');
		//serialPutchar(fd, '\x03');
		//serialPutchar(fd, '\x1b');
		//serialPutchar(fd, '\xe0');
		//serialFlush(fd);
		//if (i%2) tcdrain(fd);
		//digitalWrite(4, LOW);
		//digitalWrite(5, HIGH);
	}
}*/

}
#endif //ADVANCED_MOTION
