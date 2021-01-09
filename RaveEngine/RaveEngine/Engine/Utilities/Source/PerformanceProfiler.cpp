#include "Engine/Utilities/Include/PerformanceProfiler.h"

#include <fstream>

void rave::PerformanceProfiler::Mark()
{
	if (!init)
	{
		init = true;
		timer.Mark();
	}
	duration += timer.Mark();
	frames++;
}

void rave::PerformanceProfiler::StartSection(std::string name)
{
	map[name].begin = timer.Peek();
}

void rave::PerformanceProfiler::EndSection(std::string name)
{
	Node& n = map[name];
	n.end = timer.Peek();
	n.duration += n.end - n.begin;
}

rave::PerformanceProfiler::~PerformanceProfiler()
{
	std::ofstream os("performance.txt");
	os << "Performance\n"
		"-----------\n\n\n";
	float fframes = (float)frames;
	float av = duration / fframes;
	os << "Duration:\t" << duration << "s\n";
	os << "#Frames:\t" << frames << "\n";
	os << "Average:\t" << av << "s\n";
	os << "FPS:\t\t" << frames / duration << "\n\n";
	os << 1.0f / 120.0f - av << "s away from 120fps\n\n";

	size_t nTabs = 0;
	for (auto& p : map)
	{
		if ((p.first.size() + 1) / 4 + 1 > nTabs)
			nTabs = (p.first.size() + 1) / 4 + 1;
	}

	for (auto& p : map)
	{
		size_t tabs = (p.first.size() + 1) / 4 + 1;
		os << p.first << ":\t";
		for (size_t i = 0; i < nTabs - tabs; i++)
			os << '\t';
		os << p.second.duration / fframes << "s   ->   " << p.second.duration / duration * 100 << "%\n";
	}
}
