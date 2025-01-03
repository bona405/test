@echo off
call C:\Xilinx\SDK\2019.1\settings64.bat

for /f "tokens=1-3 delims=-" %%a in ('date /t') do (
    set year=%%a
    set month=%%b
    set day=%%c
)

set shortyear=%year:~-2%

REM set filename=image_%shortyear%%month%%day%.mfs
REM set filename=%filename: =%

REM 사용자로부터 입력 받기
set /p userinput=Enter your filename_tail input:

REM mfsgen 명령에 사용할 파일 이름 만들기
if not "%userinput%"=="" (
    set filename=image_%shortyear%%month%%day%_%userinput%.mfs
) else (
    set filename=image_%shortyear%%month%%day%.mfs
)

set filename=%filename: =%

cmd /c mfsgen -cvbf %filename% 2048 css web_images jquery js index.html

pause