#ifndef _SECTOR_HPP_
#define _SECTOR_HPP_

// INCLUDE
// OVERLAPPEDEX.hpp
// Object.hpp
#include "Session.hpp"

using c_map = concurrency::concurrent_unordered_map<int, Session*>;

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

	c_set MakeViewList(Session* client, c_map* others);

	bool OutOfSectorXR(float x);
	bool OutOfSectorXL(float x);
	bool OutOfSectorZU(float z);
	bool OutOfSectorZD(float z);

	int GetLTX() { return lt_x; }
	int GetLTZ() { return lt_z; }

private:
	int lt_x, lt_y, lt_z;		// Sector의 왼쪽 위 꼭지점 좌표

	concurrency::concurrent_unordered_set<int> objects;
	std::shared_mutex sector_lock;
};
#endif // !_SECTOR_HPP_
