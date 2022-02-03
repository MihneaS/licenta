#include "InputController.h"

#include "../Engine.h"

InputController::InputController()
{
	window = Engine::GetWindow();
	window->SubscribeToEvents(this);
	is_attached = true;
}

InputController::~InputController()
{
}

bool InputController::is_active() const
{
	return is_attached;
}

void InputController::set_active(bool value)
{
	is_attached = value;
	value ? window->SubscribeToEvents(this) : window->UnsubscribeFromEvents(this);
}
