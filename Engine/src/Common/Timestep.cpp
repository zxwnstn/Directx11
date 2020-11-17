#include "pch.h"

#include "Timestep.h"

namespace Engine {

	static std::chrono::system_clock::time_point StartPoint;
	static std::chrono::system_clock::time_point Last;

	void Timestep::SetTimePoint()
	{
		StartPoint = std::chrono::system_clock::now();
		Last = StartPoint;
	}

	Timestep::operator float()
	{
		return Elapse();
	}

	void Timestep::Update()
	{
		Last = std::chrono::system_clock::now();
	}

	float Timestep::TotalElapse()
	{
		std::chrono::duration<float> duration = std::chrono::system_clock::now() - StartPoint;
		return duration.count();
	}

	Timestep::ClockTime Timestep::TotalElapse(bool)
	{
		std::chrono::duration<float> duration = std::chrono::system_clock::now() - StartPoint;
		uint32_t total = uint32_t(duration.count());

		uint32_t sec = total % 60;
		uint32_t minute = total / 60;
		uint32_t hour = total / 3600;

		return { sec, minute, hour };
	}

	float Timestep::Elapse()
	{
		std::chrono::duration<float> duration = std::chrono::system_clock::now() - Last;
		return duration.count();
	}

}
