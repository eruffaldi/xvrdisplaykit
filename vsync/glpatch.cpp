#include <windows.h>
#include <stdio.h>
#include <gl/GL.h>
#include "patcher.h"

class Elapse
{
public:
		__int64 _frequency,_started,_now;
		
		Elapse()
		{
			QueryPerformanceFrequency((LARGE_INTEGER*)&_frequency);
			QueryPerformanceCounter((LARGE_INTEGER*)&_started);            
            _now = _started;
		}
		
		void start()
		{
			QueryPerformanceCounter((LARGE_INTEGER*)&_started);
		}
		
		double stop()
        {
			QueryPerformanceCounter((LARGE_INTEGER*)&_now);
			__int64 delta = _now-_started;
			return (double)delta/(double)_frequency;
		}
        
		double fromstop()
        {
			__int64 tmp;
			QueryPerformanceCounter((LARGE_INTEGER*)&tmp);
			__int64 delta = tmp-_now;
			return (double)delta/(double)_frequency;
		}
		
        double stoptime()
        {
            return (double)_now/(double)_frequency;
        }
        double starttime()
        {
            return (double)_started/(double)_frequency;
        }
};
			
typedef void  (__stdcall FAR * tEmpty)();
typedef int  (__stdcall FAR * tSwap)(HDC hdc);

template <class T>
struct Fx
{
    typedef T fx_t;
    Fx(): patch(0),elapsed(0),enabled(true), timed(false) {}
    fx_t original;
    Elapse timing;
    bool timed;
    bool enabled;
    double elapsed;
    CPatch * patch;
};

Elapse marktime;
float markvalue;
Fx<tEmpty> flush;
Fx<tSwap> swap;
HMODULE hgl,hgdi;

 void  __stdcall mFlush()
{      
    Fx<tEmpty> & f = flush;
    if(f.enabled)
    {
        if(f.timed)
            f.timing.start();
        f.original();
        if(f.timed)
            f.elapsed = f.timing.stop();
    }
}

int __stdcall mSwap(HDC h)
{
    Fx<tSwap> & f = swap;
    int r = 0;
    if(f.enabled)
    {
        if(f.timed)
            f.timing.start();
        r = f.original(h);
        marktime.start();
        if(f.timed)
            f.elapsed = f.timing.stop();
    }
    return r;
}

extern "C"
{
__declspec(dllexport) void init()
{
	//hgl = LoadLibrary("opengl32.dll");
	hgdi = LoadLibrary("gdi32.dll");
    //flush.set((tEmpty)GetProcAddress(hgl,"glFlush"),(tEmpty)&mFlush);
    swap.original = (tSwap)GetProcAddress(hgdi,"SwapBuffers");
    swap.patch = new CPatch(swap.original,(tSwap)&mSwap,true);
    printf("Patched\n");
}

__declspec(dllexport) void setswapenable(int where)
{
    swap.enabled = where != 0;
}

__declspec(dllexport) void settimedswapenable(int where)
{
    swap.timed = where != 0;
}

__declspec(dllexport) float getswapduration()
{
    return marktime.stoptime()-marktime.starttime();
}

__declspec(dllexport) float getswaptonow()
{
	return marktime.fromstop();
}

__declspec(dllexport) void getswaptimestr(int when, char * buf)
{
    *(double*)buf = when == 0 ? marktime.starttime() : marktime.stoptime();
}

__declspec(dllexport) float getswaptime(int what)
{
    return what == 0 ? marktime.starttime():marktime.stoptime();
}


static bool hasfreq;
static __int64 freq;

__declspec(dllexport) void QueryPerformanceTimeAsString(char * buf)
{
	if(!hasfreq)
	{
		QueryPerformanceFrequency ((LARGE_INTEGER *)&freq);
		hasfreq = true;
	}
    __int64 curtime;	
    QueryPerformanceCounter( (LARGE_INTEGER *)&curtime );
	double now = (double)(curtime) / (double)freq;
	memcpy(buf,(void*)&now,sizeof(double));
}

__declspec(dllexport) float QueryPerformanceTimeDelta(char * a,char *b)
{
	double *pa = (double*)a;
	double *pb = (double*)b;
	return (float)(*pa-*pb);
}
}

#include "patcher.cpp"
