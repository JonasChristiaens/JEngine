#pragma once

namespace dae
{
	class State
	{
	public:
		virtual ~State() = default;

		virtual void OnEnter() {}
		virtual void OnExit() {}
		virtual void HandleInput() {}
		virtual void Update() {}
		virtual void Render() const {}
	};
}
