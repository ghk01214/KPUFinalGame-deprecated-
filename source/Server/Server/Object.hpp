#ifndef _OBJECT_HPP_
#define _OBJECT_HPP_

class Object
{
public:
	Object();
	Object(POS new_x, POS new_y, POS new_z);
	virtual ~Object() = default;

	void CreateNewObject(POS new_x, POS new_y, POS new_z);

	virtual void Move(int direction) {}

	bool IsInWorld();

	constexpr POS GetX() const noexcept { return x; }
	constexpr POS GetY() const noexcept { return y; }
	constexpr POS GetZ() const noexcept { return z; }
	constexpr std::string GetName() const noexcept { return name; }

	void SetX(POS new_x) { x = new_x; }
	void SetY(POS new_y) { y = new_y; }
	void SetZ(POS new_z) { z = new_z; }
	void SetName(char* new_name) { name = new_name; }

protected:
	POS x, y, z;
	std::string name;

	float max_speed;
};

#endif // !_OBJECT_HPP_
