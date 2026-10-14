/*
 * owdll.h - forced include (wpp -fi) when wxWindows is built as a 16-bit
 * Windows DLL with OpenWatcom (GNUmakefile: make dll).
 *
 * The DLL's functions are exported by the linker, not with __export: the
 * Windows prolog of exported functions needs AX, which OpenWatcom's
 * register calling convention passes the first argument in.  Instead every
 * function loads DS with the DLL's DGROUP itself (loadds), so a program can
 * call it through an import.  _WRTLDCONV/_WRTLFCONV: the run time headers
 * mark their variables __watcall, which clashes with a changed default; the
 * calling convention stays __watcall (loadds only changes the callee's
 * prolog), so the declarations without it are the same.
 */
#include <_comdef.h>
#undef _WRTLDCONV
#define _WRTLDCONV
#undef _WRTLFCONV
#define _WRTLFCONV
#pragma aux default loadds;
