#pragma once

class Logger {
public:
	static void Write(const std::string& str);
	static void Info(const std::string& str);
	static void Warning(const std::string& str);
	static void Error(const std::string& str);
	static void Border();

private:
	static Logger* GetInstance();

	Logger();
	~Logger() = default;
	Logger(const Logger&) = delete;
	const Logger& operator=(const Logger&) = delete;

	std::ofstream stream_;
};

