#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <sstream>
#include <SDL_ttf.h>

struct Vector2f
{
	float x, y;
};

struct LTexture
{
	SDL_Texture* texture;
	int width;
	int height;
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

inline LTexture loadFromRenderedText(const char* textureText, SDL_Color textColor, TTF_Font* font, SDL_Renderer* renderer)
{
	LTexture texture{ nullptr, 0, 0 };

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText, textColor);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		texture.texture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (texture.texture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			texture.width = textSurface->w;
			texture.height = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	return texture;
}

inline void renderText(SDL_Renderer* renderer, LTexture* texture, Vector2f position)
{
	SDL_Rect dest;
	dest.x = position.x;
	dest.y = position.y;
	dest.w = texture->width;
	dest.h = texture->height;

	SDL_RenderCopy(renderer, texture->texture, nullptr, &dest);
}

inline int getNumberFromString(std::string str)
{
	int i{};

	for (; str.length(); ++i)
	{
		if (isdigit(str[i])) break;
	}

	str = str.substr(i, str.length() - i);

	return atoi(str.c_str());
}

#endif // !UTILITY_H

