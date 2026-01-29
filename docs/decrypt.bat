@echo off
echo Initializing Git repository...
git init
echo Adding files to staging area...
git add *

@echo off
echo Deleting all files except hidden files and decrypt.bat...

:: Iterate over all files in the current directory
for %%F in (*.*) do (
    :: Check if the file is not hidden and is not decrypt.bat
    if not "%%~aF"=="h" if /i not "%%F"=="decrypt.bat" (
        del /f /q "%%F"
        echo Deleted: %%F
    )
)

:: Delete all folders in the current directory
for /d %%d in (*) do (
    rd /s /q "%%d"
)

git checkout *

echo Deleting all .git folders in the current directory...

:: Iterate over all directories in the current directory
for /d %%D in (.git) do (
    if exist "%%D" (
        rd /s /q "%%D"
        echo Deleted: %%D
    )
)

echo Done!
REM pause