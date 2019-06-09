#include "cvClass.h"

inline int Round(float value)
{
	return (int)(value + (value >= 0 ? 0.5f : -0.5f));
}


inline int Floor(float value)
{
	int i = Round(value);
	float diff = (float)(value - i);
	float diff = (float)(value - i);
	return i - (diff < 0);
}
/////////////////////////////// myMat implementation ////////////////////////////////////////
template<typename _Tp, int chs>
myMat<_Tp, chs>::myMat(int _rows,int _cols,uchar value) 
{
	this->rows = _rows;
	this->cols = _cols;
	this->channels = chs;
	this->step = sizeof(_Tp)*_cols*chs;
	size_t size = this.rows *this->step;
	uchar* p = (uchar *)malloc(size);
	this->data = p;
	this->datastart = p;
	this->dataend = this->data + size;

	for (int i = 0; i < _rows; i++)
	{      
		for (int j = 0; j < _rows; j++) 
		{   
			uchar *Pixel = data + i * cols*chs + chs * j;
			for (int k = 0; k < chs; k++)
			{
				Pixel[k] = value;
			}
		}
	}
}
template<typename  _Tp, int chs>
myMat<_Tp, chs>::myMat(int _rows, int _cols, uchar* _data):rows(_rows),cols(_cols),data(_data)
{
	this->step = sizeof(_Tp)*_cols*chs
	this->datastart = this->data;
	this->dataend = this->data + this->step*this->rows;
}

//! @endcode



/////////////////////////////// AutoBuffer implementation ////////////////////////////////////////

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::AutoBuffer()
{
	ptr = buf;
	sz = fixed_size;
}

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::AutoBuffer(size_t _size)
{
	ptr = buf;
	sz = fixed_size;
	allocate(_size);
}

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::AutoBuffer(const AutoBuffer<_Tp, fixed_size>& abuf)
{
	ptr = buf;
	sz = fixed_size;
	allocate(abuf.size());
	for (size_t i = 0; i < sz; i++)
		ptr[i] = abuf.ptr[i];
}

template<typename _Tp, size_t fixed_size> inline AutoBuffer<_Tp, fixed_size>&
AutoBuffer<_Tp, fixed_size>::operator = (const AutoBuffer<_Tp, fixed_size>& abuf)
{
	if (this != &abuf)
	{
		deallocate();
		allocate(abuf.size());
		for (size_t i = 0; i < sz; i++)
			ptr[i] = abuf.ptr[i];
	}
	return *this;
}

template<typename _Tp, size_t fixed_size> inline
AutoBuffer<_Tp, fixed_size>::~AutoBuffer()
{
	deallocate();
}

template<typename _Tp, size_t fixed_size> inline void
AutoBuffer<_Tp, fixed_size>::allocate(size_t _size)
{
	if (_size <= sz)
	{
		sz = _size;
		return;
	}
	deallocate();
	sz = _size;
	if (_size > fixed_size)
	{
		ptr = new _Tp[_size];
	}
}

template<typename _Tp, size_t fixed_size> inline void
AutoBuffer<_Tp, fixed_size>::deallocate()
{
	if (ptr != buf)
	{
		delete[] ptr;
		ptr = buf;
		sz = fixed_size;
	}
}

template<typename _Tp, size_t fixed_size> inline void
AutoBuffer<_Tp, fixed_size>::resize(size_t _size)
{
	if (_size <= sz)
	{
		sz = _size;
		return;
	}
	size_t i, prevsize = sz, minsize = MIN(prevsize, _size);
	_Tp* prevptr = ptr;

	ptr = _size > fixed_size ? new _Tp[_size] : buf;
	sz = _size;

	if (ptr != prevptr)
		for (i = 0; i < minsize; i++)
			ptr[i] = prevptr[i];
	for (i = prevsize; i < _size; i++)
		ptr[i] = _Tp();

	if (prevptr != buf)
		delete[] prevptr;
}

template<typename _Tp, size_t fixed_size> inline size_t
AutoBuffer<_Tp, fixed_size>::size() const
{
	return sz;
}

/*template<> inline std::string CommandLineParser::get<std::string>(int index, bool space_delete) const
{
	return get<String>(index, space_delete);
}
template<> inline std::string CommandLineParser::get<std::string>(const String& name, bool space_delete) const
{
	return get<String>(name, space_delete);
}*/

//! @endcond

template<typename _Tp, int chs>
int resize_linear(const myMat<_Tp, chs>& src, myMat<_Tp, chs>& dst)
{
	
	int srows = src.rows, scols = src.cols;
	
	int drows = dst.rows, dcols = dst.cols;

	double inv_scale_x = (double)drows / srows;
	double inv_scale_y = (double)dcols / src.cols;
	double scale_x = 1. / inv_scale_x, scale_y = 1. / inv_scale_y;

	int iscale_x = saturate_cast<int>(scale_x);
	int iscale_y = saturate_cast<int>(scale_y);

	
	// in case of scale_x && scale_y is equal to 2
	// INTER_AREA (fast) also is equal to INTER_LINEAR


	int cn = dst.channels;
	int k, sx, sy, dx, dy;
	int xmin = 0, xmax = drows, width = drows *cn;
	bool fixpt = sizeof(_Tp) == 1 ? true : false;
	float fx, fy;
	int ksize = 2, ksize2;
	ksize2 = ksize / 2;

	AutoBuffer<uchar> _buffer((width + dsize.height)*(sizeof(int) + sizeof(float)*ksize));
	int* xofs = (int*)(uchar*)_buffer;
	int* yofs = xofs + width;
	float* alpha = (float*)(yofs + dsize.height);
	short* ialpha = (short*)alpha;
	float* beta = alpha + width * ksize;
	short* ibeta = ialpha + width * ksize;
	float cbuf[MAX_ESIZE];

	for (dx = 0; dx < dsize.width; dx++) {
		fx = (float)((dx + 0.5)*scale_x - 0.5);
		sx = Floor(fx);
		fx -= sx;

		if (sx < ksize2 - 1) {
			xmin = dx + 1;
			if (sx < 0) {
				fx = 0, sx = 0;
			}
		}

		if (sx + ksize2 >= ssize.width) {
			xmax = std::min(xmax, dx);
			if (sx >= srows - 1) {
				fx = 0, sx = srows - 1;
			}
		}

		for (k = 0, sx *= cn; k < cn; k++) {
			xofs[dx*cn + k] = sx + k;
		}

		cbuf[0] = 1.f - fx;
		cbuf[1] = fx;

		if (fixpt) {
			for (k = 0; k < ksize; k++) {
				ialpha[dx*cn*ksize + k] = saturate_cast<short>(cbuf[k] * INTER_RESIZE_COEF_SCALE);
			}
			for (; k < cn*ksize; k++) {
				ialpha[dx*cn*ksize + k] = ialpha[dx*cn*ksize + k - ksize];
			}
		}
		else {
			for (k = 0; k < ksize; k++) {
				alpha[dx*cn*ksize + k] = cbuf[k];
			}
			for (; k < cn*ksize; k++) {
				alpha[dx*cn*ksize + k] = alpha[dx*cn*ksize + k - ksize];
			}
		}
	}

	for (dy = 0; dy < dsize.height; dy++) {
		fy = (float)((dy + 0.5)*scale_y - 0.5);
		sy = fbcFloor(fy);
		fy -= sy;

		yofs[dy] = sy;
		cbuf[0] = 1.f - fy;
		cbuf[1] = fy;

		if (fixpt) {
			for (k = 0; k < ksize; k++) {
				ibeta[dy*ksize + k] = saturate_cast<short>(cbuf[k] * INTER_RESIZE_COEF_SCALE);
			}
		}
		else {
			for (k = 0; k < ksize; k++) {
				beta[dy*ksize + k] = cbuf[k];
			}
		}
	}

	if (sizeof(_Tp) == 1) { // uchar
		typedef uchar value_type; // HResizeLinear/VResizeLinear
		typedef int buf_type;
		typedef short alpha_type;
		int ONE = INTER_RESIZE_COEF_SCALE;

		resizeGeneric_Linear<_Tp, value_type, buf_type, alpha_type, chs>(src, dst,
			xofs, fixpt ? (void*)ialpha : (void*)alpha, yofs, fixpt ? (void*)ibeta : (void*)beta, xmin, xmax, ksize, ONE);
	}
	else if (sizeof(_Tp) == 4) { // float
		typedef float value_type; // HResizeLinear/VResizeLinear
		typedef float buf_type;
		typedef float alpha_type;
		int ONE = 1;

		resizeGeneric_Linear<_Tp, value_type, buf_type, alpha_type, chs>(src, dst,
			xofs, fixpt ? (void*)ialpha : (void*)alpha, yofs, fixpt ? (void*)ibeta : (void*)beta, xmin, xmax, ksize, ONE);
	}
	else {
		fprintf(stderr, "not support type\n");
		return -1;
	}

	return 0;
}

