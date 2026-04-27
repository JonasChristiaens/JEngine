#pragma once
#include <string>

namespace dae
{
    class ISoundService
    {
    public:
        virtual ~ISoundService() = default;

        virtual void PlaySound(const std::string& relativePath) = 0;
        virtual void PreloadSound(const std::string& relativePath) = 0;
    };
}
