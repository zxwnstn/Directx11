#include "pch.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "Common/Timestep.h"
#include "File/FileCommon.h"
#include "Log.h"

namespace Engine {

	std::shared_ptr<spdlog::logger> Log::s_CFLogger;
	std::shared_ptr<spdlog::logger> Log::s_CLogger;
	std::shared_ptr<spdlog::logger> Log::s_NoFormat;

	void Engine::Log::Init()
	{
		auto logpath = File::GetCommonPath(File::Log);
		File::TryCreateDir(logpath);
		auto filename = Time::Hour_Min_Sec() + ".log";
		auto logfile = logpath + '/' + filename;
		File::TryCreateFile(logfile);

		std::vector<spdlog::sink_ptr> sink;
		std::vector<spdlog::sink_ptr> sink2;
		
		sink.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());			
		sink.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile, true));
		sink2.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		sink2.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(logfile, true));

		sink[0]->set_pattern("%^[%T] %v%$");
		sink[1]->set_pattern("[%T][%l] %v");
		sink2[0]->set_pattern("%^%v%$");
		sink2[1]->set_pattern("%v");

		s_CFLogger = std::make_shared<spdlog::logger>("Log", sink.begin(), sink.end());
		spdlog::register_logger(s_CFLogger);
		s_CFLogger->set_level(spdlog::level::trace);
		s_CFLogger->flush_on(spdlog::level::trace);

		s_NoFormat = std::make_shared<spdlog::logger>("NoFormat", sink2.begin(), sink2.end());
		spdlog::register_logger(s_NoFormat);
		s_NoFormat->set_level(spdlog::level::trace);
		s_NoFormat->flush_on(spdlog::level::trace);

		s_CLogger = std::make_shared<spdlog::logger>("Console", sink[0]);
		spdlog::register_logger(s_CLogger);
		s_CLogger->set_level(spdlog::level::trace);
		s_CLogger->flush_on(spdlog::level::trace);

		LOG_INFO("Current log file : {0}\n", filename);
	}
}


