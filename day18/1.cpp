#include <iostream>
#include <cmath>
#include <variant>
#include <utility>
#include <memory>
#include <vector>
#include <array>
#include <fstream>

enum class where {
	left = 0,
	right = 1
};


template <class Ty>
struct node
{
	using Pnode = std::unique_ptr<node<Ty>>;
	using leaf = std::variant<std::monostate, Ty, Pnode>;

public:
	bool is_literal(where w) const { 
		return _data[static_cast<int>(w)].index() == 1;
	}

	bool is_empty(where w) const {
		return _data[static_cast<int>(w)].index() == 0;
	}

	bool is_node(where w) const {
		return _data[static_cast<int>(w)].index() == 2;
	}

	leaf& access(where w) {
		return _data[static_cast<int>(w)];
	}

	auto& to_node(where w) {
		return *std::get<Pnode>(_data[static_cast<int>(w)]);
	}

	Ty& to_literal(where w) {
		return std::get<Ty>(_data[static_cast<int>(w)]);
	}

public:
	std::array<leaf, 2> _data;
};

std::unique_ptr<node<int>> copy_node(const auto& oth)
{
	std::unique_ptr<node<int>> t{new node<int>{}};
	if(oth->is_node(where::left)) {
		t->access(where::left) = std::move(copy_node(&oth->to_node(where::left)));
	} else {
		t->access(where::left) = oth->to_literal(where::left);
	}

	if(oth->is_node(where::right)) {
		t->access(where::right) = std::move(copy_node(&oth->to_node(where::right)));
	} else {
		t->access(where::right) = oth->to_literal(where::right);
	}

	return t;
}

template <class Ty>
auto make_node(Ty l, Ty r)
{
	return std::unique_ptr<node<Ty>>{
		new node<Ty>{{l, r}} };
}

template <class Ty>
auto make_node(where w, Ty x)
{
	std::unique_ptr<node<Ty>> ret {
		new node<Ty>{}};
	ret->access(w) = x;
	return ret;
}

template <class Ty>
struct iterator
{
	iterator(node<Ty>& root, where w) 
	{
		node<Ty>* p = &root;
		do {
			path.push_back(p);
			p = p->is_node(w) ?
				&p->to_node(w) : nullptr;
		} while(p != nullptr);
		wt = w;
	}

	iterator(std::vector<node<Ty>*> p, where w)
		: path{p}, wt{w}
	{ }

	Ty& operator*() { return path.back()->to_literal(wt); }

	iterator& operator++()
	{
		wt = static_cast<where>((static_cast<int>(wt) + 1) % 2);
		if(wt == where::right) {
			node<Ty>* p = path.back();
			if(p->is_node(where::right)) {
				p = &p->to_node(where::right);
				path.push_back(p);
				while(p->is_node(where::left)) {
					p = &p->to_node(where::left);
					path.push_back(p);	
				}
				wt = where::left;
			}
		} else { // wt == where::left
			node<Ty>* q = path.back();
			path.pop_back();
			while(path.size() > 0 && 
				path.back()->is_node(where::right) &&
				&(path.back()->to_node(where::right)) == q)
			{
				q = path.back();
				path.pop_back();
			}
			q = path.back();

			if(path.size() == 0) {
				wt = where::left;
				return *this;
			}
			
			if (!q->is_node(where::right)) {
				wt = where::right;
				return *this;
			}
			
			q = &(q->to_node(where::right));
			path.push_back(q);

			while(q->is_node(where::left)) {
				q = &q->to_node(where::left);
				path.push_back(q);
			}
		}
		return *this;
	}

	/*
	iterator& operator--()
	{
		if(wt = where::left) {
			auto q = path.back();
			path.pop_back();
			while(true)
			{
				if(path.size() == 0) {
					wt = where::right;
					return *this;
				}

				if (path.back()->is_node(where::left)) 
			}
		}
	}
	*/

	iterator& operator--()
	{
		if(wt == where::left) {
			auto q = path.back();		
			path.pop_back();
			while(path.size() > 0 && 
				path.back()->is_node(where::left) &&
				&path.back()->to_node(where::left) == q )
			{
				q = path.back();
				path.pop_back();
			}
			if(path.size() == 0) {
				wt = where::right;
				return *this;
			}

			if(!path.back()->is_node(where::left)) {
				wt = where::left;
				return *this;
			}
			
			q = &path.back()->to_node(where::left);
			path.push_back(q);

			while(q->is_node(where::right)) {
				q = &q->to_node(where::right);
				path.push_back(q);
			}

			wt = where::right;
			return *this;
		} else {
			auto p = path.back();
			if(p->is_node(where::left)) {
				p = &p->to_node(where::left);	
				path.push_back(p);
				while(p->is_node(where::right)) {
					p = &p->to_node(where::right);	
					path.push_back(p);
				}
				wt = where::right;
			} else {
				wt = where::left;
			}
			return *this;
		}
	}

	int depth() const {
		return path.size() - 1;
	}

	bool operator!= (const iterator& oth) {
		return path.size() != oth.path.size() || 
			!std::equal(path.begin(), path.end(), oth.path.begin()) || 
				wt != oth.wt;
	}

	std::vector<node<Ty>*> path;
	where wt{where::left};
};

struct snailfish
{
	auto& append(where w, std::unique_ptr<node<int>>&& sub) 
	{
		root->access(w) = std::move(sub);	
		return root; 
	}

public:
	std::unique_ptr<node<int>> root;
};

using vnode = std::variant< std::monostate,
	int, std::unique_ptr<node<int>> >;

int c{};
std::unique_ptr<node<int>> parse(std::fstream& f)
{
	vnode a, b;

	f.ignore(1);
	if(f.peek() == '[') {
		a = parse(f);
	} else {
		a = f.get() - '0';	
	}

	f.ignore(1);

	if(f.peek() == '[') {
		b = parse(f);
	} else {
		b = f.get() - '0';	
	}
	f.ignore(1);

	std::unique_ptr<node<int>> t(new node<int>);	
	t->_data[0] = std::move(a);
	t->_data[1] = std::move(b);
	return t;
}

bool explode(auto& term)
{
	iterator<int> left_bound{{}, where::right};
	iterator<int> right_bound{{}, where::left};
	iterator beg{*term, where::left};

	for(; beg != right_bound; ++beg) 
	{
		if(beg.depth() >= 4) 
		{
			auto it{beg};
			if(--it != left_bound) (*it) += *beg;
			it = ++beg;
			if(++it != right_bound) (*it) += *beg;

			auto* q = beg.path.back();
			beg.path.pop_back();
			if(beg.path.back()->is_node(where::left) 
				&& &beg.path.back()->to_node(where::left) == q)
			{
				beg.path.back()->access(where::left) = 0;
				beg.wt = where::left;
			} else {
				beg.path.back()->access(where::right) = 0;
				beg.wt = where::right;
			}
			return true;
		}
	}

	return false;
}

bool split(auto& term)
{
	iterator<int> right_bound{{}, where::left};
	iterator beg{*term, where::left};

	for(; beg != right_bound; ++beg) 
	{
		if(auto x = *beg; x >= 10) 
		{
			auto p = make_node(
				(int)floor(x / 2.0), 
				(int)ceil(x / 2.0));
			auto pp = &(*p);
			beg.path.back()->access(beg.wt) = std::move(p);
			beg.path.push_back(pp);
			beg.wt = where::left;
			return true;
		}
	}

	return false;
}

int magnitude(const auto& term)
{
	int a, b;
	if(term->is_node(where::left)) 
		a = 3 * magnitude(&(term->to_node(where::left)));
	else 
		a = 3 * term->to_literal(where::left);

	if(term->is_node(where::right)) 
		b = 2 * magnitude(&(term->to_node(where::right)));
	else 
		b = 2 * term->to_literal(where::right);
	return a + b;
}

int main()
{
	std::fstream f{"input"};
	std::vector< std::unique_ptr<node<int>> > terms;
	while(!f.eof()) {
		terms.push_back(parse(f));
		f.ignore(1);
		f.peek();
	}

	int max{0};
	for(int i{}; i < terms.size(); ++i) 
		for(int j{}; j < terms.size(); ++j)
			if(i != j) 
				{
					auto sum = std::unique_ptr<node<int>>(new node<int>{
						copy_node(terms.at(i)), copy_node(terms.at(j))
						});
					while(explode(sum) || split(sum)) { }
					if(auto x = magnitude(sum); x > max) 
						max = x;	
				}

	std::cout<< max << std::endl;

	/*
	for(size_t i{1}; i < terms.size(); ++i) 
	{
		auto sum = std::unique_ptr<node<int>>(new node<int>{
			{ std::move(terms.at(i-1)), std::move(terms.at(i)) }
		});
		terms.at(i) = std::move(sum);
		while(explode(terms.at(i)) || split(terms.at(i)))
		{
			iterator beg{*terms.at(i), where::left};
			for(beg; beg != iterator<int>{{}, where::left}; ++beg) 
				std::cout << *beg << ' ';
			std::cout << std::endl;
		}
	}

	auto& q = terms.back();
	iterator beg{*q, where::left};

	for(beg; beg != iterator<int>{{}, where::left}; ++beg) 
		std::cout << *beg << ' ';
	std::cout << std::endl;
	*//*
	iterator last{*q, where::right};
	for(last; last != iterator<int>{{}, where::right}; --last)
		std::cout << *last << ' ';
	std::cout << std::endl;
	*/ 
	return 0;
}

/*
node<int> root;
root._data = {4, 6};
root.access(where::left) = make_node(1, 1);
root.access(where::right) = make_node(where::left, 2);

append(root.to_node(where::right), where::right, make_node(4,6));

std::cout << root.is_literal(where::right) << std::endl;
std::cout << root.to_node(where::right).is_empty(where::right) << std::endl;
std::cout << root.to_node(where::right)
	.to_node(where::right).to_literal(where::left) << std::endl;

node<int> empty;
std::cout << empty.is_empty(where::right) << std::endl;
*/
