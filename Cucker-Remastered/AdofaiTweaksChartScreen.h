#pragma once
#include "Displayer.h"
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
        NoobButton reloadButton;
        NoobButton writeButton;
        int ColumnX = 15;
    public:
        void Creation(ScreenA* screen, CanvasA* canvas, Logger* logger, Displayer* display) override {
            Interface::Creation(screen, canvas, logger, display);

            eraseButton.Init();
            eraseButton.SetProperties(new NoobButton::Properties(ColumnX + 2, 10, 10, 3, "Erase"));
            eraseButton.addEvent(NoobButton::onMousePressed, [this, display](void* args, ActionableWidget* widget) {
                parentScr->RemoveChart(chartIndex);
            });


            reloadButton.Init();
            reloadButton.SetProperties(new NoobButton::Properties(ColumnX + 14, 10, 10, 3, "Reload"));
            reloadButton.addEvent(NoobButton::onMousePressed, [this, display](void* args, ActionableWidget* widget) {
                parentScr->ReloadChart(chartIndex);
            });

            writeButton.Init();
            writeButton.SetProperties(new NoobButton::Properties(ColumnX + 26, 10, 10, 3, "Write"));
            writeButton.addEvent(NoobButton::onMousePressed, [this, display](void* args, ActionableWidget* widget) {
                parentScr->WriteChart(chartIndex);
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
            reloadButton.Render(*screen, *canvas);
            writeButton.Render(*screen, *canvas);
        }
        void Mouse(MouseEventArgs args) override {
            if (chartData) {
                eraseButton.Mouse(args);
                reloadButton.Mouse(args);
                writeButton.Mouse(args);
            }
        }
    };

    class ChartSettingScreen : public ChartParentScreen {
    private:
        int ColumnX = 15;
    public:
        void Creation(ScreenA* screen, CanvasA* canvas, Logger* logger, Displayer* display) override {
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
            canvas->DrawString(ColumnX + 2, 4, "Not Completed", textColor);
        }
        void Mouse(MouseEventArgs args) override {
            if (chartData) {
                
            }
        }
    };

    class ChartModuleScreen : public ChartParentScreen {
    private:
        int ColumnX = 15;
        NoobButton module1Button;
        NoobButton syncModuleButton;
        NoobButton reverseModuleButton;
        TextBox syncInputArgs1TextBox;
        TextBox syncInputArgs2TextBox;
    public:
        void Creation(ScreenA* screen, CanvasA* canvas, Logger* logger, Displayer* display) override {
            Interface::Creation(screen, canvas, logger, display);
            module1Button.Init();
            module1Button.SetProperties(new NoobButton::Properties(ColumnX + 2, 4, 30, 3, "Replace Effect Position"));
            module1Button.addEvent(NoobButton::onMousePressed, [this, logger](void* args, ActionableWidget* widget) {
                ChartModule::ReplaceEffectPosition(chartData->chart.GetChartDocument());
            });

            syncInputArgs1TextBox.Init();
            syncInputArgs1TextBox.SetProperties(new TextBox::Properties(ColumnX + 18, 10, 30, 1));
            syncInputArgs1TextBox.addEvent(TextBox::onTextboxActive, [this, logger](void* args, ActionableWidget* widget) {
                syncInputArgs2TextBox.getInputControl().disable();
            });
            syncInputArgs1TextBox.SetMaxLine(1);

            syncInputArgs2TextBox.Init();
            syncInputArgs2TextBox.SetProperties(new TextBox::Properties(ColumnX + 16, 12, 30, 1));
            syncInputArgs2TextBox.addEvent(TextBox::onTextboxActive, [this, logger](void* args, ActionableWidget* widget) {
                syncInputArgs1TextBox.getInputControl().disable();
            });
            syncInputArgs2TextBox.SetMaxLine(1);

            syncModuleButton.Init();
            syncModuleButton.SetProperties(new NoobButton::Properties(ColumnX + 2, 14, 10, 3, "Sync!"));
            syncModuleButton.addEvent(NoobButton::onMousePressed, [this, logger](void* args, ActionableWidget* widget) {
                ChartModule::SyncTrackPosOffset(chartData->chart.GetChartDocument(), 
                    std::stoi(syncInputArgs1TextBox.GetString()), std::stoi(syncInputArgs2TextBox.GetString()));
            });

            reverseModuleButton.Init();
            reverseModuleButton.SetProperties(new NoobButton::Properties(ColumnX + 2, 18, 10, 3, "Reverse!"));
            reverseModuleButton.addEvent(NoobButton::onMousePressed, [this, logger](void* args, ActionableWidget* widget) {
                ChartModule::ReverseTrackRotation(chartData->chart.GetChartDocument(),
                    std::stoi(syncInputArgs1TextBox.GetString()), std::stoi(syncInputArgs2TextBox.GetString()));
                });
        }
        void Render() override {
            PosCalculator posCalc;
            posCalc.setRect({ ColumnX ,0,screen->getWidth() - ColumnX,screen->getHeight() });
            canvas->DrawString(posCalc.alignCenterH(21), 1, "Chart Modules (Index)", textColor);

            if (chartData) {
                DrawOperation();
            }
            else {
                canvas->DrawString(posCalc.alignCenterH(25), posCalc.alignCenterV(), "ChartJson is nullptr!! SB", { {},{255,30,30,255} });
            }
        }
        void DrawOperation() {
            canvas->DrawString(ColumnX + 2, 8, "SyncTrackPosOffset & ReverseTrackRotation", textColor);
            canvas->DrawString(ColumnX + 4, 10, "StartFloor: ", textColor);
            canvas->DrawString(ColumnX + 4, 12, "EndFloor: ", textColor);
            canvas->DrawString(ColumnX + 14, 15, "警告：存在漏改误改风险", { {}, {255,10,10,255} });

            module1Button.Render(*screen, *canvas);
            syncInputArgs1TextBox.Render(*screen, *canvas);
            syncInputArgs2TextBox.Render(*screen, *canvas);
            syncModuleButton.Render(*screen, *canvas);
            //reverseModuleButton.Render(*screen, *canvas);
        }
        void Mouse(MouseEventArgs args) override {
            if (chartData) {
                module1Button.Mouse(args);
                syncInputArgs1TextBox.Mouse(args);
                syncInputArgs2TextBox.Mouse(args);
                syncModuleButton.Mouse(args);
                //reverseModuleButton.Mouse(args);
            }
        }
        void Key(KeyEventArgs args) override {
            if (chartData) {
                syncInputArgs1TextBox.Key(args);
                syncInputArgs2TextBox.Key(args);
            }
        }
    };

    class ChartModule {
    public:
        static void ReplaceEffectPosition(rapidjson::Document& document) {
            Logger log;
            std::string TilePositionEnum[3] = {
                "ThisTile",
                "Start",
                "End"
            };

            auto lambda = [&document, &TilePositionEnum, &log](rapidjson::Value& action, std::string tileKey, int64_t floor) {
                auto& tile = action[tileKey.c_str()];
                auto value = tile[0].GetInt64();
                std::string tilePosEnum = tile[1].GetString();
                if (tilePosEnum == "Start") {
                    if (value == 0) return;
                    int64_t tileSize = GetAngleDataSize(document);
                    tile[0].SetInt(std::max((int64_t)0, value) - floor); // 修改为 SetInt
                    tile[1].SetString(TilePositionEnum[0].c_str(), document.GetAllocator()); // 使用 SetString
                    log.info("Success: Floor: ", floor);
                }
                else if (tilePosEnum == "End") {
                    if (value == 0) return;
                    int64_t tileSize = GetAngleDataSize(document);
                    tile[0].SetInt(tileSize + std::min(value,(int64_t)0) - floor); // 修改为 SetInt
                    tile[1].SetString(TilePositionEnum[0].c_str(), document.GetAllocator()); // 使用 SetString
                    log.info("Success: Floor: ", floor);
                }
            };
            // 获取actions数组
            if (document.HasMember("actions") && document["actions"].IsArray()) {
                // 遍历actions数组
                for (auto& action : document["actions"].GetArray()) {
                    size_t floor = 0;
                    if (action.HasMember("floor") && action["floor"].IsInt64()) {
                        floor = action["floor"].GetInt64();
                    }
                    else throw std::runtime_error("Failed during get floor index");

                    // 处理 startTile
                    if (action.HasMember("startTile") && action["startTile"].IsArray()) {
                        lambda(action, "startTile", floor);
                    }

                    // 处理 endTile
                    if (action.HasMember("endTile") && action["endTile"].IsArray()) {
                        lambda(action, "endTile", floor);
                    }
                }
            }
            log.info("Completed");
        }
        static void SyncTrackPosOffset(rapidjson::Document& document,int64_t startFloor,int64_t endFloor) {
            Logger log;
            std::string TilePositionEnum[3] = {
                "ThisTile",
                "Start",
                "End"
            };
            auto lambda = [&document, &TilePositionEnum, &log](rapidjson::Value& action, std::string startTileKey,std::string endTileKey, int64_t thisFloor, int64_t startErrFloor, int64_t endErrFloor) {
                auto& startTile = action[startTileKey.c_str()];
                auto& endTile = action[endTileKey.c_str()];
                const auto startTileValue = startTile[0].GetInt64();
                const auto endTileValue = endTile[0].GetInt64();
                int64_t overflowSize = endErrFloor - startErrFloor + 1;
                int64_t TotalTileSize = GetAngleDataSize(document);
                std::string startTilePosEnum = startTile[1].GetString();
                std::string endTilePosEnum = endTile[1].GetString();

                if (startTilePosEnum == "Start") {
                    // 规则1：ErrRight <= a + ErrLen，修正：a + ErrLen, b + ErrLen
                    if (endErrFloor <= startTileValue + overflowSize) {
                        startTile[0].SetInt(startTileValue + overflowSize);
                        log.info("startTile: Modify Floor ", thisFloor, " (Start, ", startTileValue, " -> ", startTile[0].GetInt64(), ")");
                    }
                }
                else if (startTilePosEnum == "ThisTile") {
                    if (startTileValue >= 0) {
                        if (startErrFloor > thisFloor && startErrFloor <= thisFloor + startTileValue) {
                            // 修正：a + x, b + x
                            startTile[0].SetInt(startTileValue + overflowSize);
                            log.info("startTile: Modify Floor ", thisFloor, " (ThisTile, ", startTileValue, " -> ", startTile[0].GetInt64(), ")");
                        }
                    }
                    // a, b < 0 的情况（处理负数范围）
                    else if (startTileValue < 0) {
                        if (startErrFloor > thisFloor + endTileValue && endErrFloor < thisFloor) {
                            // 修正：a - ErrLen, b - ErrLen
                            startTile[0].SetInt(startTileValue - overflowSize);
                            log.info("startTile: Modify Floor ", thisFloor, " (ThisTile, ", startTileValue, " -> ", startTile[0].GetInt64(), ")");
                        }
                    }
                }

                if (endTilePosEnum == "Start") {
                    // 规则1：ErrRight <= a + ErrLen，修正：a + ErrLen, b + ErrLen
                    if (endErrFloor <= startTileValue + overflowSize) {
                        endTile[0].SetInt(endTileValue + overflowSize);
                        log.info("endTile: Modify Floor ", thisFloor, " (Start, ", endTileValue, " -> ", endTile[0].GetInt64(), ")");
                    }
                    // 规则2：ErrLeft, ErrRight 在[a, b + ErrLen]范围内，修正：a, b + ErrLen
                    else if (startErrFloor > startTileValue && endErrFloor <= endTileValue + overflowSize) {
                        endTile[0].SetInt(endTileValue + overflowSize);
                        log.info("endTile: Modify Floor ", thisFloor, " (Start, ", endTileValue, " -> ", endTile[0].GetInt64(), ")");
                    }
                }
                else if (endTilePosEnum == "ThisTile") {
                    if (endTileValue >= 0) {
                        if (startErrFloor > thisFloor && startErrFloor <= thisFloor + startTileValue) {
                            // 修正：a + x, b + x
                            endTile[0].SetInt(endTileValue + overflowSize);
                            log.info("endTile: Modify Floor ", thisFloor, " (ThisTile, ", endTileValue, " -> ", endTile[0].GetInt64(), ")");
                        }
                        else if (startErrFloor > thisFloor + startTileValue && endErrFloor < thisFloor + endTileValue + overflowSize) {
                            // 修正：a, b + ErrLen（第二个不可取等号）
                            endTile[0].SetInt(endTileValue + overflowSize);
                            log.info("endTile: Modify Floor ", thisFloor, " (ThisTile, ", endTileValue, " -> ", endTile[0].GetInt64(), ")");
                        }
                    }
                    // a, b < 0 的情况（处理负数范围）
                    else if (endTileValue < 0) {
                        // WARN: no std::mix std::max
                        if (endErrFloor >= thisFloor + endTileValue && endErrFloor < thisFloor + startTileValue) { 
                            // 若ErrRight >= thisFloor + b && ErrRight < thisFloor + a
                            // 修正：a, b - ErrLen
                            endTile[0].SetInt(endTileValue - overflowSize);
                            log.info("endTile: Modify Floor ", thisFloor, " (ThisTile, ", endTileValue, " -> ", endTile[0].GetInt64(), ")");
                        }
                        else if (startErrFloor > thisFloor + endTileValue && endErrFloor < thisFloor) {
                            // 修正：a - ErrLen, b - ErrLen
                            endTile[0].SetInt(endTileValue - overflowSize);
                            log.info("endTile: Modify Floor ", thisFloor, " (ThisTile, ", endTileValue, " -> ", endTile[0].GetInt64(), ")");
                        }
                    }
                }
            };
            // 获取actions数组
            if (document.HasMember("actions") && document["actions"].IsArray()) {
                // 遍历actions数组
                for (auto& action : document["actions"].GetArray()) {
                    size_t floor = 0;
                    if (action.HasMember("floor") && action["floor"].IsInt64()) {
                        floor = action["floor"].GetInt64();
                    }
                    else throw std::runtime_error("Failed during get floor index");

                    if (action.HasMember("startTile") && action["startTile"].IsArray()) {
                        if (action.HasMember("endTile") && action["endTile"].IsArray()) {
                            lambda(action, "startTile", "endTile", floor, startFloor, endFloor);
                        }
                    }

                }
            }
            log.info("Completed");
        }

        static void ReverseTrackRotation(rapidjson::Document& document, int64_t startFloor, int64_t endFloor) {
            if (document.HasMember("angleData") && document["angleData"].IsArray()) {
                for (auto& action : document["angleData"].GetArray()) {
                    
                }
            }
        }

        static size_t GetAngleDataSize(rapidjson::Document& document) {
            if (document.HasMember("angleData") && document["angleData"].IsArray()) {
                const rapidjson::Value& angleDatas = document["angleData"];
                return angleDatas.Size();
            }
            return 0;
        }
    };

    AdofaiChartJsonManager chartManager;
    AdofaiChartJson::LoaderSetting loaderSetting;

    ChartScreen chartScreen;
    ChartSettingScreen chartSettingScreen;
    ChartModuleScreen chartModuleScreen;

    InterfaceManager<int> ifManager;
    FpsOverlay fpsOverlay;
    NoobButton backButton;
    NoobButton forwardButton;
    NoobButton backwardButton;
    std::vector<std::shared_ptr<NoobButton>> chartButtons;

    int ScreenId = 0;
    int ScreenMax = 2;

    int ColumnX = 15;
public:
    void Creation(ScreenA* screen, CanvasA* canvas, Logger* logger, Displayer* display) override {
        Interface::Creation(screen, canvas, logger, display);
        ifManager.addInterface(0, &chartScreen);
        ifManager.addInterface(1, &chartSettingScreen);
        ifManager.addInterface(2, &chartModuleScreen);
        ifManager.addDeque(0);
        ifManager.addDeque(1);
        ifManager.addDeque(2);
        ifManager.UsingInvoke(0);
        ifManager.Creation(screen, canvas, logger, display);
        ifManager.UsingInvoke(1);
        ifManager.activateInterface(0);

        chartScreen.setParent(this);
        chartSettingScreen.setParent(this);
        chartModuleScreen.setParent(this);

        backButton.Init();
        backButton.SetProperties(new NoobButton::Properties(0, 0, 15, 3, "Back"));
        backButton.addEvent(NoobButton::onMousePressed, [this,display](void* args, ActionableWidget* widget) {
            display->Navigate("AdofaiSetupScreen"); // AdofaiSetupScreen
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
            ScreenId = std::max(ScreenId - 1, (int)0);
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
                displayer->Navigate("AdofaiSetupScreen"); // AdofaiSetupScreen
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
                ScreenId = 0;
                auto* ScreenA = static_cast<ChartParentScreen*>(ifManager.getActivatedInterface());
                auto* ScreenB = static_cast<ChartParentScreen*>(ifManager.getScreen(ScreenId));
                ScreenB->connent(ScreenA);
                ifManager.activateInterface(0);
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
    void ReloadChart(size_t index) {
        chartManager.ReloadChart(index, loaderSetting);
        logger->info("Reload chart");
    }
    void WriteChart(size_t index) {
        chartManager.OverwriteChart(index);
        logger->info("Overwrite chart");
    }
	AdofaiChartJsonManager* GetChartManager() {
		return &chartManager;
	}
    AdofaiChartJson::LoaderSetting* getLoaderSetting() {
        return &loaderSetting;
    }
};

AdofaiTweaksChartScreen* MakeAdofaiTweaksChartScreen();