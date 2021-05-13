#pragma once
#include "../framework/base.h"
#include "event.h"
#include "observer.h"

class Vector2 : public base
{
public:
	Vector2();
	Vector2(float x, float y, float z, float w = 1.0);
	~Vector2(void);

	static void describe(SymbolTable* table);
	// get a parameter by name or ordinal
	virtual base* get(base* param);

	Vector2& operator=(const Vector2& rhs);
	virtual base& operator=(const base& rhs);
	inline Vector2& operator *= (const Vector2& rhs)
	{
		m_values[0] *= rhs.m_values[0];
		m_values[1] *= rhs.m_values[1];

		return *this;
	}
	inline Vector2& operator *= (float f)
	{
		m_values[0] *= f;
		m_values[1] *= f;

		return *this;
	}
	inline Vector2& operator += (const Vector2& rhs)
	{
		m_values[0] += rhs.m_values[0];
		m_values[1] += rhs.m_values[1];

		return *this;
	}
    inline Vector2 operator+ (const Vector2& rhs) const
    {
        Vector2 sum;

        sum.m_values[0] = m_values[0] + rhs.m_values[0];
        sum.m_values[1] = m_values[1] + rhs.m_values[1];

        return sum;
    }

public:
	float length() const;
	void normalise();
	float dot(const Vector2& other);
	double angleTo(const Vector2& other);

public:
	virtual base* clone();
	virtual void construct(int params);
	
public:
	float m_values[4];	// 0=r, 1=g, 2=b, 3=a

private:
	static Vector2 m_prototype;
};
