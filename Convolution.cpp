#include "StdAfx.h"
#include ".\convolution.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

float Convolution::sharpen_filter[9] = { 0, -1, 0, -1, 4, -1, 0, -1, 0};
float Convolution::emboss_filter[9] = { 0, 0, 3, 0, -1, 0, -1, 0, 0};
float Convolution::edge_filter[9] = {-1, -1, -1, -1, 8, -1, -1, -1, -1};
float Convolution::intense_filter[9] = {0, 0, 0, 0, 1.2f, 0, 0, 0, 0 };
float Convolution::blur_filter[9] = { 0.0625f, 0.125f, 0.0625f, 0.125f, 0.25f, 0.125f, 0.0625f, 0.125f, 0.0625f };

Convolution::Convolution(void)
{
}

Convolution::~Convolution(void)
{
}

void Convolution::filter(const int x, const int y, const int width, const int height, const unsigned char* in, const int inStride, unsigned char* out, const int outStride, FilterType type)
{
	float sumr, sumg, sumb, suma;
	int stridepixels = inStride / 4;
	int stridediff = inStride - (width * 4);
	float* filter = NULL;

	switch (type)
	{
	case Emboss:
		filter = emboss_filter;
		break;
	case Sharpen:
		filter = sharpen_filter;
		break;
	case Edge:
		filter = edge_filter;
		break;
	case Intense:
		filter = intense_filter;
		break;
	case Blur:
		filter = blur_filter;
		break;
	}

	const unsigned char* p = in;
	unsigned char* op = (unsigned char *)out;
	const unsigned char* pp = p;
	const unsigned char* pp2 = p + inStride;
	const unsigned char* pp3 = p + inStride + inStride;
	int cx = x;
	int cy = y;
	unsigned long* opp = (unsigned long *)out + stridepixels; opp++; // {1, 1}
	register int f = 0;
	register int f2 = 3;
	register int f3 = 6;
	int maxx = width - 1;
	int maxy = height - 1;

	while (cy < maxy)
	{
		while (cx < maxx)
		{
			sumr = 0; sumg = 0; sumb = 0; suma = 0;
			for (f = 0, f2 = 3, f3 = 6; f < 3; f++, f2++, f3++)
			{
				sumb += (pp[0] * filter[f]);
				sumg += (pp[1] * filter[f]);
				sumr += (pp[2] * filter[f]);
				suma += (pp[3] * filter[f]);
				sumb += (pp2[0] * filter[f2]);
				sumg += (pp2[1] * filter[f2]);
				sumr += (pp2[2] * filter[f2]);
				suma += (pp2[3] * filter[f2]);
				sumb += (pp3[0] * filter[f3]);
				sumg += (pp3[1] * filter[f3]);
				sumr += (pp3[2] * filter[f3]);
				suma += (pp3[3] * filter[f3]);

				pp += 4;
				pp2 += 4;
				pp3 += 4;
			}

			// reset the pointers for the next pixel
			pp -= 8;
			pp2 -= 8;
			pp3 -= 8;

			if (sumr > 255) sumr = 255;
			if (sumg > 255) sumg = 255;
			if (sumb > 255) sumb = 255;
			if (suma > 255) suma = 255;
			if (sumr < 0) sumr = 0;
			if (sumg < 0) sumg = 0;
			if (sumb < 0) sumb = 0;
			if (suma < 0) suma = 0;

			(*opp) = ((int)suma << 24) | ((int)sumr << 16) | ((int)sumg << 8) | (int)sumb;
			opp++;
			cx++;
		}

		opp++;

		// increment by the difference between the stride and the real width in bytes
		// this should take us to the first pixel on the next line
		pp += stridediff + 8;
		pp2 += stridediff + 8;
		pp3 += stridediff + 8;

		opp += stridepixels - width;
		opp++;

		cx = 1;
		cy++;
	}
}
