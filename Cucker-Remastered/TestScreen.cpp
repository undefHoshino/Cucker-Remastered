#include "TestScreen.h"

TestScreen* MakeTestScreen() {
	return new TestScreen();
}

void TestScreen::AnimationTestPage::TimelineDrawer(ScreenA* screen, CanvasA* canvas, KeyFrameLoader loader) {
	int version = (int)loader["version"];
	if (version == 1) {
		int movement = (int)loader["moveX1"];
		unsigned char alpha = (unsigned char)loader["alpha"];
		//int textIndex = (int)loader["textIndex"];

		this->dataMovement = movement;
		this->dataAlpha = alpha;

		canvas->DrawLineH(2, 2 + movement, 15, { {255,255,255,alpha} ,{} });
		std::string msg = "Hello World!";

		canvas->DrawString(2, 15, msg.substr(0, movement), textColor);
	}
	else {
		int width = (int)loader["width"];
		int movRectX = (int)loader["movRectX"];
		int movTextX = (int)loader["movTextX"];
		unsigned char alpha = (unsigned char)loader["alpha"];
		std::string msg = "Version 2";
		canvas->DrawLineH(movRectX, movRectX + width, 15, { {255,255,255,alpha} ,{} });
		canvas->DrawString(movTextX, 15, msg.substr(0, width), textColor);
	}

}

void TestScreen::AnimationTestPage::Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) {
	Interface::Creation(screen, canvas, logger, display);

	auto lambda = [this](ScreenA* screen, CanvasA* canvas, KeyFrameLoader loader, void* args) {
		TimelineDrawer(screen, canvas, loader);
	};

	auto& animator = button.getAnimator();
	using PlaybackPreset = Animator::PlaybackActionPreset;

	button.Init();
	button.SetProperties(new Button::Properties({ 2,19,10,3,"Button" }));
	{
		animator.ConfigureTimeline("0");
		animator.ConfigureTimeline("Mouse", "FillX", {
			{150, 0, GetEasingFunction<EasingFunction::Quad>(1)},
			{0, 10}
			});
		animator.ConfigureTimeline("Key", "RectAlpha", {
			{500, 0, GetEasingFunction<EasingFunction::Sine>(0)},
			{500, 50, GetEasingFunction<EasingFunction::Sine>(1)},
			{0, 0}
			});
		animator.ManageTriggers({}, {
			{"Mouse",PlaybackPreset::InterruptiblePlayback(animator.getTimeline("Mouse"),animator.getStateMachine(),"A") }
			}, {});
		animator.ManageTriggers({}, {
			{"Key", PlaybackPreset::LoopingPlayback(animator.getTimeAxis(), animator.getTimeline("Key"),"Key")}
			});
		animator.putStates({
			{"order", false},
			{"select", false},
			{"pressed", false}
			});
		animator.Initialize();
		animator.DrawQueue({ "Mouse" }, { "0" });
	}
	button.addEvent(NoobButton::_MouseEventEx_, [this, logger](void* _args, ActionableWidget* _self) {
		using MouseCond = InputCondition::Mouse;
		Button& self = *_self->as<Button>();
		NoobButton::MouseEventArgsEx args = *reinterpret_cast<NoobButton::MouseEventArgsEx*>(_args);
		auto& animator = self.getAnimator();
		auto& trigger = animator.trigger;

		if (MouseCond::IsClicked(MouseCond::Left, args.pressResult)) {
			logger->info("Button Left Clicked!");
		}
		if (MouseCond::IsClicked(MouseCond::Right, args.pressResult)) {
			logger->info("Button Right Clicked!");
		}
		trigger.send("Mouse", args.inArea);
		});
	button.addEvent(ActionableWidget::_KeyEvent_, [this, logger](void* _args, ActionableWidget* _self) {
		KeyEventArgs args = *reinterpret_cast<KeyEventArgs*>(_args);
		Button& self = *_self->as<Button>();
		auto& container = self.getContainer();
		auto& stateMachine = button.getAnimator().getStateMachine();;

		bool keyTabSelect = args.keyCode == VirtualKeyCode::Tab;
		bool keyCancel = args.keyCode == VirtualKeyCode::Escape;
		bool keyEnter = args.keyCode == VirtualKeyCode::Enter;

		if (args.keyPressed) {
			if (keyTabSelect) {
				stateMachine["select"] = true;
				logger->info("TabSelect");
			}
			if (keyCancel) {
				stateMachine.putBatches({
					{"select",false},
					{"pressed",false}
					});
				logger->info("EscapeCancel");
			}
			if (keyEnter && stateMachine["select"]) {
				stateMachine["pressed"] = true;
				logger->info("Pressed");
			}
		}
		else if (keyEnter) {
			stateMachine["pressed"] = false;
		}
		});
	button.addEvent(ActionableWidget::_UpdateEvent_, [this](void* _, ActionableWidget* _self) {
		auto& trigger = _self->getAnimator().getTrigger();;
		trigger.forEach();
		});
	button.getAnimator().SetDrawer(nullptr, [this](ScreenA* screen, CanvasA* canvas, KeyFrameGroupLoader loader, void* args) {
		Button* self = static_cast<Button*>(args);
		auto& animator = self->getAnimator();
		auto& container = self->getContainer();
		auto& prop = *self->getProperties<Button::Properties>();
		auto& style = *self->getStyle<Button::Style>();
		auto& trigger = animator.getTrigger();
		auto& stateMachine = animator.getStateMachine();

		int index = style.getIndex();
		unsigned char alpha = 100;
		unsigned char lightAlpha = 180;

		trigger.send("Key", stateMachine["select"] && !stateMachine["pressed"]);

		if (index == 2 || stateMachine["pressed"]) {
			alpha = 70;
			lightAlpha = 150;
		}

		canvas->FillRect(prop.X, prop.Y, prop.X + prop.Width, prop.Y + prop.Height, { {255,255,255,alpha},{} });

		int fillX = (int)loader["Mouse"]["FillX"];
		canvas->FillRect(prop.X, prop.Y, prop.X + fillX, prop.Y + prop.Height, { {255,255,255,lightAlpha},{} , Pixel::BlendMode::Replace });
		this->dataMovement = fillX;

		if (stateMachine["select"] && loader.hasLoader("Key") && !stateMachine["pressed"]) {
			unsigned char RectAlpha = (unsigned char)loader["Key"]["RectAlpha"];
			canvas->FillRect(prop.X, prop.Y, prop.X + prop.Width, prop.Y + prop.Height, { {255,255,255,RectAlpha},{} , Pixel::BlendMode::Mix });
		}

		PosCalculator calc({ prop.X,prop.Y,(int)prop.Width,(int)prop.Height });

		PointI centerPoint = calc.alignCenter(prop.Caption.length());
		canvas->DrawString(centerPoint.x, centerPoint.y, prop.Caption, { {},{255,255,255,255} });
		});
}

void TestScreen::AnimationTestPage::Render() {
	canvas->DrawString(2, 2, "Hello World!", textColor);
	canvas->DrawString(2, 4, "Animation:", textColor);
	std::string msgMovement = "  Movement: [ " + std::to_string(dataMovement) + " ], Alpha: " + std::to_string(dataAlpha);
	canvas->DrawString(2, 5, msgMovement, textColor);
	if (clock) {
		canvas->DrawString(2, 8, "Clock duration: " + std::to_string(clock->getDuration<std::chrono::milliseconds>().count()), textColor);
		canvas->DrawString(2, 9, "Clock MaxDuration: " + std::to_string(clock->getMaxDuration<std::chrono::milliseconds>().count()), textColor);
		canvas->DrawString(2, 10, "Clock ElapsedTime: " + std::to_string(clock->getElapsedTime().count()), textColor);
		canvas->DrawString(2, 11, "Clock Speed: " + std::to_string(clock->getSpeed()), textColor);
		canvas->DrawString(2, 12, "Clock progress: " + std::to_string(clock->getProgress()), textColor);
		canvas->DrawString(2, 13, "isPaused: " + std::string(clock->isPaused() ? "Paused" : "Resume"), textColor);
	}
	else {
		canvas->DrawString(2, 8, "Clock is nullptr", textColor);
	}

	clock = &button.getAnimator().getTimeline("Mouse").getClock();
	//animator->Draw(screen, canvas, 0);
	button.Render(*screen, *canvas);
}
void TestScreen::AnimationTestPage::Mouse(MouseEventArgs args) {
	button.Mouse(args);

}
void TestScreen::AnimationTestPage::Key(KeyEventArgs args) {
	button.Key(args);

	if (args.keyPressed && clock) {
		if (args.keyCode == VirtualKeyCode::Space) {
			if (!togglePauseResume) {
				clock->Pause();
				logger->info("Paused");
			}
			else {
				clock->Resume();
				logger->info("Resume");
			}
			togglePauseResume = !togglePauseResume;
		}
		if (args.keyCode == VirtualKeyCode::Q) {
			clock->syncFromMaxDuration();
			logger->info("syncFromMaxDuration");
		}
		if (args.keyCode == VirtualKeyCode::S) {
			clock->Start();
			logger->info("Start");
			togglePauseResume = false;
		}
		if (args.keyCode == VirtualKeyCode::E) {
			clock->setDuration(clock->getMaxDuration<std::chrono::milliseconds>().count());
			logger->info("SetDuration: MAX");
		}
		if (args.keyCode == VirtualKeyCode::R) {
			clock->Reverse();
			if (!toggleReversed) {
				logger->info("Reversed");
			}
			else {
				logger->info("Normal");
			}
			toggleReversed = !toggleReversed;
		}
		if (args.keyCode == VirtualKeyCode::A) {
			speed = clock->getSpeed();
			speed -= 0.1;
			clock->SetSpeed(speed);
			logger->info("Speed: ", speed);
		}
		if (args.keyCode == VirtualKeyCode::D) {
			speed = clock->getSpeed();
			speed += 0.1;
			clock->SetSpeed(speed);
			logger->info("Speed: ", speed);
		}
		if (args.keyCode == VirtualKeyCode::LeftArrow) {
			clock->Rollback(-1000);
			logger->info("Rollback -1000");
		}
		if (args.keyCode == VirtualKeyCode::RightArrow) {
			clock->Rollback(1000);
			logger->info("Rollback 1000");
		}
		if (args.keyCode == VirtualKeyCode::W) {
			clock->Reset();
			logger->info("Reset");
		}
		/*if (args.keyCode == VirtualKeyCode::Z) {
			if (timelineID == "1") {
				timelineID = "2";
				animator.select(timelineID);
				logger->info("Switch to Timeline 2");
			}
			else {
				timelineID = "1";
				animator.select(timelineID);
				logger->info("Switch to Timeline 1");
			}
		}*/
	}
}

void TestScreen::AnimationTestPage::BackgroundLogic() {
	button.Update();
}

void TestScreen::TextInputerTestPage::Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) {
	Interface::Creation(screen, canvas, logger, display);
	button.Init();
	button.SetProperties(new Button::Properties({ 2,7,7,1,"[Write]" }));
	button.addEvent(NoobButton::_MouseEventEx_, [this](void* _args, ActionableWidget* _self) {
		using Cond = InputCondition::Mouse;
		NoobButton::MouseEventArgsEx args = *reinterpret_cast<NoobButton::MouseEventArgsEx*>(_args);
		if (Cond::IsClicked(Cond::Left, args.pressResult)) {
			if (!textInput.isEnabled()) {
				textInput.enable();
				button.getProperties<Button::Properties>()->Caption = "[Close]";
			}
			else {
				textInput.disable();
				button.getProperties<Button::Properties>()->Caption = "[Write]";
			}
		}
		});
	textInput.setMaxLine(-1);
}

void TestScreen::TextInputerTestPage::Render() {
	auto& buffer = textInput.getBuffer();
	auto& cursor = textInput.getCursor();
	auto& anchor = textInput.getAnchor();
	canvas->DrawString(2, 2, "TextBufferControl Test", textColor);

	std::string msgCursorPos = "Cursor Position: [";
	msgCursorPos += std::to_string(cursor.getPosition().column) + " , " + std::to_string(cursor.getPosition().row) + "]";
	canvas->DrawString(4, 3, msgCursorPos, textColor);

	std::string msgSelection = "Anchor Position: [";
	msgSelection += std::to_string(anchor.getPosition().column) + " , " + std::to_string(anchor.getPosition().row) + "]";
	msgSelection += (anchor.isActive() ? " - Active" : "");
	canvas->DrawString(4, 4, msgSelection, textColor);
	canvas->DrawString(4, 5, "Lines: " + std::to_string(buffer.size()), textColor);

	int lineMsgLen = std::to_string(buffer.size()).size() + 1;
	int textBoxX = lineMsgLen + 2;
	int textBoxY = 9;
	try {
		canvas->FillRect(textBoxX, 9, 120, textBoxY + buffer.size(), { {200,200,200,50},{} });
		for (size_t i = 0; i < buffer.size(); i++) {
			std::wstring wstr(buffer.getLine(i).data().begin(), buffer.getLine(i).data().end());
			canvas->DrawString(2, textBoxY + i, std::to_string(i), textColor);
			canvas->SetPixel(textBoxX - 1, textBoxY + i, textColor, '|');
			canvas->DrawString(textBoxX, textBoxY + i, Converter::Convert<std::wstring, std::string>(wstr), textColor);
		}

		if (anchor.isActive()) {
			DrawSelection(textBoxX, textBoxY);
		}
		else {
			canvas->SetPixel(textBoxX + textInput.getCursorPosition().column, textBoxY + textInput.getCursorPosition().row, { {200,200,200,80},{} });
		}
	}
	catch (std::exception e) {
		logger->error(e.what());
	}
	/*
	std::string msg1 = "Copied: ";
	if (textInput.clipboard.isClipboardTextAvailable()) {
		std::wstring&& wstr = textInput.clipboard.pasteWstring();
		wstr.erase(std::remove_if(wstr.begin(), wstr.end(), [](wchar_t c) {
			return c < 32;  // 检查是否是控制字符
		}), wstr.end());

		msg1 += Converter::Convert<std::wstring, std::string>(wstr);
	}
	else {
		msg1 += "(null)";
	}
	canvas->DrawString(2, 11, msg1, textColor);
	*/

	DrawHistroy();

	button.Render(*screen, *canvas);
}

void TestScreen::TextInputerTestPage::DrawHistroy() {
	auto& buffer = textInput.getBuffer();
	auto& histroy = textInput.getHistroy();

	int HistroyUndoX = 130;
	int HistroyRedoX = 130 + 64;
	int HistroyY = 3;
	using Operation = TextInputControl::Histroy::Operation;
	canvas->DrawString(HistroyUndoX, HistroyY, "UndoStack: " + std::to_string(histroy.getUndoStack().size()), textColor);
	size_t yUndo = HistroyY + 1;
	for (auto& ptr : stackToVector(histroy.getUndoStack())) {
		std::string msg = "";
		Pixel msgPixel = {};
		if (ptr.operation == Operation::Erase) {
			msg += "- ";
			msgPixel = { {},{236,28,36,255} };
		}
		else {
			msg += "+ ";
			msgPixel = { {},{14,209,69,255} };
		}
		canvas->DrawString(HistroyUndoX, yUndo, msg, msgPixel);
		std::wstringstream stream(textInput.getWstring(ptr.buffer));
		std::wstring line;
		while (std::getline(stream, line)) {
			buffer.processTabsAndControlChars(line);
			canvas->DrawString(HistroyUndoX + 2, yUndo, Converter::Convert<std::wstring, std::string>(line), textColor);
			yUndo++;
		}
	}

	canvas->DrawString(HistroyRedoX, HistroyY, "RedoStack: " + std::to_string(histroy.getRedoStack().size()), textColor);
	size_t yRedo = HistroyY + 1;
	for (auto& ptr : stackToVector(histroy.getRedoStack())) {
		std::string msg = "";
		Pixel msgPixel = {};
		if (ptr.operation == Operation::Erase) {
			msg += "+ ";
			msgPixel = { {},{14,209,69,255} };
		}
		else {
			msg += "- ";
			msgPixel = { {},{236,28,36,255} };
		}
		canvas->DrawString(HistroyRedoX, yRedo, msg, msgPixel);
		std::wstringstream stream(textInput.getWstring(ptr.buffer));
		std::wstring line;
		while (std::getline(stream, line)) {
			buffer.processTabsAndControlChars(line);
			canvas->DrawString(HistroyRedoX + 2, yRedo, Converter::Convert<std::wstring, std::string>(line), textColor);
			yRedo++;
		}
	}
}


void TestScreen::TextInputerTestPage::DrawSelection(int textBoxX, int textBoxY) {
	auto& buffer = textInput.getBuffer();
	auto& anchor = textInput.getAnchor();
	size_t count = textInput.getSelectedLineCount();
	size_t startX = anchor.startColumn();
	size_t endX = anchor.endColumn();
	size_t startY = anchor.startRow();
	size_t endY = anchor.endRow();
	if (startY == endY) {
		canvas->DrawLineH(textBoxX + startX, textBoxX + endX, startY + textBoxY, { {200,200,200,50},{} });
		return;
	}
	size_t widthTop = textBoxX + buffer.getLine(startY).size();
	/*if (textInput.buffer.getLine(startY).size() == 0) {
		widthTop++;
	}*/
	canvas->DrawLineH(textBoxX + startX, widthTop, startY + 9, { {200,200,200,50},{} });


	for (size_t y = startY + 1; y < endY; ++y) {
		size_t widthMiddle = textBoxX + buffer.getLine(y).size();
		/*if (textInput.buffer.getLine(y).size() == 0) {
			widthMiddle++;
		}*/
		canvas->DrawLineH(textBoxX, widthMiddle, y + textBoxY, {{200,200,200,50},{}});
	}

	size_t widthBottom = textBoxX + endX;
	/*if (textInput.buffer.getLine(endY).size() == 0) {
		widthBottom++;
	}*/
	canvas->DrawLineH(textBoxX, widthBottom, endY + textBoxY, { {200,200,200,50},{} });
}

void TestScreen::TextInputerTestPage::Mouse(MouseEventArgs args) {
	button.Mouse(args);
}

void TestScreen::TextInputerTestPage::Key(KeyEventArgs args) {
	textInput.KeyEvent(args);
}

std::vector<TextInputControl::Histroy::InputAction> TestScreen::TextInputerTestPage::stackToVector(std::stack<TextInputControl::Histroy::InputAction> s) {
	std::vector<TextInputControl::Histroy::InputAction> temp;
	while (!s.empty()) {
		temp.push_back(s.top());
		s.pop();
	}
	return temp;
}

void TestScreen::AnimationTestPage2::Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) {
	Interface::Creation(screen, canvas, logger, display);
	anvalueX.setDuration(500);
	anvalueX.setEasing(GetEasingFunction<EasingFunction::Back>(1));
	slider.Init();
	slider.SetProperties(new Slider::Properties(2, 6, 30, 3));
	textBox.Init();
	textBox.SetProperties(new TextBox::Properties(2, 25, 100, 10));
	textBox.SetMaxLine(10);
}

void TestScreen::AnimationTestPage2::Render() {
	auto* sliderProp = slider.getProperties<Slider::Properties>();
	canvas->DrawString(2, 2, "Slider:", textColor);
	std::string posMsg = "Progress: ";
	posMsg += std::to_string(slider.getSliderProgress());
	canvas->DrawString(4, 4, posMsg, textColor);

	canvas->DrawString(62, 2, "AnimateValue:", textColor);
	canvas->DrawString(64, 4, "Progress: " + std::to_string(anvalueX.getProgress()), textColor);
	canvas->DrawString(64, 5, "Progress Eased: " + std::to_string(anvalueX.getEasedProgress()), textColor);
	canvas->DrawString(64, 6, "Start: " + std::to_string(anvalueX.getStart()), textColor);
	canvas->DrawString(64, 7, "End: " + std::to_string(anvalueX.getEnd()), textColor);
	canvas->DrawString(64, 8, "Interpolate: " + std::to_string(anvalueX()), textColor);


	canvas->FillRect(anvalueX(), 12, anvalueX() + 10, 17, { {200,200,200,150},{} });

	anvalueX = slider.lerp<int>(2, 102);
	canvas->DrawString(2, 23, "TextBox: ", textColor);
	slider.Render(*screen, *canvas);
	textBox.Render(*screen, *canvas);

}

void TestScreen::AnimationTestPage2::Mouse(MouseEventArgs args) {
	slider.Mouse(args);
	textBox.Mouse(args);
}

void TestScreen::AnimationTestPage2::Key(KeyEventArgs args) {
	textBox.Key(args);
}


void TestScreen::EasingFunctionTestPage::Axis::SetFunctionDetail(double detail) {
	this->detail = detail;
}

void TestScreen::EasingFunctionTestPage::Axis::SetRatio(double ratioX, double ratioY) {
	this->ratioX = ratioX;
	this->ratioY = ratioY;
}

void TestScreen::EasingFunctionTestPage::Axis::SetBorder(int borderX, int borderY, int borderMaxX, int borderMaxY) {
	this->borderX = borderX;
	this->borderY = borderY;
	this->borderMaxX = borderMaxX;
	this->borderMaxY = borderMaxY;
}

void TestScreen::EasingFunctionTestPage::Axis::DrawEasingFunction(CanvasA& canvas, Easing easing) {
	Pixel easingColor = { { 255, 0, 0, 255 },{} };  // 红色曲线

	double startX = 0;
	double endX = gridSpacing * scale * detail;

	// 遍历绘制缓动函数
	for (double i = startX; i <= endX; i += 1) {
		auto t = i / endX;
		// 获取缓动函数的结果值
		double easedT = easing(t);

		// 将结果映射到屏幕坐标系
		double xCoord = posX + (t * scale * ratioX);  // i * scale 用于 x 坐标
		double yCoord = posY - (easedT * scale * ratioY);  // yValue * scale 用于 y 坐标

		// 绘制像素点
		if (xCoord > borderX && xCoord < borderMaxX && yCoord > borderY && yCoord < borderMaxY)
			canvas.SetPixel(xCoord, yCoord, easingColor);
	}
}

void TestScreen::EasingFunctionTestPage::Axis::Render(ScreenA& screen, CanvasA& canvas, Easing easing) {
	Pixel axisColor = { { 255, 255, 255, 255 },{} };  // 白色像素
	Pixel pointColor = { {255,255,255,100}, {} };
	Pixel textColor = { {},{255,255,255,255} };

	auto left = borderX;
	auto right = borderMaxX;
	auto top = borderY;
	auto bottom = borderMaxY;

	// 绘制横轴 (X轴)，水平方向，右侧为正，左侧为负
	if (posY >= top && posY <= bottom)
		canvas.DrawLineH(left, right, posY, axisColor);  // 绘制X轴
	if (posX >= left && posX <= right)
		canvas.DrawLineV(posX, top, bottom, axisColor);  // 绘制Y轴

	// 绘制网格点
	for (int x = posX - (gridSpacing * ratioX); x >= left; x -= (gridSpacing * ratioX)) {
		for (int y = posY - (gridSpacing * ratioY); y >= top; y -= (gridSpacing * ratioY)) {
			canvas.SetPixel(x, y, pointColor);  // 绘制网格点
		}
	}

	for (int x = posX + (gridSpacing * ratioX); x <= right; x += (gridSpacing * ratioX)) {
		for (int y = posY + (gridSpacing * ratioY); y <= bottom; y += (gridSpacing * ratioY)) {
			canvas.SetPixel(x, y, pointColor);  // 绘制网格点
		}
	}

	for (int x = posX - (gridSpacing * ratioX); x >= left; x -= (gridSpacing * ratioX)) {
		for (int y = posY + (gridSpacing * ratioY); y <= bottom; y += (gridSpacing * ratioY)) {
			canvas.SetPixel(x, y, pointColor);  // 绘制网格点
		}
	}

	for (int x = posX + (gridSpacing * ratioX); x <= right; x += (gridSpacing * ratioX)) {
		for (int y = posY - (gridSpacing * ratioY); y >= top; y -= (gridSpacing * ratioY)) {
			canvas.SetPixel(x, y, pointColor);  // 绘制网格点
		}
	}

	// 绘制坐标值 (以10为步长)
	for (int x = posX - (gridSpacing * ratioX); x >= left; x -= (gridSpacing * ratioX)) {
		std::string value = std::to_string((x - posX) / scale / ratioX);
		canvas.DrawString(x, posY + 1, value, textColor);  // 绘制X轴值
	}

	for (int x = posX + (gridSpacing * ratioX); x <= right; x += (gridSpacing * ratioX)) {
		std::string value = std::to_string((x - posX) / scale / ratioX);
		canvas.DrawString(x, posY + 1, value, textColor);  // 绘制X轴值
	}

	// Y轴坐标值
	// 上方值 (正值方向)
	for (int y = posY - (gridSpacing * ratioY); y >= top; y -= (gridSpacing * ratioY)) {
		std::string value = std::to_string((posY - y) / scale / ratioY);
		canvas.DrawString(posX + 1, y, value, textColor);
	}

	// 下方值 (负值方向)
	for (int y = posY + (gridSpacing * ratioY); y <= bottom; y += (gridSpacing * ratioY)) {
		std::string value = std::to_string((posY - y) / scale / ratioY);  // 正值
		canvas.DrawString(posX + 1, y, value, textColor);  // 绘制Y轴值
	}
	DrawEasingFunction(canvas, easing);
};

void TestScreen::EasingFunctionTestPage::Axis::MouseEvent(MouseEventArgs args) {
	if (!InputCondition::Mouse::InArea(borderX, borderY, borderMaxX, borderMaxY, args) && !isDragging) return;
	if (args.buttonState[0]) {
		if (!isDragging) {
			// 鼠标按下时记录偏移量
			dragOffsetX = args.position.X - posX;
			dragOffsetY = args.position.Y - posY;
			isDragging = true;
		}

		// 更新坐标轴的位置
		posX = args.position.X - dragOffsetX;
		posY = args.position.Y - dragOffsetY;
	}
	else {
		// 鼠标松开时停止拖动
		isDragging = false;
	}
	// 处理滚轮缩放
	if (args.wheel.Direction == MouseWheelArgs::Vertical) {  // 纵向滚轮
		if (args.wheel.Delta > 0) {  // 向上滚动，放大
			scale *= 1.1;
		}
		else {  // 向下滚动，缩小
			scale *= 0.9;
		}
	}

	if (args.wheel.Direction == MouseWheelArgs::Horizontal) {  // 横向滚轮
		if (args.wheel.Delta > 0) {  // 向右滚动，放大
			scale *= 1.1;
		}
		else {  // 向左滚动，缩小
			scale *= 0.9;
		}
	}
}

void TestScreen::EasingFunctionTestPage::Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) {
	Interface::Creation(screen, canvas, logger, display);
	easings.push_back(std::make_pair("Linear", GetEasingFunction<EasingFunction::Linear>(0)));
	easings.push_back(std::make_pair("Sine EaseIn", GetEasingFunction<EasingFunction::Sine>(0)));
	easings.push_back(std::make_pair("Sine EaseOut", GetEasingFunction<EasingFunction::Sine>(1)));
	easings.push_back(std::make_pair("Sine EaseInOut", GetEasingFunction<EasingFunction::Sine>(2)));
	easings.push_back(std::make_pair("Pow2 EaseIn", GetEasingFunction<EasingFunction::Pow<2.0>>(0)));
	easings.push_back(std::make_pair("Pow2 EaseOut", GetEasingFunction<EasingFunction::Pow<2.0>>(1)));
	easings.push_back(std::make_pair("Pow2 EaseInOut", GetEasingFunction<EasingFunction::Pow<2.0>>(2)));
	easings.push_back(std::make_pair("Cubic EaseIn", GetEasingFunction<EasingFunction::Cubic>(0)));
	easings.push_back(std::make_pair("Cubic EaseOut", GetEasingFunction<EasingFunction::Cubic>(1)));
	easings.push_back(std::make_pair("Cubic EaseInOut", GetEasingFunction<EasingFunction::Cubic>(2)));
	easings.push_back(std::make_pair("Quad EaseIn", GetEasingFunction<EasingFunction::Quad>(0)));
	easings.push_back(std::make_pair("Quad EaseOut", GetEasingFunction<EasingFunction::Quad>(1)));
	easings.push_back(std::make_pair("Quad EaseInOut", GetEasingFunction<EasingFunction::Quad>(2)));
	easings.push_back(std::make_pair("Quint EaseIn", GetEasingFunction<EasingFunction::Quint>(0)));
	easings.push_back(std::make_pair("Quint EaseOut", GetEasingFunction<EasingFunction::Quint>(1)));
	easings.push_back(std::make_pair("Quint EaseInOut", GetEasingFunction<EasingFunction::Quint>(2)));
	easings.push_back(std::make_pair("Expo EaseIn", GetEasingFunction<EasingFunction::Expo>(0)));
	easings.push_back(std::make_pair("Expo EaseOut", GetEasingFunction<EasingFunction::Expo>(1)));
	easings.push_back(std::make_pair("Expo EaseInOut", GetEasingFunction<EasingFunction::Expo>(2)));
	easings.push_back(std::make_pair("Circ EaseIn", GetEasingFunction<EasingFunction::Circ>(0)));
	easings.push_back(std::make_pair("Circ EaseOut", GetEasingFunction<EasingFunction::Circ>(1)));
	easings.push_back(std::make_pair("Circ EaseInOut", GetEasingFunction<EasingFunction::Circ>(2)));
	easings.push_back(std::make_pair("Back EaseIn", GetEasingFunction<EasingFunction::Back>(0)));
	easings.push_back(std::make_pair("Back EaseOut", GetEasingFunction<EasingFunction::Back>(1)));
	easings.push_back(std::make_pair("Back EaseInOut", GetEasingFunction<EasingFunction::Back>(2)));
	easings.push_back(std::make_pair("Elastic EaseIn", GetEasingFunction<EasingFunction::Elastic>(0)));
	easings.push_back(std::make_pair("Elastic EaseOut", GetEasingFunction<EasingFunction::Elastic>(1)));
	easings.push_back(std::make_pair("Elastic EaseInOut", GetEasingFunction<EasingFunction::Elastic>(2)));
	easings.push_back(std::make_pair("Bounce EaseIn", GetEasingFunction<EasingFunction::Bounce>(0)));
	easings.push_back(std::make_pair("Bounce EaseOut", GetEasingFunction<EasingFunction::Bounce>(1)));
	easings.push_back(std::make_pair("Bounce EaseInOut", GetEasingFunction<EasingFunction::Bounce>(2)));
	easings.push_back(std::make_pair("Flash EaseIn", GetEasingFunction<EasingFunction::Flash>(0)));
	easings.push_back(std::make_pair("Flash EaseOut", GetEasingFunction<EasingFunction::Flash>(1)));
	easings.push_back(std::make_pair("Flash EaseInOut", GetEasingFunction<EasingFunction::Flash>(2)));

	backwardButton.Init();
	backwardButton.SetProperties(new NoobButton::Properties{ 2,7,6,1,"[Back]" });
	backwardButton.addEvent(NoobButton::onMousePressed, [this, logger](void* args, ActionableWidget* widget) {
		if (index != 0) index--;
		logger->info("Back: ", index);
		});

	forwardButton.Init();
	forwardButton.SetProperties(new NoobButton::Properties{ 10,7,6,1,"[Next]" });
	forwardButton.addEvent(NoobButton::onMousePressed, [this, logger](void* args, ActionableWidget* widget) {
		if (index < easings.size() - 1) index++;
		logger->info("Next: ", index);
		});

	slider.Init();
	slider.SetProperties(new Slider::Properties{ 2,13,26,3 });
	slider.setSliderProgeress(1.0 / 26.0);
}

void TestScreen::EasingFunctionTestPage::Render() {
	canvas->DrawString(2, 2, "Easing Function Test", textColor);
	canvas->DrawString(2, 4, "Easing: " + easings[index].first, textColor);
	PosCalculator posCalc({ 2,4,screen->getWidth() - 2,screen->getHeight() - 2 });
	canvas->DrawLineV(30, 1, screen->getHeight(), { {255,255,255,90},{} });
	double detailValue = slider.lerp(0.0, 26.0);
	canvas->DrawString(2, 11, "Function Detail: " + std::to_string(detailValue), textColor);

	axis.SetFunctionDetail(detailValue);
	DrawAxes();
	slider.Render(*screen, *canvas);
	forwardButton.Render(*screen, *canvas);
	backwardButton.Render(*screen, *canvas);
}

void TestScreen::EasingFunctionTestPage::Mouse(MouseEventArgs args) {
	axis.MouseEvent(args);
	slider.Mouse(args);
	forwardButton.Mouse(args);
	backwardButton.Mouse(args);
}

void TestScreen::EasingFunctionTestPage::DrawAxes() {
	axis.SetBorder(31, 1, screen->getWidth(), screen->getHeight());
	axis.Render(*screen, *canvas, easings[index].second);
}

void TestScreen::EasingFunctionTestPage::DrawEasingCurve(Easing easing, int startX, int startY, int spacing) {
	Pixel pixel = { 10, 255, 10, 255 };  // 白色像素

	// 绘制缓动函数的曲线
	for (int x = 0; x < screen->getWidth(); ++x) {
		double t = static_cast<double>(x) / screen->getWidth();  // 计算0到1的时间值
		double easedValue = easing(t);  // 通过缓动函数获得变换后的值

		// 映射到Y坐标
		// 假设值域为R，我们将缓动函数的值映射到屏幕的Y轴范围（height），可以做一个简单的归一化处理
		int y = static_cast<int>((1 - easedValue) * (screen->getHeight() - 1));  // 计算对应的Y坐标（反转Y轴）

		// 绘制像素点
		canvas->SetPixel(startX + x, startY + y, pixel);
	}
}

void TestScreen::SliderTestPage::Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display) {
	Interface::Creation(screen, canvas, logger, display);
	sliderLeft.Init();
	sliderLeft.SetProperties(new Slider::Properties(10, 4, 30, 3, Slider::Direction::HorizontalLeft));
	sliderRight.Init();
	sliderRight.SetProperties(new Slider::Properties(10, 8, 30, 3, Slider::Direction::HorizontalRight));
	sliderUp.Init();
	sliderUp.SetProperties(new Slider::Properties(2, 14, 5, 10, Slider::Direction::VerticalUp));
	sliderDown.Init();
	sliderDown.SetProperties(new Slider::Properties(16, 14, 5, 10, Slider::Direction::VerticalDown));
	sliderStep.Init();
	sliderStep.SetProperties(new Slider::Properties(2, 29, 30, 3, Slider::Direction::HorizontalRight, 5));
	scrollBarHorizontal.Init();
	scrollBarHorizontal.SetProperties(new Scrollbar::Properties(62 + 20, 4, 30, 3, 10, 1000, Slider::Direction::Horizontal));
	scrollBarVertical.Init();
	scrollBarVertical.SetProperties(new Scrollbar::Properties(62 + 20, 8, 5, 10, 10, 1000, Slider::Direction::Vertical));
}

void TestScreen::SliderTestPage::Render() {
	canvas->DrawString(2, 2, "Sliders!", textColor);
	canvas->DrawString(2, 5, "Left:", textColor);
	canvas->DrawString(42, 5, std::to_string(sliderLeft.getSliderProgress()), textColor);
	canvas->DrawString(2, 9, "Right:", textColor);
	canvas->DrawString(42, 9, std::to_string(sliderRight.getSliderProgress()), textColor);
	canvas->DrawString(2, 12, "Up:", textColor);
	canvas->DrawString(2, 25, std::to_string(sliderUp.getSliderProgress()), textColor);
	canvas->DrawString(16, 12, "Down:", textColor);
	canvas->DrawString(16, 25, std::to_string(sliderDown.getSliderProgress()), textColor);
	canvas->DrawString(62 + 20, 2, "ScrollBar", textColor);

	canvas->DrawString(94 + 20, 5, std::to_string(scrollBarHorizontal.getSliderProgress()), textColor);
	canvas->DrawString(69 + 20, 9, std::to_string(scrollBarVertical.getSliderProgress()), textColor);

	canvas->DrawString(2, 27, "Step Slider:  (" + std::to_string(sliderStep.getSliderProgress()) + ")", textColor);

	sliderLeft.Render(*screen, *canvas);
	sliderRight.Render(*screen, *canvas);
	sliderUp.Render(*screen, *canvas);
	sliderDown.Render(*screen, *canvas);
	sliderStep.Render(*screen, *canvas);
	scrollBarHorizontal.Render(*screen, *canvas);
	scrollBarVertical.Render(*screen, *canvas);
}

void TestScreen::SliderTestPage::Mouse(MouseEventArgs args) {
	sliderLeft.Mouse(args);
	sliderRight.Mouse(args);
	sliderUp.Mouse(args);
	sliderDown.Mouse(args);
	sliderStep.Mouse(args);
	scrollBarHorizontal.Mouse(args);
	scrollBarVertical.Mouse(args);

}

void TestScreen::Creation(ScreenA* screen, CanvasA* canvas, LogOverlay* logger, Displayer* display)  {
	Interface::Creation(screen, canvas, logger, display);
	ifManager.addInterface(0, &anPage);
	ifManager.addInterface(1, &anPage2);
	ifManager.addInterface(2, &textInputerPage);
	ifManager.addInterface(3, &easingTestPage);
	ifManager.addInterface(4, &sliderTestPage);
	ifManager.addInterface(5, &viewportTestPage);
	ifManager.addDeque(0);
	ifManager.addDeque(1);
	ifManager.addDeque(2);
	ifManager.addDeque(3);
	ifManager.addDeque(4);
	ifManager.addDeque(5);
	ifManager.UsingInvoke(0);
	ifManager.Creation(screen, canvas, logger, display);
	ifManager.UsingInvoke(1);
	ifManager.activateInterface(PageId);

	forwardButton.Init();
	forwardButton.SetProperties(new Button::Properties({ 0,0,5,1,"---->" }));
	forwardButton.addEvent(NoobButton::_MouseEventEx_, [this, logger](void* _args, ActionableWidget* _self) {
		using Cond = InputCondition::Mouse;
		NoobButton::MouseEventArgsEx args = *reinterpret_cast<NoobButton::MouseEventArgsEx*>(_args);
		if (Cond::IsClicked(Cond::Left, args.pressResult)) {
			PageId = std::min(PageId + 1, (int)PageMax);
			ifManager.activateInterface(PageId);
			logger->info("Forward to ", (int)PageId);
		}
		});

	backwardButton.Init();
	backwardButton.SetProperties(new Button::Properties({ 0,0,5,1,"<----" }));
	backwardButton.addEvent(NoobButton::_MouseEventEx_, [this, logger](void* _args, ActionableWidget* _self) {
		using Cond = InputCondition::Mouse;
		NoobButton::MouseEventArgsEx args = *reinterpret_cast<NoobButton::MouseEventArgsEx*>(_args);
		if (Cond::IsClicked(Cond::Left, args.pressResult)) {
			PageId = std::max(PageId - 1, 0);
			ifManager.activateInterface(PageId);
			logger->info("Backward to ", (int)PageId);
		}
	});
}

void TestScreen::Render()  {
	PosCalculator calc({ 0,0,screen->getWidth(),screen->getHeight() });
	auto& prop = *forwardButton.getProperties<Button::Properties>();
	prop.X = calc.alignRight(5);
	std::string msg = "Page " + std::to_string(PageId);
	canvas->DrawLineH(5, screen->getWidth() - 5, 0, { {200,200,200,90},{} });
	canvas->DrawString(calc.alignCenterH(msg.length()), 0, msg, textColor);

	ifManager.Render();
	backwardButton.Render(*screen, *canvas);
	forwardButton.Render(*screen, *canvas);
	fpsOverlay.Render(*screen, *canvas);
}

void TestScreen::Mouse(MouseEventArgs args)  {
	backwardButton.Mouse(args);
	forwardButton.Mouse(args);
	ifManager.Mouse(args);
}

void TestScreen::Key(KeyEventArgs args)  {
	try {
		ifManager.Key(args);
	}
	catch (std::exception e) {
		logger->error(e.what());
	}
}

void TestScreen::BackgroundLogic()  {
	ifManager.BackgroundLogic();
}