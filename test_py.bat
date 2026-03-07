@echo off & python -x "%~f0" %* & exit /b %errorlevel%
import sys
print('It works!')
