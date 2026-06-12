#pragma once
#include "SceneState.h"
#include <memory>
#include <string>
#include <utility>

namespace dae
{
	class Scene;
	class GameObject;
	class RenderComponent;

	class TitleSceneState final : public SceneState
	{
	public:
		explicit TitleSceneState(SceneStateMachineComponent& owner);

		void OnEnter() override;
		void OnExit() override;
		void HandleInput() override {}
		void Update() override;
		void MoveSelection(int direction);
		void ConfirmSelection();
		void CycleMode();

	private:
		void BuildScene();
		void UpdateMenuSprites();
		void UpdateModeLabel();
		void CreateTextSprites(Scene& scene, GameObject& parent, const std::string& text, float x, float y, int renderLayer);
		void ClearScene();
		void BindInput();
		void UnbindInput();
		std::pair<float, float> GetCharacterSrcRect(char character) const;

		int m_SelectedIndex{ 0 };
		int m_ModeIndex{ 0 };
		float m_ScaleX{ 1.0f };
		float m_ScaleY{ 1.0f };
		float m_ArrowWidth{ 0.0f };
		float m_ArrowHeight{ 0.0f };

		GameObject* m_pRoot{};
		GameObject* m_pMenuRoot{};
		RenderComponent* m_pStartArrow{};
		RenderComponent* m_pModeArrow{};
		GameObject* m_pModeLabelRoot{};
		GameObject* m_pScoreRoot{};
		float m_BgmCooldown{ 0.0f };
		float m_BlinkTimer{ 0.0f };
		bool m_ArrowVisible{ true };
	};
}
