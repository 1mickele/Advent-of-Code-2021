#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <limits>

void part1() 
{
	std::ifstream f{"1input.txt"};	
	constexpr auto max = std::numeric_limits<int>::max();

	std::cout << std::count_if(std::istream_iterator<int>(f), 
		std::istream_iterator<int>(), [p = max] (int x) mutable 
			{ return std::exchange(p, x) < x; })
			<< std::endl;
}

template <class T>
auto& nexchange(T& x, T& y) {
	std::swap(x, y);
	return y;
}

template <class T, class... Args>
auto& nexchange(T& x, T& y, Args&... args) {
	std::swap(x, y);
	return nexchange(y, args...);
}

void part2() 
{
	std::ifstream f{"1input.txt"};	

	int w0, w1, w2;
	f >> w0 >> w1 >> w2;

	std::cout << std::count_if(std::istream_iterator<int>(f),
		std::istream_iterator<int>(), [&](int x) { 
			return nexchange(w0, w1, w2, x) < w2;
		}) << std::endl; 
}

int main() 
{
	part2();
	return 0;
}
