#include "vwpch.h"

#include "Log.h"

namespace VWolf {

	BEGIN_DEFINE_PRIVATE
		BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", Log::LogSeverity);
		BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime);
		BOOST_LOG_ATTRIBUTE_KEYWORD(log_thread_id, "ThreadID", boost::log::attributes::current_thread_id::value_type);

		class LogImpl {

			class ColorAttributeImpl: public boost::log::attribute::impl {
			public:
				ColorAttributeImpl(Log::LogSeverity severity): severity(severity) {};
				boost::log::attribute_value get_value()
				{
					return boost::log::attributes::make_attribute_value(getColor());
				}
				inline void setColor(Log::LogSeverity severity) {
					this->severity = severity;
				}
			private:
				std::string getColor() {
					switch (severity)
					{
					case Log::LogSeverity::info:
						return "\033[0m";
					case Log::LogSeverity::warning:
						return "\033[33m";
					case Log::LogSeverity::error:
					case Log::LogSeverity::fatal:
						return "\033[31m";
					case Log::LogSeverity::debug:
					case Log::LogSeverity::trace:
						return "\033[32m";
					default:
						break;
					}
					return "\033[0m";
				}
			private:
				Log::LogSeverity severity;
			};

			class ColorAttribute : public boost::log::attribute {
			public:
				ColorAttribute(ColorAttributeImpl* impl) : boost::log::attribute(impl) {  }
				explicit ColorAttribute(boost::log::attributes::cast_source const& source) : boost::log::attribute(source.as<ColorAttributeImpl >())
				{
				}
			};

		public:

			static void Init() {
				if (!initialize) {
					boost::shared_ptr<std::ostream> stream{ &std::cout, boost::null_deleter{} };
					auto loggerSink = boost::make_shared<boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend>>();
					boost::log::add_common_attributes();
					loggerSink->locked_backend()->add_stream(stream);
					loggerSink->locked_backend()->auto_flush(true);
					loggerSink->set_filter(severity >= minSeverity);
					loggerSink->set_formatter(boost::log::expressions::stream
						<< boost::log::expressions::attr<std::string>("Color") << "[" 
						<< boost::log::expressions::format_date_time(timestamp, "%H:%M:%S.%f") << "] ["
						<< std::setw(4) << std::left << severity << "] ["
						<< log_thread_id << "] " <<  "\033[0m"
						<< boost::log::expressions::smessage
					);

					boost::log::core::get()->add_sink(loggerSink);
					initialize = true;
				}
			}
		public:
			LogImpl() {
				LogImpl::Init();
				colorImpl = new ColorAttributeImpl(Log::LogSeverity::debug);
				logger.add_attribute("Color", ColorAttribute(colorImpl));
			}

			void setColor(Log::LogSeverity severity) {
				colorImpl->setColor(severity);
			}
		public:
			static void setSeverityFilter(Log::LogSeverity severity) {
				minSeverity = severity;
			}
		public:
			boost::log::sources::severity_logger<Log::LogSeverity> logger;
			static bool initialize;
		private:
			ColorAttributeImpl* colorImpl;
			static Log::LogSeverity minSeverity;
		};

		bool LogImpl::initialize = false;
		Log::LogSeverity LogImpl::minSeverity = Log::LogSeverity::trace;
	END_DEFINE_PRIVATE

	Log* Log::coreLog = new Log;
	Log* Log::clientLog = new Log;

	Log::Log()
	{
		_log = new Private::LogImpl();
	}

	Log::~Log() {
		delete _log;
	}

	void Log::Write(LogSeverity severity, const std::string& text)
	{		
		_log->setColor(severity);
		auto record = _log->logger.open_record(boost::log::keywords::severity = severity);	
		if (record) {
			boost::log::record_ostream strm(record);
			strm << text;
			_log->logger.push_record(boost::move(record));
		}		
	}

	void Log::setSeverityFilter(LogSeverity severity)
	{
		Private::LogImpl::setSeverityFilter(severity);
	}

	void Log::ClearLogObjects() {
		delete Log::coreLog;
		delete Log::clientLog;
	}

	std::ostream& operator<< (std::ostream& strm, Log::LogSeverity level)
	{
		static const std::array<std::string, 6> strings
		{
			std::string{"trace"},
			std::string{"debug"},
			std::string{"info"},
			std::string{"warning"},
			std::string{"error"},
			std::string{"fatal"}
		};

		strm << strings[static_cast<std::size_t>(level)];

		return strm;
	}
}