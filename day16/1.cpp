#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map> 
#include <ranges>
#include <bitset>
#include <iterator>
#include <memory>
#include <cmath>

std::unordered_map<char, std::string> htob = {
	std::make_pair('0', std::string{"0000"}),
	std::make_pair('1', std::string{"0001"}),
	std::make_pair('2', std::string{"0010"}),
	std::make_pair('3', std::string{"0011"}),
	std::make_pair('4', std::string{"0100"}),
	std::make_pair('5', std::string{"0101"}),
	std::make_pair('6', std::string{"0110"}),
	std::make_pair('7', std::string{"0111"}),
	std::make_pair('8', std::string{"1000"}),
	std::make_pair('9', std::string{"1001"}),
	std::make_pair('A', std::string{"1010"}),
	std::make_pair('B', std::string{"1011"}),
	std::make_pair('C', std::string{"1100"}),
	std::make_pair('D', std::string{"1101"}),
	std::make_pair('E', std::string{"1110"}),
	std::make_pair('F', std::string{"1111"})
};

template <class It>
uint64_t parse_int(It& first)
{	
	uint64_t v{};
	char l;
	do {
		l = *(first++);
		for(int i{}; i < 4; ++i) 
			v = (v << 1) + (*(first++) == '1' ? 1 : 0);
	} while(l == '1');
	return v;
}

template <class It>
uint64_t parse_int(It& first, size_t c)
{
	uint64_t v{};
	for(int i{}; i < c; ++i)
		v = (v << 1) + (*(first++) == '1' ? 1 : 0);
	return v;
}

struct packet
{
	template <class It>
	packet(It& it)
	{
		std::bitset<3> parse(
			std::string{it, it + 3});	
		_version = parse.to_ulong();
		it += 3;

		parse = std::bitset<3>(
			std::string(it, it + 3));
		_id = parse.to_ulong();
		it += 3;


		if(_id == 4)
			_literal = parse_int(it);

		else if(*it == '0') {
			uint64_t tl = parse_int(++it, 15);
			while(tl > 0) 
			{
				auto tmp{it};
				_subs.push_back( std::unique_ptr<packet>(new packet{it}) );
				tl -= (it - tmp);
			}

		} else {
			uint64_t tp = parse_int(++it, 11);
			for(int i{}; i < tp; ++i)
				_subs.push_back( std::unique_ptr<packet>(new packet{it}) );
		}
	}

	int version_sum() const
	{
		int t{};
		for(const auto& s : _subs)
			t += s->version_sum();
		return t + _version;
	}

	template <class Fn>
	auto loop(Fn f, int64_t t0) 
	{
		int64_t t{t0};
		for(const auto& s : _subs)
			t = f(t, s);
		return t;
	}

	int64_t value() 
	{
		switch(_id) {
			case 0:
				return loop([](int64_t t, auto& p)
					{ return t + p->value(); }, 0);
			case 1:
				return loop([](int64_t t, auto& p)
					{ return t * p->value(); }, 1);
			case 2:
				return loop([](int64_t t, auto& p)
					{ return std::min(t, p->value()); }, _subs.front()->value());
			case 3:
				return loop([](int64_t t, auto& p)
					{ return std::max(t, p->value()); }, 0);
			case 4:
				return _literal;
			case 5:
				return _subs[0]->value() > _subs[1]->value();
			case 6:
				return _subs[0]->value() < _subs[1]->value();
			case 7:
				return _subs[0]->value() == _subs[1]->value();
		}
		return -1;
	}

	int _version;
	int _id;
	int64_t _literal{};
	std::vector<
		std::unique_ptr<packet> > _subs;
};

int main()
{
	std::ifstream f{"input"};		
	std::vector<char> pstream;

	for(char c; f >> c; ) 
		std::copy(htob[c].begin(), htob[c].end(), 
			std::back_inserter(pstream));

	auto it = pstream.begin();
	packet p(it);
	std::cout << p.version_sum() << std::endl;
	std::cout << p.value() << std::endl;

	return 0;
}
