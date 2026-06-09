#include <stdexcept>
#include <SDL3_ttf/SDL_ttf.h>
#include "ResourceManager.h"
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

std::unique_ptr<dae::Texture2D> dae::ResourceManager::LoadTexture(const std::string& file)
{
	const auto fullPath = m_DataPath / file;
	return std::make_unique<Texture2D>(fullPath.string());
}

std::unique_ptr<dae::Font> dae::ResourceManager::LoadFont(const std::string& file, uint8_t size)
{
	const auto fullPath = m_DataPath / file;
	return std::make_unique<Font>(fullPath.string(), size);
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
