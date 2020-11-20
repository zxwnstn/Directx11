#pragma once

#define MAXIMUM_JOINTS 100
#define MAXIMUM_PART   12

//TODO create option cmake
#define ASSERT_ENABLE
#define LOGGING_MISC
#ifdef _DEBUG
	#define DEBUG_FEATURE
#endif

#ifdef DEBUG_FEATURE		
	#define DEBUG_CALL(Func, ...) Func(__VA_ARGS__);
#else
	#define DEBUG_CALL(Func, ...) ;
#endif

//Assert
#ifdef ASSERT_ENABLE
	#define ASSERT(x, ...) { if(!(x)) { LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define ASSERT(x, ...)     ;
#endif

#define BIT(x) 1 << x

//loggings
#ifdef LOGGING_MISC
	#define LOG_MISC(...)      ::Engine::Log::GetCFLogger()->trace(__VA_ARGS__);
	#define CLOG_MISC(...)     ::Engine::Log::GetCLogger()->trace(__VA_ARGS__);
#else
	#define LOG_MISC(...)     ;
	#define CLOG_MISC(...)    ;
#endif

#define LOCATE_DETAIL	   ::Engine::Log::GetNoFormat()->debug("-> {0} on {1} line{2}", __FUNCTION__, __FILENAME__, __LINE__);
#define ENABLE_ELAPSE		Timestep time;
#define LOG_ELAPSE			LOG_MISC("Complete {0}sec...", time.elapse());
#define CLOG_ELAPSE			CLOG_MISC("Complete {0}sec...", time.elapse());

// Target CF logger
#define LOG_TRACE(...)    ::Engine::Log::GetCFLogger()->trace(__VA_ARGS__);
#define LOG_INFO(...)     ::Engine::Log::GetCFLogger()->info(__VA_ARGS__);
#define LOG_WARN(...)     ::Engine::Log::GetCFLogger()->warn(__VA_ARGS__);
#define LOG_ERROR(...)    ::Engine::Log::GetCFLogger()->error(__VA_ARGS__);		LOCATE_DETAIL
#define LOG_CRITICAL(...) ::Engine::Log::GetCFLogger()->critical(__VA_ARGS__);

// Target C logger
#define CLOG_TRACE(...)    ::Engine::Log::GetCLogger()->trace(__VA_ARGS__);
#define CLOG_INFO(...)     ::Engine::Log::GetCLogger()->info(__VA_ARGS__);
#define CLOG_WARN(...)     ::Engine::Log::GetCLogger()->warn(__VA_ARGS__);
#define CLOG_ERROR(...)    ::Engine::Log::GetCLogger()->error(__VA_ARGS__);
#define CLOG_CRITICAL(...) ::Engine::Log::GetCLogger()->critical(__VA_ARGS__);