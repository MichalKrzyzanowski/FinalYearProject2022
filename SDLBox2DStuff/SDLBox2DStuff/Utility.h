#ifndef UTILITY_H
#define UTILITY_H

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


#endif // !UTILITY_H

