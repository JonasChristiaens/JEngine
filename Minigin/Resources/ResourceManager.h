#pragma once
#include <filesystem>
#include <string>
#include <memory>
#include <unordered_map>
#include "Singleton.h"
#include "../Rendering/Texture2D.h"

namespace dae
{
	class ResourceManager final : public Singleton<ResourceManager>
	{
	public:
		void Init(const std::filesystem::path& data);
		void Shutdown();
		~ResourceManager();
		const std::filesystem::path& GetDataPath() const;
		Texture2D* LoadTexture(const std::string& file);
		std::string LoadTextFile(const std::string& file);

	private:
		friend class Singleton<ResourceManager>;
		ResourceManager() = default;
		std::filesystem::path m_DataPath;
		bool m_IsInitialized{};
		std::unordered_map<std::string, std::unique_ptr<Texture2D>> m_Textures{};
	};
}
