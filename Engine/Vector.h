#pragma once
#include "../framework/object.h"
#include "event.h"
#include "observer.h"
#include "matrix.h"

class Vector : public object
{
public:
	Vector();
	Vector(float x, float y, float z, float w = 1.0);
	~Vector(void);

	static void describe(SymbolTable* table);
	// get a parameter by name or ordinal
	virtual base* get(base* param);
	// set a parameter
	virtual void set(base* param, base* value);
	// invoke a method on this object by name 
	virtual base* invoke(std::wstring& name, std::vector<base *>& params);

	base* rotate(std::vector<base *>& params);

	Vector& operator=(const Vector& rhs);
	virtual base& operator=(base& rhs);
	inline Vector& operator *= (const Vector& rhs)
	{
		m_values[0] *= rhs.m_values[0];
		m_values[1] *= rhs.m_values[1];
		m_values[2] *= rhs.m_values[2];
		m_values[3] *= rhs.m_values[3];

		return *this;
	}
	inline Vector& operator *= (float f)
	{
		m_values[0] *= f;
		m_values[1] *= f;
		m_values[2] *= f;
		m_values[3] *= f;

		return *this;
	}
	inline Vector& operator += (const Vector& rhs)
	{
		m_values[0] += rhs.m_values[0];
		m_values[1] += rhs.m_values[1];
		m_values[2] += rhs.m_values[2];
		m_values[3] += rhs.m_values[3];

		return *this;
	}
	inline Vector& operator -= (const Vector& rhs)
	{
		m_values[0] -= rhs.m_values[0];
		m_values[1] -= rhs.m_values[1];
		m_values[2] -= rhs.m_values[2];
		m_values[3] -= rhs.m_values[3];

		return *this;
	}
    inline Vector operator+ (const Vector& rhs) const
    {
        Vector sum;
        sum.m_values[0] = m_values[0] + rhs.m_values[0];
        sum.m_values[1] = m_values[1] + rhs.m_values[1];
        sum.m_values[2] = m_values[2] + rhs.m_values[2];
        sum.m_values[3] = m_values[3] + rhs.m_values[3];
        return sum;
    }
	inline bool operator != (const Vector& rhs) const
	{
		if (m_values[0] != rhs.m_values[0] &&
			m_values[1] != rhs.m_values[1] &&
			m_values[2] != rhs.m_values[2] &&
			m_values[3] != rhs.m_values[3])
		{
			return true;
		}
		return false;
	}
    inline Vector operator- (const Vector& rhs) const
    {
        Vector sum;

        sum.m_values[0] = m_values[0] - rhs.m_values[0];
        sum.m_values[1] = m_values[1] - rhs.m_values[1];
        sum.m_values[2] = m_values[2] - rhs.m_values[2];
        sum.m_values[3] = m_values[3] - rhs.m_values[3];

        return sum;
    }
	inline Vector operator- () const
	{
		Vector negative;
		negative.m_values[0] = -m_values[0];
		negative.m_values[1] = -m_values[1];
		negative.m_values[2] = -m_values[2];
		return negative;
	}
	Vector operator* (const Matrix& m)
	{
		int i;
		Vector v;

		for (i = 0; i < 4; i++)
		{
			v.m_values[i] = 
			m_values[0] * m[i] +
			m_values[1] * m[i + 4] +
			m_values[2] * m[i + 8] +
			m_values[3] * m[i + 12];
		}

		return v;
	}
	Vector operator/ (float f)
	{
		return Vector(m_values[0] / f, m_values[1] / f, m_values[2] / f, m_values[3] / f);
	}
	Vector operator* (float f)
	{
		return Vector(m_values[0] * f, m_values[1] * f, m_values[2] * f, m_values[3] * f);
	}
	float& operator[](int i)
	{
		return m_values[i];
	}

public:
	float length() const;
	void normalise();
	Vector crossProduct(const Vector& v) const;
	double dot(const Vector& other);
	double angleTo(const Vector& other);
	double distanceFromPlane(const Vector& u, const Vector& v, const Vector& planePt) const;

public:
	virtual base* clone();
	virtual void construct(std::vector<base *>& params);
	
public:
	float m_values[4];	// 0=r, 1=g, 2=b, 3=a

private:
	static Vector m_prototype;
};
