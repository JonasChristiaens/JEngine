#include <SDL3/SDL.h>
#include "Texture2D.h"
#include "Renderer.h"
#include <stdexcept>

dae::Texture2D::~Texture2D()
{
	if (m_Texture)
		SDL_DestroyTexture(m_Texture);
}

glm::vec2 dae::Texture2D::GetSize() const
{
    float w{}, h{};
    SDL_GetTextureSize(m_Texture, &w, &h);
    return { w, h };
}

SDL_Texture* dae::Texture2D::GetSDLTexture() const
{
	return m_Texture;
}

dae::Texture2D::Texture2D(const std::string &fullPath)
{
    SDL_Surface* surface = SDL_LoadPNG(fullPath.c_str());
    if (!surface)
    {
        throw std::runtime_error(
            std::string("Failed to load PNG: ") + SDL_GetError()
        );
    }

    m_Texture = SDL_CreateTextureFromSurface(
        Renderer::GetInstance().GetSDLRenderer(),
        surface
    );

    if (!m_Texture)
    {
        SDL_DestroySurface(surface);
        throw std::runtime_error(
            std::string("Failed to create texture from surface: ") + SDL_GetError()
        );
    }

    SDL_SetTextureScaleMode(m_Texture, SDL_SCALEMODE_NEAREST);

    SDL_DestroySurface(surface);
}

dae::Texture2D::Texture2D(SDL_Texture* texture)	: m_Texture{ texture } 
{
	assert(m_Texture != nullptr);
}

dae::Texture2D::Texture2D(Texture2D&& other) noexcept
	: m_Texture(other.m_Texture)
{
	other.m_Texture = nullptr;
}

dae::Texture2D& dae::Texture2D::operator=(Texture2D&& other) noexcept
{
	if (this != &other)
	{
		if (m_Texture)
			SDL_DestroyTexture(m_Texture);
		m_Texture = other.m_Texture;
		other.m_Texture = nullptr;
	}
	return *this;
}

