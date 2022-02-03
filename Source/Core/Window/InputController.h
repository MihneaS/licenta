#pragma once
#include <include/utils.h>
#include "WindowObject.h"

class InputController
{
	friend class WindowObject;

	public:
		InputController();
		virtual ~InputController();

		bool is_active() const;
		void set_active(bool value);

	protected:
		// Method will be called each frame before the Update() method
		// Use for real-time frame-independent interaction such as performing continuous updates when pressing a key
		// @param delta_time - frame time in seconds for the previous frame, may be used for frame time independent updates
		// @param mods - (modifiers) bit combination of GLFW_MOD_"KEY" states: CONTROL, ALT, SHIFT, SUPER
		//			   - the modifiers value holds information about the special keys pressed alongside the normal key event
		//			   - use for testing key combination such as: CONTROL + ALT + KEY, CONTROL + SHIFT + KEY, etc 
		// Use window->KeyHold(GLFW_KEY_"KEYCODE") for testing if a key is being pressed
		virtual void on_input_update(float delta_time, int mods) {};

		// If a KEY PRESS event is registered the method will be called before the Update() method
		// @param key - GLFW_KEY_"KEYCODE", where KEYCODE is a standard keyboard key
		// @param mods - bit combination of GLFW_MOD_* states: CONTROL, ALT, SHIFT, SUPER
		virtual void on_key_press(int key, int mods) {};

		// If a KEY RELEASE event is registered the method will be called before the Update() method
		// @param key - GLFW_KEY_"KEYCODE", where KEYCODE is a standard keyboard key
		// @param mods - bit combination of GLFW_MOD_* states: CONTROL, ALT, SHIFT, SUPER
		virtual void on_key_release(int key, int mods) {};

		// If a MOUSE MOVE event is registered the method will be called before the Update() method
		// @param mouse_x - X coordinate in pixels of the mouse position relative to the top-left corner ([0, 0])
		// @param mouse_y - Y coordinate in pixels of the mouse position relative to the top-left corner
		// @param delta_x - offsetX in pixels from the last reported position
		// @param delta_y - offsetY in pixels from the last reported position
		virtual void on_mouse_move(int mouse_x, int mouse_y, int delta_x, int delta_y) {};

		// If a MOUSE BUTTON PRESS event is registered the method will be called before the Update() method
		// @param mouse_x - X coordinate in pixels of the mouse position relative to the top-left corner ([0, 0])
		// @param mouse_y - Y coordinate in pixels of the mouse position relative to the top-left corner
		// @param button - bit-mask with the buttons currently pressed. Use the preprocessor helpers for testing:
		//				    
		//						IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)
		//						IS_BIT_SET(button, GLFW_MOUSE_BUTTON_RIGHT)
		//						IS_BIT_SET(button, GLFW_MOUSE_BUTTON_*) ... etc
		//
		// @param mods   - bit combination of GLFW_MOD_* states: CONTROL, ALT, SHIFT, SUPER
		virtual void on_mouse_btn_press(int mouse_x, int mouse_y, int button, int mods) {};

		// Same as on_mouse_btn_press but triggered when a button is released
		virtual void on_mouse_btn_release(int mouse_x, int mouse_y, int button, int mods) {};

		// When a MOUSE SCROLL event is registered the method will be called before the Update() method
		// @param mouse_x - X coordinate in pixels of the mouse position relative to the top-left corner ([0, 0])
		// @param mouse_y - Y coordinate in pixels of the mouse position relative to the top-left corner
		// @param offset_x - scroll steps on X axis
		// @param offset_y - scroll steps on Y axis 
		virtual void on_mouse_scroll(int mouse_x, int mouse_y, int offset_x, int offset_y) {};

		// If the window was resized during the last frame this method will be before the Update() method
		// @param width - the new width of the window
		// @param height - the new height of the window
		virtual void on_window_resize(int width, int height) {};

	protected:
		WindowObject *window;

	private:
		bool is_attached;
};