#include <stdexcept>
#include "ResourceManager.h"
#include "Texture2D.h"
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

void dae::ResourceManager::Init(const std::filesystem::path& dataPath)
{
	m_DataPath = dataPath;
	m_IsInitialized = true;
}

void dae::ResourceManager::Shutdown()
{
	if (!m_IsInitialized)
		return;

	m_Textures.clear();
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

dae::Texture2D* dae::ResourceManager::LoadTexture(const std::string& file)
{
	const auto fullPath = (m_DataPath / file).string();

	auto it = m_Textures.find(file);
	if (it != m_Textures.end())
		return it->second.get();

	auto texture = std::make_unique<Texture2D>(fullPath);
	auto* ptr = texture.get();
	m_Textures.emplace(file, std::move(texture));
	return ptr;
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
