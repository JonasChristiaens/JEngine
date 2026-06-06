#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "ResourceManager.h"
#include "Renderer.h"
#include "Texture2D.h"
#include "Font.h"
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

void dae::ResourceManager::Init(const std::filesystem::path& dataPath)
{
	m_DataPath = dataPath;

	if (m_IsInitialized)
		return;

	if (!TTF_Init())
	{
		throw std::runtime_error(std::string("Failed to load support for fonts: ") + SDL_GetError());
	}

	m_IsInitialized = true;
}

void dae::ResourceManager::Shutdown()
{
	m_LoadedFonts.clear();
	m_LoadedTextures.clear();

	if (!m_IsInitialized)
		return;

	TTF_Quit();
	m_IsInitialized = false;
}

dae::ResourceManager::~ResourceManager()
{
	Shutdown();
}

const std::filesystem::path& dae::ResourceManager::GetDataPath() const
{
	return m_DataPath;
}

std::shared_ptr<dae::Texture2D> dae::ResourceManager::LoadTexture(const std::string& file)
{
	const auto fullPath = m_DataPath/file;
	const auto filename = fs::path(fullPath).filename().string();
	if(m_LoadedTextures.find(filename) == m_LoadedTextures.end())
		m_LoadedTextures.insert(std::pair(filename,std::make_shared<Texture2D>(fullPath.string())));
	return m_LoadedTextures.at(filename);
}

std::shared_ptr<dae::Font> dae::ResourceManager::LoadFont(const std::string& file, uint8_t size)
{
	const auto fullPath = m_DataPath/file;
	const auto filename = fs::path(fullPath).filename().string();
	const auto key = std::pair<std::string, uint8_t>(filename, size);
	if(m_LoadedFonts.find(key) == m_LoadedFonts.end())
		m_LoadedFonts.insert(std::pair(key,std::make_shared<Font>(fullPath.string(), size)));
	return m_LoadedFonts.at(key);
}

std::string dae::ResourceManager::LoadTextFile(const std::string& file)
{
	const auto fullPath = m_DataPath / file;
	std::ifstream stream(fullPath);
	if (!stream.is_open())
	{
		throw std::runtime_error("Failed to open text file");
	}
	std::stringstream buffer;
	buffer << stream.rdbuf();
	return buffer.str();
}

void dae::ResourceManager::UnloadUnusedResources()
{
	for (auto it = m_LoadedTextures.begin(); it != m_LoadedTextures.end();)
	{
		if (it->second.use_count() == 1)
			it = m_LoadedTextures.erase(it);
		else
			++it;
	}

	for (auto it = m_LoadedFonts.begin(); it != m_LoadedFonts.end();)
	{
		if (it->second.use_count() == 1)
			it = m_LoadedFonts.erase(it);
		else
			++it;
	}
}
