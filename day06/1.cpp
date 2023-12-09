#include <iostream>
#include <vector>
#include <fstream>
#include <ranges>
#include <algorithm>
#include <numeric>

namespace ranges = std::ranges;

template <class T>
void print(const T& v)
{
	for(auto &x : v)
		std::cout << x << ' ';
}

int main()
{
	std::fstream f{"input.txt"};
	std::array<uint64_t, 9> pop{};

	for(uint64_t p{}; f >> p; f.ignore(1))
		pop[p] += 1;

	for(size_t d{}; d < 256; ++d) {
		uint64_t f = pop.front();	
		std::rotate(pop.begin(), pop.begin() + 1, pop.end());
		pop[6] += f;
	}
	std::cout << std::accumulate(pop.begin(), pop.end(), uint64_t{0}) 
		<< std::endl;

	return 0;
}
