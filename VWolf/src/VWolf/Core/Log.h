#pragma once
#include "Private.h"

namespace VWolf {

	DECLARE_PRIVATE(LogImpl)
	
	class Log {
	public:
		enum class LogSeverity {
			trace,
			debug,
			info,
			warning,
			error,
			fatal
		};
		Log();
		~Log();

		void Write(LogSeverity severity, const std::string& text);

		template<typename ... Args>
		inline void Write(LogSeverity severity, const std::string& format, Args ... args) {
			int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1;
			if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
			auto size = static_cast<size_t>(size_s);
			std::unique_ptr<char[]> buf(new char[size]);
			std::snprintf(buf.get(), size, format.c_str(), args ...);
			auto text = std::string(buf.get(), buf.get() + size - 1);
			this->Write(severity, text);
		}
	public:
		static void setSeverityFilter(LogSeverity severity);
#ifdef VWOLF_CORE
		static void ClearLogObjects();
#endif
	public:
		static Log* coreLog;
		static Log* clientLog;	
	 
	private:
		Private::LogImpl* _log;
	};	
}

#ifdef VWOLF_CORE
#ifdef VWOLF_PLATFORM_WINDOWS
#define VWOLF_CORE_TRACE(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::trace, text, __VA_ARGS__)
#define VWOLF_CORE_DEBUG(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::debug, text, __VA_ARGS__)
#define VWOLF_CORE_INFO(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::info, text, __VA_ARGS__)
#define VWOLF_CORE_WARNING(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::warning, text, __VA_ARGS__)
#define VWOLF_CORE_ERROR(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::error, text, __VA_ARGS__)
#define VWOLF_CORE_FATAL(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::fatal, text, __VA_ARGS__)
#else
#define VWOLF_CORE_TRACE(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::trace, text, ##__VA_ARGS__)
#define VWOLF_CORE_DEBUG(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::debug, text, ##__VA_ARGS__)
#define VWOLF_CORE_INFO(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::info, text, ##__VA_ARGS__)
#define VWOLF_CORE_WARNING(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::warning, text, ##__VA_ARGS__)
#define VWOLF_CORE_ERROR(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::error, text, ##__VA_ARGS__)
#define VWOLF_CORE_FATAL(text, ...) ::VWolf::Log::coreLog->Write(::VWolf::Log::LogSeverity::fatal, text, ##__VA_ARGS__)
#endif
#endif

#ifdef VWOLF_PLATFORM_WINDOWS
#define VWOLF_CLIENT_TRACE(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::trace, text, __VA_ARGS__)
#define VWOLF_CLIENT_DEBUG(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::debug, text, __VA_ARGS__)
#define VWOLF_CLIENT_INFO(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::info, text, __VA_ARGS__)
#define VWOLF_CLIENT_WARNING(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::warning, text, __VA_ARGS__)
#define VWOLF_CLIENT_ERROR(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::error, text, __VA_ARGS__)
#define VWOLF_CLIENT_FATAL(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::fatal, text, __VA_ARGS__)
#else
#define VWOLF_CLIENT_TRACE(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::trace, text, ##__VA_ARGS__)
#define VWOLF_CLIENT_DEBUG(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::debug, text, ##__VA_ARGS__)
#define VWOLF_CLIENT_INFO(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::info, text, ##__VA_ARGS__)
#define VWOLF_CLIENT_WARNING(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::warning, text, ##__VA_ARGS__)
#define VWOLF_CLIENT_ERROR(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::error, text, ##__VA_ARGS__)
#define VWOLF_CLIENT_FATAL(text, ...) ::VWolf::Log::clientLog->Write(::VWolf::Log::LogSeverity::fatal, text, ##__VA_ARGS__)
#endif

#ifdef DEBUG
#define VWOLF_DUMP_ARRAY(array, count) \
std::stringstream ss;\
ss << #array << ": "; \
for (int i = 0; i < count; i++) \
ss << *(array + i) << ", "; \
ss << std::endl;\
VWOLF_CORE_DEBUG(ss.str().c_str())

#define VWOLF_DUMP_TYPED_ARRAY(array, size, type) \
std::stringstream ss;\
ss << #array << ": "; \
for (int i = 0; i < size / sizeof(type); i++) \
ss << *(array + i) << ", "; \
ss << std::endl;\
VWOLF_CORE_DEBUG(ss.str().c_str())

#else

#define VWOLF_DUMP_ARRAY(array, count)
#define VWOLF_DUMP_TYPED_ARRAY(array, size, type)

#endif
