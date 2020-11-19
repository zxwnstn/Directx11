#include "pch.h"

#include "Timestep.h"

namespace Engine {

	static std::chrono::system_clock::time_point ProgramStartPoint;
	static std::chrono::system_clock::time_point StartPoint;
	static std::chrono::system_clock::time_point Last;

	std::time_t Time::GetNow()
	{
		return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	}

	int Time::GetYear()
	{
		tm timeData;
		auto curtime = std::time(nullptr);
		localtime_s(&timeData, &curtime);

		return timeData.tm_year + 1900;
	}

	int Time::GetMon()
	{
		tm timeData;
		auto curtime = std::time(nullptr);
		localtime_s(&timeData, &curtime);
		
		return timeData.tm_mon + 1;
	}

	int Time::GetDay()
	{
		tm timeData;
		auto curtime = std::time(nullptr);
		localtime_s(&timeData, &curtime);

		return timeData.tm_mday;
	}

	std::string Time::Year_Mon_Day()
	{
		tm timeData;
		auto curtime = std::time(nullptr);
		localtime_s(&timeData, &curtime);

		std::stringstream ss;

		ss << timeData.tm_year + 1900 << '-';
		if (timeData.tm_mon < 10)
			ss << '0';
		ss << timeData.tm_mon + 1 << '-';
		if (timeData.tm_mday < 10)
			ss << '0';
		ss << timeData.tm_mday;

		return  ss.str();
	}

	std::string Time::Hour_Min_Sec()
	{
		tm timeData;
		auto curtime = std::time(nullptr);
		localtime_s(&timeData, &curtime);

		std::stringstream ss;
		ss << '#';
		if (timeData.tm_hour < 10)
			ss << '0';
		ss << timeData.tm_hour;
		if (timeData.tm_min < 10)
			ss << '0';
		ss << timeData.tm_min;
		if (timeData.tm_sec < 10)
			ss << '0';
		ss << timeData.tm_sec;
		
		return ss.str();
	}

	void Timestep::Init()
	{
		ProgramStartPoint = std::chrono::system_clock::now();
		SetTimePoint();
	}

	void Timestep::SetTimePoint()
	{
		StartPoint = std::chrono::system_clock::now();
		Last = StartPoint;
	}

	Timestep::operator float()
	{
		return elapse();
	}

	Timestep::Timestep()
	{
		ownLast = std::chrono::system_clock::now();
	}

	float Timestep::elapse(bool beUpdate)
	{
		std::chrono::duration<float> duration = std::chrono::system_clock::now() - ownLast;
		if (beUpdate)
			update();

		return (float)duration.count();
	}

	void Timestep::update()
	{
		ownLast = std::chrono::system_clock::now();
		Last = std::chrono::system_clock::now();
	}

	void Timestep::Update()
	{
		Last = std::chrono::system_clock::now();
	}

	Timestep::ClockTime Timestep::TotalRunTime()
	{
		std::chrono::duration<float> duration = std::chrono::system_clock::now() - ProgramStartPoint;
		uint32_t total = uint32_t(duration.count());

		uint32_t sec = total % 60;
		uint32_t minute = total / 60;
		uint32_t hour = total / 3600;

		return { sec, minute, hour };
	}

	float Timestep::TotalElapse()
	{
		std::chrono::duration<float> duration = std::chrono::system_clock::now() - StartPoint;
		return duration.count();
	}

	float Timestep::Elapse()
	{
		std::chrono::duration<float> duration = std::chrono::system_clock::now() - Last;
		return duration.count();
	}

	

}
