#include "LogManager.h"

std::string OUTPUT_PATH = "C:\\OpenVRConduit\\log_OpenVRConduit.log";

std::ofstream LogManager::logFile;
bool LogManager::initialized = false;

bool LogManager::initialize() {
    // Opens the log file
	logFile.open(OUTPUT_PATH, std::ios::out | std::ios::trunc);
	if (!logFile.is_open()) {
		return false;
	}

    // Redirects cerr, clog, and cout
	std::cout.rdbuf(logFile.rdbuf());
	std::clog.rdbuf(logFile.rdbuf());
	std::cerr.rdbuf(logFile.rdbuf());

    // Report a successful initialization
	initialized = true;
	return true;
}

void LogManager::shutdown() {
	logFile.flush();
	logFile.close();
}