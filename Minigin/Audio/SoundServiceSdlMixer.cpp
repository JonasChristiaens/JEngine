#include "SoundServiceSdlMixer.h"
#include "../Resources/ResourceManager.h"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <algorithm>
#include <filesystem>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace dae
{
	class SoundServiceSdlMixer::SoundServiceSdlMixerImpl
	{
	public:
		SoundServiceSdlMixerImpl()
		{
			if (!MIX_Init())
			{
				SDL_Log("MIX_Init failed: %s", SDL_GetError());
				return;
			}

			m_pMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
			if (m_pMixer == nullptr)
			{
				SDL_Log("MIX_CreateMixerDevice failed: %s", SDL_GetError());
				MIX_Quit();
				return;
			}

			m_Worker = std::thread(&SoundServiceSdlMixerImpl::ThreadMain, this);
		}

		~SoundServiceSdlMixerImpl()
		{
			{
				std::lock_guard lock(m_QueueMutex);
				m_Queue.push_back({ RequestType::Stop, {} });
				m_Running = false;
			}
			m_Condition.notify_one();

			if (m_Worker.joinable())
				m_Worker.join();

			for (auto* pTrack : m_ActiveTracks)
			{
				if (pTrack)
				{
					MIX_StopTrack(pTrack, 0);
					MIX_DestroyTrack(pTrack);
				}
			}
			m_ActiveTracks.clear();
			m_ActiveTrackCount.store(0);

			for (auto* pTrack : m_LoopingTracks)
			{
				if (pTrack)
				{
					MIX_StopTrack(pTrack, 0);
					MIX_DestroyTrack(pTrack);
				}
			}
			m_LoopingTracks.clear();

			for (auto& [_, pAudio] : m_LoadedSounds)
			{
				if (pAudio)
					MIX_DestroyAudio(pAudio);
			}
			m_LoadedSounds.clear();

			if (m_pMixer)
			{
				MIX_DestroyMixer(m_pMixer);
				m_pMixer = nullptr;
			}

			MIX_Quit();
		}

		void PlaySound(const std::string& relativePath)
		{
			if (m_Muted)
				return;

			std::lock_guard lock(m_QueueMutex);
			m_Queue.push_back({ RequestType::Play, relativePath });
			m_Condition.notify_one();
		}

		void PlayLooping(const std::string& relativePath)
		{
			if (m_Muted)
				return;

			std::lock_guard lock(m_QueueMutex);
			m_Queue.push_back({ RequestType::PlayLooping, relativePath });
			m_Condition.notify_one();
		}

		bool IsPlaying() const
		{
			return !m_Muted && m_ActiveTrackCount.load() > 0;
		}

		void PreloadSound(const std::string& relativePath)
		{
			std::lock_guard lock(m_QueueMutex);
			m_Queue.push_back({ RequestType::Preload, relativePath });
			m_Condition.notify_one();
		}

		void SetMuted(bool muted)
		{
			m_Muted = muted;
			if (muted)
			{
				std::lock_guard lock(m_QueueMutex);
				m_Queue.clear();
			}
			if (m_pMixer)
			{
				MIX_SetMixerGain(m_pMixer, muted ? 0.0f : 1.0f);
			}
		}

		void StopAll()
		{
			std::lock_guard lock(m_QueueMutex);
			m_Queue.push_back({ RequestType::StopAll, {} });
			m_Condition.notify_one();
		}

	private:
		enum class RequestType
		{
			Preload,
			Play,
			PlayLooping,
			Stop,
			StopAll
		};

		struct Request
		{
			RequestType type{};
			std::string path{};
		};

		void ThreadMain()
		{
			for (;;)
			{
				Request request{};

				{
					std::unique_lock lock(m_QueueMutex);
					m_Condition.wait(lock, [this]() { return !m_Queue.empty() || !m_Running; });

					if (m_Queue.empty())
					{
						if (!m_Running)
							break;
						continue;
					}

					request = std::move(m_Queue.front());
					m_Queue.erase(m_Queue.begin());
				}

				if (request.type == RequestType::Stop)
					break;

				ProcessRequest(request);
			}
		}

		void ProcessRequest(const Request& request)
		{
			if (m_pMixer == nullptr)
				return;

			if (request.type == RequestType::StopAll)
			{
				CleanupFinishedTracks();
				for (auto* pTrack : m_ActiveTracks)
				{
					if (pTrack)
					{
						MIX_StopTrack(pTrack, 0);
						MIX_DestroyTrack(pTrack);
					}
				}
				m_ActiveTracks.clear();
				m_ActiveTrackCount.store(0);

				for (auto* pTrack : m_LoopingTracks)
				{
					if (pTrack)
					{
						MIX_StopTrack(pTrack, 0);
						MIX_DestroyTrack(pTrack);
					}
				}
				m_LoopingTracks.clear();

				return;
			}

			if (request.path.empty())
				return;

			CleanupFinishedTracks();

			MIX_Audio* pAudio = LoadSoundIfNeeded(request.path);
			if (pAudio == nullptr)
				return;

			if (request.type == RequestType::Preload)
				return;

			if (request.type == RequestType::PlayLooping)
			{
				auto* pLoopTrack = MIX_CreateTrack(m_pMixer);
				if (pLoopTrack == nullptr)
				{
					SDL_Log("MIX_CreateTrack failed for looping: %s", SDL_GetError());
					return;
				}

				if (!MIX_SetTrackAudio(pLoopTrack, pAudio))
				{
					SDL_Log("MIX_SetTrackAudio failed for looping: %s", SDL_GetError());
					MIX_DestroyTrack(pLoopTrack);
					return;
				}

				const SDL_PropertiesID props = SDL_CreateProperties();
				SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
				if (!MIX_PlayTrack(pLoopTrack, props))
				{
					SDL_Log("MIX_PlayTrack failed for looping: %s", SDL_GetError());
					SDL_DestroyProperties(props);
					MIX_DestroyTrack(pLoopTrack);
					return;
				}
				SDL_DestroyProperties(props);

				m_LoopingTracks.push_back(pLoopTrack);
				return;
			}

			auto* pTrack = MIX_CreateTrack(m_pMixer);
			if (pTrack == nullptr)
			{
				SDL_Log("MIX_CreateTrack failed: %s", SDL_GetError());
				return;
			}

			if (!MIX_SetTrackAudio(pTrack, pAudio))
			{
				SDL_Log("MIX_SetTrackAudio failed: %s", SDL_GetError());
				MIX_DestroyTrack(pTrack);
				return;
			}

			if (!MIX_PlayTrack(pTrack, 0))
			{
				SDL_Log("MIX_PlayTrack failed: %s", SDL_GetError());
				MIX_DestroyTrack(pTrack);
				return;
			}

			m_ActiveTracks.push_back(pTrack);
			++m_ActiveTrackCount;
		}

		MIX_Audio* LoadSoundIfNeeded(const std::string& relativePath)
		{
			const auto found = m_LoadedSounds.find(relativePath);
			if (found != m_LoadedSounds.end())
				return found->second;

			const fs::path fullPath = ResourceManager::GetInstance().GetDataPath() / relativePath;
			auto* pAudio = MIX_LoadAudio(m_pMixer, fullPath.string().c_str(), true);
			if (pAudio == nullptr)
			{
				SDL_Log("MIX_LoadAudio failed for '%s': %s", fullPath.string().c_str(), SDL_GetError());
				return nullptr;
			}

			m_LoadedSounds.emplace(relativePath, pAudio);
			return pAudio;
		}

		void CleanupFinishedTracks()
		{
			const auto before = m_ActiveTracks.size();
			m_ActiveTracks.erase(
				std::remove_if(m_ActiveTracks.begin(), m_ActiveTracks.end(), [](MIX_Track* pTrack)
					{
						if (pTrack == nullptr)
							return true;

						if (MIX_TrackPlaying(pTrack))
							return false;

						MIX_DestroyTrack(pTrack);
						return true;
					}),
				m_ActiveTracks.end()
			);
			if (m_ActiveTracks.size() < before)
			{
				m_ActiveTrackCount.store(static_cast<int>(m_ActiveTracks.size()));
			}
		}

		std::atomic<bool> m_Running{ true };
		std::thread m_Worker{};
		std::mutex m_QueueMutex{};
		std::condition_variable m_Condition{};
		std::vector<Request> m_Queue{};

		MIX_Mixer* m_pMixer{};
		std::unordered_map<std::string, MIX_Audio*> m_LoadedSounds{};
		std::vector<MIX_Track*> m_ActiveTracks{};
		std::vector<MIX_Track*> m_LoopingTracks{};
		std::atomic<int> m_ActiveTrackCount{ 0 };
		bool m_Muted{ false };
	};

	SoundServiceSdlMixer::SoundServiceSdlMixer()
		: m_pImpl(std::make_unique<SoundServiceSdlMixerImpl>())
	{}

	SoundServiceSdlMixer::~SoundServiceSdlMixer() = default;

	SoundServiceSdlMixer::SoundServiceSdlMixer(SoundServiceSdlMixer&&) noexcept = default;
	SoundServiceSdlMixer& SoundServiceSdlMixer::operator=(SoundServiceSdlMixer&&) noexcept = default;

	void SoundServiceSdlMixer::PlaySound(const std::string& relativePath)
	{
		m_pImpl->PlaySound(relativePath);
	}

	void SoundServiceSdlMixer::PlayLooping(const std::string& relativePath)
	{
		m_pImpl->PlayLooping(relativePath);
	}

	bool SoundServiceSdlMixer::IsPlaying() const
	{
		return m_pImpl->IsPlaying();
	}

	void SoundServiceSdlMixer::PreloadSound(const std::string& relativePath)
	{
		m_pImpl->PreloadSound(relativePath);
	}

	void SoundServiceSdlMixer::SetMuted(bool muted)
	{
		m_pImpl->SetMuted(muted);
	}

	void SoundServiceSdlMixer::StopAll()
	{
		m_pImpl->StopAll();
	}
}
