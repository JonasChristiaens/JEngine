#pragma once
#include "ISoundService.h"
#include <memory>

namespace dae
{
    class SoundServiceSdlMixer final : public ISoundService
    {
    public:
        SoundServiceSdlMixer();
        ~SoundServiceSdlMixer() override;

        SoundServiceSdlMixer(const SoundServiceSdlMixer&) = delete;
        SoundServiceSdlMixer& operator=(const SoundServiceSdlMixer&) = delete;
        SoundServiceSdlMixer(SoundServiceSdlMixer&&) noexcept;
        SoundServiceSdlMixer& operator=(SoundServiceSdlMixer&&) noexcept;

        void PlaySound(const std::string& relativePath) override;
        void PreloadSound(const std::string& relativePath) override;

    private:
        class SoundServiceSdlMixerImpl;
        std::unique_ptr<SoundServiceSdlMixerImpl> m_pImpl;
    };
}
