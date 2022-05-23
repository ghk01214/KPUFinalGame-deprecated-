#ifndef _SECTOR_HPP_
#define _SECTOR_HPP_

// INCLUDE
// OVERLAPPEDEX.hpp
// Object.hpp
#include "Session.hpp"

class Sector
{
public:
	Sector() = default;
	Sector(int x, int y);
	~Sector() = default;

	// 복사 생성자 및 복사 할당 연산자
	Sector(const Sector& other);
	Sector& operator=(const Sector& other);

	// 이동 생성자 및 이동 할당 연산자
	Sector(Sector&& other) noexcept;
	Sector& operator=(Sector&& other) noexcept;

	void EnterSector(int id);
	void LeaveSector(int id);

	concurrency::concurrent_unordered_set<int> MakeViewList(Session* client, concurrency::concurrent_unordered_map<int, Session*>* others);

	inline bool IsInSight(Session* cli1, Session* cli2);

	int GetLTX() { return lt_x; }
	int GetLTZ() { return lt_z; }

private:
	int lt_x, lt_y, lt_z;		// Sector의 왼쪽 위 꼭지점 좌표

	concurrency::concurrent_unordered_set<int> objects;
	std::shared_mutex sector_lock;
};

inline bool Sector::IsInSight(Session* cli1, Session* cli2)
{
	if (SIGHT_RANGE <= std::abs(cli1->GetMyObject()->GetX() - cli2->GetMyObject()->GetX()))
		return false;
	if (SIGHT_RANGE <= std::abs(cli1->GetMyObject()->GetY() - cli2->GetMyObject()->GetY()))
		return false;
	if (SIGHT_RANGE <= std::abs(cli1->GetMyObject()->GetZ() - cli2->GetMyObject()->GetZ()))
		return false;

	return true;
}

#endif // !_SECTOR_HPP_
