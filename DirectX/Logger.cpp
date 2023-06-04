#include "stdafx.h"
#include "Logger.h"

void Logger::Write(const std::string& str) {
	GetInstance()->stream_ << str << std::endl;
}

void Logger::Info(const std::string& str) {
	GetInstance()->stream_ << "Info      : " + str << std::endl;
}

void Logger::Warning(const std::string& str) {
	GetInstance()->stream_ << "Warning   : " + str << std::endl;
}

void Logger::Error(const std::string& str) {
	GetInstance()->stream_ << "Error     : " + str << std::endl;
}

void Logger::Border() {
	GetInstance()->stream_ << "-------------------------------------------------" << std::endl;
}

Logger* Logger::GetInstance() {
	static Logger instance;
	return &instance;
}

Logger::Logger() {
	stream_.open("Log.txt");
}
