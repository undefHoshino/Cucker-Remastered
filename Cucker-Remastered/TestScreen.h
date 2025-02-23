#pragma once
#include "Interface.h"
#include "ConsoleThread.h"
#include "FpsOverlay.h"
#include "Overlay.h"
#include "HpetClock.h"
#include "Animator.h"
#include "EasingFunction.h"
#include "Widget.h"
#include "StateMachine.h" 
#include "AnimationBuilder.h"
#include "TextInputControl.h"
#include "AnimateValue.h"
#include "Viewport.h"

class TestOverlay : public Overlay {
public:
	TestOverlay() = default;
protected:
	void Render() override {
		canvas->DrawLineH(0, screen->getWidth() - 1, 0, { {255,255,255,100},{} });
		canvas->DrawLineH(0, screen->getWidth() - 1, screen->getHeight() - 1, { {255,255,255,100},{} });
		canvas->DrawLineV(0, screen->getHeight() - 1, 0, { {255,255,255,100},{} });
		canvas->DrawLineV(screen->getWidth() - 1, 0, screen->getHeight() - 1, { {255,255,255,100},{} });
		canvas->DrawLine(0, 0, screen->getWidth() - 1, screen->getHeight() - 1, {{255,255,255,100},{}}, 'X');
	}
};

class TestScreen : public Interface {
private:
	class AnimationTestPage : public Interface {
		Pixel textColor = { {},{255,255,255,255} };
		NoobButton button;
		
		HpetClock* clock = nullptr;
		int dataMovement = 0;
		int dataAlpha = 0;
		std::string timelineID = "1";
		bool togglePauseResume = false;
		bool toggleReversed = false;
		double speed = 1.0;
	public:
		void TimelineDrawer(ScreenA* screen, CanvasA* canvas, KeyFrameLoader loader);
		void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override;
		void Render() override;
		void Mouse(MouseEventArgs args) override;
		void Key(KeyEventArgs args) override;
		void BackgroundLogic() override;
	};
	class TextInputerTestPage : public Interface {
		TextInputControl textInput;
		Pixel textColor = { {},{255,255,255,255} };
		NoobButton button;
	public:
		void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override;
		void Render() override;
		void Mouse(MouseEventArgs args) override;
		void Key(KeyEventArgs args) override;
		void DrawHistroy();
		void DrawSelection(int textBoxX, int textBoxY);
		std::vector<TextInputControl::Histroy::InputAction> stackToVector(std::stack<TextInputControl::Histroy::InputAction> s);
	};

	class AnimationTestPage2 : public Interface {
	private:
		Slider slider;
		AnimateValue<int> anvalueX = 2;
		Pixel textColor = { {},{255,255,255,255} };
		TextBox textBox;
		int lastSliderX = 0;
	public:
		void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override;
		void Render() override;
		void Mouse(MouseEventArgs args) override;
		void Key(KeyEventArgs args) override;
	};

	class EasingFunctionTestPage : public Interface {
	private:
		class Axis {
		private:
			using Easing = std::function<double(double)>;
			int posX = 60;
			int posY = 30;
			int borderX;
			int borderY;
			int borderMaxX;
			int borderMaxY;
			double scale = 1.0;  // 坐标轴缩放比例
			bool isDragging = false;  // 标记是否正在拖动
			int dragOffsetX = 0;  // 鼠标与坐标轴原点的X轴偏移量
			int dragOffsetY = 0;  // 鼠标与坐标轴原点的Y轴偏移量
			int gridSpacing = 10;  // 网格间距
			double ratioX = 2;
			double ratioY = 1;
			double detail = 1.0;
		public:
			void SetFunctionDetail(double detail);
			void SetRatio(double ratioX, double ratioY);
			void SetBorder(int borderX, int borderY, int borderMaxX, int borderMaxY);
			void DrawEasingFunction(CanvasA& canvas, Easing easing);
			void Render(ScreenA& screen, CanvasA& canvas, Easing easing);
			void MouseEvent(MouseEventArgs args);
		};
		using Easing = std::function<double(double)>;
		std::vector<std::pair<std::string,Easing>> easings;
		Pixel textColor = { {},{255,255,255,255} };

		Axis axis;
		NoobButton forwardButton;
		NoobButton backwardButton;
		Slider slider;
		size_t index = 0;
	public:
		void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override;
		void Render() override;
		void Mouse(MouseEventArgs args) override;
		void DrawAxes();
		void DrawEasingCurve(Easing easing, int startX, int startY, int spacing);
	};

	class SliderTestPage : public Interface {
	private:
		Slider sliderLeft;
		Slider sliderRight;
		Slider sliderUp;
		Slider sliderDown;
		Slider sliderStep;
		Scrollbar scrollBarHorizontal;
		Scrollbar scrollBarVertical;

		Pixel textColor = { {},{255,255,255,255} };
	public:
		void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override;
		void Render() override;
		void Mouse(MouseEventArgs args) override;
	};

	class ViewportTestPage : public Interface {
	private:
		Viewport viewport;

		Pixel textColor = { {},{255,255,255,255} };
	public:
		void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override {
			Interface::Creation(screen, canvas, logger, display);
			viewport.Init(screen, logger, display);
			viewport.SetProperties(new Viewport::Properties({ 2, 4, 20, 10 }, { 2, 4, 120, 60 }, Viewport::MoveMode::Viewport));
			viewport.SetScrollbarSize(1, 2);
			viewport.UpdateScrollBar();
			viewport.getAnimator().SetDrawer(nullptr, [this](ScreenA* screen , CanvasA* canvas, KeyFrameGroupLoader, void*) {
				viewport.getStyle<Viewport::Style>()->DrawPrimitive(*screen, *canvas, &viewport);
				canvas->DrawString(2, 4, "Viewport Top===========================================", textColor);
				canvas->DrawString(2, 32, "A Sample Text", textColor);
				canvas->DrawString(2, 63, "Viewport Bottom========================================", textColor);
			});
		}
		void Render() override {
			canvas->DrawString(2, 2, "Viewport Test", textColor);
			//viewport.Render(*screen, *canvas);
		}
		void Mouse(MouseEventArgs args) override {
			//viewport.Mouse(args);
		}
	};

	class StableTestPage : public Interface {
	private:
		NoobButton logSpamButton;
		NoobButton crashBufferButton;
		NoobButton throwExceptionButton;
		bool crashFlag = false;
		bool renderThrown = false;
	public:
		Pixel textColor = { {},{255,255,255,255} };
		void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override {
			Interface::Creation(screen, canvas, logger, display);
			logSpamButton.Init();
			logSpamButton.SetProperties(new NoobButton::Properties(2,4,10,3,"Log Spam"));
			logSpamButton.addEvent(NoobButton::onMouseHeld, [this](void* args, ActionableWidget* widget) {
				for (size_t i = 0; i < 10; i++) {
					this->logger->info("Spam!!!");
					this->logger->warn("Spam!!!");
					this->logger->error("Spam!!!");
					this->logger->fatal("Spam!!!");
				}
			});
			crashBufferButton.Init();
			crashBufferButton.SetProperties(new NoobButton::Properties(2, 8, 20, 3, "Crash Buffers"));
			crashBufferButton.addEvent(NoobButton::onMousePressed, [this](void* args, ActionableWidget* widget) {
				auto& conThread = parent->GetComponent<ConsoleThread>();
				if (conThread.hasThread("Crasher")) {
					conThread.stopUserThread("Crasher");
					this->logger->info("Stop Crasher Thread");
					return;
				}
				conThread.addUserThread<ThreadManager::ThreadHandler>("Crasher", [this](ThreadManager::ThreadHandler& handler) -> int{
					while (CheckConsoleInstance()) {
						if (handler.shouldStop()) return -1;
						HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
						CONSOLE_SCREEN_BUFFER_INFO csbi;
						srand(time(0));
						GetConsoleScreenBufferInfo(hConsole, &csbi);
						short width = csbi.dwSize.X - rand() % 10;
						short height = csbi.dwSize.Y - rand() % 10;
						SetConsoleScreenBufferSize(hConsole, { width ,height });
					}
					handler.Stop();
					this->logger->error("Stopped!");
					return 0;
				});
				this->logger->info("Create Crasher Thread");
			});
			throwExceptionButton.Init();
			throwExceptionButton.SetProperties(new NoobButton::Properties(24, 8, 20, 3, "Throw Exception"));
			throwExceptionButton.addEvent(NoobButton::onMousePressed, [this](void* args, ActionableWidget* widget) {
				renderThrown = true;
			});
		}
		void Render() override {
			canvas->DrawString(2, 2, "Stability Test", textColor);
			canvas->DrawString(14, 5, "Logs: " + std::to_string(logger->size()), textColor);

			logSpamButton.Render(*screen, *canvas);
			crashBufferButton.Render(*screen, *canvas);
		}
		void Mouse(MouseEventArgs args) override {
			logSpamButton.Mouse(args);
			crashBufferButton.Mouse(args);
		}
	};

	/*
	class SecretPage : public Interface {
	private:
		Pixel textColor = { {},{255,255,255,255} };
		Animator animator;
		NoobButton rickrollButton;
		NoobButton timerScreenButton;
		NoobButton mainScreenButton;
		NoobButton adofaiScreenButton;
		NoobButton testScreenButton;

		bool gotRickroll = false;
	public:
		void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Display* display) override {
			Interface::Creation(screen, canvas, logger, display);
			auto& timeline = animator.ConfigureTimeline("LineAn0");

			rickrollButton.Init();
			rickrollButton.SetProperties(new NoobButton::Properties(0, 0, 6, 1, "[更多]"));
			rickrollButton.addEvent(NoobButton::onMousePressed, [this](void* args, ActionableWidget* widget) {
				std::wstring url = L"https://www.bilibili.com/video/BV1GJ411x7h7";
				ShellExecute(0, L"open", url.c_str(), 0, 0, SW_SHOW);
				gotRickroll = true;
			});

			timerScreenButton.Init();
			timerScreenButton.SetProperties(new NoobButton::Properties(2, 22, 15, 3, "TimerScreen"));
			timerScreenButton.addEvent(NoobButton::onMousePressed, [this, display,logger](void* args, ActionableWidget* widget) {
				display->Navigate(2);
				logger->info("Navigate to TimerScreen");
			});

			mainScreenButton.Init();
			mainScreenButton.SetProperties(new NoobButton::Properties(19, 22, 15, 3, "MainScreen"));
			mainScreenButton.addEvent(NoobButton::onMousePressed, [this, display, logger](void* args, ActionableWidget* widget) {
				display->Navigate(0);
				logger->info("Navigate to MainScreen");
			});

			adofaiScreenButton.Init();
			adofaiScreenButton.SetProperties(new NoobButton::Properties(36, 22, 15, 3, "AdofaiScreen"));
			adofaiScreenButton.addEvent(NoobButton::onMousePressed, [this, display, logger](void* args, ActionableWidget* widget) {
				display->Navigate(3);
				logger->info("Navigate to AdofaiScreen");
			});

			testScreenButton.Init();
			testScreenButton.SetProperties(new NoobButton::Properties(53, 22, 15, 3, "TestScreen"));
			testScreenButton.addEvent(NoobButton::onMousePressed, [this, display, logger](void* args, ActionableWidget* widget) {
				logger->error("你隔着搁那呢");
			});


		}
		void Render() override {
			canvas->DrawString(2, 2, "SB 生日快乐！", { {}, {30,255,30,255} });
			canvas->DrawString(2, 4, "你的年度总结：", textColor);
			canvas->DrawString(4, 5, "Today are is do have has " + GetTimeFormatted(), textColor);
			canvas->DrawString(4, 7, "在2024年，你过了2024年，大约过了365天，度过了31536000秒，长达3.1536 * 10^13 nanoseconds！", textColor);
			canvas->DrawString(4, 8, "在2024年，你最喜欢玩的游戏：{}, 最喜欢听的音乐：{}, 击毙了 {} 首谱面, {}！ {}，%s，%d！烫烫烫烫烫烫烫烫烫", textColor);
			canvas->DrawString(4, 9, "Render throws an exception: string too long",textColor);
			canvas->DrawString(4, 10, "Render throws an exception: string too long", textColor);
			canvas->DrawString(4, 11, "Render throws an exception: string too long", textColor);
			canvas->DrawString(4, 12, "Render throws an exception: string too long", textColor);
			canvas->DrawString(4, 13, "Render throws an exception: string too long", textColor);
			canvas->DrawString(4, 15, "666不想写了，你知道我写这玩意很jb麻烦的，虽然简略也没办法了，讲究一下吧哈哈weowowwoweowowoweowowoweowoweowoooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo我知道你会看到这里", textColor);
			canvas->DrawString(4, 17, "（原本这里要塞个小动画的但是我懒得塞了）", textColor);
			canvas->DrawString(2, 19, "- 下列按钮可前往一些已经开发的Screen（没写返回！部分未完成）", textColor);
			canvas->DrawString(2, 20, "  （MainScreen有抽象立方体玩）", textColor);


			auto& prop = *rickrollButton.getProperties<NoobButton::Properties>();
			prop.X = 0;
			prop.Y = screen->getHeight() - 1;

			if (gotRickroll) {
				canvas->DrawString(9, screen->getHeight() - 1, "上当力（喜", { {},{30,255,30,255} });
			}
			rickrollButton.Render(*screen, *canvas);
			timerScreenButton.Render(*screen, *canvas);
			mainScreenButton.Render(*screen, *canvas);
			adofaiScreenButton.Render(*screen, *canvas);
			testScreenButton.Render(*screen, *canvas);
		}
		void Mouse(MouseEventArgs args) override {
			rickrollButton.Mouse(args);
			timerScreenButton.Mouse(args);
			mainScreenButton.Mouse(args);
			adofaiScreenButton.Mouse(args);
			testScreenButton.Mouse(args);
		}
		std::string GetTimeFormatted() {
			std::string formattedText = "";
			std::time_t t = std::time(nullptr);
			std::tm* tm = std::localtime(&t);
			char buffer[100];
			std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S %A", tm);
			formattedText.append(buffer);
			return formattedText;
		}
	};
	*/

	InterfaceManager<unsigned char> ifManager;

	FpsOverlay fpsOverlay;
	Pixel textColor = { {},{255,255,255,255} };

	unsigned char PageId = 0;
	unsigned char PageMax = 6;

	AnimationTestPage   anPage;
	AnimationTestPage2  anPage2;
	TextInputerTestPage	textInputerPage;
	EasingFunctionTestPage easingTestPage;
	SliderTestPage	sliderTestPage;
	ViewportTestPage viewportTestPage;
	StableTestPage  stableTestPage;
	
	NoobButton backwardButton;
	NoobButton forwardButton;
public:
	void Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) override;
	void Render() override;
	void Mouse(MouseEventArgs args) override;
	void Key(KeyEventArgs args) override;
	void BackgroundLogic() override;
};

TestScreen* MakeTestScreen();

/*
		{
			auto& timelineA = animator.addTimeline("1");
			timelineA.addSegment("version").push(0, 1);
			timelineA.addSegment("moveX1")
				.push(1500, 0, GetEasingFunction<EasingFunction::Elastic>(1))
				.push(0, 30);
			timelineA.addSegment("alpha")
				.setOffset(1000)
				.push(500, 100, GetEasingFunction<EasingFunction::Flash>(1))
				.push(500, 0, GetEasingFunction<EasingFunction::Flash>(1))
				.push(0, 100);
			timelineA.setDrawer(lambda);
			timelineA.Setup();

			auto& timelineB = animator.addTimeline("2");
			timelineB.addSegment("version").push(0, 2);
			timelineB.addSegment("width")
				.push(500, 0, GetEasingFunction<EasingFunction::Sine>(1))
				.push(0, 30);
			timelineB.addSegment("movRectX")
				.push(500, -8, GetEasingFunction<EasingFunction::Quad>(1))
				.push(0, 2);
			timelineB.addSegment("movTextX")
				.push(500, -6, GetEasingFunction<EasingFunction::Back>(1))
				.push(0, 2);
			timelineB.addSegment("alpha")
				.push(500, 2, GetEasingFunction<EasingFunction::Sine>(1))
				.push(0, 100);
			timelineB.setDrawer(lambda);
			timelineB.Setup();
		}
		animator.select("1");
		animator->Play();
*/