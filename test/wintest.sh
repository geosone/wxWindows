#!/bin/bash
# Run the samples in Windows for Workgroups 3.11 and Windows 98 SE (QEMU)
# and take a screenshot of each: one QEMU per sample, JOBS at a time.
#
#   test/wintest.sh [win311|win98|all] [sample ...]     (default: all, all)
#
# Environment:
#   WIN311IMG  WfW 3.11 hard disk image, C: = FAT16 partition at sector 63
#              (default ../386MAX/build/win311/win311.img, made by
#              386MAX's linux/test/mkwin311.sh)
#   WIN98IMG   Windows 98 SE image (default ../386MAX/build/win98/
#              win98-base.img, made by 386MAX's linux/test/mkwin98.sh)
#   CTL3DDLL   CTL3DV2.DLL to copy next to the samples if Windows lacks it
#              (default ../386MAX/build/bin/CTL3DV2.DLL if present)
#   SECS       seconds until the screenshot (default 90)
#   JOBS       QEMUs at the same time (default 4)
#
# Output: build/test/<os>/<sample>.png and index.png (all screenshots)
set -uo pipefail
TOP=$(cd "$(dirname "$0")/.." && pwd)
OS=${1:-all}; shift 2>/dev/null
WIN311IMG=${WIN311IMG:-$TOP/../386MAX/build/win311/win311.img}
WIN98IMG=${WIN98IMG:-$TOP/../386MAX/build/win98/win98-base.img}
CTL3DDLL=${CTL3DDLL:-$TOP/../386MAX/build/bin/CTL3DV2.DLL}
SECS=${SECS:-90}
JOBS=${JOBS:-4}
SAMPLES=${*:-$(sed -e 's/#.*//' -e '/^ *$/d' "$TOP/samples.lst" | awk '{print $1}')}

crlf() { tr '|' '\n' | sed 's/$/\r/'; }

run_one() {         # run_one os sample
    local os=$1 s=$2 img dir out P
    dir=$(awk -v s="$s" '$1==s {print $2}' "$TOP/samples.lst")
    out=$TOP/build/test/$os/$s; rm -rf "$out"; mkdir -p "$out"
    case $os in win311) img=$WIN311IMG ;; win98) img=$WIN98IMG ;; esac
    cp --sparse=always "$img" "$out/test.img"
    P="$out/test.img@@32256"
    local D=WXTEST/$(echo "$dir" | tr a-z A-Z)
    mmd -i "$P" ::/WXTEST ::/$D 2>/dev/null
    # the sample's data files and the program
    for f in "$TOP/samples/$dir"/*; do
        case ${f##*.} in cpp|h|rc|def|o|obj|unx|vms|wat|nt|bcc|b32|dos|sc|g95) continue ;; esac
        case ${f##*/} in makefile*) continue ;; esac
        [ -f "$f" ] && mcopy -o -i "$P" "$f" ::/$D/
    done
    mcopy -o -i "$P" "$TOP/build/samples/$dir/$s.exe" ::/$D/
    [ -f "$CTL3DDLL" ] && mcopy -o -i "$P" "$CTL3DDLL" ::/$D/
    if [ $os = win311 ]; then
        echo 'DEVICE=C:\HIMEM.SYS|FILES=40|BUFFERS=20|SHELL=C:\COMMAND.COM C:\ /P' | crlf > "$out/CONFIG.SYS"
        echo "@ECHO OFF|PATH C:\\;C:\\WINDOWS|C:|CD \\$D|C:\\WINDOWS\\WIN $s.EXE" | sed 's#/#\\#g' | crlf > "$out/AUTOEXEC.BAT"
        mcopy -o -i "$P" "$out/CONFIG.SYS" "$out/AUTOEXEC.BAT" ::/
        local drive=(-drive if=ide,index=0,format=raw,file="$out/test.img") cpu=() mem=32
    else
        # Windows 98: WIN.INI run= starts a batch file that changes there
        echo "@ECHO OFF|C:|CD \\$D|START $s.EXE" | sed 's#/#\\#g' | crlf > "$out/WXRUN.BAT"
        mcopy -o -i "$P" "$out/WXRUN.BAT" ::/
        mcopy -o -i "$P" ::/WINDOWS/WIN.INI "$out/win.ini"
        sed -i 's#^run=.*#run=C:\\WXRUN.BAT\r#' "$out/win.ini"
        mcopy -o -i "$P" "$out/win.ini" ::/WINDOWS/WIN.INI
        local drive=(-drive if=none,id=hd,format=raw,file="$out/test.img"
                     -device ide-hd,drive=hd,bus=ide.0,lcyls=520,lheads=64,lsecs=63) cpu=(-cpu pentium3) mem=128
    fi
    { sleep "$SECS"; echo "screendump $out/screen.ppm"; sleep 1; echo quit; } |
    qemu-system-i386 -machine pc ${ACCEL:--accel kvm} "${cpu[@]}" -m $mem -display none -vga std \
        -monitor stdio "${drive[@]}" -boot c -nic none -rtc base=localtime > "$out/qemu.log" 2>&1
    magick "$out/screen.ppm" "$TOP/build/test/$os/$s.png" 2>/dev/null && rm -f "$out/screen.ppm"
    rm -f "$out/test.img"
    echo "$os $s"
}

for os in win311 win98; do
    [ "$OS" = all ] || [ "$OS" = $os ] || continue
    case $os in win311) img=$WIN311IMG ;; win98) img=$WIN98IMG ;; esac
    [ -f "$img" ] || { echo "no $os image ($img)"; continue; }
    for s in $SAMPLES; do
        while [ "$(jobs -rp | wc -l)" -ge "$JOBS" ]; do wait -n; done
        run_one $os "$s" &
    done
    wait
    ls "$TOP/build/test/$os/"*.png > /dev/null 2>&1 &&
        magick montage $(for s in $SAMPLES; do f=$TOP/build/test/$os/$s.png; [ -f "$f" ] && echo "$f"; done) \
            -tile 4x -geometry 400x300+3+3 -label '%t' "$TOP/build/test/$os/index.png"
done
