if not exist ".\Packages\Tanks Kombat" mkdir ".\Packages\Tanks Kombat"
xcopy ".\Release.win32\*.exe" ".\Packages\Tanks Kombat" /y /s
xcopy ".\Release.win32\*.dll" ".\Packages\Tanks Kombat" /y /s
xcopy ".\Release.win32\Resources" ".\Packages\Tanks Kombat\Resources" /y /s /i