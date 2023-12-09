#include <iostream>
#include <fstream>
#include <ranges>
#include <iterator>
#include <vector>
#include <algorithm>
#include <bitset>
#include <numeric>
#include <list>

namespace ranges = std::ranges;
namespace views = std::views;

constexpr uint32_t N = 25;

std::array<std::bitset<N>, 10> winning = {
	std::bitset<N>{"1111100000000000000000000"},
	std::bitset<N>{"0000011111000000000000000"},
	std::bitset<N>{"0000000000111110000000000"},
	std::bitset<N>{"0000000000000001111100000"},
	std::bitset<N>{"0000000000000000000011111"},

	std::bitset<N>{"1000010000100001000010000"},
	std::bitset<N>{"0100001000010000100001000"},
	std::bitset<N>{"0010000100001000010000100"},
	std::bitset<N>{"0001000010000100001000010"},
	std::bitset<N>{"0000100001000010000100001"}
};

// flattened representation of a board
struct board
{
	// the bit one in the set represents a number
	// drawn from the pool
	std::bitset<N> set;	
	
	// flattened representation of a board with 
	// the key being the value and the content
	// its position on the board
	std::unordered_map<int, int> content; 

	board(std::istream& in)
	{
		size_t i{};
		for(int j{}; j < N; ++j) {
			size_t x;
			in >> x;
			content.insert(std::make_pair(x, i++));
		}
		in >> std::ws;
	}

	bool push(int y) 
	{
		if(auto it = content.find(y); it != content.end())
		{
			set[content.extract(it).mapped()] = 1;
			return std::find_if(winning.begin(), winning.end(), 
				[&](const auto& scheme) { 
					return (set & scheme) == scheme; }
					) != winning.end();	
		} 
		return false;
	}

	int sum() 
	{
		auto rng = views::keys(content);
		return std::accumulate(rng.begin(), rng.end(), int{0});
	}
};

int main() {
	std::fstream f{"input"};
	std::vector<int> input;
	std::list<board> cards;
	
	std::copy(std::istream_iterator<int>(f),
		std::istream_iterator<int>(),
		std::back_inserter(input));

	f.ignore();
	f.clear();

	while(!f.eof()) 
		cards.push_back(board{f});

	std::cout << input.size() << ' ' << cards.size() << std::endl;
	/*
	for (auto x : input) {
		for (auto& b : cards) {
			if(b.push(x)) {
				std::cout << x * b.sum() << std::endl;
				return 0;
			}
		}
	}
	*/ 

	for (auto x : input) {
		for (auto it{cards.begin()}; it != cards.end(); ++it) {
			if(it->push(x)) {
				std::cout << x * it->sum() << std::endl;
				it = cards.erase(it);
			}
		}
	}

	return 0;
}
