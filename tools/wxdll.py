#!/usr/bin/env python3
"""Export list and import library of the wxWindows DLL (16-bit Windows,
OpenWatcom).

    wxdll.py exports DLLNAME OUT.lnk OBJ...   wlink directives: every public
             symbol of the objects as "export W<n>.<n>='<symbol>'" (short
             names: the full C++ names would overflow the 64 KB name table)
    wxdll.py implib DLLNAME EXPORTS.lnk OUT.cmd
             wlib commands "++'<symbol>'.<DLLNAME>..<n>": import by ordinal
             (the Windows loader compares imported names in upper case)
"""
import re
import sys

SKIP = {'LibMain', 'LIBMAIN', 'WEP', '_WEP'}


def publics(path):
    """Names of the PUBDEF (90h/91h) records of an OMF object file."""
    data = open(path, 'rb').read()
    i = 0
    while i + 3 <= len(data):
        typ = data[i]
        ln = data[i+1] | data[i+2] << 8
        rec = data[i+3:i+3+ln-1]
        i += 3 + ln
        if typ not in (0x90, 0x91):
            continue

        def index(j):
            v = rec[j]
            j += 1
            if v & 0x80:
                v = (v & 0x7f) << 8 | rec[j]
                j += 1
            return v, j
        _, j = index(0)
        seg, j = index(j)
        if seg == 0:
            j += 2
        while j < len(rec):
            n = rec[j]
            yield rec[j+1:j+1+n].decode('latin-1')
            j += 1 + n + (4 if typ == 0x91 else 2)
            _, j = index(j)


def main():
    if sys.argv[1] == 'exports':
        out = []
        seen = set()
        for obj in sys.argv[4:]:
            for name in publics(obj):
                if name in seen or name in SKIP:
                    continue
                seen.add(name)
                n = len(out) + 1
                out.append("export W%d.%d='%s'" % (n, n, name.replace("'", "\\'")))
        open(sys.argv[3], 'w').write('\n'.join(out) + '\n')
    elif sys.argv[1] == 'implib':
        dll = sys.argv[2]
        out = []
        for line in open(sys.argv[3]):
            m = re.match(r"export W(\d+)\.(\d+)='(.*)'$", line.rstrip('\n'))
            if m:
                out.append("++'%s'.%s..%s" % (m.group(3), dll, m.group(2)))
        open(sys.argv[4], 'w').write('\n'.join(out) + '\n')
    else:
        sys.exit(__doc__)


main()
