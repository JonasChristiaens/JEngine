#include "HudComponent.h"
#include "Scene/GameObject.h"
#include "Components/ScoreComponent.h"
#include "Components/HealthComponent.h"
#include "Rendering/Texture2D.h"
#include "Rendering/Renderer.h"
#include "Core/GameTime.h"
#include "Resources/ResourceManager.h"
#include "EventQueue/EventManager.h"
#include <sstream>
#include <iomanip>

namespace dae
{
	HudComponent::HudComponent(GameObject* pOwner, float hudWidth, float hudHeight, const std::vector<GameObject*>& players)
		: BaseComponent(pOwner)
		, m_Width(hudWidth)
		, m_Height(hudHeight)
		, m_Players(players)
	{
		const size_t count = players.size();
		m_Scores.resize(count, 0);
		m_Lives.resize(count, 4);
		m_pFontTexture = ResourceManager::GetInstance().LoadTexture("BombermanSprites_Titlescreen.png");
		RefreshStats();
		EventManager::GetInstance().AddObserver(*this);
	}

	HudComponent::~HudComponent()
	{
		if (EventManager::IsAlive())
			EventManager::GetInstance().RemoveObserver(*this);
	}

	void HudComponent::Update()
	{
		m_RemainingMs -= GameTime::GetInstance().GetDeltaTime() * 1000.0f;
		if (m_RemainingMs < 0.0f)
			m_RemainingMs = 0.0f;
	}

	void HudComponent::Render() const
	{
		SDL_Renderer* renderer = Renderer::GetInstance().GetSDLRenderer();
		SDL_SetRenderDrawColor(renderer, 173, 173, 173, 255);
		SDL_FRect bgRect{ 0.0f, 0.0f, m_Width, m_Height };
		SDL_RenderFillRect(renderer, &bgRect);

		const float scale = m_Height / 24.0f;
		const float y = m_Height * 0.25f;
		const int timerDisplay = static_cast<int>(m_RemainingMs / kTimerScale);
		const int seconds = std::max(0, timerDisplay);
		std::ostringstream timerText;
		timerText << "TIME " << std::setw(3) << std::setfill('0') << seconds;
		RenderText(timerText.str(), m_Width * 0.02f, y, scale);

		if (m_Players.size() <= 1)
		{
			std::ostringstream scoreText;
			scoreText << std::setw(2) << std::setfill('0') << m_Scores[0];
			RenderText(scoreText.str(), m_Width * 0.48f, y, scale);

			std::ostringstream livesText;
			livesText << "LEFT " << m_Lives[0];
			RenderText(livesText.str(), m_Width * 0.70f, y, scale);
		}
		else
		{
			std::ostringstream scoreText;
			for (size_t i = 0; i < m_Players.size(); ++i)
			{
				if (i > 0) scoreText << "  ";
				scoreText << "p" << (i + 1) << " " << std::setw(2) << std::setfill('0') << m_Scores[i];
			}
			RenderText(scoreText.str(), m_Width * 0.38f, y, scale);

			std::ostringstream livesText;
			for (size_t i = 0; i < m_Players.size(); ++i)
			{
				if (i > 0) livesText << "  ";
				livesText << "p" << (i + 1) << " " << m_Lives[i];
			}
			RenderText(livesText.str(), m_Width * 0.70f, y, scale);
		}
	}

	void HudComponent::Notify(GameObject& actor, Event event)
	{
		const auto changeScoreId = make_sdbm_hash("ChangeScoreEvent");
		const auto changeHealthId = make_sdbm_hash("ChangeHealthEvent");

		for (size_t i = 0; i < m_Players.size(); ++i)
		{
			if (&actor != m_Players[i])
				continue;

			if (event.id == changeScoreId && event.nbArgs > 0)
				m_Scores[i] += event.args[0].i;
			else if (event.id == changeHealthId)
			{
				auto* health = actor.GetComponent<HealthComponent>();
				if (health)
					m_Lives[i] = health->GetHealth();
			}
			break;
		}
	}

	void HudComponent::RefreshStats()
	{
		for (size_t i = 0; i < m_Players.size(); ++i)
		{
			if (!m_Players[i]) continue;
			auto* score = m_Players[i]->GetComponent<ScoreComponent>();
			if (score) m_Scores[i] = score->GetScore();
			auto* health = m_Players[i]->GetComponent<HealthComponent>();
			if (health) m_Lives[i] = health->GetHealth();
		}
	}

	void HudComponent::RenderText(const std::string& text, float x, float y, float scale) const
	{
		SDL_Renderer* renderer = Renderer::GetInstance().GetSDLRenderer();
		for (size_t i = 0; i < text.size(); ++i)
		{
			const char c = text[i];
			SDL_FRect src{};
			src.w = static_cast<float>(kCharSize);
			src.h = static_cast<float>(kCharSize);

			if (c >= '0' && c <= '9')
			{
				src.x = static_cast<float>((c - '0') * kCharSize);
				src.y = 264.0f;
			}
			else if (c >= 'A' && c <= 'Z')
			{
				src.x = static_cast<float>((c - 'A') * kCharSize);
				src.y = 272.0f;
			}
			else if (c >= 'a' && c <= 'z')
			{
				src.x = static_cast<float>((c - 'a') * kCharSize);
				src.y = 272.0f;
			}
			else if (c == '>')
			{
				src.x = 80.0f;
				src.y = 264.0f;
			}
			else
			{
				x += kCharSize * scale;
				continue;
			}

			SDL_FRect dst{ x, y, kCharSize * scale, kCharSize * scale };
			SDL_RenderTexture(renderer, m_pFontTexture->GetSDLTexture(), &src, &dst);
			x += kCharSize * scale;
		}
	}
}
