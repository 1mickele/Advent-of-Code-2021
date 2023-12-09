#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <queue>
#include <ranges>
#include <numeric>

int main()
{
	std::fstream f{"input"};
	std::vector<int> heights;

	size_t ln{};
	auto update = [b = true, &ln] (std::istream& f) mutable {
		if(b) { 
			b = (f.peek() != '\n');
			++ln;
		}
	};

	for(int8_t h; f >> h; update(f)) {
		heights.push_back(h - '0');
	}

	auto access = [&] (auto x, auto y) {
		if (x < 0 || x >= ln) return int{9};
		if (y < 0 || y >= (heights.size() / ln)) return int{9};
		return heights[x + y * ln];
	};

	auto map = [=] (auto n) {
		return std::make_pair<int, int>(n % ln,
			static_cast<int>(n / ln));
	};

	auto at = [=] (auto x, auto y) {
		return x + y * ln;
	};
	
	auto basin = [&] (auto x, auto y) {
		size_t sz{};
		std::queue< std::pair<int, int> > next;
		next.push(std::make_pair(x, y));
		while (!next.empty()) {
			auto [x, y] = next.front();
			auto th = heights[at(x,y)];
			for( auto [xn, yn] : { std::make_pair(x+1,y), std::make_pair(x-1,y),
							   	   std::make_pair(x,y+1), std::make_pair(x,y-1) }) 
				if (access(xn, yn) != 9) {
					next.push(std::make_pair(xn, yn));
					heights[at(xn,yn)] = 9;
				}
			heights[at(x,y)] = 9;
			next.pop();
			++sz;
		}
		return sz;
	};

	std::vector<int> basin_sizes;
	for(size_t n{}; n < heights.size(); ++n) {
		if (auto [x,y] = map(n); std::min(
				{ access(x+1,y), access(x-1,y), 
				  access(x,y+1), access(x,y-1) }) > heights[n])
			basin_sizes.push_back(basin(x,y));
	}

	std::ranges::sort(basin_sizes);

	std::cout << std::accumulate(basin_sizes.rbegin(), basin_sizes.rbegin() + 3, 1,
		[](auto x, auto y) { return x * y; }) << std::endl;

	return 0;
}
