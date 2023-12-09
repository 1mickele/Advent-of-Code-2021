#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <ranges>

struct page
{
	std::vector<std::pair<int, int>> pts;

	void fold(char c, int w)
	{
		if(c == 'x') {
			for(auto& [x,y] : pts)
				x -= bool(x - w > 0) * 2 * (x - w);
		} else {
			for(auto& [x,y] : pts)
				y -= bool(y - w > 0) * 2 * (y - w);
		}
	}
};

template <class It, class Fn>
int maxx(It first, It last, Fn&& f) 
{
	auto res{f(*first)};
	while(first != last)
	{
		auto tmp = f(*first);
		if(res < tmp) res = tmp;
		++first;
	}
	return res;
}

int main()
{
	std::fstream f{"input"};
	page p;
	std::vector<std::pair<char, int>> folds;

	do {
		int x,y{};
		f >> x; f.ignore(1); f >> y;
		p.pts.push_back(std::make_pair(x,y));
		f.ignore(1);
	} while(f.peek() != '\n');

	do {
		f.ignore(11); 
		char c{}; 
		int x{};
		f >> c; f.ignore(1); f >> x;
		folds.push_back(std::make_pair(c,x));
		f.ignore(1);
		f.peek();
	} while(!f.eof());

	for(auto [c, w] : folds)
		p.fold(c, w);

	std::sort(p.pts.begin(), p.pts.end(), [](auto l, auto r){
		if(l.first < r.first) return true;
		if(l.first > r.first) return false;
		return l.second < r.second;
	});

	auto e = std::unique(p.pts.begin(), p.pts.end(), [](auto l, auto r){
		return l.first == r.first && l.second == r.second;
	});
	p.pts.erase(e, p.pts.end());

	int mx = maxx(p.pts.begin(), p.pts.end(), [](const auto& p) { return p.first; }) + 1;
	int my = maxx(p.pts.begin(), p.pts.end(), [](const auto& p) { return p.second; }) + 1;
	
	std::cout << mx << ' ' << my << std::endl;
	std::vector<char> v(mx * my, ' ');

	for(const auto& [x, y] : p.pts)
		v[x + mx * y] = '#';

	for(int j{}; j < my; ++j) 
	{
		for(int i{}; i < mx; ++i) 
			std::cout << v[i + mx * j];
		std::cout << std::endl;
	}

	// std::cout << mx << ' ' << my << std::endl;
}
