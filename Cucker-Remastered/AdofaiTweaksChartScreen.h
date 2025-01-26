#pragma once
#include "Interface.h"
#include "AdofaiChart.h"
#include "Widget.h"

class AdofaiTweaksChartScreen : public Interface {
private:
    class ChartParentScreen : public Interface {
    protected:
        AdofaiChartJsonManager::AdofaiChartData* chartData;
        size_t chartIndex = 0;
        Pixel textColor = { {},{255,255,255,255} };
        AdofaiTweaksChartScreen* parentScr = nullptr;

    public:
        void setParent(AdofaiTweaksChartScreen* parentScr) {
            this->parentScr = parentScr;
        }
        void setChartJson(AdofaiChartJsonManager::AdofaiChartData* data,size_t index) {
            this->chartData = data;
            this->chartIndex = index;
        }
        void connent(ChartParentScreen* screenA) {
            setChartJson(screenA->getChartJson(), screenA->getChartIndex());
        }
        size_t getChartIndex() {
            return chartIndex;
        }
        AdofaiChartJsonManager::AdofaiChartData* getChartJson() {
            return chartData;
        }
    };

    class ChartScreen : public ChartParentScreen {
    private:
        NoobButton eraseButton;
        NoobButton loadSettingButton;
        int ColumnX = 15;
    public:
        void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Display* display) override {
            Interface::Creation(screen, canvas, logger, display);

            eraseButton.Init();
            eraseButton.SetProperties(new NoobButton::Properties(ColumnX + 2, 10, 10, 3, "Erase"));
            eraseButton.addEvent(NoobButton::onMousePressed, [this, display](void* args, ActionableWidget* widget) {
                parentScr->RemoveChart(chartIndex);
            });
        }
        void Render() override {
            PosCalculator posCalc;
            posCalc.setRect({ ColumnX ,0,screen->getWidth() - ColumnX,screen->getHeight() });
            canvas->DrawString(posCalc.alignCenterH(5), 1, "Chart", textColor);

            if (chartData) {
                DrawOperation();
            }
            else {
                canvas->DrawString(posCalc.alignCenterH(25), posCalc.alignCenterV(), "ChartJson is nullptr!! SB", { {},{255,30,30,255} });
            }
        }
        void DrawOperation() {
            canvas->DrawString(ColumnX + 2, 4, "Chart: " + chartData->chartName, textColor);
            canvas->DrawString(ColumnX + 4, 6, "Index: " + std::to_string(chartIndex), textColor);
            canvas->DrawString(ColumnX + 4, 8, "FilePath: " + chartData->filepath, textColor);

            eraseButton.Render(*screen, *canvas);
        }
        void Mouse(MouseEventArgs args) override {
            if (chartData) {
                eraseButton.Mouse(args);
            }
        }
    };

    class ChartSettingScreen : public ChartParentScreen {
    private:
        int ColumnX = 15;
    public:
        void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Display* display) override {
            Interface::Creation(screen, canvas, logger, display);

        }
        void Render() override {
            PosCalculator posCalc;
            posCalc.setRect({ ColumnX ,0,screen->getWidth() - ColumnX,screen->getHeight() });
            canvas->DrawString(posCalc.alignCenterH(14), 1, "Chart Settings", textColor);

            if (chartData) {
                DrawOperation();
            }
            else {
                canvas->DrawString(posCalc.alignCenterH(25), posCalc.alignCenterV(), "ChartJson is nullptr!! SB", {{},{255,30,30,255}});
            }
        }
        void DrawOperation() {

        }
        void Mouse(MouseEventArgs args) override {
            if (chartData) {
                
            }
        }

    };

    AdofaiChartJsonManager chartManager;

    ChartScreen chartScreen;
    ChartSettingScreen chartSettingScreen;

    InterfaceManager<int> ifManager;
    FpsOverlay fpsOverlay;
    NoobButton backButton;
    NoobButton forwardButton;
    NoobButton backwardButton;
    std::vector<std::shared_ptr<NoobButton>> chartButtons;

    int ScreenId = 0;
    int ScreenMax = 1;

    int ColumnX = 15;
public:
    void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Display* display) override {
        Interface::Creation(screen, canvas, logger, display);
        ifManager.addInterface(0, &chartScreen);
        ifManager.addInterface(1, &chartSettingScreen);
        ifManager.addDeque(0);
        ifManager.addDeque(1);

        ifManager.UsingInvoke(0);
        ifManager.Creation(screen, canvas, logger, display);
        ifManager.UsingInvoke(1);
        ifManager.activateInterface(0);

        chartScreen.setParent(this);

        backButton.Init();
        backButton.SetProperties(new NoobButton::Properties(0, 0, 15, 3, "Back"));
        backButton.addEvent(NoobButton::onMousePressed, [this,display](void* args, ActionableWidget* widget) {
            display->Navigate(3); // AdofaiSetupScreen
        });
        chartManager.SetMaxSize(5);

        forwardButton.Init();
        forwardButton.SetProperties(new NoobButton::Properties(0, 0, 10, 3, "----->"));
        forwardButton.addEvent(NoobButton::onMousePressed, [this, display](void* args, ActionableWidget* widget) {
            ScreenId = std::min(ScreenId + 1, (int)ScreenMax);
            auto* ScreenA = static_cast<ChartParentScreen*>(ifManager.getActivatedInterface());
            auto* ScreenB = static_cast<ChartParentScreen*>(ifManager.getScreen(ScreenId));
            ScreenB->connent(ScreenA);
            ifManager.activateInterface(ScreenId);
        });
        backwardButton.Init();
        backwardButton.SetProperties(new NoobButton::Properties(0, 0, 10, 3, "<-----"));
        backwardButton.addEvent(NoobButton::onMousePressed, [this, display](void* args, ActionableWidget* widget) {
            ScreenId = std::min(ScreenId - 1, (int)ScreenMax);
            auto* ScreenA = static_cast<ChartParentScreen*>(ifManager.getActivatedInterface());
            auto* ScreenB = static_cast<ChartParentScreen*>(ifManager.getScreen(ScreenId));
            ScreenB->connent(ScreenA);
            ifManager.activateInterface(ScreenId);
        });
    }
    void Load() override {
        SetupChartButtons();
    }
    void Render() override {
        canvas->FillRect(ColumnX + 10, 0, screen->getWidth() - 10, 3, { {200,200,200,80},{} });
        canvas->FillRect(0, chartButtons.size() * 3, 15, screen->getHeight() - 3, {{200,200,200,80},{}});
        backButton.SetWidgetRectPosition(0, screen->getHeight() - 3);
        backButton.Render(*screen, *canvas);

        backwardButton.SetWidgetRectPosition(ColumnX, 0);
        forwardButton.SetWidgetRectPosition(screen->getWidth() - 10, 0);
        backwardButton.Render(*screen, *canvas);
        forwardButton.Render(*screen, *canvas);
        for (auto& button : chartButtons) {
            button->Render(*screen, *canvas);
        }
        ifManager.Render();
        fpsOverlay.Render(*screen, *canvas);
    }
    void Mouse(MouseEventArgs args) override {
        backButton.Mouse(args);

        backwardButton.Mouse(args);
        forwardButton.Mouse(args);

        for (auto& button : chartButtons) {
            button->Mouse(args);
        }
        ifManager.Mouse(args);
    }
    void Key(KeyEventArgs args) override {
        if (args.keyPressed) {
            if (args.keyCode == VirtualKeyCode::Escape) {
                display->Navigate(3); // AdofaiSetupScreen
            }
        }
        ifManager.Key(args);
    }
    void SetupChartButtons() {
        size_t size = std::max((int64_t)0, (int64_t)(chartManager.size() - chartButtons.size()));
        for (size_t i = 0; i < size; i++) {
            auto button = std::make_shared<NoobButton>();
            button->Init();
            chartButtons.push_back(button);
        }

        size_t index = 0;
        size_t buttonListY = 0;
        for (auto& button : chartButtons) {
            button->SetProperties(new NoobButton::Properties(0, buttonListY, 15, 3, "Chart #" + std::to_string(buttonListY / 3)));
            button->refreshEvent(NoobButton::onMousePressed);
            button->addEvent(NoobButton::onMousePressed, [this, index](void* args, ActionableWidget* widget) {
                OpenChart(index);
            });
            buttonListY += 3;
            index++;
        }
        if (!chartButtons.empty()) {
            OpenChart(index - 1);
        }
    }
    void OpenChart(size_t index) {
        if (index >= chartManager.size()) throw std::out_of_range("Chart Index out of range");
        auto* iface = ifManager.getActivatedInterface();
        if (iface) {
            static_cast<ChartParentScreen*>(iface)->setChartJson(&chartManager.GetChart(index),index);
        }
    }
    void RemoveChart(size_t index) {
        chartManager.EraseChart(index);
        chartButtons.erase(chartButtons.begin() + index);
        if (chartManager.size() != 0) {
            chartScreen.setChartJson(&chartManager.GetChart(0), 0);
        }
        else {
            chartScreen.setChartJson(nullptr, -1);
        }
        logger->info("Resetting charts");
        SetupChartButtons();
    }
	AdofaiChartJsonManager* GetChartManager() {
		return &chartManager;
	}
};

AdofaiTweaksChartScreen* MakeAdofaiTweaksChartScreen();