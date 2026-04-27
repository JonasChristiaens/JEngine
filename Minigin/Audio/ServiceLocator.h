#pragma once
#include <memory>
#include "ISoundService.h"
#include "NullSoundService.h"

namespace dae
{
    class ServiceLocator final
    {
    public:
        static ISoundService& GetSoundService();
        static void RegisterSoundService(std::unique_ptr<ISoundService> pService);

    private:
        static std::unique_ptr<ISoundService> m_pSoundService;
        static NullSoundService m_NullSoundService;
    };
}
