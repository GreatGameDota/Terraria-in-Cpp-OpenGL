#include <Windows.h>
#include "../common.h"
#include "TimeElapsed.h"

inline long long PerformanceCounter() noexcept
{
  LARGE_INTEGER li;
  ::QueryPerformanceCounter(&li);
  return li.QuadPart;
}

inline long long PerformanceFrequency() noexcept
{
  LARGE_INTEGER li;
  ::QueryPerformanceFrequency(&li);
  return li.QuadPart;
}

long long start{};

void startTimer()
{
  start = PerformanceCounter();
}

void finish()
{
  long long finish = PerformanceCounter();
  double frequency = PerformanceFrequency();
  double milli = ((finish - start) * 1000.0) / frequency;
  long hr = milli / 3600000;
  milli = milli - 3600000 * hr;
  long min = milli / 60000;
  milli = milli - 60000 * min;
  long sec = milli / 1000;
  milli = milli - 1000 * sec;
  cout << "\nTime elapsed: " << hr << "h:" << min << "m:" << sec << "s:" << milli << "ms" << endl;
}