@echo off
del *.obj > nul
del *.exe > nul
wmake

md bin > nul
copy bmpio.exe .\bin\bmpio.exe
del *.exe
