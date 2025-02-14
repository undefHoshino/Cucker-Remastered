#pragma once
#include "Engine.h"

class Console : public ConsoleEngine::Component {
private:
	Logger logger;
public:
	virtual void Init(ConsoleEngine& source) override;
	virtual void redirectIOStream();
	virtual void redirectLogStream();
	virtual void setMode();
	bool setConsoleVirtual();
};