#pragma once
namespace dae
{
	template <typename T>
	class Singleton
	{
	public:
		[[nodiscard]] static T& GetInstance()
		{
			static T instance{};
			return instance;
		}

		[[nodiscard]] static bool IsAlive() { return s_Alive; }

		virtual ~Singleton() { s_Alive = false; }
		Singleton(const Singleton& other) = delete;
		Singleton(Singleton&& other) = delete;
		Singleton& operator=(const Singleton& other) = delete;
		Singleton& operator=(Singleton&& other) = delete;

	protected:
		Singleton() { s_Alive = true; }

	private:
		inline static bool s_Alive{ false };
	};
}