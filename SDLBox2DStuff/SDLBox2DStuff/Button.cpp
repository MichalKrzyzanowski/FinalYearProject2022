#include "Button.h"

/// <summary>
/// UI button setup
/// </summary>
/// <param name="pos">button position</param>
/// <param name="title">button title</param>
UI::Button::Button(Vector2f pos, const char* title) :
	m_button{ pos.x, pos.y, 40.0f, 40.0f },
	m_title{ title }
{
}

/// <summary>
/// render the button
/// </summary>
/// <param name="renderer">SDL renderer</param>
void UI::Button::render(SDL_Renderer* renderer)
{
	SDL_RenderDrawRectF(renderer, &m_button);
}
