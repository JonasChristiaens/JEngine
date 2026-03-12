#pragma once
#include "BaseComponent.h"
#include <vector>

namespace dae
{
	class CacheExerciseComponent final : public BaseComponent
	{
	public:
		explicit CacheExerciseComponent(GameObject* pOwner);

		void Update() override {}
		void Render() const override;

	private:
		void RenderExercise1() const;
		void RenderExercise2() const;

		static std::vector<float> BenchmarkIntegers(int samples, int bufferPow2);
		static std::vector<float> BenchmarkGameObject3D(int samples, int bufferPow2);
		static std::vector<float> BenchmarkGameObject3DAlt(int samples, int bufferPow2);

		static constexpr int STEP_COUNT = 11; // 1, 2, 4, ..., 1024
		static constexpr float STEP_SIZES[STEP_COUNT] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
		static constexpr int BUFFER_POW2 = 26;

		mutable int m_Ex1Samples{ 10 };
		mutable int m_Ex2Samples{ 100 };

		mutable std::vector<float> m_Ex1Results;
		mutable std::vector<float> m_Ex2Results3D;
		mutable std::vector<float> m_Ex2Results3DAlt;
	};
}