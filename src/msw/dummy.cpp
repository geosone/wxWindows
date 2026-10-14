/*
 * File:	dummy.cc
 * Purpose:	See below
 * Author:	Julian Smart
 * Created:	1993
 * Updated:	
 * Copyright:	(c) 1993, AIAI, University of Edinburgh
 */

/* static const char sccsid[] = "@(#)dummy.cc	1.2 5/9/94"; */

/* A dummy file to include wx.h. If precompiling wx.h, I
 * always start by compiling this and producing the PCH file.
 * Then subsequent source files use the PCH file.
 *
 * If precompiling wx.h for wxWindows and derived apps,
 * link dummy.obj with your program (the MSC 7 linker complains otherwise).
 *
 * This is the only convenient way I found to use precompiled headers
 * under MSC 7.
 *
 * This will produce a big PCH file.
 */



#if defined(__BORLANDC__)
#if !(defined(WIN32) || defined(__NT__) || defined(__WIN32__))
#pragma hdrfile "c:\wx\src\msw\wx.pch"
#endif

#pragma hdrstart
#endif

#include "wx_prec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

// Foils optimizations in Visual C++ (see also wx_main.cc)
char wxDummyChar=0;

#if defined(WXUSINGDLL)

// NT defines APIENTRY, 3.x not
#if !defined(APIENTRY)
#define APIENTRY FAR PASCAL
#endif
 
#ifdef __WATCOMC__
int PASCAL
#else
int APIENTRY
#endif
 WinMain(HANDLE hInstance, HANDLE hPrevInstance, LPSTR m_lpCmdLine,
                    int nCmdShow )
{
#if defined(__WATCOMC__) && !defined(__386__)
  // 16-bit DLL build (OpenWatcom): the program's code loads its DGROUP
  // (owdll.h), which is the first instance's - activate that one instead
  if (hPrevInstance)
  {
    HWND hWnd;
    for (hWnd = GetWindow(GetDesktopWindow(), GW_CHILD); hWnd; hWnd = GetWindow(hWnd, GW_HWNDNEXT))
      if (GetWindowWord(hWnd, GWW_HINSTANCE) == (WORD)hPrevInstance &&
          !GetWindow(hWnd, GW_OWNER) && IsWindowVisible(hWnd))
      {
        if (IsIconic(hWnd))
          ShowWindow(hWnd, SW_RESTORE);
        BringWindowToTop(hWnd);
        break;
      }
    return 0;
  }
#endif
  return wxEntry(hInstance, hPrevInstance, m_lpCmdLine, nCmdShow);
}

#if defined(__WATCOMC__) && !defined(__386__)
// 16-bit Windows DLL (OpenWatcom): objects come from the DLL's heap, the
// DLL deletes some of them (see wx_main.cpp)
void *wxDllAlloc(size_t size);
void wxDllFree(void *p);
void *operator new(size_t size) { return wxDllAlloc(size); }
void operator delete(void *p) { wxDllFree(p); }
void *operator new[](size_t size) { return wxDllAlloc(size); }
void operator delete[](void *p) { wxDllFree(p); }
#endif
#endif


