#include "LevelList.h"

// init static members
std::vector<std::string> LevelList::m_levels{};
int LevelList::m_currentLevelIndex = 0;

void LevelList::updateList()
{
	std::string directoryPath{ "levels//" };

	m_levels.clear();

	for (const auto& file : std::filesystem::directory_iterator(directoryPath))
	{
		m_levels.push_back(file.path().filename().string());
		std::cout << file.path().filename().string() << "\n";
	}
}

std::string LevelList::getLevelData(int t_index)
{
	return m_levels.at(t_index);
}

void LevelList::setCurrentLevel(int t_index)
{
	m_currentLevelIndex = t_index;
}

int LevelList::getCurrentLevel()
{
	return m_currentLevelIndex;
}

size_t LevelList::size()
{
	return m_levels.size();
}
