#pragma once

#include "vector.h"

class Spline
{
public:
	Spline(const Vector& v1, const Vector& v2, const Vector& v3, const Vector& v4, bool cache = false);
	virtual ~Spline(void);

// operations
public:
	void createPoints();
	void createPoint(double interval, Vector& v);
	double blend(int k, int t, double interval);
	Vector& get(double interval);

// accessors
public:
	int resolution() { return m_resolution; }
	void resolution(int r)  {m_resolution = r; }

// data members
protected:
	std::vector<int> m_knots;
	std::vector<Vector> m_input;
	std::vector<Vector> m_points;
	int m_resolution;
	int m_t;
};
