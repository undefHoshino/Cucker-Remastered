#include "TextInputControl.h"

#undef min
#undef max

///TextInputControl::LineColumn

bool TextInputControl::LineColumn::islegal(LineColumn a, LineColumn b) {
	if (a.row < b.row) {
		return true;  // a 在 b 上方，合法
	}
	if (a.row == b.row && a.column <= b.column) {
		return true;  // 同一行，a 在 b 左边或等于，合法
	}
	return false;  // 否则，非法
}

bool TextInputControl::LineColumn::operator==(LineColumn other) {
	return this->row == other.row && this->column == other.column;
}

/// TextInputControl::LineBuffer

void TextInputControl::LineBuffer::pop(size_t pos) {
	if (!buffer.empty() && pos != 0) {
		buffer.erase(buffer.begin() + pos - 1);
	}
}

void TextInputControl::LineBuffer::push(size_t pos, wchar_t c) {
	if (pos > buffer.size()) return;
	buffer.insert(buffer.begin() + pos, c);
}

void TextInputControl::LineBuffer::erase(size_t left, size_t right) {
	if (buffer.empty() || left >= right || right > buffer.size()) {
		return; // 边界检查
	}
	buffer.erase(buffer.begin() + left, buffer.begin() + right);
}

void TextInputControl::LineBuffer::shrink() {
	buffer.shrink_to_fit();
}

void TextInputControl::LineBuffer::insert(size_t pos, std::wstring wstr) {
	if (pos > buffer.size()) return;
	buffer.insert(buffer.begin() + pos, wstr.begin(), wstr.end());
}

void TextInputControl::LineBuffer::insert(size_t pos, std::vector<wchar_t> vec) {
	if (pos > buffer.size()) return;
	buffer.insert(buffer.begin() + pos, vec.begin(), vec.end());
}

bool TextInputControl::LineBuffer::empty() {
	return buffer.empty();
}

wchar_t TextInputControl::LineBuffer::get(size_t pos) {
	if (pos < buffer.size()) {
		return buffer[pos];
	}
	return 0;
}

size_t TextInputControl::LineBuffer::size() {
	return buffer.size();
}

std::vector<wchar_t>& TextInputControl::LineBuffer::data() {
	return buffer;
}

/// TextInputControl::Buffer::SafeLineRemover

void TextInputControl::Buffer::SafeLineRemover::remove(size_t index) {
	lines.push(index);
}

void TextInputControl::Buffer::SafeLineRemover::execute() {
	while (!lines.empty()) {
		parent->removeLine(lines.top());
		lines.pop();
	}
}

TextInputControl::Buffer::SafeLineRemover::~SafeLineRemover() {
	if (!lines.empty()) execute();
}

/// TextInputControl::Buffer

void TextInputControl::Buffer::push(LineColumn pos, wchar_t c) {
	getLine(pos.row).push(pos.column, c);
}

void TextInputControl::Buffer::pop(LineColumn pos) {
	getLine(pos.row).pop(pos.column);
}

void TextInputControl::Buffer::erase(LineColumn start, LineColumn end) {
	if (buffer.empty() || !LineColumn::islegal(start, end)) {
		return;
	}

	if (start.row == end.row) {
		eraseAtLine(start, start.column, end.column);
		return;
	}

	SafeLineRemover remover(this);

	// 删除起始行的部分内容
	auto& startLine = getLine(start.row);
	startLine.erase(start.column, startLine.size());
	
	// 删除从start.row+1到end.row-1之间的所有行
	for (size_t y = start.row + 1; y < end.row; ++y) {
		remover.remove(y);
	}

	// 删除结束行的内容
	auto& lastLine = getLine(end.row - 1);
	auto& endLine = getLine(end.row);

	endLine.erase(0, end.column);
	auto tempData = endLine.data();
	remover.remove(end.row);
	startLine.insert(startLine.size(), tempData);

	//~SafeLineRemover();
}

void TextInputControl::Buffer::eraseAtLine(LineColumn pos, size_t left, size_t right) {
	getLine(pos.row).erase(left, right);
}

void TextInputControl::Buffer::insertAtLine(LineColumn pos, std::wstring wstr) {
	getLine(pos.row).insert(pos.column, wstr);
}

void TextInputControl::Buffer::insertAtLine(LineColumn pos, std::vector<wchar_t> vec) {
	getLine(pos.row).insert(pos.column, vec);
}

TextInputControl::LineColumn TextInputControl::Buffer::insert(LineColumn pos, std::vector<LineBuffer> vec) {
	size_t currentPosX = pos.column;
	size_t currentPosY = pos.row;
	auto& _currentline = getLine(pos.row);

	std::vector<wchar_t> tempBuffer(_currentline.data().begin() + pos.column, _currentline.data().end());
	_currentline.erase(pos.column, _currentline.size());

	bool firstLine = true;
	bool overflow = false;
	for (auto& line : vec) {
		if (!forEachInsertOperator(_currentline, line, currentPosX, currentPosY, firstLine)) {
			overflow = false;
			continue;
		}
		overflow = true;
	}
	if (currentPosY != 0 || overflow)
		currentPosY--; // overflow

	insertAtLine({ currentPosX ,currentPosY }, tempBuffer);
	return { currentPosX,currentPosY };
}

TextInputControl::LineColumn TextInputControl::Buffer::insert(LineColumn pos, std::wstring wstr, std::vector<LineBuffer>* vecOutput) {
	std::wstringstream stream(wstr);
	std::wstring line;
	size_t currentPosX = pos.column;
	size_t currentPosY = pos.row;
	auto& _currentline = getLine(pos.row);

	std::vector<wchar_t> tempBuffer(_currentline.data().begin() + pos.column, _currentline.data().end());
	_currentline.erase(pos.column, _currentline.size());

	bool firstLine = true;
	bool overflow = false;
	while (std::getline(stream, line)) {
		processTabsAndControlChars(line);
		if (!forEachInsertOperator(_currentline, line, currentPosX, currentPosY, firstLine)) {
			auto& back = vecOutput->back();
			back.insert(back.size(), line);
			overflow = false;

			continue;
		}
		vecOutput->push_back(line);
		overflow = true;
	}
	if(currentPosY != 0 || overflow)
		currentPosY--; // overflow

	insertAtLine({ currentPosX ,currentPosY }, tempBuffer);
	if (!tempBuffer.empty()) {
		vecOutput->push_back(tempBuffer);
	}
	return { currentPosX,currentPosY };
}

void TextInputControl::Buffer::pushLine(std::vector<wchar_t> buf) {
	buffer.push_back(LineBuffer(std::move(buf)));
}

void TextInputControl::Buffer::newLine(size_t row, LineBuffer buf) {
	buffer.insert(buffer.begin() + row, buf);
}

void TextInputControl::Buffer::insertLine(size_t pos, std::vector<wchar_t> buf) {
	buffer.insert(buffer.begin() + pos, LineBuffer(std::move(buf)));
}

void TextInputControl::Buffer::removeLine(size_t lineNum) {
	if (lineNum < buffer.size()) {
		buffer.erase(buffer.begin() + lineNum);
	}
}

void TextInputControl::Buffer::shrink() {
	for (auto& ptr : buffer) {
		ptr.shrink();
	}
	buffer.shrink_to_fit();
}

wchar_t TextInputControl::Buffer::getChar(LineColumn pos) {
	return buffer[pos.row].get(pos.column);
}

TextInputControl::LineBuffer& TextInputControl::Buffer::getLine(size_t lineNum) {
	if (lineNum < buffer.size()) {
		return buffer[lineNum];
	}
	lineOutofRangeException(lineNum);
}

void TextInputControl::Buffer::lineOutofRangeException(size_t lineNum) {
	throw std::out_of_range("LineNum out of range: " + std::to_string(lineNum));
}

std::vector<TextInputControl::LineBuffer>& TextInputControl::Buffer::data() {
	return buffer;
}

size_t TextInputControl::Buffer::size() const {
	return buffer.size();
}

void TextInputControl::Buffer::processTabsAndControlChars(std::wstring& line) {
	size_t pos = 0;
	std::wstring spaces = L"    ";  // 4 spaces string

	while ((pos = line.find(L'\t', pos)) != std::wstring::npos) {
		line.replace(pos, 1, spaces);  // 替换\t为4个空格
		pos += 4;  // 跳过已插入的4个空格
	}

	// 过滤掉所有ASCII控制字符
	line.erase(std::remove_if(line.begin(), line.end(), [](wchar_t c) {
		return c < 32;
		}), line.end());
}

bool TextInputControl::Buffer::checkIsOutofMaxLine() {
	return buffer.size() >= maxLine;
}

void TextInputControl::Buffer::setMaxLine(size_t maxline) {
	this->maxLine = maxline;
}

size_t TextInputControl::Buffer::getMaxLine() {
	return maxLine;
}

/// TextInputControl::Cursor

TextInputControl::LineColumn TextInputControl::Cursor::setPosition(size_t column, size_t row) {
	if (row != -1) cursorPos.row = std::min(row, buffer->size());
	if (column != -1) cursorPos.column = std::min(column, buffer->getLine(cursorPos.row).size());
	return cursorPos;
}

TextInputControl::LineColumn TextInputControl::Cursor::moveStep(int64_t column, int64_t row) {
	if (row < 0) {
		cursorPos.row = std::max((int64_t)0, (int64_t)cursorPos.row + row);
	}
	else {
		cursorPos.row = std::min((int64_t)cursorPos.row + row, (int64_t)buffer->size() - 1);
	}
	if (column < 0) {
		cursorPos.column = std::max((int64_t)0, (int64_t)cursorPos.column + column);
	}
	else {
		cursorPos.column = std::min((int64_t)cursorPos.column + column, (int64_t)buffer->getLine(cursorPos.row).size());
	}
	return cursorPos;
}

TextInputControl::LineColumn TextInputControl::Cursor::getPosition() {
	return cursorPos;
}

/// TextInputControl::Anchor

TextInputControl::LineColumn TextInputControl::Anchor::setPosition(size_t column, size_t row) {
	if (row != -1) anchorPos.row = std::min(row, buffer->size());
	if (column != -1) anchorPos.column = std::min(column, buffer->getLine(anchorPos.row).size());
	return anchorPos;
}

TextInputControl::LineColumn TextInputControl::Anchor::moveStep(int64_t column, int64_t row) {
	if (row < 0) {
		anchorPos.row = std::max((int64_t)0, (int64_t)anchorPos.row + row);
	}
	else {
		anchorPos.row = std::min((int64_t)anchorPos.row + row, (int64_t)buffer->size() - 1);
	}
	if (column < 0) {
		anchorPos.column = std::max((int64_t)0, (int64_t)anchorPos.column + column);
	}
	else {
		anchorPos.column = std::min((int64_t)anchorPos.column + column, (int64_t)buffer->getLine(anchorPos.row).size());
	}
	return anchorPos;
}

void TextInputControl::Anchor::init() {
	active = true;
	anchorPos = cursor->getPosition();
}

void TextInputControl::Anchor::close() {
	active = false;
}

bool TextInputControl::Anchor::isActive() {
	return active;
}
size_t TextInputControl::Anchor::startColumn() {
	auto cursorPos = cursor->getPosition();
	if (anchorPos.row == cursorPos.row) {
		if (anchorPos.column >= cursorPos.column) return cursorPos.column;
		else return anchorPos.column;
	}
	if (anchorPos.row >= cursorPos.row) return cursorPos.column;
	else return anchorPos.column;
}
size_t TextInputControl::Anchor::endColumn() {
	auto cursorPos = cursor->getPosition();
	if (anchorPos.row == cursorPos.row) {
		if (anchorPos.column >= cursorPos.column) return anchorPos.column;
		else return cursorPos.column;
	}
	if (anchorPos.row > cursorPos.row) return anchorPos.column;
	else return cursorPos.column;
}
size_t TextInputControl::Anchor::startRow() {
	auto cursorPos = cursor->getPosition();
	if (anchorPos.row >= cursorPos.row) return cursorPos.row;
	else return anchorPos.row;
}
size_t TextInputControl::Anchor::endRow() {
	auto cursorPos = cursor->getPosition();
	if (anchorPos.row >= cursorPos.row) return anchorPos.row;
	else return cursorPos.row;
}

TextInputControl::LineColumn TextInputControl::Anchor::getPosition() {
	return anchorPos;
}

/// TextInputControl::Histroy

void TextInputControl::Histroy::record(std::vector<LineBuffer> buffer, LineColumn cursorPos, LineColumn anchorPos, size_t lineRowPos, InputAction::Operation operation) {
	if (cursorPos == anchorPos) return;
	InputAction inAction(std::move(buffer), cursorPos, anchorPos, lineRowPos, operation);
	undoStack.push(inAction);
	clearRedoStack();
}

void TextInputControl::Histroy::undo() {
	if (undoStack.empty()) return;
	InputAction action = undoStack.top();

	undoStack.pop();
	redoStack.push(action);

	auto start = action.cursorPos;
	auto end = action.anchorPos;
	sort(start, end);

	switch (action.operation) {
	case Histroy::Operation::Insert: {
		// 撤销插入：删除文本
		buffer->erase(start, end);
		cursor->setPosition(action.cursorPos.column, action.cursorPos.row);
		anchor->close();
		break;
	}
	case Histroy::Operation::Erase: {
		// 撤销删除：插入文本
		buffer->insert(start, action.buffer.data());
		cursor->setPosition(action.cursorPos.column, action.cursorPos.row);
		anchor->setPosition(action.anchorPos.column, action.anchorPos.row);
		break;
	}
	case Histroy::Operation::AddLine: {
		// 撤销添加行：删除新添加的行
		buffer->removeLine(action.lineRowPos);
		cursor->moveStep(0, -1);
		cursor->setPosition(action.cursorPos.column, -1);
		anchor->close();
		break;
	}
	case Histroy::Operation::RemoveLine: {
		// 撤销删除行：恢复删除的行
		buffer->newLine(action.lineRowPos, action.buffer.data()[0]);
		cursor->moveStep(0, 1);
		cursor->setPosition(action.cursorPos.column, -1);
		break;
	}
	}
}

void TextInputControl::Histroy::redo() {
	if (redoStack.empty()) return;
	InputAction action = redoStack.top();

	redoStack.pop();
	undoStack.push(action);

	auto start = action.cursorPos;
	auto end = action.anchorPos;
	sort(start, end);

	switch (action.operation) {
	case Histroy::Operation::Insert: {
		// 重做插入：插入文本
		buffer->insert(start, action.buffer.data());
		cursor->setPosition(action.cursorPos.column, action.cursorPos.row);
		anchor->setPosition(action.anchorPos.column, action.anchorPos.row);
		break;
	}
	case Histroy::Operation::Erase: {
		// 重做删除：删除文本
		buffer->erase(start, end);
		cursor->setPosition(action.cursorPos.column, action.cursorPos.row);
		anchor->close();
		break;
	}
	case Histroy::Operation::AddLine: {
		// 重做删除行：恢复删除的行
		buffer->newLine(action.lineRowPos, action.buffer.data()[0]);
		cursor->moveStep(0, 1);
		cursor->setPosition(action.cursorPos.column, -1);
		break;
	}
	case Histroy::Operation::RemoveLine: {
		// 重做添加行：删除新添加的行
		buffer->removeLine(action.lineRowPos);
		cursor->moveStep(0, -1);
		cursor->setPosition(action.cursorPos.column, -1);
		anchor->close();
		break;
	}
	};
}

void TextInputControl::Histroy::sort(LineColumn& a, LineColumn& b) {
	if (a.row == b.row) {
		if (a.column > b.column) std::swap(a.column, b.column);
	}
	if (a.row > b.row) {
		std::swap(a.row, b.row);
		std::swap(a.column, b.column);
	}
}

void TextInputControl::Histroy::clearRedoStack() {
	if (!redoStack.empty())
		redoStack = std::stack<InputAction>();
}

void TextInputControl::Histroy::clearHistroy() {
	undoStack = std::stack<InputAction>();
	redoStack = std::stack<InputAction>();
}

void TextInputControl::Histroy::Inserted(std::vector<LineBuffer> buffer, LineColumn cursorPos, LineColumn anchorPos) {
	record(buffer, cursorPos, anchorPos, -1, Operation::Insert);
}

void TextInputControl::Histroy::Erased(std::vector<LineBuffer> buffer, LineColumn cursorPos, LineColumn anchorPos) {
	record(buffer, cursorPos, anchorPos, -1, Operation::Erase);
}

void TextInputControl::Histroy::LineAdded(LineBuffer buffer, LineColumn currentCursorPos, size_t lineRowPos) {
	record({ buffer }, currentCursorPos, { size_t(-1),size_t(-1) }, lineRowPos, Operation::AddLine);
}

void TextInputControl::Histroy::LineRemoved(LineBuffer buffer, LineColumn currentCursorPos, size_t lineRowPos) {
	record({ buffer }, currentCursorPos, { size_t(-1),size_t(-1) }, lineRowPos, Operation::RemoveLine);
}

std::stack<TextInputControl::Histroy::InputAction>& TextInputControl::Histroy::getUndoStack() {
	return undoStack;
}

std::stack<TextInputControl::Histroy::InputAction>& TextInputControl::Histroy::getRedoStack() {
	return redoStack;
}

/// TextInputControl::TextEditor

void TextInputControl::TextEditor::onDelete() {
	if (anchor->isActive()) {
		onCut();
		return;
	}

	if (getCursorPos().column != 0) {
		onPop();
	}
	else {
		onDelLine();
	}

}

void TextInputControl::TextEditor::onCut() {
	histroy->Erased(parent->convertToLineBuffers(parent->getSelectedBuffer()), cursor->getPosition(), anchor->getPosition());
	buffer->erase({ anchor->startColumn(),anchor->startRow() }, { anchor->endColumn(),anchor->endRow() });
	cursor->setPosition(anchor->startColumn(), anchor->startRow());
	anchor->close();
}

void TextInputControl::TextEditor::onPop() {
	auto&& cursorPosition = getCursorPos();
	if (cursorPosition.column != 0) {
		histroy->Erased(
			{ LineBuffer({buffer->getChar({cursorPosition.column - 1,cursorPosition.row})}) },
			{ cursorPosition.column - 1,cursorPosition.row }, cursorPosition
		);
	}

	buffer->pop(cursorPosition);
	cursor->moveStep(-1, 0);
}

void TextInputControl::TextEditor::onPush(wchar_t c) {
	if (c < 32 || c == '\x7F') {
		ReadControlChar(c);
		return;
	}

	if (anchor->isActive()) {
		onCut();
	}
	auto&& cursorPos = getCursorPos();
	auto column = cursorPos.column + 1;
	auto row = cursorPos.row;
	histroy->Inserted({ LineBuffer({ c }) }, cursorPos, { column , row });
	buffer->push(cursor->getPosition(), c);
	cursor->moveStep(1, 0);
}

void TextInputControl::TextEditor::onCopy() {
	clipboard->copyWstring(parent->getSelectedWstring());
}

void TextInputControl::TextEditor::onPaste() {
	if (anchor->isActive()) {
		onCut();
	}
	std::wstring&& wstr = clipboard->pasteWstring();

	auto&& cursorPos = getCursorPos();
	auto column = cursorPos.column + wstr.length();
	auto row = cursorPos.row;
	std::vector<LineBuffer> output;
	LineColumn lConlumn = buffer->insert(cursorPos, wstr, &output);
	histroy->Inserted(output, cursorPos, lConlumn);
	cursor->setPosition(lConlumn.column, lConlumn.row);
}

void TextInputControl::TextEditor::onNextLine() {
	if (anchor->isActive()) {
		onCut();
	}
	shouldNewLineAndMove();
}

void TextInputControl::TextEditor::onDelLine() {
	auto cursorPos = cursor->getPosition();
	if (buffer->size() <= 1) return;
	if (cursor->getPosition().column == 0) {
		shouldRemoveLineAndMove(cursorPos);
	}
}

void TextInputControl::TextEditor::onTab() {
	auto cursorPos = cursor->getPosition();
	std::vector<wchar_t> vec(L"    ", L"    " + 4);
	histroy->Inserted({ std::move(vec) }, cursorPos, { cursorPos.column + 4, cursorPos.row });
	getThisLine().insert(getCursorPos().column, L"    ");
	cursor->moveStep(4, 0);
}

void TextInputControl::TextEditor::shouldNewLineAndMove() {
	if (buffer->checkIsOutofMaxLine()) return;
	auto cursorPos = cursor->getPosition();
	auto tempData = std::vector<wchar_t>(
		buffer->getLine(cursorPos.row).data().begin() + cursorPos.column,
		buffer->getLine(cursorPos.row).data().end()
		);
	histroy->Erased({ tempData }, cursorPos, { cursorPos.column + tempData.size(), cursorPos.row });
	histroy->LineAdded(tempData, cursorPos, cursorPos.row + 1);

	buffer->eraseAtLine(cursorPos, cursorPos.column, buffer->getLine(cursorPos.row).size());
	buffer->newLine(cursorPos.row + 1, std::move(tempData));
	cursor->moveStep(0, 1);
	cursor->setPosition(0, -1);
}
void TextInputControl::TextEditor::shouldRemoveLineAndMove(LineColumn& cursorPos) {
	if (cursorPos.row == 0) return;
	auto& currentLine = buffer->getLine(cursorPos.row);
	auto& prevLine = buffer->getLine(cursorPos.row - 1);
	auto tempData = std::vector<wchar_t>(
		currentLine.data().begin(),
		currentLine.data().end()
		);
	histroy->Inserted({ tempData },
		{ cursorPos.column + prevLine.size(),cursorPos.row - 1 },
		{ cursorPos.column + prevLine.size() + tempData.size(),cursorPos.row - 1 }
	);
	histroy->LineRemoved(tempData, cursorPos, cursorPos.row);
	// First Moving
	cursor->moveStep(0, -1);
	cursor->setPosition(prevLine.size(), -1);
	// Second Buffer operation
	prevLine.insert(prevLine.size(), buffer->getLine(cursorPos.row).data());
	buffer->removeLine(cursorPos.row);
}

void TextInputControl::TextEditor::ReadControlChar(wchar_t c) {
	switch (c) {
	case '\t':
		onTab();
		break;
	case '\n':
	case '\r': // NextLine
		onNextLine();
		break;
	case '\b':
	case '\x7F': {//Del
		onDelete();
		break;
	}
	default:
		break;
	}
}

TextInputControl::LineBuffer& TextInputControl::TextEditor::getThisLine() {
	return buffer->getLine(cursor->getPosition().row);
}

TextInputControl::LineColumn TextInputControl::TextEditor::getCursorPos() {
	return cursor->getPosition();
}

TextInputControl::LineColumn TextInputControl::TextEditor::getAnchorPos() {
	return anchor->getPosition();
}

/// TextInputControl

TextInputControl::TextInputControl() :cursor(&buffer), anchor(&buffer, &cursor), histroy(&buffer, &cursor, &anchor), editor(this) {
	buffer.pushLine({});
};

void TextInputControl::cursorHandle(KeyEventArgs args) {
	switch (args.keyCode) {
	case VirtualKeyCode::LeftArrow: {
		if (anchor.isActive()) {
			cursor.setPosition(anchor.startColumn(), anchor.startRow());
		}
		else {
			cursor.moveStep(-1, 0);
		}
		anchor.close();
		break;
	}
	case VirtualKeyCode::RightArrow: {
		if (anchor.isActive()) {
			cursor.setPosition(anchor.endColumn(), anchor.endRow());
		}
		else {
			cursor.moveStep(1, 0);
		}
		anchor.close();
		break;
	}
	case VirtualKeyCode::UpArrow: {
		if (anchor.isActive()) {
			cursor.setPosition(anchor.startColumn(), anchor.startRow());
			cursor.moveStep(-1, -1);
		}
		else {
			cursor.moveStep(0, -1);
		}
		anchor.close();
		break;
	}
	case VirtualKeyCode::DownArrow: {
		if (anchor.isActive()) {
			cursor.setPosition(anchor.endColumn(), anchor.endRow());
			cursor.moveStep(1, 1);
		}
		else {
			cursor.moveStep(0, 1);
		}
		anchor.close();
		break;
	}
	case VirtualKeyCode::Home: { // X min
		cursor.setPosition(0, -1);
		anchor.close();
		break;
	}
	case VirtualKeyCode::End: { // X max
		cursor.setPosition(buffer.getLine(cursor.getPosition().row).size(), -1);
		anchor.close();
		break;
	}
	}
}

void TextInputControl::ctrlKeyCombHandle(KeyEventArgs args) {
	if (!anchor.isActive()) anchor.init();
	switch (args.keyCode) {
	case VirtualKeyCode::LeftArrow:
		anchor.moveStep(-1,0);
		break;
	case VirtualKeyCode::RightArrow:
		anchor.moveStep(1,0);
		break;
	case VirtualKeyCode::UpArrow:
		anchor.moveStep(0, -1);
		break;
	case VirtualKeyCode::DownArrow:
		anchor.moveStep(0, 1);
		break;
	case VirtualKeyCode::Home:
		anchor.setPosition(0,-1);
		break;
	case VirtualKeyCode::End:
		anchor.setPosition(buffer.getLine(anchor.getPosition().row).size(),-1);
		break;
	case VirtualKeyCode::A: { // Select All
		cursor.setPosition(0, 0);
		auto& lastline = buffer.getLine(buffer.size() - 1);
		anchor.setPosition(lastline.size(), buffer.size() - 1);
		break;
	}
	case VirtualKeyCode::C: // Copy
	case VirtualKeyCode::Insert:
		editor.onCopy();
		break;
	case VirtualKeyCode::X:
		editor.onCopy();
		editor.onCut();
		break;
	case VirtualKeyCode::D:
		//clear();
		break;
	case VirtualKeyCode::V: // Paste
		editor.onPaste();
		break;
	case VirtualKeyCode::Y:
		histroy.redo();
		break;
	case VirtualKeyCode::Z:
		histroy.undo();
		break;
	}
}

void TextInputControl::KeyEvent(KeyEventArgs args) {
	if (args.keyPressed && isEnabled()) {
		if (args.controlKeyState & LEFT_CTRL_PRESSED) {
			ctrlKeyCombHandle(args);
			return;
		}

		cursorHandle(args);
		if (args.unicodeChar != 0) {
			editor.onPush(args.unicodeChar);
		}
	}
}

void TextInputControl::enable() {
	IgnoreCtrlCExit(true);
	Enable = true;
}

void TextInputControl::disable() {
	IgnoreCtrlCExit(false);
	Enable = false;
	shrink();
}

bool TextInputControl::isEnabled() {
	return Enable;
}

void TextInputControl::shrink() {
	buffer.shrink();
	histroy.clearHistroy();
}

void TextInputControl::setMaxLine(size_t maxline) {
	buffer.setMaxLine(maxline);
}

size_t TextInputControl::getMaxLine() {
	return buffer.getMaxLine();
}


TextInputControl::LineColumn TextInputControl::getCursorPosition() {
	return cursor.getPosition();
}

size_t TextInputControl::getSelectedLineCount() {
	return anchor.startRow() - anchor.endRow() + 1;
}

void TextInputControl::forEachSelectedBuffer(std::function<void(LineBuffer&, size_t, size_t)> func) {
	size_t startX = anchor.startColumn();
	size_t endX = anchor.endColumn();
	size_t startY = anchor.startRow();
	size_t endY = anchor.endRow();    // 选区的结束行

	if (startY == endY) {
		auto& line = buffer.getLine(startY);
		func(line, startX, endX);
		return;
	}

	// 逐行处理选区
	for (size_t y = startY; y <= endY; ++y) {
		auto& line = buffer.getLine(y);  // 获取当前行
		if (y == startY) {
			// 对于第一行，从 anchor.startColumn() 到 line.size()
			startX = anchor.startColumn();
			endX = line.size();
		}
		else if (y == endY) {
			// 对于最后一行，从 anchor.endColumn() 到 line.size()
			startX = 0;
			endX = anchor.endColumn();
		}
		else {
			// 对于中间行，整个行都被选中
			startX = 0;
			endX = line.size();
		}
		func(line, startX, endX);
	}
}

std::vector<std::vector<wchar_t>> TextInputControl::getSelectedBuffer() {
	if (!anchor.isActive()) return {};
	std::vector<std::vector<wchar_t>> result;

	forEachSelectedBuffer([&result](LineBuffer& line, size_t start, size_t end) {
		std::vector<wchar_t> selectedLine(line.data().begin() + start, line.data().begin() + end);
		result.push_back(std::move(selectedLine));  // 将选中的内容添加到结果中
	});

	return result;
}

std::wstring TextInputControl::getSelectedWstring() {
	if (!anchor.isActive()) return L"";

	std::wstring result = L"";
	forEachSelectedBuffer([&result](LineBuffer& line, size_t start, size_t end) {
		result.append(line.data().begin() + start, line.data().begin() + end);
		result.push_back(L'\n');
	});

	if (!result.empty()) {
		result.pop_back();  // 去掉最后一个多余的换行符
	} 
	return result;
}

std::vector<TextInputControl::LineBuffer> TextInputControl::convertToLineBuffers(std::vector<std::vector<wchar_t>>&& buf) {
	std::vector<LineBuffer> selectedLineBuffers;
	selectedLineBuffers.reserve(buf.size());  // 预分配内存

	for (auto& line : buf) {
		// 使用构造函数将每行的 std::vector<wchar_t> 转为 LineBuffer
		selectedLineBuffers.emplace_back(std::move(line));  // 使用 move 以避免复制
	}

	return selectedLineBuffers;
}

std::wstring TextInputControl::getWstring(Buffer& buffer) {
	std::wstring result = L"";
	for (auto& ptr : buffer.data()) {
		result.append(ptr.data().begin(), ptr.data().end());
		result.push_back(L'\n');
	}
	if (!result.empty()) {
		result.pop_back();  // 去掉最后一个多余的换行符
	}
	return result;
}

std::wstring TextInputControl::getWstring() {
	return getWstring(buffer);
}

std::string TextInputControl::getString() {
	return Converter::Convert<std::wstring, std::string>(getWstring());
}

TextInputControl::Buffer& TextInputControl::getBuffer() {
	return buffer;
}

TextInputControl::Cursor& TextInputControl::getCursor() {
	return cursor;
}

TextInputControl::Anchor& TextInputControl::getAnchor() {
	return anchor;
}

Clipboard& TextInputControl::getClipboard() {
	return clipboard;
}

TextInputControl::Histroy& TextInputControl::getHistroy() {
	return histroy;
}

TextInputControl::TextEditor& TextInputControl::getTextEditor() {
	return editor;
}



//////////// vvv  Reversed  vvv ////////////
/*
#include "WidgetModule.h"

#undef min
#undef max

void TextInputControl::Buffer::pop(size_t pos) {
	if (!buffer.empty() && pos != 0) {
		buffer.erase(buffer.begin() + pos - 1);
	}
}

void TextInputControl::Buffer::push(size_t pos, wchar_t c) {
	if (pos > buffer.size()) return;
	buffer.insert(buffer.begin() + pos, c);
}

void TextInputControl::Buffer::erase(size_t left, size_t right) {
	if (buffer.empty() || left >= right || right > buffer.size()) {
		return; // 边界检查
	}
	buffer.erase(buffer.begin() + left, buffer.begin() + right);
}

void TextInputControl::Buffer::shrink() {
	buffer.shrink_to_fit();
}

void TextInputControl::Buffer::insert(size_t pos, std::wstring wstr) {
	if (pos > buffer.size()) return;
	buffer.insert(buffer.begin() + pos, wstr.begin(), wstr.end());
}

void TextInputControl::Buffer::insert(size_t pos, std::vector<wchar_t> vec) {
	if (pos > buffer.size()) return;
	buffer.insert(buffer.begin() + pos, vec.begin(), vec.end());
}

bool TextInputControl::Buffer::empty() {
	return buffer.empty();
}

wchar_t TextInputControl::Buffer::get(size_t pos) {
	if (pos < buffer.size()) {
		return buffer[pos];
	}
	return 0;
}

size_t TextInputControl::Buffer::size() {
	return buffer.size();
}

std::vector<wchar_t>& TextInputControl::Buffer::data() {
	return buffer;
}

size_t TextInputControl::Cursor::setPosition(size_t pos) {
	cursorPos = buffer->empty() ? 0 : std::min(pos, buffer->size());
	return cursorPos;
}

size_t TextInputControl::Cursor::moveStep(int64_t step) {
	if (step < 0 && std::abs(step) > cursorPos) {
		cursorPos = 0;
	}
	else {
		cursorPos = std::min(cursorPos + step, buffer->size());
	}
	return cursorPos;
}

size_t TextInputControl::Cursor::getPosition() {
	return cursorPos;
}

size_t TextInputControl::Selection::setPosition(size_t pos) {
	selectionPos = buffer->empty() ? 0 : std::min(pos, buffer->size());
	return selectionPos;
}

size_t TextInputControl::Selection::moveStep(int64_t step) {
	if (step < 0 && std::abs(step) > selectionPos) {
		selectionPos = 0;
	}
	else {
		selectionPos = std::min(selectionPos + step, buffer->size());
	}
	return selectionPos;
}

void TextInputControl::Selection::init() {
	active = true;
	selectionPos = cursor->getPosition();
}

void TextInputControl::Selection::close() {
	active = false;
}

bool TextInputControl::Selection::isActive() {
	return active;
}

size_t TextInputControl::Selection::start() {
	auto cursorPos = cursor->getPosition();
	if (selectionPos >= cursorPos) return cursorPos;
	else return selectionPos;
}

size_t TextInputControl::Selection::end() {
	auto cursorPos = cursor->getPosition();
	if (selectionPos >= cursorPos) return selectionPos;
	else return cursorPos;
}

size_t TextInputControl::Selection::getPosition() {
	return selectionPos;
}

void TextInputControl::Histroy::record(std::vector<wchar_t> buffer, size_t cursorPos, size_t selectionPos, InputAction::Operation operation) {
	if (cursorPos == selectionPos) return;
	InputAction inAction(std::move(buffer), cursorPos, selectionPos, operation);
	undoStack.push(inAction);
	clearRedoStack();
}

void TextInputControl::Histroy::undo() {
	if (undoStack.empty()) return;
	InputAction action = undoStack.top();

	undoStack.pop();
	redoStack.push(action);

	auto start = action.cursorPos;
	auto end = action.selectionPos;
	sort(start, end);

	if (action.operation == Histroy::Operation::Insert) {
		// 撤销插入：删除文本
		buffer->erase(start, end);
		cursor->setPosition(action.cursorPos);
		selection->close();
	}
	else {
		// 撤销删除：插入文本
		buffer->insert(start, action.buffer.data());
		cursor->setPosition(action.cursorPos);
		selection->setPosition(action.selectionPos);
	}
}

void TextInputControl::Histroy::redo() {
	if (redoStack.empty()) return;
	InputAction action = redoStack.top();

	redoStack.pop();
	undoStack.push(action);

	auto start = action.cursorPos;
	auto end = action.selectionPos;
	sort(start, end);

	if (action.operation == Histroy::Operation::Insert) {
		// 重做插入：插入文本
		buffer->insert(start, action.buffer.data());
		cursor->setPosition(action.cursorPos);
		selection->setPosition(action.selectionPos);
	}
	else {
		// 重做删除：删除文本
		buffer->erase(start, end);
		cursor->setPosition(action.cursorPos);
		selection->close();
	}
}

void TextInputControl::Histroy::sort(size_t& a, size_t& b) {
	if (a > b) std::swap(a, b);
}

void TextInputControl::Histroy::clearRedoStack() {
	if (!redoStack.empty())
		redoStack = std::stack<InputAction>();
}

void TextInputControl::Histroy::Inserted(std::vector<wchar_t> buffer, size_t cursorPos, size_t selectionPos) {
	record(buffer, cursorPos, selectionPos, Operation::Insert);
}

void TextInputControl::Histroy::Erased(std::vector<wchar_t> buffer, size_t cursorPos, size_t selectionPos) {
	record(buffer, cursorPos, selectionPos, Operation::Erase);
}

std::stack<TextInputControl::Histroy::InputAction>& TextInputControl::Histroy::getUndoStack() {
	return undoStack;
}

std::stack<TextInputControl::Histroy::InputAction>& TextInputControl::Histroy::getRedoStack() {
	return redoStack;
}

void TextInputControl::controlChar(wchar_t c) {
	switch (c) {
	case '\n':
		break;
	case '\r':
		break;
	case '\b':
	case '\x7F'://Del
		onDelete();
		break;
	default:
		break;
	}
}

void TextInputControl::cursorHandle(KeyEventArgs args) {
	switch (args.keyCode) {
	case VirtualKeyCode::LeftArrow:
		if (selection.isActive()) {
			cursor.setPosition(selection.start());
		}
		else {
			cursor.moveStep(-1);
		}
		selection.close();
		break;
	case VirtualKeyCode::RightArrow:
		if (selection.isActive()) {
			cursor.setPosition(selection.end());
		}
		else {
			cursor.moveStep(1);
		}
		selection.close();
		break;
	case VirtualKeyCode::Home:
		cursor.setPosition(0);
		selection.close();
		break;
	case VirtualKeyCode::End:
		cursor.setPosition(buffer.size());
		selection.close();
		break;
	}
}

void TextInputControl::CtrlKeyCombHandle(KeyEventArgs args) {
	if (!selection.isActive()) selection.init();
	switch (args.keyCode) {
	case VirtualKeyCode::LeftArrow:
		selection.moveStep(-1);
		break;
	case VirtualKeyCode::RightArrow:
		selection.moveStep(1);
		break;
	case VirtualKeyCode::Home:
		selection.setPosition(0);
		break;
	case VirtualKeyCode::End:
		selection.setPosition(buffer.size());
		break;
	case VirtualKeyCode::A: // Select All
		cursor.setPosition(0);
		selection.setPosition(buffer.size());
		break;
	case VirtualKeyCode::C: // Copy
	case VirtualKeyCode::Insert:
		onCopy();
		break;
	case VirtualKeyCode::X:
		onCopy();
		onCut();
		break;
	case VirtualKeyCode::D:
		//clear();
		break;
	case VirtualKeyCode::V: // Paste
		onPaste();
		break;
	case VirtualKeyCode::Y:
		histroy.redo();
		break;
	case VirtualKeyCode::Z:
		histroy.undo();
		break;
	}
}

void TextInputControl::onCopy() {
	clipboard.copyWstring(getSelectedWstring());
}

//////// vvv Buffer Operation vvv ////////

void TextInputControl::onDelete() {
	if (selection.isActive()) {
		onCut();
	}
	else {
		pop_back();
	}
}

void TextInputControl::onCut() {
	histroy.Erased(getSelectedBuffer(), cursor.getPosition(), selection.getPosition());
	buffer.erase(selection.start(), selection.end());
	cursor.setPosition(selection.start());
	selection.close();
}

void TextInputControl::onPaste() {
	if (selection.isActive()) {
		onCut();
	}
	std::wstring&& wstr = clipboard.pasteWstring();
	wstr.erase(std::remove_if(wstr.begin(), wstr.end(), [](wchar_t c) {
		return c < 32;  // 检查是否是控制字符
		}), wstr.end());

	histroy.Inserted(std::vector<wchar_t>(wstr.begin(), wstr.end()), cursor.getPosition(), cursor.getPosition() + wstr.length());
	buffer.insert(cursor.getPosition(), wstr);
	cursor.moveStep(wstr.length());
}

void TextInputControl::pop_back() {
	auto&& cursorPosition = cursor.getPosition();
	if (cursorPosition != 0) {
		histroy.Erased({ buffer.get(cursorPosition - 1) }, cursorPosition - 1, cursorPosition);
	}
	buffer.pop(cursorPosition);
	cursor.moveStep(-1);
}

void TextInputControl::push_back(wchar_t c) {
	if (c < 32 || c == '\x7F') {
		controlChar(c);
		return;
	}
	if (selection.isActive()) {
		onCut();
	}

	histroy.Inserted({ c }, cursor.getPosition(), cursor.getPosition() + 1);
	buffer.push(cursor.getPosition(), c);
	cursor.moveStep(1);
}

//////// ^^^ Buffer Operation ^^^ ////////

void TextInputControl::KeyEvent(KeyEventArgs args) {
	if (args.keyPressed && isEnabled()) {
		if (args.controlKeyState & LEFT_CTRL_PRESSED) {
			CtrlKeyCombHandle(args);
		}
		else {
			cursorHandle(args);
			if (args.unicodeChar != 0) push_back(args.unicodeChar);
		}
	}
}

void TextInputControl::enable() {
	IgnoreCtrlCExit(true);
	Enable = true;
}

void TextInputControl::disable() {
	IgnoreCtrlCExit(false);
	Enable = false;
	buffer.shrink();
}

bool TextInputControl::isEnabled() {
	return Enable;
}

size_t TextInputControl::getCursorPosition() {
	return cursor.getPosition();
}

std::vector<wchar_t> TextInputControl::getSelectedBuffer() {
	if (!selection.isActive()) return {};
	std::vector<wchar_t> result(buffer.data().begin() + selection.start(), buffer.data().begin() + selection.end());
	return result;
}

std::wstring TextInputControl::getSelectedWstring() {
	if (!selection.isActive()) return L"";

	std::wstring result(buffer.data().begin() + selection.start(), buffer.data().begin() + selection.end());
	return result;
}

std::wstring TextInputControl::getWstring() {
	return std::wstring(buffer.data().begin(), buffer.data().end());
}

std::string TextInputControl::getString() {
	return Converter::Convert<std::wstring, std::string>(getWstring());
}
*/
//////////// ^^^  Reversed  ^^^ ////////////