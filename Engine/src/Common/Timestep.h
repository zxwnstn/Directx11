#pragma once

struct Timestep
{
public:
	//hour, min, minute
	using ClockTime = std::tuple<uint32_t, uint32_t, uint32_t>;

	static void SetTimePoint();
	static float TotalElapse();
	static void Update();
	static ClockTime TotalElapse(bool);
	operator float();
};