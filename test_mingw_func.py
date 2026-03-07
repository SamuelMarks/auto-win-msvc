import os, subprocess, tempfile

def run_mingw(module_dir, build_dir):
    mingw_bin = r"C:\msys64\mingw64\bin"
    bat_file = os.path.join(build_dir, "run_mingw.bat")
    with open(bat_file, "w") as f:
        f.write(f'@echo off\n')
        f.write(f'set PATH={mingw_bin};%PATH%\n')
        f.write(f'cd /d "{os.path.abspath(module_dir)}"\n')
        f.write(f'cmake -B "{os.path.abspath(build_dir)}" -G "MinGW Makefiles"\n')
        f.write(f'if errorlevel 1 exit /b 1\n')
        f.write(f'cmake --build "{os.path.abspath(build_dir)}"\n')
        f.write(f'if errorlevel 1 exit /b 1\n')
        f.write(f'cd /d "{os.path.abspath(build_dir)}"\n')
        f.write(f'ctest -V\n')
        f.write(f'if errorlevel 1 exit /b 2\n')
        f.write(f'exit /b 0\n')
        
    res = subprocess.run(["cmd.exe", "/c", bat_file], capture_output=True, text=True)
    return res

with tempfile.TemporaryDirectory() as md:
    with open(os.path.join(md, 'CMakeLists.txt'), 'w') as f:
        f.write("cmake_minimum_required(VERSION 3.0)\nproject(dummy C)\n")
    bd = os.path.join(md, 'build')
    os.makedirs(bd, exist_ok=True)
    res = run_mingw(md, bd)
    print("Return code:", res.returncode)
    print("STDOUT:", res.stdout[-200:])
