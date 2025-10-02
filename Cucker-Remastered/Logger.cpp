#include "Logger.h"

LogStream Logger::ILogSink::logStream;

bool Logger::ILogSink::withANSI = false;

Pixel Logger::LogOverlay::logColors[5] = {
    {{ 180,180,180,255},{255,255,255,255}},       // Debug
    {{ 0,120,180,255 }, { 75,188,255,255 }},      // Info
    {{ 180,150,0,255 }, { 255,235,85,255 }},      // Warning
    {{ 160,20,20,255 }, { 255,75,50,255 }},       // Error
    {{ 160,0,0,255 }, { 255,0,0,255 }}			 // Fatal
};
std::deque<Logger::LogOverlay::LogEntry> Logger::LogOverlay::logEntries;
std::mutex Logger::LogOverlay::dataMute;
std::once_flag Logger::LogOverlay::initFlag;
Timer Logger::LogOverlay::tickTimer;
int Logger::LogOverlay::MaxTick = 1600;