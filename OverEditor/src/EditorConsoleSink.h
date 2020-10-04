#pragma once

#include "Panels/ConsolePanel.h"

#include <spdlog/sinks/base_sink.h>
#include <spdlog/details/null_mutex.h>
#include <mutex>

namespace OverEditor
{
	template<typename Mutex>
	class EditorConsoleSink : public spdlog::sinks::base_sink<Mutex>
	{
	public:
		EditorConsoleSink(ConsolePanel* consolePanel)
			: m_ConsolePanel(consolePanel) {}
	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{

			// log_msg is a struct containing the log entry info like level, timestamp, thread id etc.
			// msg.raw contains pre formatted log

			// If needed (very likely but not mandatory), the sink formats the message before sending it to its final destination:
			spdlog::memory_buf_t formatted;
			base_sink<Mutex>::formatter_->format(msg, formatted);

			m_Message.LoggerName = fmt::to_string(msg.logger_name);
			m_Message.MessageString = fmt::to_string(formatted);
			m_Message.MessageLevel = (ConsolePanel::Level)msg.level;
		}

		void flush_() override
		{
			auto message = CreateRef<ConsolePanel::Message>();
			message->LoggerName = m_Message.LoggerName;
			message->MessageString = m_Message.MessageString;
			message->MessageLevel = m_Message.MessageLevel;
			m_ConsolePanel->AddMessage(message);
		}
	private:
		ConsolePanel* m_ConsolePanel;
		ConsolePanel::Message m_Message;
	};

	using EditorConsoleSink_mt = EditorConsoleSink<std::mutex>;
	using EditorConsoleSink_st = EditorConsoleSink<spdlog::details::null_mutex>;
}
