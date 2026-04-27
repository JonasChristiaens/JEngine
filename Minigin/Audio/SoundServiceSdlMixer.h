#pragma once
#include "ISoundService.h"
#include <unordered_map>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

struct MIX_Mixer;
struct MIX_Audio;
struct MIX_Track;

namespace dae
{
    class SoundServiceSdlMixer final : public ISoundService
    {
    public:
        SoundServiceSdlMixer();
        ~SoundServiceSdlMixer() override;

        void PlaySound(const std::string& relativePath) override;
        void PreloadSound(const std::string& relativePath) override;

    private:
        enum class RequestType
        {
            Preload,
            Play,
            Stop
        };

        struct Request
        {
            RequestType type{};
            std::string path{};
        };

        void ThreadMain();
        void ProcessRequest(const Request& request);
        MIX_Audio* LoadSoundIfNeeded(const std::string& relativePath);
        void CleanupFinishedTracks();

        std::atomic<bool> m_Running{ true };
        std::thread m_Worker{};
        std::mutex m_QueueMutex{};
        std::condition_variable m_Condition{};
        std::vector<Request> m_Queue{};

        MIX_Mixer* m_pMixer{};
        std::unordered_map<std::string, MIX_Audio*> m_LoadedSounds{};
        std::vector<MIX_Track*> m_ActiveTracks{};
    };
}
