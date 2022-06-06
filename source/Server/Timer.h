#pragma once

enum class EVENT
{
	MOVE, HEAL, ATTACK
};

struct TimerEvent
{
	int id;
	int target_id;
	EVENT event;
	std::chrono::steady_clock::time_point act_time;

	constexpr bool operator<(const TimerEvent& right) const
	{
		return act_time > right.act_time;
	}
};

class Timer
{
public:
	Timer();
	~Timer();

private:
	std::priority_queue<TimerEvent> timer;
	std::shared_mutex timer_lock;
};

