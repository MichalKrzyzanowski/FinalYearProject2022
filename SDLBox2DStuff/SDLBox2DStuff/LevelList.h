#ifndef LEVEL_LIST_H
#define LEVEL_LIST_H

#include <vector>
#include <iostream>
#include <filesystem>

class LevelList
{
public:
	static void updateList();
	
	static std::string getLevelData(int t_index);
	static void setCurrentLevel(int t_index);
	static int getCurrentLevel();
	static size_t size();
	static std::vector<std::string> levels() { return m_levels; }

private:
	static std::vector<std::string> m_levels;
	static int m_currentLevelIndex;
};

#endif // !LEVEL_LIST_H