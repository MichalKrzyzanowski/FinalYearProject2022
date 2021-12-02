#ifndef UI_SHAPE_SELECT_BUTTON_H
#define UI_SHAPE_SELECT_BUTTON_H

#include <SDL.h>
#include "Utility.h"

namespace UI
{
	class Button
	{
	private:
		SDL_FRect m_button;
		const char* m_title;

	public:
		Button(Vector2f pos, const char* title = "");
		void render(SDL_Renderer* renderer);

		Vector2f position() { return Vector2f{ m_button.x, m_button.y }; }
		float width() { return m_button.w; }
		float height() { return m_button.h; }
	};
}

#endif // !UI_SHAPE_SELECT_BUTTON_H

