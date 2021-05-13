#pragma once
#include "../framework/base.h"
#include "event.h"
#include "observer.h"

class Matrix : public base
{
public:
	Matrix();
	Matrix(float x, float y, float z, float w = 1.0);
	~Matrix(void);

	static void describe(SymbolTable* table);
	// get a parameter by name or ordinal
	virtual base* get(base* param);

	Matrix& operator=(const Matrix& rhs);
	virtual base& operator=(const base& rhs);
	Matrix operator* (const Matrix& rhs);
	inline Matrix& operator *= (float f)
	{
		m_values[0] *= f;
		m_values[1] *= f;
		m_values[2] *= f;
		m_values[3] *= f;

		return *this;
	}
	inline Matrix& operator += (const Matrix& rhs)
	{
		m_values[0] += rhs.m_values[0];
		m_values[1] += rhs.m_values[1];
		m_values[2] += rhs.m_values[2];
		m_values[3] += rhs.m_values[3];

		return *this;
	}
    inline Matrix operator+ (const Matrix& rhs) const
    {
        Matrix sum;

        sum.m_values[0] = m_values[0] + rhs.m_values[0];
        sum.m_values[1] = m_values[1] + rhs.m_values[1];
        sum.m_values[2] = m_values[2] + rhs.m_values[2];
        sum.m_values[3] = m_values[3] + rhs.m_values[3];

        return sum;
    }
	float& operator[](int i) const
	{
		return (float &)m_values[i];
	}

public:
	void fromRotation(float angle, float x, float y, float z);
	void fromTranslation(float x, float y, float z);
	Matrix inverse();

	void getAngles(float* x, float* y, float* z);

protected:
	float DetIJ(int i, int j);

public:
	virtual base* clone();
	virtual void construct(std::vector<base *>& params);
	
public:
	float m_values[16];	// 0-3 is column one, 4-7 column two etc.

private:
	static Matrix m_prototype;
};
