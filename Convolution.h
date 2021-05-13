#pragma once

class Convolution
{
public:
	Convolution(void);
	~Convolution(void);

enum FilterType
{
	Emboss, Sharpen, Edge, Intense, Blur
};

public:
	void filter(const int x, const int y, const int width, const int height, const unsigned char* in, const int inStride, unsigned char* out, const int outStride, FilterType type);

	static float sharpen_filter[9];
	static float emboss_filter[9];
	static float edge_filter[9];
	static float intense_filter[9];
	static float blur_filter[9];
};
