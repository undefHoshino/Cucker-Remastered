#pragma once
#include <io.h>       // For _setmode()
#include "Date.h"
#include "Engine.h"

class Console : public EngineComponent {
private:
	Logger logger;
public:
	virtual void Init(ConsoleEngine* source, void* args) override;
	virtual void redirectIOStream();
	virtual void redirectLogStream();
	virtual void setMode();
	bool setConsoleVirtual();
};