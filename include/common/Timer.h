#pragma once
#include <chrono>

namespace common {

using raw_duration_t = std::chrono::duration<double>;

using std::chrono::milliseconds,
			std::chrono::seconds,
			std::chrono::minutes;

using miliseconds_t =
  decltype(std::chrono::duration_cast<milliseconds>(raw_duration_t{}));

using seconds_t =
  decltype(std::chrono::duration_cast<seconds>(raw_duration_t{}));

using minutes_t =
  decltype(std::chrono::duration_cast<minutes>(raw_duration_t{}));

/** \brief inline-замена для duration_cast в milliseconds */
[[nodiscard]] inline miliseconds_t
dur_cast_to_ms(raw_duration_t & rDur) {
  return std::chrono::duration_cast<milliseconds>(rDur);
}

/** \brief inline-замена для duration_cast в seconds */
[[nodiscard]] inline seconds_t
dur_cast_to_sec(raw_duration_t & rDur) {
  return std::chrono::duration_cast<seconds>(rDur);
}

/** \brief inline-замена для duration_cast в minutes */
[[nodiscard]] inline minutes_t
dur_cast_to_min(raw_duration_t & rDur) {
  return std::chrono::duration_cast<minutes>(rDur);
}

class Timer {
	// std::chrono::time_point<std::chrono::steady_clock> start, end; // for windows
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end; 
	
 public: 
	Timer() {
		start = std::chrono::high_resolution_clock::now();
	}
	void ClickStart() {
		start = std::chrono::high_resolution_clock::now();
	}
	void ClickEnd() {
		end = std::chrono::high_resolution_clock::now();
	}

	raw_duration_t CountInterval() {
		return end - start;
	}

	~Timer() = default;
};

}