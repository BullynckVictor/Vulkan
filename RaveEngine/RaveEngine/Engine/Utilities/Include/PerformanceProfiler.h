#pragma once
#include <string>
#include <unordered_map>
#include "Engine/Utilities/Include/Timer.h"

namespace rave
{
	class PerformanceProfiler
	{
	private:
		struct Node
		{
			float begin;
			float end;
			float duration;
		};
	public:
		void Mark();

		void StartSection(std::string name);
		void EndSection(std::string name);

		~PerformanceProfiler();

	private:
		std::unordered_map<std::string, Node> map;
		size_t frames = 0;
		float duration = 0.0f;

		Timer timer;

		bool init = false;
	};
}