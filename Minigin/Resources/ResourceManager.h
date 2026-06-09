#pragma once
#include <filesystem>
#include <string>
#include <memory>
#include "Singleton.h"

namespace dae
{
	class Texture2D;
	class Font;
	class ResourceManager final : public Singleton<ResourceManager>
	{
	public:
		void Init(const std::filesystem::path& data);
		void Shutdown();
		~ResourceManager();
		const std::filesystem::path& GetDataPath() const;
		std::unique_ptr<Texture2D> LoadTexture(const std::string& file);
		std::unique_ptr<Font> LoadFont(const std::string& file, uint8_t size);
		std::string LoadTextFile(const std::string& file);
	private:
		friend class Singleton<ResourceManager>;
		ResourceManager() = default;
		std::filesystem::path m_DataPath;
		bool m_IsInitialized{};
	};
}
