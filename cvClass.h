#pragma once

typedef unsigned int uint;
typedef signed char schar;
typedef unsigned char uchar;
typedef unsigned short ushort;
enum InterpolationFlags {

	INTER_NEAREST = 0,

	INTER_LINEAR = 1,

	INTER_CUBIC = 2,

	INTER_AREA = 3,

	INTER_LANCZOS4 = 4,

	INTER_MAX = 7,

	WARP_FILL_OUTLIERS = 8,

	WARP_INVERSE_MAP = 16
};

template<typename _Tp> class mySize;
template<typename _Tp> class myRect;
template<typename _Tp, int chs>class myMat;
template<typename _Tp> class myScalar;

template<typename _Tp, size_t fixed_size = 1024 / sizeof(_Tp) + 8> class AutoBuffer
{
public:
	typedef _Tp value_type;

	//! the default constructor
	AutoBuffer();
	//! constructor taking the real buffer size
	explicit AutoBuffer(size_t _size);

	//! the copy constructor
	AutoBuffer(const AutoBuffer<_Tp, fixed_size>& buf);
	//! the assignment operator
	AutoBuffer<_Tp, fixed_size>& operator = (const AutoBuffer<_Tp, fixed_size>& buf);

	//! destructor. calls deallocate()
	~AutoBuffer();

	//! allocates the new buffer of size _size. if the _size is small enough, stack-allocated buffer is used
	void allocate(size_t _size);
	//! deallocates the buffer if it was dynamically allocated
	void deallocate();
	//! resizes the buffer and preserves the content
	void resize(size_t _size);
	//! returns the current buffer size
	size_t size() const;
	//! returns pointer to the real buffer, stack-allocated or heap-allocated
	inline _Tp* data() { return ptr; }
	//! returns read-only pointer to the real buffer, stack-allocated or heap-allocated
	inline const _Tp* data() const { return ptr; }

#if !defined(OPENCV_DISABLE_DEPRECATED_COMPATIBILITY) // use to .data() calls instead
	//! returns pointer to the real buffer, stack-allocated or heap-allocated
	operator _Tp* () { return ptr; }
	//! returns read-only pointer to the real buffer, stack-allocated or heap-allocated
	operator const _Tp* () const { return ptr; }
#else
	//! returns a reference to the element at specified location. No bounds checking is performed in Release builds.
	inline _Tp& operator[] (size_t i) { CV_DbgCheckLT(i, sz, "out of range"); return ptr[i]; }
	//! returns a reference to the element at specified location. No bounds checking is performed in Release builds.
	inline const _Tp& operator[] (size_t i) const { CV_DbgCheckLT(i, sz, "out of range"); return ptr[i]; }
#endif

protected:
	//! pointer to the real buffer, can point to buf if the buffer is small enough
	_Tp* ptr;
	//! size of the real buffer
	size_t sz;
	//! pre-allocated buffer. At least 1 element to confirm C++ standard requirements
	_Tp buf[(fixed_size > 0) ? fixed_size : 1];
};

template<typename _Tp, int chs>class myMat 
{   
	public:
		typedef _Tp value_type;

		myMat() :rows(0), cols(0), channls(0), data(NULL);

		//¶þÎ¬¾ØÕó
		myMat(int _rows, int _cols, const Scalar& _s);
		myMat(int _rows,int _cols, uchar* _data)£»

		int rows,cols,channels,step;
		uchar* data;
		const uchar* databegin;
		const uchar* dataend;

		
};



template<typename _Tp, int chs>
myMat<_Tp, chs>::myMat(int _rows, int _cols, const Scalar& _s)
{
	FBC_Assert(_rows > 0 && _cols > 0 && chs > 0);

	this->rows = _rows;
	this->cols = _cols;
	this->channels = chs;
	this->step = sizeof(_Tp) * _cols * chs;
	this->allocated = true;

	size_t size_ = this->rows * this->step;
	uchar* p = (uchar*)fastMalloc(size_);
	FBC_Assert(p != NULL);
	this->data = p;
	this->datastart = this->data;
	this->dataend = this->data + size_;

	for (int i = 0; i < _rows; i++) {
		_Tp* pRow = (_Tp*)this->data + i * _cols * chs;

		for (int j = 0; j < _cols; j++) {
			_Tp* pPixel = pRow + j * chs;

			for (int m = 0, n = 0; m < chs && n < 4; m++, n++) {
				pPixel[n] = saturate_cast<_Tp>(_s.val[n]);
			}
		}
	}
}


template<typename _Tp, int chs> int resize_linar(const myMat<_Tp, chs>& _src, myMat<_Tp, chs>& _dst);

template<typename _Tp,int chs>
bool resize(const myMat<_Tp, chs>& _src, myMat<_Tp, chs>& _dst, int interpolation = InterpolationFlags::INTER_LINEAR)
{
	switch (interpolation) 
	{
		case 1: {
			resize_linear(src, dst);
		break;
	   }
	}

}



//////////////////////////////// mySize  ////////////////////////////////
// Template class for specifying the size of an image or rectangle

template<typename _Tp> class mySize
{
public:
	typedef _Tp value_type;

	//! various constructors
	mySize();
	mySize(_Tp _width, _Tp _height);
	mySize(const mySize& sz);
	mySize& operator = (const mySize& sz);

	//! the area (width*height)
	_Tp area() const;

	//! conversion of another data type.
	template<typename _Tp2> operator Size_<_Tp2>() const;
	_Tp width, height; // the width and the height

};


template<typename _Tp> inline
mySize<_Tp>::mySize()
	: width(0), height(0) {}

template<typename _Tp> inline
mySize<_Tp>::mySize(_Tp _width, _Tp _height)
	: width(_width), height(_height) {}

template<typename _Tp> inline
mySize<_Tp>::mySize(const mySize& sz)
	: width(sz.width), height(sz.height) {}

template<typename _Tp> inline
mySize<_Tp>& mySize<_Tp>::operator = (const mySize<_Tp>& sz)
{
	width = sz.width; height = sz.height;
	return *this;
}
template<typename _Tp> inline
_Tp mySize<_Tp>::area() const
{
	return width * height;

}


/////////////!encod
typedef mySize<int> Size2i;
typedef mySize<float> Size2f;
typedef mySize<double> Size2d;
typedef Size2i Size;



//////////////////////////////// myPoint ////////////////////////////////
// Template class for 2D points specified by its coordinates `x` and `y`
template<typename _Tp> class myPoint
{
public:
	typedef _Tp value_type;

	// various constructors
	myPoint();
	myPoint(_Tp _x, _Tp _y);
	myPoint(const myPoint& pt);
	myPoint(const mySize<_Tp>& sz);
	myPoint(const Vec<_Tp, 2>& v);

	myPoint& operator = (const myPoint& pt);
	//! conversion to another data type
	template<typename _Tp2> operator myPoint<_Tp2>() const;

	//! conversion to the old-style C structures
	operator Vec<_Tp, 2>() const;

	//! dot product
	_Tp dot(const myPoint& pt) const;
	//! dot product computed in double-precision arithmetics
	double ddot(const myPoint& pt) const;
	//! cross-product
	double cross(const myPoint& pt) const;
	//! checks whether the point is inside the specified rectangle
	bool inside(const myRect<_Tp>& r) const;

	_Tp x, y; //< the point coordinates
};


typedef myPoint<int> Point2i;
typedef myPoint<float> Point2f;
typedef myPoint<double> Point2d;
typedef Point2i Point;


//////////////////////////////// 2D Point ///////////////////////////////

template<typename _Tp> inline
myPoint<_Tp>::myPoint()
	: x(0), y(0) {}

template<typename _Tp> inline
myPoint<_Tp>::myPoint(_Tp _x, _Tp _y)
	: x(_x), y(_y) {}

template<typename _Tp> inline
myPoint<_Tp>::myPoint(const myPoint& pt)
	: x(pt.x), y(pt.y) {}

template<typename _Tp> inline
myPoint<_Tp>::myPoint(const mySize<_Tp>& sz)
	: x(sz.width), y(sz.height) {}

template<typename _Tp> inline
myPoint<_Tp>& myPoint<_Tp>::operator = (const myPoint& pt)
{
	x = pt.x; y = pt.y;
	return *this;
}

template<typename _Tp> template<typename _Tp2> inline
myPoint<_Tp>::operator myPoint<_Tp2>() const
{
	return myPoint<_Tp2>(saturate_cast<_Tp2>(x), saturate_cast<_Tp2>(y));
}

template<typename _Tp> inline
_Tp myPoint<_Tp>::dot(const myPoint& pt) const
{
	return saturate_cast<_Tp>(x*pt.x + y * pt.y);
}
//////////////////////////////// myRect ////////////////////////////////
// Template class for 2D rectangles

template<typename _Tp> class myRect
{
public:
	typedef _Tp value_type;

	//! various constructors
	myRect();
	myRect(_Tp _x, _Tp _y, _Tp _width, _Tp _height);
	myRect(const myRect& r);
	myRect(const myPoint<_Tp>& org, const mySize<_Tp>& sz);
	myRect(const myPoint<_Tp>& pt1, const myPoint<_Tp>& pt2);

	myRect& operator = (const myRect& r);
	//! the top-left corner
	myPoint<_Tp> tl() const;
	//! the bottom-right corner
	myPoint<_Tp> br() const;

	//! size (width, height) of the rectangle
	mySize<_Tp> size() const;
	//! area (width*height) of the rectangle
	_Tp area() const;

	//! conversion to another data type
	template<typename _Tp2> operator myRect<_Tp2>() const;

	//! checks whether the rectangle contains the point
	bool contains(const Point_<_Tp>& pt) const;

	_Tp x, y, width, height; //< the top-left corner, as well as width and height of the rectangle
};

typedef myRect<int> Rect2i;
typedef myRect<float> Rect2f;
typedef myRect<double> Rect2d;
typedef Rect2i Rect;
//////////////////////////////// myRect ////////////////////////////////
template<typename _Tp> inline
myRect<_Tp>::myRect(): x(0), y(0), width(0), height(0) {}

template<typename _Tp> inline
myRect<_Tp>::myRect(_Tp _x, _Tp _y, _Tp _width, _Tp _height)
	: x(_x), y(_y), width(_width), height(_height) {}

template<typename _Tp> inline
myRect<_Tp>::myRect(const myRect<_Tp>& r)
	: x(r.x), y(r.y), width(r.width), height(r.height) {}


template<typename _Tp> inline
myRect<_Tp>::myRect(const myPoint<_Tp>& org, const mySize<_Tp>& sz)
	: x(org.x), y(org.y), width(sz.width), height(sz.height) {}

template<typename _Tp> inline
myRect<_Tp>::myRect(const myPoint<_Tp>& pt1, const myPoint<_Tp>& pt2)
{
	x = std::min(pt1.x, pt2.x);
	y = std::min(pt1.y, pt2.y);
	width = std::max(pt1.x, pt2.x) - x;
	height = std::max(pt1.y, pt2.y) - y;
}

template<typename _Tp> inline
myRect<_Tp>& myRect<_Tp>::operator = (const myRect<_Tp>& r)
{
	x = r.x;
	y = r.y;
	width = r.width;
	height = r.height;
	return *this;
}

template<typename _Tp> inline
myPoint<_Tp>  myRect<_Tp>::tl() const
{
	return myPoint<_Tp>(x, y);
}

template<typename _Tp> inline
myPoint<_Tp> myRect<_Tp>::br() const
{
	return myPoint<_Tp>(x + width, y + height);
}

template<typename _Tp> inline
mySize<_Tp> myRect<_Tp>::size() const
{
	return mySize<_Tp>(width, height);
}

template<typename _Tp> inline
_Tp myRect<_Tp>::area() const
{
	return width * height;
}


//////////////////////////////// myScalar ////////////////////////////////
template<typename _Tp> class myScalar
{
public:
	//! various constructors
	enum {
		rows = m,
		cols = n,
		channels = rows * cols,
		shortdim = (m < n ? m : n)
	};
	myScalar();
	myScalar(_Tp v0, _Tp v1, _Tp v2 = 0, _Tp v3 = 0);
	myScalar(_Tp v0);

	template<typename _Tp2, int cn>
	myScalar(const Vec<_Tp2, cn>& v);

	//! returns a scalar with all elements set to v0
	static myScalar<_Tp> all(_Tp v0);

	//! conversion to another data type
	template<typename T2> operator myScalar<T2>() const;

	//! per-element product
	myScalar<_Tp> mul(const myScalar<_Tp>& a, double scale = 1) const;

	// returns (v0, -v1, -v2, -v3)
	myScalar<_Tp> conj() const;

	// returns true iff v1 == v2 == v3 == 0
	bool isReal() const;

	_Tp val[m*n];
};

typedef myScalar<double> Scalar;

///////////////////////////////// Scalar_ ////////////////////////////////
template<typename _Tp> inline
myScalar<_Tp>::myScalar()
{
	this->val[0] = this->val[1] = this->val[2] = this->val[3] = 0;
}

template<typename _Tp> inline
myScalar<_Tp>::myScalar(_Tp v0, _Tp v1, _Tp v2, _Tp v3)
{
	this->val[0] = v0;
	this->val[1] = v1;
	this->val[2] = v2;
	this->val[3] = v3;
}

template<typename _Tp> inline
myScalar<_Tp>::myScalar(_Tp v0)
{
	this->val[0] = v0;
	this->val[1] = this->val[2] = this->val[3] = 0;
}

template<typename _Tp> inline
myScalar<_Tp> myScalar<_Tp>::all(_Tp v0)
{
	return myScalar<_Tp>(v0, v0, v0, v0);
}

template<typename _Tp> template<typename T2> inline
myScalar<_Tp>::operator myScalar<T2>() const
{
	return Scalar_<T2>(saturate_cast<T2>(this->val[0]),
		saturate_cast<T2>(this->val[1]),
		saturate_cast<T2>(this->val[2]),
		saturate_cast<T2>(this->val[3]));
}

template<typename _Tp> inline
myScalar<_Tp> myScalar<_Tp>::mul(const myScalar<_Tp>& a, double scale) const
{
	return Scalar_<_Tp>(saturate_cast<_Tp>(this->val[0] * a.val[0] * scale),
		saturate_cast<_Tp>(this->val[1] * a.val[1] * scale),
		saturate_cast<_Tp>(this->val[2] * a.val[2] * scale),
		saturate_cast<_Tp>(this->val[3] * a.val[3] * scale));
}