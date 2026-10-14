@echo off
rem Tar up an external distribution of wxWindows: but
rem putting in separate ASCII and binary files
rem This seems to be the one that works, using
rem separate tar programs for conversion/non-conversion
rem of ASCII/binary files.

if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external wxWindows distribution:
echo   From   %1
echo   To     %2\wx168_1.tgz, %2\wx168_2.tgz, %2\wx168hlp.tgz, %2\wx168ps.tgz, %2\wx168htm.tgz, %2\wx168pdf.tgz
echo CTRL-C if this is not correct.
inkey /W4 `Press any key to continue...` %%input

erase %2\*.tgz
cd %1

rem First, expand the wildcards in the rsp files

rem Create empty list file
erase %1\distrib\*.lis
c:\bin\touch %1\distrib\wx168asc.lis
c:\bin\touch %1\distrib\wx168bin.lis
c:\bin\touch %1\distrib\wx168hlp.lis
c:\bin\touch %1\distrib\wx168ps.lis
c:\bin\touch %1\distrib\wx168htm.lis
c:\bin\touch %1\distrib\wx168pdf.lis

rem Create a .rsp file with backslashes instead
rem of forward slashes
rem No need if using ls2 (from UNIX95 distribution)
rem sed -e "s/\//\\/g" %1\distrib\wx_asc.rsp > %1\distrib\wx_asc.rs2

rem Do some renaming to get case right
call %1\distrib\lowrcase.bat %1\docs\pdf\dialoged.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\faq.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\hytext.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\ogl.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\prologio.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\tex2rtf.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\wx.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\wxbuild.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\wxchart.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\wxclips.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\wxgraph.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\wxgrid.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\wxhelp.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\wxhelp2.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\wxprop.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\wxtab.pdf
call %1\distrib\lowrcase.bat %1\docs\pdf\wxtree.pdf

call %1\distrib\expdwild.bat %1\distrib\wx_asc.rsp %1\distrib\wx168asc.lis
call %1\distrib\expdwild.bat %1\distrib\util_asc.rsp %1\distrib\wx168asc.lis
call %1\distrib\expdwild.bat %1\distrib\utl2_asc.rsp %1\distrib\wx168asc.lis
call %1\distrib\expdwild.bat %1\distrib\utl3_asc.rsp %1\distrib\wx168asc.lis
call %1\distrib\expdwild.bat %1\distrib\utl4_asc.rsp %1\distrib\wx168asc.lis
call %1\distrib\expdwild.bat %1\distrib\smpl_asc.rsp %1\distrib\wx168asc.lis
call %1\distrib\expdwild.bat %1\distrib\wxim1asc.rsp %1\distrib\wx168asc.lis
call %1\distrib\expdwild.bat %1\distrib\wxim2asc.rsp %1\distrib\wx168asc.lis

call %1\distrib\expdwild.bat %1\distrib\wx_bin.rsp %1\distrib\wx168bin.lis
call %1\distrib\expdwild.bat %1\distrib\util_bin.rsp %1\distrib\wx168bin.lis
call %1\distrib\expdwild.bat %1\distrib\utl2_bin.rsp %1\distrib\wx168bin.lis
call %1\distrib\expdwild.bat %1\distrib\utl3_bin.rsp %1\distrib\wx168bin.lis
call %1\distrib\expdwild.bat %1\distrib\utl4_bin.rsp %1\distrib\wx168bin.lis
call %1\distrib\expdwild.bat %1\distrib\smpl_bin.rsp %1\distrib\wx168bin.lis
call %1\distrib\expdwild.bat %1\distrib\wxim1bin.rsp %1\distrib\wx168bin.lis

rem Docs
call %1\distrib\expdwild.bat %1\distrib\wx_hlp.rsp %1\distrib\wx168hlp.lis
call %1\distrib\expdwild.bat %1\distrib\wx_ps.rsp %1\distrib\wx168ps.lis
call %1\distrib\expdwild.bat %1\distrib\wx_html.rsp %1\distrib\wx168htm.lis
call %1\distrib\expdwild.bat %1\distrib\wx_pdf.rsp %1\distrib\wx168pdf.lis

rem Do some further massaging of the .lis files
sed -e "s/\\/\//g" %1\distrib\wx168asc.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx168asc.lis

sed -e "s/\\/\//g" %1\distrib\wx168bin.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx168bin.lis

sed -e "s/\\/\//g" %1\distrib\wx168hlp.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx168hlp.lis

sed -e "s/\\/\//g" %1\distrib\wx168ps.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx168ps.lis

sed -e "s/\\/\//g" %1\distrib\wx168htm.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx168htm.lis

sed -e "s/\\/\//g" %1\distrib\wx168pdf.lis > c:\temp\temp.tmp
sed -e "s/D:\/wx\///g" c:\temp\temp.tmp > %1\distrib\wx168pdf.lis

rem 'tar' converts linefeeds.
tar -c -T %1\distrib\wx168asc.lis -f %2\wx168.tar
rem pause Press a key to continue.

rem This converts to lower case
ren %2\wx168.tar %2\wx168_1.tar
gzip32 %2\wx168_1.tar
ren %2\wx168_1.tar.gz %2\wx168_1.tgz

rem No linefeed conversion wanted
rem Note: GNU tar seems to crash with a full destination path, so
rem pander to it.
targnu -c -T %1\distrib\wx168bin.lis -f wx168_2.tar
move wx168_2.tar %2
gzip32 %2\wx168_2.tar
ren %2\wx168_2.tar.gz %2\wx168_2.tgz

targnu -c -T %1\distrib\wx168hlp.lis -f wx168_hlp.tar
move wx168_hlp.tar %2
gzip32 %2\wx168_hlp.tar
ren %2\wx168_hlp.tar.gz %2\wx168hlp.tgz

targnu -c -T %1\distrib\wx168pdf.lis -f wx168_pdf.tar
move wx168_pdf.tar %2
gzip32 %2\wx168_pdf.tar
ren %2\wx168_pdf.tar.gz %2\wx168pdf.tgz

tar -c -T %1\distrib\wx168ps.lis -f %2\wx168ps.tar
gzip32 %2\wx168ps.tar
ren %2\wx168ps.tar.gz %2\wx168ps.tgz

targnu -c -T %1\distrib\wx168htm.lis -f wx168htm.tar
move wx168htm.tar %2
gzip32 %2\wx168htm.tar
ren %2\wx168htm.tar.gz %2\wx168htm.tgz

cd %2
echo wxWindows archived.
goto end

:usage
echo Tar/gzip wxWindows distribution under DOS, making an ASCII and binary file
echo Usage: tardist2 source destination
echo e.g. tardist2 d:\wx d:\wx\deliver

:end


