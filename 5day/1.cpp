#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <memory>
#include <algorithm>

int sgn(int x) {
	return (x > 0) - (x < 0);
}

struct line
{
	int x1, y1, x2, y2;

	bool is_trivial() { return (x1 == x2) || (y1 == y2); }

	// to be refactored
	int length() { 
		return 1 + std::max(std::abs(x1 - x2), std::abs(y1 - y2));
	}

	auto generate() {
		return [this](int d) {
			int xf = sgn(x2 - x1) * d + x1;
			int yf = sgn(y2 - y1) * d + y1;
			return std::make_pair(xf, yf);
		};
	}
};

line parse_line(std::istream& is) {
	line res;
	char bin;
	is >> res.x1 >> bin >> res.y1;
	is.ignore(3) >> res.x2 >> bin >> res.y2;
	return res;
}

int main() 
{
	std::fstream f{"input.txt"};
	std::vector<line> lines;
	while(!f.eof()) {
		lines.push_back(parse_line(f));
	}

	constexpr int N = 1000 * 1000;
	auto a = std::make_unique<int[]>(N);
	for (line& l : lines) { 
		for (int i{}; i < l.length(); ++i) {
			auto [x,y] = l.generate()(i);
			a[1000 * y + x] += 1;
		}
	}

	std::cout << std::count_if(&a[0], &a[N], 
		[](auto x) { return x >= 2; }) << std::endl;
}
