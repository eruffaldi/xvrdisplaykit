#include <windows.h>
#include "dwmapi.h"
#include <stdio.h>

#pragma comment(lib,"dwmapi.lib")
#pragma comment(lib,"user32.lib")

int main(int argc, char * argv[])
{
    BOOL b = 0;
    DwmIsCompositionEnabled(&b);
    printf("%d\n",b);
    if(!b)
        DwmEnableComposition(DWM_EC_ENABLECOMPOSITION);
    DwmIsCompositionEnabled(&b);
    printf("%d\n",b);
    DwmEnableMMCSS(1);
    DWM_TIMING_INFO ti;
    ti.cbSize = sizeof(ti);
    ZeroMemory(&ti,sizeof(ti));
    DwmGetCompositionTimingInfo(GetDesktopWindow(),&ti);
    printf("%d Rate %d/%d  Refresh Count:%f %f\n",GetLastError(),ti.rateRefresh.uiNumerator,ti.rateRefresh.uiDenominator,(double)ti.cRefresh,(double)ti.qpcRefreshPeriod);
    Sleep(100);
    DwmGetCompositionTimingInfo(GetDesktopWindow(),&ti);
    printf("%d Rate %d/%d  Refresh Count:%f %f\n",GetLastError(),ti.rateRefresh.uiNumerator,ti.rateRefresh.uiDenominator,(double)ti.cRefresh,(double)ti.qpcRefreshPeriod);
    return 0;
}