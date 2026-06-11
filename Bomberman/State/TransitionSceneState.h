#pragma once
#include "SceneState.h"
#include <memory>
#include <string>

namespace dae
{
	class Scene;
	class GameObject;
	class State;

	class TransitionSceneState final : public SceneState
	{
	public:
		TransitionSceneState(SceneStateMachineComponent& owner,
			const std::string& text,
			std::unique_ptr<State> nextState);
		~TransitionSceneState() override;

		void OnEnter() override;
		void OnExit() override;
		void Update() override;
		void Render() const override;

	private:
		void BuildScene();
		void CreateTextSprites(Scene& scene, GameObject& parent, const std::string& text, float startX, float startY);
		void ClearScene();
		std::pair<float, float> GetCharacterSrcRect(char character) const;

		std::string m_Text;
		std::unique_ptr<State> m_NextState;
		bool m_ShouldTransition{ false };
		bool m_SoundPlayed{ false };
		bool m_SoundFinished{ false };
		float m_Elapsed{ 0.0f };
		float m_GraceTimer{ 0.0f };
		GameObject* m_Root{};
		Scene* m_pScene{};
	};
}
