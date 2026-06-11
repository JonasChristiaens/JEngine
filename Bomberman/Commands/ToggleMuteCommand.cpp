#include "ToggleMuteCommand.h"
#include "Audio/ServiceLocator.h"

namespace dae
{
	void ToggleMuteCommand::Execute()
	{
		m_Muted = !m_Muted;
		ServiceLocator::GetSoundService().SetMuted(m_Muted);
	}
}
