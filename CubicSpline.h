#pragma once

#include <gdiplus.h>
using namespace Gdiplus;

typedef float matrix[4][4];
typedef struct { double x,y,z; } point;

class CubicSpline
{
public:
	CubicSpline(void);
	CubicSpline(point* pts, int count, double tension = 0.5, int grain = 20);
	~CubicSpline(void);

	struct SplineHitTest
	{
		enum
		{
			None,
			Point,
			Line
		} Type;

		int index;
	};

public:
	static point* getPoints(point* knots, int& n, double tension = 1, int grain = 20);
	point* getPoints();
	point* getBetaPoints(point* knots, int& n, double bias, double tension = 0.5, int grain = 20);
	SplineHitTest HitTest(Point& test);
	void addPoint(point point, int after);
	point* knots() { return m_points; }
	int count() { return m_count; }
	int grain() { return m_grain; }

protected:
	void GetBetaMatrix(double b0, double b1, matrix m);
	static void GetCardinalMatrix(double a, matrix m);
	static float Matrix(float a, float b, float c, float d, float alpha, matrix m);

protected:
	int m_tol;
	point* m_points;
	int m_count;
	double m_tension;
	int m_grain;
	double m_alpha[256]; // optimise this
	float m_matrix[4][4];
};
