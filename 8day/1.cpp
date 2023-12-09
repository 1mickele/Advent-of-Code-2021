#include <iostream>
#include <fstream>
#include <vector>
#include <ranges>

using entry = std::vector<std::string>;
using entryln = std::pair<entry, entry>;

int main()
{
	std::fstream f{"input"};
	std::vector< entryln > data; 

	/* It is this long because the cpp sucks at managing
	 * istream in a flexible way.
	 */ 
	while(!f.eof()) {
		entry tmp{};
		for(std::string s{}; f >> s && s[0] != '|'; )
			tmp.push_back(std::move(s));
		
		entry tmp2{};
		for(std::string s; (f.peek() != '\n') && f >> s; )
			tmp2.push_back(std::move(s));
		f.ignore(1);

		std::cout << tmp.size() << ' ' << tmp2.size() << std::endl;

		data.push_back( std::make_pair(
			std::move(tmp), std::move(tmp2)) );

		// forces a read to end the loop
		f.peek();
	}

	auto match = [](auto& str) { return str.size() == 2 
										|| str.size() == 3 
										|| str.size() == 4 
										|| str.size() == 7; };

	std::size_t cnt{};
	for(const auto &x : data)
		for(const auto& s : x.second)
			if (match(s)) ++cnt;
	std::cout << cnt << std::endl;

	return 0;
}
