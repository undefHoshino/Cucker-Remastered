#pragma once
#include "Displayer.h"
#include "ThreadManager.h"
#include "Engine.h"
class ConsoleThread : public ThreadManager, public ConsoleEngine::Component {
protected:
	class RenderHandler : public ThreadHandler {};
	class InputWorkerHandler : public ThreadHandler {};
    Logger logger;
public:
	void Init(ConsoleEngine& source) override {
		ConsoleEngine::Component::Init(source);
        logger.SetClassName("ConsoleThread");
	}
    void Supervisor() {
        if (CheckConsoleInstance()) return;

        Stop();
        if (RestoreConsoleInstance()) {
            CreateSystemThread();
        }
        else {
            throw ReportException("Console Terminated.");
        }
    }
    void CreateSystemThread() {
        auto& displayer = parent->GetComponent<Displayer>();
        addSystemThread<RenderHandler>("Render", [this, &displayer](std::shared_ptr<ThreadHandler>handler) {
            while (handler->getSignal() != ThreadManager::Signal::Exit) {
                try {
                    displayer.Render();
                }
                catch (const std::exception& ex) {
                    logger.error(ex.what());
                }
                catch (...) {
                    logger.error("Unknown exception caught");
                }
            }
            });
        addSystemThread<InputWorkerHandler>("InputWorker", [this, &displayer](std::shared_ptr<ThreadHandler>handler) {
            DWORD EventMAX = ConsoleSetting().inputEventMAX;
            HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
            INPUT_RECORD InputRecord;
            DWORD eventsRead;
            DWORD NumberOfEvents = 0;
            while (ReadConsoleInputW(hInput, &InputRecord, 1, &eventsRead)) {
                try {
                    GetNumberOfConsoleInputEvents(hInput, &NumberOfEvents);
                    if (NumberOfEvents >= EventMAX) {
                        FlushConsoleInputBuffer(hInput);
                        displayer.logOverlay.error("Events Overflow.");
                    }
                    switch (InputRecord.EventType) {
                    case MOUSE_EVENT:
                        displayer.SendInputArgs<MouseEventArgs>(MouseEventHandler::Translate(InputRecord.Event.MouseEvent));
                        break;
                    case KEY_EVENT:
                        displayer.SendInputArgs<KeyEventArgs>(KeyEventHandler::Translate(InputRecord.Event.KeyEvent));
                        break;
                    case FOCUS_EVENT:
                        displayer.SendInputArgs<FocusEventArgs>(FocusEventHandler::Translate(InputRecord.Event.FocusEvent));
                        //if (!InputRecord.Event.FocusEvent.bSetFocus) {
                        //    view.display().logger.LogInfo("[!] Shrinking memory... ");
                        //    view.FreeMemory();
                        //}
                        break;
                    case MENU_EVENT:
                        displayer.SendInputArgs<MenuEventArgs>(MenuEventHandler::Translate(InputRecord.Event.MenuEvent));
                        break;
                    case WINDOW_BUFFER_SIZE_EVENT:
                        displayer.SendInputArgs<BufferEventArgs>(BufferEventHandler::Translate(InputRecord.Event.WindowBufferSizeEvent));
                        break;
                    }
                }
                catch (const std::exception& ex) {
                    displayer.logOverlay.error(ex.what());
                }
                catch (...) {
                    displayer.logOverlay.error("Error during key.");
                }
            }
            logger.fatal("InputWorker unexpectedly exited");
        });
    }
};