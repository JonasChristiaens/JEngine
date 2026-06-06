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

		int m_selectedIndex{ 0 };
		int m_modeIndex{ 0 };
		float m_scaleX{ 1.0f };
		float m_scaleY{ 1.0f };
		float m_arrowWidth{ 0.0f };
		float m_arrowHeight{ 0.0f };

		GameObject* m_root{};
		GameObject* m_menuRoot{};
		RenderComponent* m_startArrow{};
		RenderComponent* m_modeArrow{};
		GameObject* m_modeLabelRoot{};
		GameObject* m_scoreRoot{};
	};
}
