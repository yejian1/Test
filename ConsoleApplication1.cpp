// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#define DBL_EPSILON      2.2204460492503131e-016

#include<algorithm>
#include <iostream>
#include <stdio.h>
#include<Math.h>
#include <opencv2\opencv.hpp>
using namespace std;



void resize(unsigned char * _src, unsigned char *_dst, double inv_scale_x, double inv_scale_y, int interpolation)
{
	//判断输入的正确性
	if (*_src == NULL) 
		return;
	 
}
class Image {
public:
	Image(int _height, int _width, int _depth) :
		height(_height), width(_width), depth(_depth), dims(1), channel(1), elemSize(1)
	{
		  data = new unsigned char [height*width*depth*channel];
		 

	}
	void inital() {
		int i;
		unsigned char x = 0;
		for (i = 0; i < height*width*depth*channel; i ++)
			
		{   
			data[i] = x;
			x++;
		}
	}
	unsigned char* data;
	int height;
	int width;
	int depth;
	int step;
	int dims;
	int elemSize;
	int channel;
};

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


class  Range
{
public:
	Range();
	Range(int _start, int _end);
	int size() const;
	bool empty() const;
	static Range all();

	int start, end;
};
inline
Range::Range()
	: start(0), end(0) {}

inline
Range::Range(int _start, int _end)
	:start(_start),end(_end){}


int Range::size() const
{
	return end - start;
}

bool Range::empty() const
{
	return start == end;
}


class resizeNNInvoker 
{
public:
	resizeNNInvoker(const Image& _src, Image& _dst, int *_x_ofs, int _pix_size4, double _ify,Range& _range) :
		 src(_src), dst(_dst), x_ofs(_x_ofs), pix_size4(_pix_size4), ify(_ify),range(_range)
	{
	}

	 void cal()
	{
		int y, x, pix_size = (int)src.elemSize;
		for (y = range.start; y < range.end; y++)
		{

			unsigned char* D = dst.data + dst.step*y;
			int sy =(int(floor(y*ify)), src.height - 1);
			//指向第sy行
			const unsigned char *S = src.data + src.elemSize*sy;
			switch (pix_size) 
			{
			case 1:
				for (x = 0; x <= dst.width - 2; x += 2)
				{
					unsigned char t0 = S[x_ofs[x]];
					unsigned char t1 = S[x_ofs[x + 1]];
					D[x] = t0;
					D[x + 1] = t1;
				}
				for (; x < dst.width; x++)
					D[x] = S[x_ofs[x]];

			}
		}

	}
private:
	const Image src;
	Range range;
	Image dst;
	int* x_ofs, pix_size4;
	double ify;
	resizeNNInvoker(const resizeNNInvoker&);
	//resizeNNInvoker& operator=(const resizeNNInvoker&);


};
void resizeNN(Image src, Image dst, double fx, double fy)
{
	int width = src.width, height = src.height;
	int a =8;
	AutoBuffer<int>  _x_ofs(a);
	int* x_ofs = _x_ofs.data();
	int pix_size = (int)src.elemSize;
	int pix_size4 = (int)(pix_size / sizeof(int));
	double ifx = 1./ fx , ify = 1./fy;
	int x;
	for (x = 0; x < width; x++)
	{
		int sx = floor(x*ifx);
		x_ofs[x] = std::min(sx, width - 1)*pix_size;
	}
	Range range(0, dst.height);
	resizeNNInvoker invoker(src, dst, x_ofs, pix_size4, ify, range);
	invoker.cal();
}


int main()
{
	
	Image src(4, 4, 1);
	src.inital();
	double scale_x = 2, scale_y = 2;
	Image dst((int)(10 * scale_x), (int)(10 * scale_y), 1);
	resizeNN(src, dst,0.5,0.5);
	
	cv::Mat input(4,4,CV_8UC1);
	cv::Mat output(8,8, CV_8UC1);
	unsigned char * data = input.data;
	for (int i = 0; i < 100; i++)
	{
		data[i] = i;
	}
	cv::resize(input, output, cv::Size(8,8),  cv::INTER_NEAREST);
	int a,b;
	for (int i = 0; i < 64; i++)
	{
		 a = (int)dst.data[i];
		 b = (int)output.data[i];
		 cout <<i<< ": "<< a<<endl;
		 cout << i << ": " << b << endl;
	}
	system("pause");
	return 0;

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
