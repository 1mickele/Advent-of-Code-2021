#include <iostream>
#include <fstream>
#include <list>
#include <unordered_map>
#include <algorithm>

template<class T>
void print(const T& tt)
{
	for(const auto& [x, y] : tt)
		std::cout << x << ' ' << y << std::endl;
}

template <class M, class K, class T>
void try_update(M& m, const K& k, const T& u)
{
	auto it = m.find(k);
	if (it != m.end()) {
		it->second += u;	
	} else {
		m.emplace(k, uint64_t{1});
	}
}

int main()
{
	std::fstream f{"input"};
	std::list<char> poly;

	std::unordered_map<std::string, char> rules;
	std::unordered_map<std::string, uint64_t> count;
	std::unordered_map<char, uint64_t> ccount;

	do {
		char c = f.get();
		poly.push_back(c);
		try_update(ccount, c, 1);	
	} while(f.peek() != '\n');
	f.ignore(1);

	while(!f.eof()) {
		std::string m;
		f >> m;
		f.ignore(4);
		count.try_emplace(std::string{m}, 0);
		rules.emplace(m, f.get());
		f.ignore(1);
		f.peek();
	}

	for(auto it1{poly.begin()}, it2{++poly.begin()};
		it2 != poly.end(); ++it2, ++it1)
	{
		count.at(std::string{*it1, *it2}) += 1;
	}

	for(int i{}; i < 40; ++i) {
		auto cold{count};
		for(auto& [p, u] : cold)
		{
			if (u > 0) {
				const char q = rules.find(p)->second;
				count.at(std::string{p[0], q}) += u; 						
				count.at(std::string{q, p[1]}) += u; 						
				count.at(std::string{p}) -= u; 						

				try_update(ccount, q, u);
			} 
		}
	}
	print(ccount);
	
	size_t b = std::max_element(ccount.begin(), ccount.end(),
		[](const auto& p, const auto& q){ return p.second < q.second; }) -> second;
	size_t a = std::min_element(ccount.begin(), ccount.end(),
		[](const auto& p, const auto& q){ return p.second < q.second; }) -> second;
	std::cout << "Result: " << (b-a) << std::endl;
}
