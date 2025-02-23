#pragma once
#include "Displayer.h"
#include "ThreadManager.h"
#include "Engine.h"
class ConsoleThread : public ThreadManager, public EngineComponent {
protected:
	class RenderHandler : public ThreadHandler {};
	class InputWorkerHandler : public ThreadHandler {};
    Logger logger;
public:
    void Init(ConsoleEngine* source, void* args) override;
    void Start() override;
    void Supervisor();
    void CreateSystemThread();
};