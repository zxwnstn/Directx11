#pragma once

namespace Engine {

	struct Time
	{
		static std::time_t GetNow();
		static int GetYear();
		static int GetMon();
		static int GetDay();

		//return cur date info as string eg. 2020-11-18
		static std::string Year_Mon_Day();

		//return cur clock info as string eg. 23:47:05 
		static std::string Hour_Min_Sec();
	};

	struct Timestep
	{
	public:
		//hour, min, minute
		using ClockTime = std::tuple<uint32_t, uint32_t, uint32_t>;

		static void Init();
		static void SetTimePoint();
		static float TotalElapse();
		static void Update();
		static ClockTime TotalRunTime();
		static float Elapse();

	public:
		//This is Supporting local variable function
		Timestep();
		float elapse(bool update = true);
		void update();
		operator float();

	private:
		std::chrono::system_clock::time_point ownLast; //for local timestep varible
	};

}
