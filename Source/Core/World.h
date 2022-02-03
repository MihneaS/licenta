#pragma once

#include <unordered_map>

class Mesh;
class Shader;

#include "Window/InputController.h"

class World : public InputController
{
	public:
		World();
		virtual ~World() {};
		virtual void init() {};
		virtual void frame_start() {};
		virtual void update(float delta_time_seconds) {};
		virtual void frame_end() {};

		virtual void run() final;
		virtual void pause() final;
		virtual void exit() final;

		virtual double get_last_frame_time() final;

	private:
		void compute_frame_delta_time();
		void loop_update();

	private:
		double previous_time;
		double elapsed_time;
		double delta_time;
		bool paused;
		bool should_close;
};