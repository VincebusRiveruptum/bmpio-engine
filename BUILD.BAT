@echo off
del .\BIN\*.obj > nul
del *.obj > nul
del .\BIN\*.exe > nul
wmake

md bin > nul
copy bmpio.exe .\BIN\bmpio.exe
del *.exe
del .\BIN\*.obj > nul
del *.obj > nul