#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <ranges>

void print(const std::vector<int>& d, std::size_t ln)
{
	for(size_t i{}; i < d.size() / ln; ++i)
	{
		for(size_t j{}; j < ln; ++j)
			std::cout << d[i * ln + j];
		std::cout << std::endl;
	}
}

int main()
{
	std::fstream f{"input"};
	std::vector<int> octodata;

	std::size_t ln;
	while(!f.eof()) 
	{
		ln = std::size_t{0};
		for(char c; (f.peek() != '\n') && f >> c; ++ln) 
			octodata.push_back(c - '0');
		f.ignore(1);	
		f.peek();
	}

	auto lts = [=](auto x) { return std::make_pair(x % ln, x / ln); };
	auto stl = [=](auto x, auto y) { return x + ln*y; };
	auto in = [&](int x, int y) {
		if (x < 0 || x >= ln) return false;		
		if (y < 0 || y >= octodata.size() / ln) return false;
		return true;
	};

	std::vector< std::pair<int, int> > nghs = {
		std::make_pair(-1, -1), std::make_pair(0, -1), std::make_pair(1, -1),
		std::make_pair(-1,  0), 					   std::make_pair(1,  0),
		std::make_pair(-1,  1), std::make_pair(0,  1), std::make_pair(1,  1)
	};
	
	size_t sol{};
	for(size_t i{}; i < 400; ++i)
	{
		std::for_each(octodata.begin(), octodata.end(), [](auto& x) { return ++x; });

		auto gen = std::ranges::iota_view{size_t{0}, octodata.size()} 
			| std::views::transform(lts);
		std::queue<std::pair<int, int>> grid; // (gen.begin(), gen.end());
		for(std::pair<int, int> a : gen) grid.push(a);

		while(!grid.empty())	
		{	
			auto [x,y] = grid.front();
			if(octodata.at(stl(x, y)) > 9) // it will flash 
			{
				octodata.at(stl(x, y)) = 0;		
				++sol;
				for (auto [xn, yn] : nghs) { 
					if (in(x + xn, y + yn) && 
						octodata.at(stl(x + xn, y + yn)) > 0) 
					{
						++octodata.at(stl(x + xn, y + yn));
						grid.push(std::make_pair(x + xn, y + yn));
					}
				}
			}
			grid.pop();
		}
		print(octodata, ln);
		std::cout << std::endl;
		std::cin.get();
		// std::cout << std::cin.get() << std::endl;
		if (std::all_of(octodata.begin(), octodata.end(),
			[](auto x){ return x == 0; })) 
		{
			std::cout << i + 1 << std::endl;
		}
	}
	std::cout << sol << std::endl;
}
