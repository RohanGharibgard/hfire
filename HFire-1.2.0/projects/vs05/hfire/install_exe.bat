set BINDIR=c:\hfire\bin

if exist debug\HFire12d.exe copy debug\HFire12d.exe %BINDIR%
if exist release\HFire12.exe copy release\HFire12.exe %BINDIR%
if exist release\roth1972cmp.exe copy release\roth1972cmp.exe %BINDIR%
if exist release\calcros.exe copy release\calcros.exe %BINDIR%
