#include "Logger.h"

std::mutex logMutex;

std::wstring LogLevelToString(LogLevel level) {
    switch (level) {
    case EMPTY:
        return L"";
    case INFO:
        return L"INFO";
    case DBG:
        return L"DEBUG";
    case WARNING:
        return L"WARNING";
    default:
        return L"UNKNOWN";
    }
}

void Log(const std::wstring& message, LogLevel level) {
    if (level == DBG && !g_DebugModeEnable) {
        return;
    }

    std::lock_guard<std::mutex> lock(logMutex);
    
    std::wstring levelPrefix;
    if (level != EMPTY) {
        levelPrefix = L"[" + LogLevelToString(level) + L"] ";
    }

    if (!g_LogFileName.empty()) {
        try {
            FILE* logFile = nullptr;
            errno_t err = _wfopen_s(&logFile, g_LogFileName.c_str(), L"a, ccs=UTF-16LE");
            if (logFile == nullptr || err != 0) {
                std::wcerr << L"Can't create or open logfile: " << g_LogFileName << L" (Error: " << err << L")" << std::endl;
                std::wcout << levelPrefix << message << std::endl;
                return;
            }
            
            auto fileGuard = std::unique_ptr<FILE, decltype(&fclose)>(logFile, fclose);
            _setmode(_fileno(logFile), _O_U16TEXT);
        
            fwprintf(logFile, L"%ls%ls\n", levelPrefix.c_str(), message.c_str());
        }
        catch (const std::exception& e) {
            std::wcerr << L"Error writing to log file: " << 
                std::wstring(e.what(), e.what() + strlen(e.what())) << std::endl;
            
            std::wcout << levelPrefix << message << std::endl;
        }
    }
    else {
        std::wcout << levelPrefix << message << std::endl;
    }
}
