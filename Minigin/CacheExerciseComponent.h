#pragma once
#include "BaseComponent.h"
#include <vector>

namespace dae
{
	class CacheExerciseComponent final : public BaseComponent
	{
	public:
		CacheExerciseComponent(GameObject* pOwner);
		void Update(float) override {}
		void Render() const override;

	private:
		static constexpr int STEP_COUNT = 11;
		// Precomputed x-axis step sizes: 1, 2, 4, ..., 1024
		static constexpr float STEP_SIZES[STEP_COUNT] = { 
			1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024
		};

		// Exercise 1 state
		mutable int m_Ex1Samples{ 10 };
		mutable std::vector<float> m_Ex1Results{};

		// Exercise 2 state
		mutable int m_Ex2Samples{ 100 };
		mutable std::vector<float> m_Ex2Results3D{};
		mutable std::vector<float> m_Ex2Results3DAlt{};

		void RenderExercise1() const;
		void RenderExercise2() const;

		static std::vector<float> BenchmarkIntegers(int samples);
		static std::vector<float> BenchmarkGameObject3D(int samples);
		static std::vector<float> BenchmarkGameObject3DAlt(int samples);
	};
}