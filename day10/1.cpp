#include <iostream>
#include <fstream>
#include <stack>
#include <unordered_map>
#include <vector>
#include <list>

std::unordered_map<char, std::pair<char, int>> rtl = {
	std::make_pair(')', std::make_pair('(', 3)),
	std::make_pair(']', std::make_pair('[', 57)),
	std::make_pair('}', std::make_pair('{', 1197)),
	std::make_pair('>', std::make_pair('<', 25137)),
};

std::unordered_map<char, int> ltr = {
	std::make_pair('(', 1),
	std::make_pair('[', 2),
	std::make_pair('{', 3),
	std::make_pair('<', 4)
};

/* 
 * Some Shitty Code..
 */ 

int main()
{
	std::fstream f{"input"};
	std::vector<std::string> inputs;
	std::vector<std::stack<char>> trunc;

	for (std::string inp; std::getline(f, inp); )
		inputs.push_back(std::move(inp));
	
	int score{};
	for(auto it{ inputs.begin() }; it != inputs.end(); ++it) 
	{
		std::stack<char> chk{};
		for (char c : *it)
		{
			if(auto itt = rtl.find(c); itt != rtl.end()) {	
				if (!chk.empty() && chk.top() == (itt->second).first) {
					chk.pop();
				} else {
					score += rtl[c].second;
					chk = std::stack<char>{};
					break;
				}
			} else {
				chk.push(c);
			}
		}
		if (chk.size() > 0) 
			trunc.push_back(std::move(chk));

	}
	// 266301
	std::cout << score << std::endl;

	std::vector<uint64_t> tres;
	for(auto it{ trunc.begin() }; it != trunc.end(); ++it) 
	{	
		uint64_t sc{};
		while(!it->empty()) {
			sc *= 5;
			sc += ltr[it->top()];
			it->pop();
		}
		tres.push_back(sc);
	}

	std::sort(tres.begin(), tres.end());
	std::cout << tres[tres.size() / 2] << std::endl;

	return 0;
}
