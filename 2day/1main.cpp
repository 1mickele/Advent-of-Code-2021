#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <functional>
#include <iterator>

struct Move
{
	std::string m;
	int s;
};

struct Position
{
	int x;
	int y;
	int aim;
};

std::istream& operator>> (std::istream& in, Move& mv)
{
	std::string m;
	int s;
	in >> mv.m;
	in >> mv.s;
	return in;
}

std::map< std::string, std::function<void(Position&, int)> > 
	action = { 
		{ "forward", [](Position& pos, int s) 
			{ pos.x += s; pos.y += pos.aim * s; } },
		{ "up", [](Position& pos, int s) { pos.aim -= s; } },
		{ "down", [] (Position& pos, int s) { pos.aim += s; } }
	};

int main()
{
	std::ifstream f{"input.txt"};
	Position pos{};
	
	// range over [std::istream_iterator<Move>(f), 
	// 		std::istream_iterator<Move>() )
	// would do.
	for(Move mv{}; f >> mv; ) {
		action[mv.m](pos, mv.s);
	}
	std::cout << pos.x * pos.y << std::endl;
	
	return 0;
}
