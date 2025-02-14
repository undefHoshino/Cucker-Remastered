#include "LogOverlay.h"

Pixel LogOverlay::colors[5] = {
       {{ 180,180,180,255},{255,255,255,255}},       //Debug
       {{ 0,120,180,255 }, { 75,188,255,255 }},      //Info
       {{ 180,150,0,255 }, { 255,235,85,255 }},      //Warning
       {{ 160,20,20,255 }, { 255,75,50,255 }},       //Error
       {{ 160,0,0,255 }, { 255,0,0,255 }}			  //Fatal
};

void LogOverlay::Render(ScreenA& screen, CanvasA& canvas) {
    if (logEntries.size() == 0)
        return;
    int y = screen.getHeight() - 3;
    int64_t time = tickTimer.elapsed();

    for (auto& logs : logEntries) {
        if (time > logs.endtime)
            continue;
        float Alpha = std::min((static_cast<double>(logs.endtime - time) / static_cast<double>(MaxTick)) * 2, 1.0);
        canvas.DrawString(0, y--, logs.msg, colors[logs.level] * Alpha);
        if (y < 10)
            break;
    }

    Update();
}
void LogOverlay::Update() {
    std::lock_guard<std::mutex> lock(dataMutex);

    if (logEntries.size() == 0) return;
    int64_t time = tickTimer.elapsed();
    while (!logEntries.empty()) {
        if (time > logEntries.back().endtime)
            logEntries.pop_back();
        else
            break;
    }
}