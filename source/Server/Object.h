#pragma once

class Object
{
public:
	Object();
	Object(POS x, POS y, POS z);
	virtual ~Object();

	virtual void Move(int direction) {}
	virtual void Rotate(float cx, float cy) {}

	virtual void Reset();

	constexpr POS GetX() const noexcept { return x; }
	constexpr POS GetY() const noexcept { return y; }
	constexpr POS GetZ() const noexcept { return z; }
	constexpr const char* GetName() const noexcept { return name.c_str(); }

	void SetName(const char* new_name) { name = new_name; }
	void SetName(std::string_view new_name) { name = new_name; }
	void SetPos(POS new_x, POS new_y, POS new_z);

	void CopyName(char* buf) { name.copy(buf, sizeof(VAR::NAME)); }

protected:
	POS x, y, z;
	std::string name;
};

