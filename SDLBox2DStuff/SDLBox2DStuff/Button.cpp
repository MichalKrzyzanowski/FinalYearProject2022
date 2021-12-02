#include "Button.h"

UI::Button::Button(Vector2f pos, const char* title) :
	m_button{ pos.x, pos.y, 40.0f, 40.0f },
	m_title{ title }
{
}

void UI::Button::render(SDL_Renderer* renderer)
{
	SDL_RenderDrawRectF(renderer, &m_button);
}
