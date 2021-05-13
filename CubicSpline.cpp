#include "stdafx.h"
#include <xmmintrin.h>
#include ".\cubicspline.h"

CubicSpline::CubicSpline(void)
	: m_tol(5)
	, m_points(NULL)
	, m_count(0)
	, m_tension(0.5)
	, m_grain(20)
{
}

CubicSpline::CubicSpline(point* pts, int count, double tension, int grain)
	: m_tol(2)
	, m_count(count)
	, m_tension(tension)
	, m_grain(grain)
{
	m_points = new point[count];
	memcpy(m_points, pts, sizeof(point) * count);

	GetCardinalMatrix(tension, m_matrix);

	double alphastep = 1.0 / grain;
	double alphav = 0.0;
	for (int i = 0; i < grain; i++) {
		m_alpha[i] = alphav;
		alphav += alphastep;
	}
}

CubicSpline::~CubicSpline(void)
{
	if (m_points) delete [] m_points;
}

/* CubicSpline.c—Generates a cubic spline through given list of (x,y,z)
triples.
Given: Number of knots n (including two endpoint knots),
the list of knots as (x,y,z) triples,
the grain = number of line segments desired between knots,
the type of spline (BETA, CARDINAL),
the parameters for tension and bias.
Returned: (n - 3) * grain + 1 points along spline, including the knots.
The two endpoints (used for end conditions only)
and any points between them and their nearest neighbor
knots are NOT returned.
CubicSpline() returns pointer to list of points.
(NULL is returned for error.)
*/
#define BETA 1
#define CARDINAL 2
#define NULL 0
#define PNTLMT 4096 /* max. no. of points which may be returned */
#define GRNCNT 256 /* max. no. of line segments between knots */
static point Spline[PNTLMT]; /* points returned here */

point *CubicSpline::getPoints(point* knots, int& n, double tension, int grain)
{
	register point* s, *k0, *km1, *k2, *k1;
	register int j, last, p = n;
	double alpha[GRNCNT];
	__declspec(align(16)) matrix m;

	if (n < 3 || grain < 0 || grain > GRNCNT) return(NULL);
	last = (n - 3) * grain + 1;
	if (last > PNTLMT) return(NULL);

	GetCardinalMatrix(tension, m);

	double alphastep = 1.0 / grain;
	double alphav = 0.0;
	for (int i = 0; i < grain; i++) {
		alpha[i] = alphav;
		alphav += alphastep;
	}

	n = 0;
	s = Spline;
	km1 = knots;
	k0 = knots; k1 = k0 + 1; k2 = k1 + 1;
	for (i = 0; i < p - 1; i++) {
		for (j = 0; j < grain; j++) {
			s->X = Matrix(km1->X, k0->X, k1->X, k2->X, alpha[j], m);
			s->Y = Matrix(km1->Y, k0->Y, k1->Y, k2->Y, alpha[j], m);
			s->Z = Matrix(km1->Z, k0->Z, k1->Z, k2->Z, alpha[j], m);
			s++;
			n++;
		}

		if (km1 != k0) km1++;
		k0++; k1++;
		if (k2 != &knots[p - 1]) k2++;
	}
	return(Spline);
}

point *CubicSpline::getPoints()
{
	register point* s, *k0, *km1, *k2, *k1;
	register int j, last, p = m_count;

	s = Spline;
	km1 = m_points;
	k0 = m_points; k1 = k0 + 1; k2 = k1 + 1;
	for (int i = 0; i < p - 1; i++) {
		for (j = 0; j < m_grain; j++) {
			s->X = Matrix(km1->X, k0->X, k1->X, k2->X, m_alpha[j], m_matrix);
			s->Y = Matrix(km1->Y, k0->Y, k1->Y, k2->Y, m_alpha[j], m_matrix);
			s->Z = Matrix(km1->Z, k0->Z, k1->Z, k2->Z, m_alpha[j], m_matrix);
			s++;
		}

		if (km1 != k0) km1++;
		k0++; k1++;
		if (k2 != &m_points[p - 1]) k2++;
	}
	(*s++) = m_points[m_count-1];

	return(Spline);
}

point *CubicSpline::getBetaPoints(point* knots, int& n, double bias, double tension, int grain)
{
	register point *s, *k0, *km1, *k2, *k1;
	int j, last;
	double alpha[GRNCNT];
	matrix m;

	if (n < 3 || grain < 0 || grain > GRNCNT) return(NULL);
	last = (n - 3) * grain + 1;
	if (last > PNTLMT) return(NULL);
	if (bias == 0. && tension == 0.) return(NULL);

	GetBetaMatrix(bias, tension, m);

	for (int i = 0; i < grain; i++) alpha[i] = ((double)i) / grain;

	s = Spline;
	km1 = knots;
	k0 = km1 + 1; k1 = k0 + 1; k2 = k1 + 1;
	for (i = 1; i < n - 1; i++) {
		for (j = 0; j < grain; j++) {
			s->x = Matrix(km1->X, k0->X, k1->X, k2->X, alpha[j], m);
			s->y = Matrix(km1->Y, k0->Y, k1->Y, k2->Y, alpha[j], m);
			s->z = Matrix(km1->Z, k0->Z, k1->Z, k2->Z, alpha[j], m);
			s++;
		}
		k0++; km1++; k1++; k2++;
	}
	return(Spline);
}

void CubicSpline::GetBetaMatrix(double b0, double b1, matrix m)
{
	register i, j;
	double d, b2, b3;
	b2 = b0 * b0;
	b3 = b0 * b2;
	m[0][0] = -2. * b3;
	m[0][1] = 2. * (b1 + b3 + b2 + b0);
	m[0][2] = -2. * (b1 + b2 + b0 + 1.);

	m[1][0] = 6. * b3;
	m[1][1] = -3. * (b1 + 2. * b3 + 2. * b2);
	m[1][2] = 3. * (b1 + 2. * b2);
	m[2][0] = -6. * b3;
	m[2][1] = 6. * (b3 - b0);
	m[2][2] = 6. * b0;
	m[3][0] = 2. * b3;
	m[3][1] = b1 + 4. * (b2 + b0);
	m[0][3] = m[3][2] = 2.;
	m[1][3] = m[2][3] = m[3][3] = 0.;
	d = 1. / (b1 + 2. * b3 + 4. * b2 + 4. * b0 + 2.);
	for (i = 0; i < 4; i++) for (j = 0; j < 4; j++) m[i][j] *= d;
}

void CubicSpline::GetCardinalMatrix(double a, matrix m)
{
	m[0][1] = 2. - a;
	m[0][2] = a - 2.;
	m[1][0] = 2. * a;
	m[1][1] = a - 3.;
	m[1][2] = 3. - 2 * a;
	m[3][1] = 1.;
	m[0][3] = m[2][2] = a;
	m[0][0] = m[1][3] = m[2][0] = -a;
	m[2][1] = m[2][3] = m[3][0] = m[3][2] = m[3][3] = 0.;
}

float CubicSpline::Matrix(float a, float b, float c, float d, float alpha, matrix m)
{
	register float p0, p1, p2, p3;

/*
	_declspec(align(16)) float f[4] = { a, b, c, d };

	_mm_load_ps(m[0]);

	__asm
	{
		movaps xmm4, xmmword ptr f;
		movaps xmm0, xmmword ptr m;
		movaps xmm1, xmmword ptr m + 0x10;
		movaps xmm2, xmmword ptr m + 0x20;
		movaps xmm3, xmmword ptr m + 0x30;
	}
*/
	p0 = m[0][0] * a + m[0][1] * b + m[0][2] * c + m[0][3] * d;
	p1 = m[1][0] * a + m[1][1] * b + m[1][2] * c + m[1][3] * d;
	p2 = m[2][0] * a + m[2][1] * b + m[2][2] * c + m[2][3] * d;
	p3 = m[3][0] * a + m[3][1] * b + m[3][2] * c + m[3][3] * d;

	return (p3 + alpha * (p2 + alpha * (p1 + alpha * p0)));
}

CubicSpline::SplineHitTest CubicSpline::HitTest(Point& test)
{
	SplineHitTest ht;
	ht.Type = SplineHitTest::None;

	// first check the points - this could really be done without calling this function but hey
	for (int i = 0; i < m_count; i++)
	{
		if (abs(test.X - m_points[i].X) <= m_tol && abs(test.Y - m_points[i].Y) <= m_tol)
		{
			ht.Type = SplineHitTest::Point;
			ht.index = i;
			break;
		}
	}

	// we didn't find a hit on the point so we will have to get all the intermediate points and try for them
	if (ht.Type == SplineHitTest::None)
	{
		register point* s, *k0, *km1, *k2, *k1;
		register int j, last, p = m_count;
		double alpha[GRNCNT];
		__declspec(align(16)) matrix m;

		GetCardinalMatrix(m_tension, m);

		double alphastep = 1.0 / m_grain;
		double alphav = 0.0;
		for (int i = 0; i < m_grain; i++) {
			alpha[i] = alphav;
			alphav += alphastep;
		}

		s = Spline;
		km1 = m_points;
		k0 = m_points; k1 = k0 + 1; k2 = k1 + 1;
		for (i = 0; i < p - 1; i++) {
			for (j = 0; j < m_grain; j++) {
				s->X = Matrix(km1->X, k0->X, k1->X, k2->X, alpha[j], m);
				s->Y = Matrix(km1->Y, k0->Y, k1->Y, k2->Y, alpha[j], m);
				s->Z = Matrix(km1->Z, k0->Z, k1->Z, k2->Z, alpha[j], m);

				if (abs(test.X - s->X) <= m_tol && abs(test.Y - s->Y) <= m_tol)
				{
					ht.Type = SplineHitTest::Line;
					ht.index = i;
					break;
				}
			}

			if (km1 != k0) km1++;
			k0++; k1++;
			if (k2 != &m_points[p - 1]) k2++;
		}
	}
	return ht;
}

void CubicSpline::addPoint(point point, int after)
{
	point* p = new point[m_count + 1];

	memcpy(p, m_points, sizeof(point) * (after + 1));
	p[after + 1] = point;
	memcpy(&p[after + 2], &m_points[after + 1], sizeof(point) * (m_count - (after + 1)));

	delete [] m_points;
	m_points = p;
	m_count++;
}