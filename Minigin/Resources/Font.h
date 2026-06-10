#pragma once
#include <string>

struct TTF_Font;
namespace dae
{
	/**
	 * Simple RAII wrapper for a TTF_Font
	 */
	class Font final
	{
	public:
		TTF_Font* GetFont() const;
		explicit Font(const std::string& fullPath, float size);
		~Font();

		Font(const Font&) = delete;
		Font(Font&&) noexcept;
		Font& operator= (const Font&) = delete;
		Font& operator= (Font&&) noexcept;
	private:
		TTF_Font* m_Font;
	};
}
