#include "ServiceLocator.h"

std::unique_ptr<dae::ISoundService> dae::ServiceLocator::m_pSoundService{};
dae::NullSoundService dae::ServiceLocator::m_NullSoundService{};

dae::ISoundService& dae::ServiceLocator::GetSoundService()
{
    if (m_pSoundService)
        return *m_pSoundService;

    return m_NullSoundService;
}

void dae::ServiceLocator::RegisterSoundService(std::unique_ptr<ISoundService> pService)
{
    m_pSoundService = std::move(pService);
}
