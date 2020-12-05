#pragma once

#include <spdlog/spdlog.h>

namespace Engine {

	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<spdlog::logger> GetCFLogger() { return s_CFLogger; }
		static std::shared_ptr<spdlog::logger> GetCLogger() { return s_CLogger; }
		static std::shared_ptr<spdlog::logger> GetNoFormat() { return s_NoFormat; }

	private:
		static std::shared_ptr<spdlog::logger> s_CFLogger;  //target both console and file
		static std::shared_ptr<spdlog::logger> s_CLogger;   //target only console 
		static std::shared_ptr<spdlog::logger> s_NoFormat;  //target only console 
	};

}

