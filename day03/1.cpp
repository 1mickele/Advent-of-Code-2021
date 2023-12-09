#include <iostream>
#include <vector>
#include <fstream>
#include <iterator>
#include <algorithm>

int btoi2(const std::string& s) 
{
	int g{}, b{1};
	for(int i{(int)s.size() - 1}; i >= 0; --i) {
		g += (s[i] == '1' ? 1 : 0 ) * b; 
		b *= 2;
	}
	return g;
}	

struct lexico
{
	inline bool operator() 
		(const std::string& l, const std::string& r)
	{
		for(std::size_t i{}; i < r.size(); ++i) {
			if( static_cast<int>(r[i]) > static_cast<int>(l[i]) )
				return true;
			else if ( static_cast<int>(r[i]) < static_cast<int>(l[i]) )
				return false;
		}
		return true;
	}
};

/* se count(..) == true, allora sono di più gli '1' 
 * altrimenti se count(..) == false sono di più gli '0'
 */ 
template <class It>
int count2(It first, It end, std::size_t i, char c)
{
	int cnt{};
	while (first != end) {
		cnt += (((*first)[i] == c) ? 1 : -1);
		++first;
	}
	return cnt;
}

template <class It>
It mid(It first, It end, std::size_t i)
{
	while (first != end && (*first)[i] == '0') 
		{ ++first; }
	return first;
}

template <class It, class Fn>
int process(It first, It last, Fn f)
{

	std::string r;
	for (std::size_t i{}; i < (*first).size(); ++i) {
		auto piv = mid(first, last, i);
		// siccome l'insieme è già ordinato, non serve
		// andare a contare quale tra le occorrenze di 1 o 0
		// è più numerosa: std::distance(first, mid) è il count di 0
		// mentre std::distance(mid, end) è il count di 1.
		if( f(first, last, i) ) 
			first = piv;
		else 
			last = piv;

		if (last - first <= 1) break;
	}
	std::cout << *first << std::endl;
	r = *first;
	return btoi2(r);
}

int part2(std::vector<std::string> vs) 
{
	std::sort(vs.begin(), vs.end(), lexico{});	
	std::copy(vs.begin(), vs.end(), 
		std::ostream_iterator<std::string>(std::cout, "\n"));

	return process(vs.begin(), vs.end(), 
		[](auto b, auto e, std::size_t i) { 
			return count2(b, e, i, '1') >= 0;
		}) *
		process(vs.begin(), vs.end(), 
		[](auto b, auto e, std::size_t i) {
			return count2(b, e, i, '0') > 0;
		});
}

int main()
{
	std::fstream f{"input.txt"};

	std::vector<std::string> v{
		std::istream_iterator<std::string>(f),
		std::istream_iterator<std::string>()
	};

	std::cout << part2(v) << std::endl;
	return 0;
}

/*
int part1(const std::vector<std::string>& vs)
{
	std::string gamma;
	for(std::size_t i{}; i < vs.front().size(); ++i) {
		int cnt{};
		for(const auto& s : vs) 
			cnt += (s[i] == '1') ? 1 : -1;

		gamma.push_back((cnt >= 0) ? '1' : '0');
	}

	auto [e,g] = btoi2(gamma);
	return e * g;
}
*/ 
