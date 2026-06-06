#pragma once
#include <SDL3/SDL.h>
#include "Singleton.h"

namespace dae
{
	class Texture2D;
	/**
	 * Simple RAII wrapper for the SDL renderer
	 */
	class Renderer final : public Singleton<Renderer>
	{
		SDL_Renderer* m_Renderer{};
		SDL_Window* m_Window{};
		SDL_Color m_ClearColor{};
	public:
		void Init(SDL_Window* window);
		void Render() const;
		void Destroy();

		void RenderTexture(const Texture2D& texture, float x, float y) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height, const SDL_FRect& sourceRect) const;
		void SetCameraOffset(float x, float y) { m_CameraOffsetX = x; m_CameraOffsetY = y; }
		void ResetCameraOffset() { SetCameraOffset(0.0f, 0.0f); }
		void GetCameraOffset(float& x, float& y) const { x = m_CameraOffsetX; y = m_CameraOffsetY; }
		SDL_Point GetWindowSize() const;

		SDL_Renderer* GetSDLRenderer() const;

		const SDL_Color& GetBackgroundColor() const { return m_ClearColor; }
		void SetBackgroundColor(const SDL_Color& color) { m_ClearColor = color; }

	private:
		float m_CameraOffsetX{ 0.0f };
		float m_CameraOffsetY{ 0.0f };
	};
}

