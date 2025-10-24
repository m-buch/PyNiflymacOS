/*
	Simple logger for returning messages across the DLL interface
	*/
#include <string>
#include <vector>
#include <cstdarg>
#include <algorithm>
#include <cstring>
#include <cstdio>

namespace niflydll {
	static std::vector<std::string> messageLog;

	void LogInit() {
		messageLog.clear();
	}

	void LogWrite(std::string msg) {
		messageLog.push_back(msg);
	}

        void LogWriteMf(std::string fmt, ...)
        {
                char buf[500];
                va_list args;
                va_start(args, fmt);
                std::string msg = "Info: " + fmt;
                vsnprintf(buf, sizeof(buf), msg.c_str(), args);
                messageLog.push_back(buf);
                va_end(args);
        }

        void LogWriteWf(std::string fmt, ...)
        {
                char buf[500];
                va_list args;
                va_start(args, fmt);
                std::string msg = "WARNING: " + fmt;
                vsnprintf(buf, sizeof(buf), msg.c_str(), args);
                messageLog.push_back(buf);
                va_end(args);
        }

        void LogWriteEf(std::string fmt, ...)
        {
                char buf[500];
                va_list args;
                va_start(args, fmt);
                std::string msg = "ERROR: " + fmt;
                vsnprintf(buf, sizeof(buf), msg.c_str(), args);
                messageLog.push_back(buf);
                va_end(args);
        }

	int LogGetLen() {
		int len = 0;
		for (std::string s : messageLog) {
			len += int(s.size() + 1);
		}
		return len;
	}

        int LogGet(char* buf, int len) {
                if (!buf || len <= 0) {
                        return 0;
                }

                std::string outStr;
                outStr.reserve(messageLog.size() * 32);
                for (const auto& s : messageLog) {
                        outStr += s;
                        outStr.push_back('\n');
                }

                const size_t capacity = static_cast<size_t>(len - 1);
                const size_t bytesToCopy = std::min(outStr.size(), capacity);

                if (bytesToCopy > 0) {
                        std::memcpy(buf, outStr.data(), bytesToCopy);
                }
                buf[bytesToCopy] = '\0';

                return static_cast<int>(outStr.size());
        }

}