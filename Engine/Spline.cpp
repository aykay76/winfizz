#include ".\spline.h"

Spline::Spline(const Vector& v1, const Vector& v2, const Vector& v3, const Vector& v4, bool cache)
	: m_t(3)
	, m_resolution(50)
{
	// store the input values
	m_input.push_back(v1);
	m_input.push_back(v2);
	m_input.push_back(v3);
	m_input.push_back(v4);

	// add the knots
	int j;
	int n = (int)m_input.size() - 1;
	for (j = 0; j <= n + m_t; j++)
	{
		if (j < m_t)
			m_knots.push_back(0);
		else if (j <= n)
			m_knots.push_back(j - m_t + 1);
		else if (j > n)
			m_knots.push_back(n - m_t + 2);
	}

	if (cache)
	{
		createPoints();
	}
}

Spline::~Spline(void)
{
}

void Spline::createPoints()
{
	int i = 0;
	double interval = 0.0;
	double increment = 0.0;
	int n = (int)m_input.size() - 1;

	m_points.resize(m_resolution);

	interval = 0;
	increment = (n - m_t + 2) / (double)(m_resolution - 1);
	for (i = 0; i < m_resolution - 1; i++)
	{
		createPoint(interval, m_points[i]);
		
		// SplinePoint(knots, n, t, interval, inp, &(outp[i]));
		interval += increment;
	}

	m_points[m_resolution - 1] = m_input[n];
}

void Spline::createPoint(double interval, Vector& v)
{
	int k = 0;
	double b = 0.0;
	int n = (int)m_input.size() - 1;

	v.m_values[0] = 0.0f;
	v.m_values[1] = 0.0f;
	v.m_values[2] = 0.0f;

	for (k = 0; k <= n; k++)
	{
		b = blend(k, m_t, interval);

		v.m_values[0] += m_input[k].m_values[0] * (float)b;
		v.m_values[1] += m_input[k].m_values[1] * (float)b;
		v.m_values[2] += m_input[k].m_values[2] * (float)b;
	}
}

double Spline::blend(int k, int t, double interval)
{
   double value;

   if (t == 1)
   {
      if ((m_knots[k] <= interval) && (interval < m_knots[k + 1]))
         value = 1;
      else
         value = 0;
   }
   else
   {
      if ((m_knots[k + t - 1] == m_knots[k]) && (m_knots[k + t] == m_knots[k + 1]))
         value = 0;
      else if (m_knots[k + t - 1] == m_knots[k]) 
         value = (m_knots[k + t] - interval) / (m_knots[k + t] - m_knots[k + 1]) * blend(k + 1, t - 1, interval);
      else if (m_knots[k + t] == m_knots[k + 1])
         value = (interval - m_knots[k]) / (m_knots[k + t - 1] - m_knots[k]) * blend(k, t - 1, interval);
     else
         value = (interval - m_knots[k]) / (m_knots[k + t - 1] - m_knots[k]) * blend(k, t - 1, interval) + 
                 (m_knots[k + t] - interval) / (m_knots[k + t] - m_knots[k + 1]) * blend(k + 1, t - 1, interval);
   }

   return value;
}

Vector& Spline::get(double interval)
{
	return m_points[(int)(interval * m_resolution)];
}