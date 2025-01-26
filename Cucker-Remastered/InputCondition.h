#pragma once
#include "InputHandler.h"

#define Condition(name) static bool name
class InputCondition {
public:
	class Mouse {
	public:
		enum ButtonState {
			Left,
			Left2,
			Left3,
			Left4,
			Right
		};
		Condition(InArea)(int X, int Y, int Width, int Height, MouseEventArgs args) {
			return args.position.X >= X &&
				args.position.X < X + Width &&
				args.position.Y >= Y &&
				args.position.Y < Y + Height;
		}
		Condition(IsClicked)(ButtonState state, MouseEventHandler::ButtonPressHandler::Result* result) {
			return result != nullptr && result->buttonStateRecord[state] && !result->hold;
		}
	};
};
#undef Condition