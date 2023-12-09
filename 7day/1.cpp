#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <ranges>

int main() {
	std::fstream f{"input"};
	std::vector<size_t> hpos;

	for (size_t h; f >> h; f.ignore())
		hpos.push_back(h);

	auto cost = [](size_t x, size_t w) {
		auto t = std::max(x, w) - std::min(x, w);
		return t * (t + 1) / 2;
	};

	auto r = std::ranges::min(
			std::views::iota(size_t{0}) 
			| std::views::take(hpos.size()) 
			| std::views::transform([&](size_t w) {
				return std::accumulate(hpos.begin(), hpos.end(), size_t{0}, 
					[=](size_t p, size_t x) { return p + cost(x, w); });
			})
		);
	std::cout << r << std::endl;

	/*
	int min{2147483647};
	for (size_t w{0}; w < hpos.size(); ++w) {
		auto now = std::accumulate(hpos.begin(), hpos.end(), size_t{0}, 
			[=](size_t p, size_t x) { return p + cost(x, w); });
		if (now < min) min = now;
	}
	std::cout << min << std::endl;
	*/

	return 0;
}
