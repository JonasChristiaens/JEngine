#include "SoundServiceSdlMixer.h"
#include "../ResourceManager.h"
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace dae
{
    SoundServiceSdlMixer::SoundServiceSdlMixer()
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

        m_Worker = std::thread(&SoundServiceSdlMixer::ThreadMain, this);
    }

    SoundServiceSdlMixer::~SoundServiceSdlMixer()
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

    void SoundServiceSdlMixer::PlaySound(const std::string& relativePath)
    {
        std::lock_guard lock(m_QueueMutex);
        m_Queue.push_back({ RequestType::Play, relativePath });
        m_Condition.notify_one();
    }

    void SoundServiceSdlMixer::PreloadSound(const std::string& relativePath)
    {
        std::lock_guard lock(m_QueueMutex);
        m_Queue.push_back({ RequestType::Preload, relativePath });
        m_Condition.notify_one();
    }

    void SoundServiceSdlMixer::ThreadMain()
    {
        for (;;)
        {
            Request request{};

            {
                std::unique_lock lock(m_QueueMutex);
                m_Condition.wait(lock, [this]() { return !m_Queue.empty() || !m_Running; });

                if (m_Queue.empty() && !m_Running)
                    break;

                request = std::move(m_Queue.front());
                m_Queue.erase(m_Queue.begin());
            }

            if (request.type == RequestType::Stop)
                break;

            ProcessRequest(request);
        }
    }

    void SoundServiceSdlMixer::ProcessRequest(const Request& request)
    {
        if (m_pMixer == nullptr)
            return;

        if (request.path.empty())
            return;

        CleanupFinishedTracks();

        MIX_Audio* pAudio = LoadSoundIfNeeded(request.path);
        if (pAudio == nullptr)
            return;

        if (request.type == RequestType::Preload)
            return;

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
    }

    MIX_Audio* SoundServiceSdlMixer::LoadSoundIfNeeded(const std::string& relativePath)
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

    void SoundServiceSdlMixer::CleanupFinishedTracks()
    {
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
    }
}
