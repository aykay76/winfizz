#pragma once
#include "../framework/base.h"
#include "vector.h"
#include "event.h"
#include "observer.h"

class Quaternion : public base
{
public:
	Quaternion();
	Quaternion(float x, float y, float z, float w);
	~Quaternion(void);

	static void describe(SymbolTable* table);
	// get a parameter by name or ordinal
	virtual base* get(base* param);

	Quaternion& operator=(const Quaternion& rhs);
	virtual base& operator=(const base& rhs);
	Vector operator* (const Vector& v) const;
	Quaternion operator*(const Quaternion& rhs);

public:
	void normalise();
	void fromAngleAxis(float angle, float x, float y, float z);
	void toAngleAxis(float& angle, float& x, float& y, float& z);
	double getYaw(void);
	double getPitch(void);
	double getRoll(void);

public:
	virtual base* clone();
	virtual void construct(int params);
	
public:
	float m_values[4];	// 0=r, 1=g, 2=b, 3=a

private:
	static Quaternion m_prototype;
};
