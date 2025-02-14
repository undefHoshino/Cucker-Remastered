#pragma once
#include "AdofaiTweaksChartScreen.h"
#include "Widget.h"
#include "Displayer.h"

class AdofaiSetupScreen : public Interface {
private:
    /// <summary>
    /// Must: SetParent();
    /// </summary>
    class ParentScreen : public Interface {
    protected:
        AdofaiSetupScreen* parent = nullptr;
    public:
        void SetParent(AdofaiSetupScreen* parent) {
            this->parent = parent;
        }
    };

    class SetupScreen : public ParentScreen {
    private:
        Pixel textColor = { {},{255,255,255,255} };
        NoobButton button;
        NoobButton chartButton;
        NoobButton settingButton;
        TextBox textBox;
        Label errorLabel;
        bool isReady = false;
    public:
        void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override {
            Interface::Creation(screen, canvas, logger, display);
            button.Init();
            button.SetProperties(new NoobButton::Properties(0, 0, 10, 3, "Build!"));
            button.addEvent(NoobButton::onMousePressed, [this, logger](void* args, ActionableWidget* widget) {
                try {
                    BuildChart();
                }
                catch (std::exception e) {
                    logger->error(e.what());
                }
            });

            settingButton.Init();
            settingButton.SetProperties(new NoobButton::Properties(0,0,10,3,"Settings"));
            settingButton.addEvent(NoobButton::onMousePressed, [this](void* args, ActionableWidget* widget) {
                parent->ifManager.activateInterface("Setup_Setting");
            });

            chartButton.Init();
            chartButton.SetProperties(new NoobButton::Properties(0, 0, 10, 3, "Chart"));
            chartButton.addEvent(NoobButton::onMousePressed, [this](void* args, ActionableWidget* widget) {
                if (!isReady) return;
                parent->NavigateToTweakScreen();
            });

            textBox.Init();
            textBox.SetProperties(new TextBox::Properties(0, 0, 60, 1));
            textBox.SetMaxLine(1);
        }
        void BuildChart() {
            auto& setting = parent->loaderSetting;
            Timer timer;
            timer.begin();
            if(!parent->chartJsonManager->AddChart(textBox.GetString(), *setting)) return;

            int64_t costTime = timer.elapsed();
            logger->info("Build Complete in ", costTime, "ms");
            parent->NavigateToTweakScreen();
            isReady = true;
        }
        void Render() override {
            PosCalculator posCalc;
            auto& buttonProp = *button.getProperties<NoobButton::Properties>();
            auto& textBoxProp = *textBox.getProperties<TextBox::Properties>();
            posCalc.setRect({ 0,0,screen->getWidth(),screen->getHeight() });
            std::string title = "Adofai Chart Tweaks";
            canvas->DrawString(posCalc.alignCenterH(title.length()), posCalc.alignCenterV() - 4, title, textColor);

            button.SetWidgetRectPosition(posCalc.alignCenterH(10), posCalc.alignCenterV() + 2);
            settingButton.SetWidgetRectPosition(2, screen->getHeight() - 4);
            textBox.SetWidgetRectPosition(posCalc.alignCenterH(60), posCalc.alignCenterV());
            chartButton.SetWidgetRectPosition(14, screen->getHeight() - 4);

            button.Render(*screen, *canvas);
            textBox.Render(*screen, *canvas);
            settingButton.Render(*screen, *canvas);
            if (isReady) chartButton.Render(*screen, *canvas);
        }
        void Mouse(MouseEventArgs args) override {
            button.Mouse(args);
            textBox.Mouse(args);
            settingButton.Mouse(args);
            if (isReady) chartButton.Mouse(args);
        }
        void Key(KeyEventArgs args) override {
            textBox.Key(args);
        }
    };

    class SetupSettingScreen : public ParentScreen {
    private:
        Pixel textColor = { {},{255,255,255,255} };
        RadioButton isRemoveBOMButton;
        RadioButton clearAnsiCharButton;
        RadioButton fixTrailingCommasButton;
        RadioButton fixTailCommasButton;
        RadioButton readbyUtf16Button;
        RadioButton jsonVerifyButton;
    public:
        void InitRadioButton(RadioButton& button,int x,int y,bool toggle) {
            button.Init();
            button.SetProperties(new RadioButton::Properties(x, y));
            button.setStateToggle(toggle);
        }
        void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override {
            Interface::Creation(screen, canvas, logger, display);
            InitRadioButton(isRemoveBOMButton, 16, 6, true);
            InitRadioButton(clearAnsiCharButton, 20, 8, false);
            InitRadioButton(fixTrailingCommasButton, 20, 10, false);
            InitRadioButton(fixTailCommasButton, 26, 12, false);
            InitRadioButton(readbyUtf16Button, 20, 14, false);

            InitRadioButton(jsonVerifyButton, 30, 18, true);
        }
        void Render() override {
            canvas->FillRect(0, 0, screen->getWidth(), 3, { {200,200,200,50},{} });
            canvas->DrawString(2, 1, "Setting", textColor);
            canvas->DrawString(2, 4, "Json Content", textColor);
            canvas->DrawString(4, 6, "ÒÆ³ý BOM: ", textColor);
            canvas->DrawString(4, 8, "ÇåÀí¿ØÖÆ×Ö·û: ", textColor);
            canvas->DrawString(4, 10, "ÒÆ³ýÎ²Ëæ¶ººÅ: ", textColor);
            canvas->DrawString(4, 12, "ÐÞ¸´Î²²¿È±Ê§µÄ¶ººÅ: ", textColor);
            canvas->DrawString(4, 14, "ÒÔUTF-16¶ÁÈ¡: ", textColor);

            canvas->DrawString(2, 16, "Json Parse", textColor);
            canvas->DrawString(4, 18, "Json ÄÚÈÝÑéÖ¤ (      ): ", textColor);
            canvas->DrawString(19, 18, "¾¯¸æ£¡", { {},{255,25,25,255} });

            canvas->DrawString(0, screen->getHeight() - 1, "Esc ·µ»Ø", textColor);
            isRemoveBOMButton.Render(*screen,*canvas);
            clearAnsiCharButton.Render(*screen, *canvas);
            fixTrailingCommasButton.Render(*screen, *canvas);
            fixTailCommasButton.Render(*screen, *canvas);
            readbyUtf16Button.Render(*screen, *canvas);

            jsonVerifyButton.Render(*screen, *canvas);
        }
        void Mouse(MouseEventArgs args) override {
            isRemoveBOMButton.Mouse(args);
            clearAnsiCharButton.Mouse(args);
            fixTrailingCommasButton.Mouse(args);
            fixTailCommasButton.Mouse(args);
            readbyUtf16Button.Mouse(args);

            jsonVerifyButton.Mouse(args);
        }
        void Key(KeyEventArgs args) override {
            if (args.keyPressed) {
                if (args.keyCode == VirtualKeyCode::Escape) {
                    Config();
                    parent->ifManager.activateInterface("Setup");
                }
            }
        }
        void Config() {
            parent->loaderSetting->removeBOM = isRemoveBOMButton.getStateToggle();
            parent->loaderSetting->clearControlChar = clearAnsiCharButton.getStateToggle();
            parent->loaderSetting->fixTrailingCommas = fixTrailingCommasButton.getStateToggle();
            parent->loaderSetting->fixTailCommas = fixTailCommasButton.getStateToggle();
            parent->loaderSetting->readByUtf16 = readbyUtf16Button.getStateToggle();
            AdofaiChartParser::JsonValueAccessor::SetVerify(jsonVerifyButton.getStateToggle());
        }
    };

    friend class SetupScreen;
    friend class SetupSettingScreen;

    Pixel textColor = { {},{255,255,255,255} };

    FpsOverlay fpsOverlay;
    InterfaceManager<std::string> ifManager;

    AdofaiChartJsonManager* chartJsonManager = nullptr;
    AdofaiChartJson::LoaderSetting* loaderSetting = nullptr;
    SetupScreen setupScreen;
    SetupSettingScreen setupSettingScreen;
public:
    AdofaiSetupScreen() {};
    void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override {
        Interface::Creation(screen, canvas, logger, display);
        ifManager.addInterface("Setup", &setupScreen);
        ifManager.addInterface("Setup_Setting", &setupSettingScreen);
        ifManager.addDeque("Setup");
        ifManager.addDeque("Setup_Setting");
        ifManager.UsingInvoke(0);
        ifManager.Creation(screen, canvas, logger, display);
        ifManager.UsingInvoke(1);
        ifManager.activateInterface("Setup");

        setupScreen.SetParent(this);
        setupSettingScreen.SetParent(this);
    }
    void Load() override {
        auto* screen = static_cast<AdofaiTweaksChartScreen*>(displayer->Get("AdofaiTweaksChartScreen"));
        chartJsonManager = screen->GetChartManager();
        loaderSetting = screen->getLoaderSetting();
    }
    void Render() override {
        ifManager.Render();
        fpsOverlay.Render(*screen, *canvas);
    }
    void Mouse(MouseEventArgs args) override {
        ifManager.Mouse(args);
    }
    void Key(KeyEventArgs args) override {
        ifManager.Key(args);
    }

    void NavigateToTweakScreen() {
        displayer->Navigate("AdofaiTweaksChartScreen"); // AdofaiTweaksChartScreen
    }
};

AdofaiSetupScreen* MakeAdofaiSetupScreen();