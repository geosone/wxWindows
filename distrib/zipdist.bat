@echo off
rem Zip up an external distribution of wxWindows
if "%1" == "" goto usage
if "%2" == "" goto usage
echo About to archive an external wxWindows distribution:
echo   From   %1
echo   To     %2\wx168_1-3.zip, %2\wx168ps.zip, %2\wx168hlp.zip, %2\wx168htm.zip
echo CTRL-C if this is not correct.
pause

erase %2\wx168*.zip

cd %1
echo Zipping...
zip -P %3 %4 %5 %6 %7 %8 %2\wx168_1.zip @%1\distrib\wx_asc.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_1.zip @%1\distrib\wx_bin.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_1.zip @%1\distrib\utl3_asc.rsp

zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_2.zip @%1\distrib\util_asc.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_2.zip @%1\distrib\util_bin.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_2.zip @%1\distrib\utl2_asc.rsp

zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_3.zip @%1\distrib\smpl_bin.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_3.zip @%1\distrib\smpl_asc.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_3.zip @%1\distrib\wxim1asc.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_3.zip @%1\distrib\wxim2asc.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_3.zip @%1\distrib\wxim1bin.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_3.zip @%1\distrib\utl2_bin.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_3.zip @%1\distrib\utl3_bin.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_3.zip @%1\distrib\utl4_asc.rsp
zip -a -P %3 %4 %5 %6 %7 %8 %2\wx168_3.zip @%1\distrib\utl4_bin.rsp

zip -P %3 %4 %5 %6 %7 %8 %2\wx168ps.zip @%1\distrib\wx_ps.rsp
zip -P %3 %4 %5 %6 %7 %8 %2\wx168hlp.zip @%1\distrib\wx_hlp.rsp
zip -P %3 %4 %5 %6 %7 %8 %2\wx168htm.zip @%1\distrib\wx_html.rsp
zip -P %3 %4 %5 %6 %7 %8 %2\wx168pdf.zip @%1\distrib\wx_pdf.rsp

cd %2
mkdir wx
cd wx
unzip32 -o -L ..\wx168_1.zip
unzip32 -o -L ..\wx168_2.zip
unzip32 -o -L ..\wx168_3.zip
unzip32 -o -L ..\wx168hlp.zip
erase docs\winhelp\wxclips.hlp docs\winhelp\wxclips.cnt

cd %2

echo wxWindows archived.
goto end

:usage
echo DOS wxWindows distribution.
echo Usage: zipdist source destination
echo e.g. zipdist c:\wx b:

:end
