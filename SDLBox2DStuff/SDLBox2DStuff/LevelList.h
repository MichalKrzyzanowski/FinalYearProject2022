#ifndef LEVEL_LIST_H
#define LEVEL_LIST_H

#include <vector>
#include <iostream>
#include <filesystem>

class LevelList
{
public:
	static void updateList();
	static std::vector<std::string> levels() { return m_levels; }

private:
	static std::vector<std::string> m_levels;
};

#endif // !LEVEL_LIST_H