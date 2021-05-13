#include "../framework/real.h"
#include ".\Matrix.h"
#include "engine.h"
#include <math.h>

#include "constants.h"

Matrix Matrix::m_prototype;

Matrix::Matrix()
{
	for (int i = 0; i < 16; i++) m_values[i] = 0.0f;
	m_values[0] = 1.0f;
	m_values[5] = 1.0f;
	m_values[10] = 1.0f;
	m_values[15] = 1.0f;
}

base* Matrix::clone()
{
	return new Matrix(*this);
}

Matrix::~Matrix(void)
{
}

Matrix Matrix::operator* (const Matrix& rhs)
{
	Matrix ret;

    ret.m_values[0]  = m_values[0] * rhs.m_values[0] + m_values[4] * rhs.m_values[1] + m_values[8] * rhs.m_values[2] + m_values[12] * rhs.m_values[3];
    ret.m_values[4]  = m_values[0] * rhs.m_values[4] + m_values[4] * rhs.m_values[5] + m_values[8] * rhs.m_values[6] + m_values[12] * rhs.m_values[7];
    ret.m_values[8]  = m_values[0] * rhs.m_values[8] + m_values[4] * rhs.m_values[9] + m_values[8] * rhs.m_values[10] + m_values[12] * rhs.m_values[11];
    ret.m_values[12] = m_values[0] * rhs.m_values[12] + m_values[4] * rhs.m_values[13] + m_values[8] * rhs.m_values[14] + m_values[12] * rhs.m_values[15];
        
    ret.m_values[1]  = m_values[1] * rhs.m_values[0] + m_values[5] * rhs.m_values[1] + m_values[9] * rhs.m_values[2] + m_values[13] * rhs.m_values[3];
    ret.m_values[5]  = m_values[1] * rhs.m_values[4] + m_values[5] * rhs.m_values[5] + m_values[9] * rhs.m_values[6] + m_values[13] * rhs.m_values[7];
    ret.m_values[9]  = m_values[1] * rhs.m_values[8] + m_values[5] * rhs.m_values[9] + m_values[9] * rhs.m_values[10] + m_values[13] * rhs.m_values[11];
    ret.m_values[13] = m_values[1] * rhs.m_values[12] + m_values[5] * rhs.m_values[13] + m_values[9] * rhs.m_values[14] + m_values[13] * rhs.m_values[15];
        
    ret.m_values[2]  = m_values[2] * rhs.m_values[0] + m_values[6] * rhs.m_values[1] + m_values[10] * rhs.m_values[2] + m_values[14] * rhs.m_values[3];
    ret.m_values[6]  = m_values[2] * rhs.m_values[4] + m_values[6] * rhs.m_values[5] + m_values[10] * rhs.m_values[6] + m_values[14] * rhs.m_values[7];
    ret.m_values[10] = m_values[2] * rhs.m_values[8] + m_values[6] * rhs.m_values[9] + m_values[10] * rhs.m_values[10] + m_values[14] * rhs.m_values[11];
    ret.m_values[14] = m_values[2] * rhs.m_values[12] + m_values[6] * rhs.m_values[13] + m_values[10] * rhs.m_values[14] + m_values[14] * rhs.m_values[15];
        
    ret.m_values[3]  = m_values[3] * rhs.m_values[0] + m_values[7] * rhs.m_values[1] + m_values[11] * rhs.m_values[2] + m_values[15] * rhs.m_values[3];
    ret.m_values[7]  = m_values[3] * rhs.m_values[4] + m_values[7] * rhs.m_values[5] + m_values[11] * rhs.m_values[6] + m_values[15] * rhs.m_values[7];
    ret.m_values[11] = m_values[3] * rhs.m_values[8] + m_values[7] * rhs.m_values[9] + m_values[11] * rhs.m_values[10] + m_values[15] * rhs.m_values[11];
    ret.m_values[15] = m_values[3] * rhs.m_values[12] + m_values[7] * rhs.m_values[13] + m_values[11] * rhs.m_values[14] + m_values[15] * rhs.m_values[15];     

	return ret;
}

Matrix& Matrix::operator=(const Matrix& rhs)
{
	for (int i = 0; i < 16; i++)
		m_values[i] = rhs.m_values[i];

	return *this;
}

base& Matrix::operator=(const base& rhs)
{
	const Matrix& rhs2 = static_cast<const Matrix&>(rhs);

	for (int i = 0; i < 16; i++)
		m_values[i] = rhs2.m_values[i];

	return *this;
}

void Matrix::construct(std::vector<base *>& params)
{
	real* type = NULL;

	for (int i = 0; i < 16; i++)
	{
		type = (real *)params[i];
		m_values[i] = (float)(*type);
	}
}

void Matrix::describe(SymbolTable* table)
{
	SymbolTableNode* node = table->enter(std::wstring(L"Matrix"), dtTypeDeclaration);
	SymbolTable* newTable = new SymbolTable(table);
	node->symbols(newTable);
	node->object(&m_prototype);
}

// get a parameter by name or ordinal
base* Matrix::get(base* param)
{
	framework::string* name = reinterpret_cast<framework::string *>(param);

//	// TODO: finish this - still not sure it's the best way
//	if (wcscmp(L"x", (const wchar_t *)(*name)) == 0) return new real(&m_values[0], true);
//	if (wcscmp(L"y", (const wchar_t *)(*name)) == 0) return new real(&m_values[1], true);
//	if (wcscmp(L"z", (const wchar_t *)(*name)) == 0) return new real(&m_values[2], true);
//	if (wcscmp(L"w", (const wchar_t *)(*name)) == 0) return new real(&m_values[3], true);

	return NULL;
}

void Matrix::fromRotation(float angle, float x, float y, float z)
{
	float vecLength, sinSave, cosSave, oneMinusCos;
	float xx, yy, zz, xy, yz, zx, xs, ys, zs;

	// If NULL vector passed in, this will blow up...
	if (x == 0.0f && y == 0.0f && z == 0.0f)
	{
		return;
	}

	// Scale vector
	vecLength = (float)sqrt( x*x + y*y + z*z );

	// Rotation matrix is normalized
	x /= vecLength;
	y /= vecLength;
	z /= vecLength;

	sinSave = (float)sin(angle);
	cosSave = (float)cos(angle);
	oneMinusCos = 1.0f - cosSave;

	xx = x * x;
	yy = y * y;
	zz = z * z;
	xy = x * y;
	yz = y * z;
	zx = z * x;
	xs = x * sinSave;
	ys = y * sinSave;
	zs = z * sinSave;

	m_values[0] = (oneMinusCos * xx) + cosSave;
	m_values[4] = (oneMinusCos * xy) - zs;
	m_values[8] = (oneMinusCos * zx) + ys;
	m_values[12] = 0.0f;

	m_values[1] = (oneMinusCos * xy) + zs;
	m_values[5] = (oneMinusCos * yy) + cosSave;
	m_values[9] = (oneMinusCos * yz) - xs;
	m_values[13] = 0.0f;

	m_values[2] = (oneMinusCos * zx) - ys;
	m_values[6] = (oneMinusCos * yz) + xs;
	m_values[10] = (oneMinusCos * zz) + cosSave;
	m_values[14] = 0.0f;

	m_values[3] = 0.0f;
	m_values[7] = 0.0f;
	m_values[11] = 0.0f;
	m_values[15] = 1.0f;
}

void Matrix::fromTranslation(float x, float y, float z)
{
	m_values[12] = x; m_values[13] = y; m_values[14] = z;
}

float Matrix::DetIJ(int i, int j)
{
	int x, y, ii, jj;
	float ret, mat[3][3];

	x = 0;
	for (ii = 0; ii < 4; ii++)
	{
		if (ii == i) continue;
		y = 0;
		for (jj = 0; jj < 4; jj++)
		{
			if (jj == j) continue;
			mat[x][y] = m_values[(ii*4)+jj];
			y++;
		}
		x++;
	}

	ret =  mat[0][0]*(mat[1][1]*mat[2][2]-mat[2][1]*mat[1][2]);
	ret -= mat[0][1]*(mat[1][0]*mat[2][2]-mat[2][0]*mat[1][2]);
	ret += mat[0][2]*(mat[1][0]*mat[2][1]-mat[2][0]*mat[1][1]);

	return ret;
}

Matrix Matrix::inverse()
{
	int i, j;
	float det, detij;
	Matrix inv;

	// calculate 4x4 determinant
	det = 0.0f;
	for (i = 0; i < 4; i++)
	{
		det += (i & 0x1) ? (-m_values[i] * DetIJ(0, i)) : (m_values[i] * DetIJ(0, i));
	}
	det = 1.0f / det;

	// calculate inverse
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			detij = DetIJ(j, i);
			inv.m_values[(i * 4) + j] = ((i + j) & 0x1) ? (-detij * det) : (detij * det); 
		}
	}

	return inv;
}

void Matrix::getAngles(float* x, float* y, float* z)
{
	(*x) = (float)atan2(m_values[9], m_values[10]) * 180.0f / constants::pi;
	(*y) = (float)asin(-m_values[8]) * 180.0f / constants::pi;
	(*z) = (float)atan2(m_values[4], m_values[0]) * 180.0f / constants::pi;
}
