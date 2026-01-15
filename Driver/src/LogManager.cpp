#include "LogManager.h"

std::string OUTPUT_PATH = "C:\\OpenVRConduit\\log_OpenVRConduit.log";

std::ofstream LogManager::logFile;
bool LogManager::initialized = false;

bool LogManager::initialize() {
    // Create directory if required
    std::filesystem::path logPath(OUTPUT_PATH);
    std::filesystem::path logDir = logPath.parent_path();
    
    try {
        if (!logDir.empty() && !std::filesystem::exists(logDir)) {
            std::filesystem::create_directories(logDir);
        }
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }

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