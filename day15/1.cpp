#include <iostream>
#include <list>
#include <fstream>
#include <cmath>
#include <algorithm>

constexpr int m{100000};

template <class D>
void print(const D& dmap, size_t ln)
{
	for(int i{}; i < ln; ++i)
	{
		for(int j{}; j < ln; ++j)
			std::cout << (char)((dmap.at(i+ln*j) != m) ? '0' + dmap.at(i+ln*j) : '#');
		std::cout << std::endl;
	}
	std::cin.get();
}

template <class D>
std::vector<int> gen_map(const D& dmap, size_t ln)
{
	std::vector<int> ddmap((ln * 5) * (ln * 5), 0);
	for(int i{}; i < 5; ++i)
		for(int j{}; j < 5; ++j)
			for(int k{}; k < dmap.size(); ++k) 
			{
				int x = k % ln;
				int y = size_t{k / ln};
				ddmap.at(x + ln*i + (y + ln * j) * (5*ln)) 
					= (dmap.at(x+ln*y) + i + j - 1) % 9 + 1;
			}

	return ddmap;
}

template <class D>
auto safe_access(const D& dmap, size_t ln, 
	int x, int y)
{
	if(x < 0 || x >= ln) return m;
	if(y < 0 || y >= size_t{dmap.size() / ln}) return m;
	return dmap.at(x + ln * y);
}

template <class D, class Fn1, class Fn2>
auto compute(D& dmap, size_t ln, Fn1 f, Fn2 g)
{
	std::list<std::pair<int, int>> acc{std::make_pair(0, 0)};
	for(size_t c{1}; c < m; ++c)
	{
		for(auto it{acc.begin()}; it != acc.end(); ++it)
		{
			auto [w, d] = *it;
			auto [x, y] = g(w);
			for (auto [dx, dy] : { std::make_pair(-1,0), std::make_pair(1,0),
								   std::make_pair(0,-1), std::make_pair(0,1)})
			{
				if(safe_access(dmap, ln, x+dx, y+dy) + d == c) 
				{
					acc.push_back(std::make_pair( f(x+dx, y+dy), c ));
					dmap.at( f(x+dx, y+dy) ) = m;
					if(acc.back().first == f(ln-1,ln-1)) 
						return c;
				}
				
			}

			if(d + 9 < c) it = acc.erase(it);
		}
		print(dmap, ln);
	}

	return size_t{0};
}

int main()
{
	std::vector<int> dmap;
	std::fstream f{"input2"};

	for(uint8_t e; f >> e; )
		dmap.push_back(e - '0');
	size_t ln = static_cast<int>(std::sqrt(dmap.size()));

	dmap = gen_map(dmap, ln);
	ln *= 5;

	auto w = compute(dmap, ln, 
		[=](auto x, auto y) { return x + y * ln; },
		[=](auto w) { return std::make_pair(w % ln, w / ln); } );
	std::cout << w << std::endl;
}

