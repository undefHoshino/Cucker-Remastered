#pragma once
#include <stack>
#include <algorithm>
#include "Clipboard.h"
#include "InputHandler.h"
#include "Converter.h"
#include "ConsoleFunction.h"

#undef min
#undef max

class TextInputControl {
public:
	struct LineColumn {
		size_t column; // X
		size_t row;	   // Y

		static bool islegal(LineColumn a, LineColumn b);
		bool operator==(LineColumn other);
	};

	class LineBuffer {
	private:
		std::vector<wchar_t> buffer;
	public:
		LineBuffer() = default;
		LineBuffer(std::wstring& buf) :buffer(buf.begin(),buf.end()) {};
		LineBuffer(std::vector<wchar_t>& buf) : buffer(buf) {};
		LineBuffer(std::vector<wchar_t>&& buf) : buffer(std::move(buf)) {};
		void push(size_t pos, wchar_t c);
		void pop(size_t pos);
		void erase(size_t left, size_t right);
		void shrink();
		void insert(size_t pos, std::wstring wstr);
		void insert(size_t pos, std::vector<wchar_t> vec);
		bool empty();
		wchar_t get(size_t pos);
		size_t size();
		std::vector<wchar_t>& data();
	};

	class Buffer {
	public:
		class SafeLineRemover {
		private:
			Buffer* parent;
			std::stack<size_t> lines;
		public:
			SafeLineRemover(Buffer* parent) :parent(parent) {};
			void remove(size_t index);
			void execute();
			~SafeLineRemover();
		};
	private:
		std::vector<LineBuffer> buffer;
		size_t maxLine = -1;
	public:
		Buffer() = default;
		Buffer(std::vector<LineBuffer>&& buf) : buffer(std::move(buf)) {};
		void pushLine(std::vector<wchar_t> buf);
		void newLine(size_t row, LineBuffer buf);
		void insertLine(size_t pos, std::vector<wchar_t> buf);
		void removeLine(size_t lineNum);
		void eraseAtLine(LineColumn pos, size_t left, size_t right);
		void insertAtLine(LineColumn pos, std::wstring wstr);
		void insertAtLine(LineColumn pos, std::vector<wchar_t> vec);
		void push(LineColumn pos, wchar_t c);
		void pop(LineColumn pos);
		void erase(LineColumn start, LineColumn end);
		LineColumn insert(LineColumn pos, std::vector<LineBuffer> vec);
		LineColumn insert(LineColumn pos, std::wstring wstr, std::vector<LineBuffer>* vecOutput);
		void processTabsAndControlChars(std::wstring& line);
		void lineOutofRangeException(size_t lineNum);
		void shrink();
		bool checkIsOutofMaxLine();
		void setMaxLine(size_t maxline);
		size_t getMaxLine();
		wchar_t getChar(LineColumn pos);
		LineBuffer& getLine(size_t lineNum);
		std::vector<LineBuffer>& data();
		size_t size() const;
	protected:
		template<typename _Linebuf>
		bool forEachInsertOperator(LineBuffer& currentline, _Linebuf& line, size_t& currentPosX, size_t& currentPosY, bool& firstLine) {
			// 默认行为，什么都不做或抛出异常
			static_assert(sizeof(_Linebuf) == 0, "Unsupported line type.");
		}

		template<>
		bool forEachInsertOperator(LineBuffer& currentline, LineBuffer& line, size_t& currentPosX, size_t& currentPosY, bool& firstLine) {
			if (firstLine) {
				// 第一行直接插入，不加换行
				currentline.insert(currentPosX, line.data());
				currentPosX += line.size();
				firstLine = false;  
			}
			else {
				if (checkIsOutofMaxLine()) {
					auto _last = currentPosY - 1;
					auto& lastLine = getLine(_last);
					size_t column = lastLine.size();

					insertAtLine({ column,_last }, std::vector<wchar_t>(line.data().begin(), line.data().end()));
					currentPosX = lastLine.size();
					return false;
				}
				// 后续行插入换行
				insertLine(currentPosY, std::vector<wchar_t>(line.data().begin(), line.data().end()));
				currentPosX = line.size();
			}
			currentPosY++; 
			return true;
		}

		template<>
		bool forEachInsertOperator(LineBuffer& currentline, std::wstring& line, size_t& currentPosX, size_t& currentPosY, bool& firstLine) {
			if (firstLine) {
				// 第一行直接插入，不加换行
				currentline.insert(currentPosX, line.c_str());
				currentPosX += line.size();
				firstLine = false;
			}
			else {
				if (checkIsOutofMaxLine()) {
					auto _last = currentPosY - 1;
					auto& lastLine = getLine(_last);
					size_t column = lastLine.size();

					insertAtLine({ column,_last }, std::vector<wchar_t>(line.begin(), line.end()));
					currentPosX = lastLine.size();
					return false;
				}
				// 后续行插入换行
				insertLine(currentPosY, std::vector<wchar_t>(line.begin(), line.end()));
				currentPosX = line.size();
			}
			currentPosY++; 
			return true;
		}
	};

	class Cursor {
	private:
		Buffer* buffer;
		LineColumn cursorPos = { 0,0 };
	public:
		Cursor(Buffer* buffer) :buffer(buffer) {};
		LineColumn setPosition(size_t x, size_t y);
		LineColumn moveStep(int64_t x, int64_t y);
		LineColumn getPosition();
	};

	class Anchor {
	private:
		Buffer* buffer;
		Cursor* cursor;
		LineColumn anchorPos = { 0,0 };
		bool active = false;
	public:
		Anchor(Buffer* buffer, Cursor* cursor) :buffer(buffer), cursor(cursor) {};
		LineColumn setPosition(size_t x, size_t y);
		LineColumn moveStep(int64_t x, int64_t y);
		void init();
		void close();
		bool isActive();
		size_t startRow();
		size_t endRow();
		size_t startColumn();
		size_t endColumn();
		LineColumn getPosition();
	};

	class Histroy {
	public:
		struct InputAction {
			enum Operation {
				Insert,
				Erase,
				AddLine,
				RemoveLine
			};
			Buffer buffer;
			LineColumn cursorPos;
			LineColumn anchorPos;
			size_t lineRowPos;
			Operation operation;
			InputAction(std::vector<LineBuffer>&& buffer, LineColumn cursorPos, LineColumn anchorPos, size_t lineRowPos, InputAction::Operation operation) :
				buffer(std::move(buffer)), cursorPos(cursorPos), anchorPos(anchorPos), lineRowPos(lineRowPos), operation(operation) {};
		};
		using Operation = InputAction::Operation;
	private:
		Buffer* buffer;
		Cursor* cursor;
		Anchor* anchor;
		std::stack<InputAction> undoStack;
		std::stack<InputAction> redoStack;
	public:
		Histroy(Buffer* buffer, Cursor* cursor, Anchor* anchor) :buffer(buffer), cursor(cursor), anchor(anchor) {};
		void record(std::vector<LineBuffer> buffer, LineColumn cursorPos, LineColumn anchorPos, size_t lineRowPos, InputAction::Operation operation);
		void undo();
		void redo();
		void sort(LineColumn& a, LineColumn& b);
		void clearRedoStack();
		void clearHistroy();
		void Inserted(std::vector<LineBuffer> buffer, LineColumn cursorPos, LineColumn anchorPos);
		void Erased(std::vector<LineBuffer> buffer, LineColumn cursorPos, LineColumn anchorPos);
		void LineAdded(LineBuffer buffer, LineColumn currentCursorPos, size_t lineRowPos);
		void LineRemoved(LineBuffer buffer, LineColumn currentCursorPos, size_t lineRowPos);
		std::stack<InputAction>& getUndoStack();
		std::stack<InputAction>& getRedoStack();
	};

	class TextEditor {
	private:
		TextInputControl* parent;
		Buffer* buffer;
		Cursor* cursor;
		Anchor* anchor;
		Histroy* histroy;
		Clipboard* clipboard;
	public:
		TextEditor(TextInputControl* parent) :parent(parent),
			buffer(&parent->buffer), cursor(&parent->cursor), anchor(&parent->anchor),histroy(&parent->histroy),clipboard(&parent->clipboard) {};

		//////// vvv Buffer Operation vvv ////////
		void onDelete();
		void onCut();
		void onPop();
		void onPush(wchar_t c);
		void onCopy();
		void onPaste();
		void onNextLine();
		void onDelLine();
		void onTab();
	protected:
		void shouldNewLineAndMove();
		void shouldRemoveLineAndMove(LineColumn& cursorPos);
		//////// ^^^ Buffer Operation ^^^ ////////
	protected:
		void ReadControlChar(wchar_t c);
		LineBuffer& getThisLine();
		LineColumn getCursorPos();
		LineColumn getAnchorPos();
	};

	friend class LineEditor;
protected:
	Buffer buffer;
	Cursor cursor;
	Anchor anchor;
	Clipboard clipboard;
	Histroy histroy;
	TextEditor editor;
	bool Enable = false;

	void cursorHandle(KeyEventArgs args);
	void ctrlKeyCombHandle(KeyEventArgs args);
	void forEachSelectedBuffer(std::function<void(LineBuffer&, size_t, size_t)> func);
public:
	TextInputControl();
	
	void KeyEvent(KeyEventArgs args);
	void enable();
	void disable();
	bool isEnabled();
	void shrink();
	void setMaxLine(size_t maxline);

	size_t getMaxLine();
	LineColumn getCursorPosition();
	size_t getSelectedLineCount();
	std::vector<LineBuffer> convertToLineBuffers(std::vector<std::vector<wchar_t>>&& buf);
	std::vector<std::vector<wchar_t>> getSelectedBuffer();
	std::wstring getSelectedWstring();
	std::wstring getWstring(Buffer& buffer);
	std::wstring getWstring();
	std::string getString();

	Buffer& getBuffer();
	Cursor& getCursor();
	Anchor& getAnchor();
	Clipboard& getClipboard();
	Histroy& getHistroy();
	TextEditor& getTextEditor();
};



//////////// vvv  Reversed  vvv ////////////
/*
class TextInputControl {
public:
	class Buffer {
	private:
		std::vector<wchar_t> buffer;
	public:
		Buffer() = default;
		Buffer(std::vector<wchar_t>&& buf) : buffer(std::move(buf)) {};
		void push(size_t pos, wchar_t c);
		void pop(size_t pos);
		void erase(size_t left, size_t right);
		void shrink();
		void insert(size_t pos, std::wstring wstr);
		void insert(size_t pos, std::vector<wchar_t> vec);
		bool empty();
		wchar_t get(size_t pos);
		size_t size();
		std::vector<wchar_t>& data();
	};

	class Cursor {
	private:
		Buffer* buffer;
		size_t cursorPos = 0;
	public:
		Cursor(Buffer* buffer) :buffer(buffer) {};
		size_t setPosition(size_t pos);
		size_t moveStep(int64_t step);
		size_t getPosition();
	};

	class Selection {
	private:
		Buffer* buffer;
		Cursor* cursor;
		size_t selectionPos;
		bool active = false;
	public:
		Selection(Buffer* buffer, Cursor* cursor) :buffer(buffer), cursor(cursor) {};
		size_t setPosition(size_t pos);
		size_t moveStep(int64_t step);
		void init();
		void close();
		bool isActive();
		size_t start();
		size_t end();
		size_t getPosition();
	};

	class Histroy {
	public:
		struct InputAction {
			enum Operation {
				Insert,
				Erase,
			};
			Buffer buffer;
			size_t cursorPos;
			size_t selectionPos;
			Operation operation;
			InputAction(std::vector<wchar_t>&& buffer, size_t cursorPos, size_t selectionPos, InputAction::Operation operation) :
				buffer(std::move(buffer)), cursorPos(cursorPos), selectionPos(selectionPos), operation(operation) {};
		};
		using Operation = InputAction::Operation;
	private:
		Buffer* buffer;
		Cursor* cursor;
		Selection* selection;
		std::stack<InputAction> undoStack;
		std::stack<InputAction> redoStack;
	public:
		Histroy(Buffer* buffer, Cursor* cursor, Selection* selection) :buffer(buffer), cursor(cursor), selection(selection) {};
		void record(std::vector<wchar_t> buffer, size_t cursorPos, size_t selectionPos, InputAction::Operation operation);
		void undo();
		void redo();
		void sort(size_t& a, size_t& b);
		void clearRedoStack();
		void Inserted(std::vector<wchar_t> buffer, size_t cursorPos, size_t selectionPos);
		void Erased(std::vector<wchar_t> buffer, size_t cursorPos, size_t selectionPos);
		std::stack<InputAction>& getUndoStack();
		std::stack<InputAction>& getRedoStack();
	};
public:
	Buffer buffer;
	Cursor cursor;
	Selection selection;
	Clipboard clipboard;
	Histroy histroy;
	bool Enable = false;
protected:
	void controlChar(wchar_t c);
	void cursorHandle(KeyEventArgs args);
	void CtrlKeyCombHandle(KeyEventArgs args);
	void onCopy();

	//////// vvv Buffer Operation vvv ////////

	void onDelete();
	void onCut();
	void onPaste();
	void pop_back();
	void push_back(wchar_t c);

	//////// ^^^ Buffer Operation ^^^ ////////
public:
	TextInputControl() :cursor(&buffer), selection(&buffer, &cursor), histroy(&buffer, &cursor, &selection) {};

	void KeyEvent(KeyEventArgs args);
	void enable();
	void disable();
	bool isEnabled();
	size_t getCursorPosition();
	std::vector<wchar_t> getSelectedBuffer();
	std::wstring getSelectedWstring();
	std::wstring getWstring();
	std::string getString();
};
*/
//////////// ^^^  Reversed  ^^^ ////////////