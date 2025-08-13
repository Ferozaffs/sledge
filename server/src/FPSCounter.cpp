#include "FPSCounter.h"
#include <algorithm>
#include <numeric>
#include <stdio.h>

using namespace Metrics;
using namespace std::chrono;

void FPSCounter::Frame()
{
    auto now = high_resolution_clock::now();

    double duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastFrameTime).count();
    double fps = 1.0 / duration;
    frameTimes.push_back(fps);

    lastFrameTime = now;
}

void FPSCounter::PrintFrameData()
{
    if (frameTimes.empty() == true)
    {
        printf("No frames recorded in the last minute.\n");
        return;
    }

    double minFPS = *std::min_element(frameTimes.begin(), frameTimes.end());
    double maxFPS = *std::max_element(frameTimes.begin(), frameTimes.end());
    double avgFPS = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0) / frameTimes.size();

    printf("Min FPS: %f\n", minFPS);
    printf("Max FPS: %f\n", maxFPS);
    printf("Avg FPS: %f\n", avgFPS);

    frameTimes.clear();
}
