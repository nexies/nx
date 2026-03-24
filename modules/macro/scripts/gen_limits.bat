@echo off
setlocal EnableExtensions EnableDelayedExpansion

REM defaults (как в bash)
set "LIMIT=1024"
set "OUT_DIR=include\nx\macro\detail\generated"

REM ---------------------------
REM parse args: --limit N, --dir PATH
REM ---------------------------
:PARSE
if "%~1"=="" goto :PARSE_DONE

if /I "%~1"=="--limit" (
  if "%~2"=="" goto :BAD_ARGS
  set "LIMIT=%~2"
  shift
  shift
  goto :PARSE
)

if /I "%~1"=="--dir" (
  if "%~2"=="" goto :BAD_ARGS
  set "OUT_DIR=%~2"
  shift
  shift
  goto :PARSE
)

echo Unknown parameter: %~1
echo Usage: %~nx0 --limit N [--dir path]
exit /b 1

:BAD_ARGS
echo Error: missing value for %~1
echo Usage: %~nx0 --limit N [--dir path]
exit /b 1

:PARSE_DONE

REM ---------------------------
REM validate LIMIT is integer (only digits)
REM ---------------------------
for /f "delims=0123456789" %%A in ("%LIMIT%") do (
  echo Error: NX_MODULE_MACRO_LIMITS_MAX_NUMBER has to be an integer number, not '%LIMIT%' 1>&2
  exit /b 1
)

REM make numeric
set /a N=%LIMIT% >nul 2>&1 || (
  echo Error: NX_MODULE_MACRO_LIMITS_MAX_NUMBER has to be an integer number, not '%LIMIT%' 1>&2
  exit /b 1
)

REM ---------------------------
REM validate power of two: n>0 and (n & (n-1))==0
REM ---------------------------
set /a NM1=N-1
set /a ANDVAL=N ^& NM1
if %N% LEQ 0 (
  echo Error: NX_MODULE_MACRO_LIMITS_MAX_NUMBER should be a power of two ^(2^n^), not %LIMIT% 1>&2
  exit /b 1
)
if NOT %ANDVAL%==0 (
  echo Error: NX_MODULE_MACRO_LIMITS_MAX_NUMBER should be a power of two ^(2^n^), not %LIMIT% 1>&2
  exit /b 1
)

REM LIMIT <= 64 => exit 0
if %N% LEQ 64 exit /b 0

REM ensure out dir exists
if not exist "%OUT_DIR%" mkdir "%OUT_DIR%" >nul 2>&1

REM generate_powers for each name (как в bash)
call :generate_powers while  %N%
call :generate_powers bool   %N%
call :generate_powers inc    %N%
call :generate_powers dec    %N%
call :generate_powers is_max %N%
call :generate_powers arg_tk %N%
call :generate_powers sequence %N%
call :generate_powers choose %N%

exit /b 0


REM ============================================================
REM generate_powers(name, limit): base=128; gen base..doubling..; add tail if needed
REM ============================================================
:generate_powers
set "NAME=%~1"
set /a LIM=%~2
set /a BASE=128

if %LIM% LEQ %BASE% (
  call :generate "%NAME%" %LIM%
  goto :eof
)

set /a CUR=%BASE%
:POW_LOOP
if %CUR% GTR %LIM% goto :POW_DONE
call :generate "%NAME%" %CUR%
set /a CUR=CUR*2
goto :POW_LOOP

:POW_DONE
set /a PREV=CUR/2
if NOT %PREV%==%LIM% (
  call :generate "%NAME%" %LIM%
)
goto :eof


REM ============================================================
REM generate(name, limit): writes OUT_DIR\name_limit.hpp
REM ============================================================
:generate
set "NAME=%~1"
set /a LIM=%~2
set "FILE=%OUT_DIR%\%NAME%_%LIM%.hpp"

REM uppercase NAME via PowerShell (проще и надёжно в cmd)
for /f "usebackq delims=" %%U in (`powershell -NoProfile -Command "$s='%NAME%'; $s.ToUpperInvariant()"`) do set "UNAME=%%U"

REM truncate file
break > "%FILE%"

call :generate_main_ifndef "%NAME%" "%UNAME%" %LIM% "%FILE%"
call :generate_private_include "%NAME%" "%FILE%"
call :generate_with_format "%NAME%" 0 %LIM% "%FILE%"
call :generate_limit_define "%UNAME%" %LIM% "%FILE%"

>>"%FILE%" echo(#endif
goto :eof


REM ============================================================
REM generate_with_format(name, min, max, file)
REM ============================================================
:generate_with_format
set "NAME=%~1"
set /a MIN=%~2
set /a MAX=%~3
set "FILE=%~4"

for /l %%I in (%MIN%,1,%MAX%) do (
  call :format_dispatch "%NAME%" %%I "%FILE%"
)
goto :eof


REM ============================================================
REM dispatch formatter
REM ============================================================
:format_dispatch
set "NAME=%~1"
set /a I=%~2
set "FILE=%~3"

if /I "%NAME%"=="while"   call :format_nx_while   %I% "%FILE%" & goto :eof
if /I "%NAME%"=="inc"     call :format_nx_inc     %I% "%FILE%" & goto :eof
if /I "%NAME%"=="dec"     call :format_nx_dec     %I% "%FILE%" & goto :eof
if /I "%NAME%"=="bool"    call :format_nx_bool    %I% "%FILE%" & goto :eof
if /I "%NAME%"=="is_max"  call :format_nx_is_max  %I% "%FILE%" & goto :eof
if /I "%NAME%"=="arg_tk"  call :format_nx_arg_tk  %I% "%FILE%" & goto :eof
if /I "%NAME%"=="sequence" call :format_nx_sequence %I% "%FILE%" & goto :eof
if /I "%NAME%"=="choose"  call :format_nx_choose  %I% "%FILE%" & goto :eof

goto :eof


REM ============================================================
REM header blocks
REM ============================================================
:generate_main_ifndef
set "NAME=%~1"
set "UNAME=%~2"
set /a LIM=%~3
set "FILE=%~4"

>>"%FILE%" echo(#ifndef NX_LIMITS_%UNAME%_%LIM%_HPP
>>"%FILE%" echo(#define NX_LIMITS_%UNAME%_%LIM%_HPP
>>"%FILE%" echo(
goto :eof

:generate_private_include
set "NAME=%~1"
set "FILE=%~2"

>>"%FILE%" echo(// This file is auto-generated
>>"%FILE%" echo(// You can set NX_MODULE_MACRO_LIMITS_MAX_NUMBER variable to your desired value in CMakeFile
>>"%FILE%" echo(
>>"%FILE%" echo(
>>"%FILE%" echo(#ifndef NX_MACRO_PRIVATE_INCLUDE
>>"%FILE%" echo(    #error "This file was not meant to be included directly. Include ^<nx/macro/detail/%NAME%_limit.hpp^> instead"
>>"%FILE%" echo(#endif
>>"%FILE%" echo(
goto :eof

:generate_limit_define
set "UNAME=%~1"
set /a LIM=%~2
set "FILE=%~3"

>>"%FILE%" echo(
>>"%FILE%" echo( # define NX_LIMITS_%UNAME% %LIM%
>>"%FILE%" echo(
goto :eof


REM ============================================================
REM formatters (полностью повторяют printf из bash, но с подстановкой чисел)
REM ============================================================
:format_nx_while
set /a I=%~1
set "FILE=%~2"
set /a N=I+1

>>"%FILE%" echo( # define _nx_while_%I%(c, o, r, ...) _nx_while_%I%_c(c(%N%, __VA_ARGS__), c, o, r, __VA_ARGS__)
>>"%FILE%" echo( # define _nx_while_%I%_c(p, c, o, r, ...) _nx_logic_if(p^)(_nx_while_%N%(c, o, r, o(%N%, __VA_ARGS__)^), r(%N%, __VA_ARGS__)^)
goto :eof

:format_nx_inc
set /a I=%~1
set "FILE=%~2"
set /a N=I+1
if %I% LSS 0 goto :eof
>>"%FILE%" echo( # define _nx_numeric_inc_%I% %N%
goto :eof

:format_nx_dec
set /a I=%~1
set "FILE=%~2"
set /a N=I-1
if %N% LSS 0 goto :eof
>>"%FILE%" echo( # define _nx_numeric_dec_%I% %N%
goto :eof

:format_nx_bool
set /a I=%~1
set "FILE=%~2"
if %I% LSS 0 goto :eof
set /a NVAL=0
if %I% GTR 0 set /a NVAL=1
>>"%FILE%" echo( # define _nx_logic_bool_%I% %NVAL%
goto :eof

:format_nx_is_max
set /a I=%~1
set "FILE=%~2"
set /a NVAL=0
if %I% GEQ %LIMIT% set /a NVAL=1
>>"%FILE%" echo( # define _nx_numeric_is_max_%I% %NVAL%
goto :eof

:format_nx_arg_tk
set /a I=%~1
set "FILE=%~2"

>>"%FILE%" echo( # define _nx_arg_tk_%I% _nx_arg_tk_%I%_(
>>"%FILE%" echo( # define _nx_arg_tk_%I%_(arg^) (%I%, arg^)
goto :eof

:format_nx_sequence
set /a I=%~1
set "FILE=%~2"
set /a N=I-1
set /a C=1
if %N% LEQ 1 set /a C=0

if %I%==0 (
  >>"%FILE%" echo( # define _nx_sequence_0^(...^)
) else (
  >>"%FILE%" echo( # define _nx_sequence_%I%(c, h, m, ...^) _nx_sequence_%N%(%C%, h, m, __VA_ARGS__^)  h(c, _nx_apply(m, %I%, __VA_ARGS__^)^)
)
goto :eof

:format_nx_choose
set /a I=%~1
set "FILE=%~2"
set /a N=I-1

if %I%==0 (
  >>"%FILE%" echo( # define _nx_choose_0(c, ...^) c
  goto :eof
)

>>"%FILE%" echo( # define _nx_choose_%I%(c, ...^) _nx_choose_%N%(__VA_ARGS__^)
goto :eof
