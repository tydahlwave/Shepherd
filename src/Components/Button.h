#pragma once

#include "Component.h"
#include "../GameController.h"

typedef int (GameController::*ButtonFunc)();

class Button : public Component {
public:
	Button() {
		name = "Button";
	}
	ButtonFunc callback;
	virtual ~Button() {};
};