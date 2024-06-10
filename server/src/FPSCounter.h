#pragma once
#include <chrono>
#include <vector>

namespace Metrics
{
class FPSCounter
{
  public:
    void Frame();
    void PrintFrameData();

  private:
    std::vector<double> frameTimes;
    std::chrono::high_resolution_clock::time_point lastFrameTime = std::chrono::high_resolution_clock::now();
};

} // namespace Metrics
