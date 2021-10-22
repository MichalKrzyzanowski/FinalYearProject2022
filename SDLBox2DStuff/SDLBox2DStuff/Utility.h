#ifndef UTILITY_H
#define UTILITY_H

#define PI 3.14159265358979323846

inline float Deg2Rad(float deg)
{
	return deg * (PI / 180.0f);
}

inline float Rad2Deg(float rad)
{
	return rad * (180.0f / PI);
}

#endif // !UTILITY_H

