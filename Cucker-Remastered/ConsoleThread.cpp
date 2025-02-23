#include "ConsoleThread.h"

void ConsoleThread::Init(ConsoleEngine* source, void* args) {
    EngineComponent::Init(source, args);
    logger.SetClassName("ConsoleThread");
}

void ConsoleThread::Start() {
    CreateSystemThread();
}

void ConsoleThread::Supervisor() {
    if (CheckConsoleInstance()) return;

    Stop();
    if (RestoreConsoleInstance()) {
        CreateSystemThread();
    }
    else {
        throw ReportException("Console Terminated.");
    }
}

void ConsoleThread::CreateSystemThread() {
    auto& displayer = parent->GetComponent<Displayer>();
    addSystemThread<RenderHandler>("Render", [this, &displayer](ThreadHandler& handler) -> int {
        while (!handler.shouldStop()) {
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
        return -1;
    });
    addSystemThread<InputWorkerHandler>("InputWorker", [this, &displayer] (ThreadHandler& handler) -> int {
        DWORD EventMAX = $CSetting.inputEventMAX;
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        INPUT_RECORD InputRecord;
        DWORD eventsRead;
        DWORD NumberOfEvents = 0;
        while (ReadConsoleInputW(hInput, &InputRecord, 1, &eventsRead)) {
            if (handler.shouldStop()) return 0;
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

        return -1;
    });
}