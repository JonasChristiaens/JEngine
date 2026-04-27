#pragma once
#include "ISoundService.h"

namespace dae
{
    class NullSoundService final : public ISoundService
    {
    public:
        void PlaySound(const std::string& /*relativePath*/) override {}
        void PreloadSound(const std::string& /*relativePath*/) override {}
    };
}
