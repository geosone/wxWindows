# wxWindows (1.68E, Windows 3.11 compatible) for OpenWatcom

This repository holds **wxWindows 1.68E** (1996, Julian Smart, AIAI,
University of Edinburgh), the last wxWindows version compatible with
Windows 3.1x / Windows for Workgroups 3.11, built for **16-bit Windows
(Windows 3.1x, Windows 9x) with OpenWatcom on Linux**.

The first commit is the unchanged original (wx168_1.tgz + wx168_2.tgz); everything after
it is the OpenWatcom port.  Used by [Open 386MAX](https://github.com/geosone/386MAX)
for its Windows programs.

License: see [LICENSE](LICENSE) (the original wxWindows 1.x permission
notice).

## Build

OpenWatcom 2 (`WATCOM`, default `/opt/watcom`) and GNU make on Linux:

```
make            # build/lib/wx.lib (16-bit Windows, large model) and the samples
make lib        # only the library
make samples    # build/samples/<dir>/<program>.exe, list in samples.lst
```

Changes against the original are small and kept compatible with the other
compilers (see the git history): `#include` paths with `/`, OpenWatcom
headers (no 16-bit ODBC, `drivinit.h` for `print.h`, `iostream.h`), a few
C++ fixes, resource file names in the case of the files.

## Tests

`test/wintest.sh [win311|win98|all] [sample ...]` runs every sample in
Windows for Workgroups 3.11 and Windows 98 SE in QEMU and saves a
screenshot of each (`build/test/<os>/`, overview `index.png`).  The
Windows images come from Open 386MAX (`linux/test/mkwin311.sh`,
`linux/test/mkwin98.sh`), see `WIN311IMG`/`WIN98IMG`.

All 22 samples of `samples.lst` start and draw in both.  Not built: mfc
(MFC), odbc (no 16-bit ODBC in OpenWatcom), static, vlist (switched off in
`wx_setup.h`), memcheck (debug build only).
