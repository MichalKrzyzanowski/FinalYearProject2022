#include "LevelList.h"

// init static members
std::vector<std::string> LevelList::m_levels{};

/// <summary>
/// gets the levels from the levels directory and updates the levels vector
/// </summary>
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