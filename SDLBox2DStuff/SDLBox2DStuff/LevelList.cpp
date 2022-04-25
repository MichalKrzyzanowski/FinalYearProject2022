#include "LevelList.h"

// init static members
std::vector<std::string> LevelList::m_levels{};

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