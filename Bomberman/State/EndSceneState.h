#pragma once
#include "SceneState.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace dae
{
	class Scene;
	class GameObject;
	class RenderComponent;

	class EndSceneState final : public SceneState
	{
	public:
		EndSceneState(SceneStateMachineComponent& owner, int player1Score, int player2Score, bool hasTwoPlayers);

		void OnEnter() override;
		void OnExit() override;
		void Update() override;

		void MoveLetter(int direction);
		void ConfirmLetter();
		void ConfirmReturn();

	private:
		static constexpr float kBackgroundSrcX{ 264.0f };
		static constexpr float kBackgroundSrcY{ 0.0f };
		static constexpr float kBackgroundWidth{ 256.0f };
		static constexpr float kBackgroundHeight{ 240.0f };
		static constexpr float kGlyphWidth{ 8.0f };
		static constexpr float kGlyphHeight{ 8.0f };
		static constexpr float kGlyphRowY{ 264.0f };
		static constexpr float kGlyphRow2Y{ 272.0f };
		static constexpr int kNameLength{ 4 };
		static constexpr int kScoreDigits{ 5 };
		static constexpr int kBackgroundLayer{ 0 };
		static constexpr int kTextLayer{ 1 };
		static constexpr int kReturnArrowLayer{ 3 };
		static constexpr float kArrowGlyphX{ 80.0f };

		float m_Scale{ 1.0f };
		float m_OffsetX{ 0.0f };
		float m_OffsetY{ 0.0f };

		int m_PlayerScores[2]{};
		bool m_HasTwoPlayers{ false };

		struct PendingEntry
		{
			int score;
			int playerIndex;
			int entryIndex;
		};
		std::vector<PendingEntry> m_PendingEntries{};
		int m_CurrentPendingIndex{ 0 };

		char m_CurrentName[kNameLength + 1]{};
		int m_CurrentCharIndex{ 0 };
		bool m_ReturnActive{ false };

		enum class Phase
		{
			CheckScores,
			EnterName,
			WaitForReturn
		};
		Phase m_Phase{ Phase::CheckScores };

		Scene* m_pScene{};
		GameObject* m_pRoot{};
		GameObject* m_pTextRoot{};
		RenderComponent* m_pCurrentLetterRender{};
		bool m_IndicatorVisible{ true };
		float m_BlinkTimer{ 0.0f };
		GameObject* m_pReturnArrow{};
		RenderComponent* m_pReturnArrowRender{};

		void BuildScene();
		void ClearScene();
		void BindInput(int playerIndex = -1);
		void UnbindInput();
		void RebuildTextDisplay();
		void RefreshScoreDisplay();
		void RefreshNameDisplay();
		void AdvancePhase();
		void SubmitCurrentName();

		RenderComponent* CreateGlyph(Scene& scene, GameObject& parent, char c, float localX, float localY, int layer);
		std::pair<float, float> GetCharacterSrcRect(char character) const;
		char CycleLetter(char current, int direction) const;
		std::string ScoreToString(int score) const;
	};
}
