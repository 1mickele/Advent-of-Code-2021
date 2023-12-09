#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cctype>
#include <fstream>

struct cave
{
	bool is_small() {
		return std::all_of(name.begin(), name.end(),
			[](auto c) { return std::islower(c); });
	}

	std::string name;
	bool visited{false};
	std::vector<cave*> neigh; 
};

std::unordered_map<std::string, cave> sys;
size_t count{};

void parsing() 
{
	std::fstream f{"input"};
	while(!f.eof()) 
	{
		std::string n, m;		
		std::getline(f, n, '-');
		std::getline(f, m);
		
		if(!sys.contains(n))
			sys.insert( std::make_pair(n, cave{n, false, {}}) );
		if(!sys.contains(m))
			sys.insert( std::make_pair(m, cave{m, false, {}}) );

		(sys.at(n).neigh).push_back(&sys.at(m));
		(sys.at(m).neigh).push_back(&sys.at(n));
		f.peek();
	}
}

void navigate(std::vector<cave*>& path)
{
	path.back()->visited = true;
	for(auto* cv : path.back()->neigh) 
	{
		if(cv->name == "end") {
			++count;
			continue;
		} else if(!cv->is_small() || !cv->visited) {
			path.push_back(cv);	
			navigate(path);
			path.pop_back();
			cv->visited = false;
		}
	}
}

void navigate2(std::vector<cave*>& path, bool dbvisit)
{
	path.back()->visited = true;
	for(auto* cv : path.back()->neigh) 
	{
		if(cv->name == "end") {
			++count;
			continue;
		} else if(!cv->is_small() || !cv->visited) {
			path.push_back(cv);	
			navigate2(path, dbvisit);
			path.pop_back();
			cv->visited = false;
		} else if(dbvisit && cv->name != "start") {
			path.push_back(cv);	
			navigate2(path, false);
			path.pop_back();
		}
	}
}

int main()
{

	parsing();

	std::vector<cave*> path{&sys.at("start")};
	navigate2(path, true);
	std::cout << count << std::endl;

}
