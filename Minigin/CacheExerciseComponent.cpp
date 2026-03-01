#include "CacheExerciseComponent.h"
#include "GameObject.h"

#include <imgui.h>
#include <imgui_plot.h>

#include <chrono>
#include <vector>
#include <algorithm>
#include <numeric>

namespace
{
	struct Transform
	{
		float matrix[16] = {
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};
	};

	class GameObject3D
	{
	public:
		Transform* transform{};
		int id{};
	};

	class GameObject3DAlt
	{
	public:
		Transform transform{};
		int ID{};
	};

	constexpr int ARR_SIZE = 1 << 26; // 2^26

	// Runs `samples` iterations, removes top/bottom outliers, returns the average in microseconds
	std::vector<float> RunBenchmark(int samples, int stepCount, auto benchmarkStep)
	{
		std::vector<float> results;
		results.reserve(stepCount);

		for (int s = 0; s < stepCount; ++s)
		{
			const int stepsize = 1 << s;
			std::vector<long long> timings(samples);

			for (int i = 0; i < samples; ++i)
			{
				timings[i] = benchmarkStep(stepsize);
			}

			std::sort(timings.begin(), timings.end());

			// Remove top and bottom outliers
			const int trimCount = (samples > 2) ? 1 : 0;

			long long sum = 0;
			int count = 0;
			for (int i = trimCount; i < samples - trimCount; ++i)
			{
				sum += timings[i];
				++count;
			}

			results.push_back(count > 0 ? static_cast<float>(sum) / static_cast<float>(count) : 0.f);
		}

		return results;
	}
}

dae::CacheExerciseComponent::CacheExerciseComponent(GameObject* pOwner)
	: BaseComponent(pOwner)
{
}

void dae::CacheExerciseComponent::Render() const
{
	RenderExercise1();
	RenderExercise2();
}

// -- Exercise 1 ----------------------------------------

void dae::CacheExerciseComponent::RenderExercise1() const
{
	ImGui::Begin("Exercise 1");

	ImGui::PushItemWidth(80);
	ImGui::InputInt("##samples_ex1", &m_Ex1Samples, 0);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("-##ex1")) { if (m_Ex1Samples > 1) --m_Ex1Samples; }
	ImGui::SameLine();
	if (ImGui::Button("+##ex1")) ++m_Ex1Samples;
	ImGui::SameLine();
	ImGui::Text("# samples");

	if (ImGui::Button("Trash the cache"))
	{
		m_Ex1Results = BenchmarkIntegers(m_Ex1Samples);
	}

	if (!m_Ex1Results.empty())
	{
		ImU32 color = ImColor(255, 165, 0); // Orange

		ImGui::PlotConfig conf{};
		conf.values.xs = STEP_SIZES;
		conf.values.ys = m_Ex1Results.data();
		conf.values.count = STEP_COUNT;
		conf.values.colors = &color;
		conf.scale.min = *std::min_element(m_Ex1Results.begin(), m_Ex1Results.end());
		conf.scale.max = *std::max_element(m_Ex1Results.begin(), m_Ex1Results.end());
		conf.tooltip.show = true;
		conf.frame_size = ImVec2(250, 120);
		conf.line_thickness = 2.f;
		ImGui::Plot("##ex1_plot", conf);
	}

	ImGui::End();
}

// -- Exercise 2 ----------------------------------------

void dae::CacheExerciseComponent::RenderExercise2() const
{
	ImGui::Begin("Exercise 2");

	ImGui::PushItemWidth(80);
	ImGui::InputInt("##samples_ex2", &m_Ex2Samples, 0);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("-##ex2")) { if (m_Ex2Samples > 1) --m_Ex2Samples; }
	ImGui::SameLine();
	if (ImGui::Button("+##ex2")) ++m_Ex2Samples;
	ImGui::SameLine();
	ImGui::Text("# samples");

	// --- GameObject3D ---
	if (ImGui::Button("Trash the cache with GameObject3D"))
	{
		m_Ex2Results3D = BenchmarkGameObject3D(m_Ex2Samples);
	}

	if (!m_Ex2Results3D.empty())
	{
		ImU32 color = ImColor(0, 200, 0); // Green

		ImGui::PlotConfig conf{};
		conf.values.xs = STEP_SIZES;
		conf.values.ys = m_Ex2Results3D.data();
		conf.values.count = STEP_COUNT;
		conf.values.colors = &color;
		conf.scale.min = *std::min_element(m_Ex2Results3D.begin(), m_Ex2Results3D.end());
		conf.scale.max = *std::max_element(m_Ex2Results3D.begin(), m_Ex2Results3D.end());
		conf.tooltip.show = true;
		conf.frame_size = ImVec2(250, 120);
		conf.line_thickness = 2.f;
		ImGui::Plot("##ex2_3d_plot", conf);
	}

	// --- GameObject3DAlt ---
	if (ImGui::Button("Trash the cache with GameObject3DAlt"))
	{
		m_Ex2Results3DAlt = BenchmarkGameObject3DAlt(m_Ex2Samples);
	}

	if (!m_Ex2Results3DAlt.empty())
	{
		ImU32 color = ImColor(0, 200, 200); // Teal

		ImGui::PlotConfig conf{};
		conf.values.xs = STEP_SIZES;
		conf.values.ys = m_Ex2Results3DAlt.data();
		conf.values.count = STEP_COUNT;
		conf.values.colors = &color;
		conf.scale.min = *std::min_element(m_Ex2Results3DAlt.begin(), m_Ex2Results3DAlt.end());
		conf.scale.max = *std::max_element(m_Ex2Results3DAlt.begin(), m_Ex2Results3DAlt.end());
		conf.tooltip.show = true;
		conf.frame_size = ImVec2(250, 120);
		conf.line_thickness = 2.f;
		ImGui::Plot("##ex2_3dalt_plot", conf);
	}

	// --- Combined ---
	if (!m_Ex2Results3D.empty() && !m_Ex2Results3DAlt.empty())
	{
		ImGui::Text("Combined:");

		const float* ysList[] = { m_Ex2Results3D.data(), m_Ex2Results3DAlt.data() };
		ImU32 colors[] = { ImColor(0, 200, 0), ImColor(0, 200, 200) }; // Green, Teal

		const float combinedMin = std::min(
			*std::min_element(m_Ex2Results3D.begin(), m_Ex2Results3D.end()),
			*std::min_element(m_Ex2Results3DAlt.begin(), m_Ex2Results3DAlt.end())
		);
		const float combinedMax = std::max(
			*std::max_element(m_Ex2Results3D.begin(), m_Ex2Results3D.end()),
			*std::max_element(m_Ex2Results3DAlt.begin(), m_Ex2Results3DAlt.end())
		);

		ImGui::PlotConfig conf{};
		conf.values.xs = STEP_SIZES;
		conf.values.ys_list = ysList;
		conf.values.ys_count = 2;
		conf.values.count = STEP_COUNT;
		conf.values.colors = colors;
		conf.scale.min = combinedMin;
		conf.scale.max = combinedMax;
		conf.tooltip.show = true;
		conf.frame_size = ImVec2(250, 120);
		conf.line_thickness = 2.f;
		ImGui::Plot("##ex2_combined_plot", conf);
	}

	ImGui::End();
}

// --- Benchmark implementations ----------------------------------------

std::vector<float> dae::CacheExerciseComponent::BenchmarkIntegers(int samples)
{
	std::vector<int> arr(ARR_SIZE);

	return RunBenchmark(samples, STEP_COUNT, [&](int stepsize) -> long long
		{
			auto start = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < ARR_SIZE; i += stepsize)
			{
				arr[i] *= 2;
			}
			auto end = std::chrono::high_resolution_clock::now();
			return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		});
}

std::vector<float> dae::CacheExerciseComponent::BenchmarkGameObject3D(int samples)
{
	std::vector<GameObject3D> arr(ARR_SIZE);

	return RunBenchmark(samples, STEP_COUNT, [&](int stepsize) -> long long
		{
			auto start = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < ARR_SIZE; i += stepsize)
			{
				arr[i].id *= 2;
			}
			auto end = std::chrono::high_resolution_clock::now();
			return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		});
}

std::vector<float> dae::CacheExerciseComponent::BenchmarkGameObject3DAlt(int samples)
{
	std::vector<GameObject3DAlt> arr(ARR_SIZE);

	return RunBenchmark(samples, STEP_COUNT, [&](int stepsize) -> long long
		{
			auto start = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < ARR_SIZE; i += stepsize)
			{
				arr[i].ID *= 2;
			}
			auto end = std::chrono::high_resolution_clock::now();
			return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		});
}