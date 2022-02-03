#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <sstream>

struct Vector2f
{
	float x, y;
};


#define PI 3.14159265358979323846

const float SCALING_FACTOR{ 100.0f };

inline float Deg2Rad(float deg)
{
	return deg * (PI / 180.0f);
}

inline float Rad2Deg(float rad)
{
	return rad * (180.0f / PI);
}

inline Vector2f rotateVector(Vector2f v, float angle)
{
	float cs = cos(angle);
	float sn = sin(angle);

	float px = v.x * cs - v.y * sn;
	float py = v.x * sn + v.y * cs;

	return Vector2f{ px, py };
}

inline void rotatePoint(float cx, float cy, float angle, SDL_FPoint& point)
{
	point.x -= cx;
	point.y -= cy;

	float newX = point.x * cosf(angle) - point.y * sinf(angle);
	float newY = point.x * sinf(angle) + point.y * cosf(angle);

	point.x = newX + cx;
	point.y = newY + cy;
}

inline float average(int* numbers, size_t size)
{
	int total{};

	for (int i{}; i < size; ++i)
	{
		total += numbers[i];
	}

	return static_cast<float>(total) / size;
}

inline std::vector<std::string> splitString(const std::string& s, char delimiter)
{
	std::vector<std::string> splits;
	std::string split;
	std::istringstream ss(s);
	while(std::getline(ss, split, delimiter))
	{
		splits.push_back(split);
	}
	return splits;
}


#endif // !UTILITY_H

