#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "Font.h"

TTF_Font* dae::Font::GetFont() const {
	return m_Font;
}

dae::Font::Font(const std::string& fullPath, float size) : m_Font(nullptr)
{
	m_Font = TTF_OpenFont(fullPath.c_str(), size);
	if (m_Font == nullptr) 
	{
		throw std::runtime_error(std::string("Failed to load font: ") + SDL_GetError());
	}
}

dae::Font::~Font()
{
	if (m_Font)
		TTF_CloseFont(m_Font);
}

dae::Font::Font(Font&& other) noexcept
	: m_Font(other.m_Font)
{
	other.m_Font = nullptr;
}

dae::Font& dae::Font::operator=(Font&& other) noexcept
{
	if (this != &other)
	{
		if (m_Font)
			TTF_CloseFont(m_Font);
		m_Font = other.m_Font;
		other.m_Font = nullptr;
	}
	return *this;
}
