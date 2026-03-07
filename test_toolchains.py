import os, subprocess, tempfile, shutil

def test_generator(name, generator, env=None):
    with tempfile.TemporaryDirectory() as d:
        with open(os.path.join(d, 'CMakeLists.txt'), 'w') as f:
            f.write("cmake_minimum_required(VERSION 3.0)\nproject(dummy C)\n")
        try:
            res = subprocess.run(['cmake', '-G', generator, '.'], cwd=d, env=env, capture_output=True, text=True)
            if res.returncode == 0:
                print(f"{name} FOUND")
            else:
                print(f"{name} NOT FOUND (failed configure)")
                #print(res.stderr)
        except Exception as e:
            print(f"{name} ERROR {e}")

test_generator("MSVC 2005", "Visual Studio 8 2005")
test_generator("MSVC 2022", "Visual Studio 17 2022")
test_generator("MSVC 2026", "Visual Studio 18 2026") # Try a guess
test_generator("MinGW", "MinGW Makefiles")
test_generator("Cygwin", "Unix Makefiles")
