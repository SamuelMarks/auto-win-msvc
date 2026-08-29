import os
import subprocess
import sys
import shutil
import tempfile

def run_cmd(cmd, cwd=None, env=None):
    print(f"Running: {' '.join(cmd)} in {cwd or os.getcwd()}")
    result = subprocess.run(cmd, cwd=cwd, env=env)
    if result.returncode != 0:
        print(f"Command failed with exit code {result.returncode}")
        sys.exit(result.returncode)

def setup_repo(repo_name, github_url, branch=None):
    parent_dir = os.path.abspath(os.path.join(os.getcwd(), '..'))
    local_repo_path = os.path.join(parent_dir, repo_name)

    if os.path.exists(local_repo_path):
        print(f"Found local clone of {repo_name} at {local_repo_path}")
        return local_repo_path, False
    else:
        print(f"Local clone of {repo_name} not found. Cloning into a temporary directory...")
        # Create a parent temp directory so we can place siblings if needed
        parent_tmp = tempfile.mkdtemp(prefix=f"{repo_name}-parent-")
        tmp_dir = os.path.join(parent_tmp, repo_name)
        clone_cmd = ['git', 'clone', '--depth', '1']
        if branch:
            clone_cmd.extend(['--branch', branch])
        clone_cmd.extend([github_url, tmp_dir])
        run_cmd(clone_cmd)
        return tmp_dir, True

def main():
    for k in list(os.environ.keys()):
        if k.startswith('GIT_CONFIG_'):
            del os.environ[k]
    current_awm_dir = os.getcwd()

    ignore_pats = shutil.ignore_patterns('.git', 'build*', '__pycache__', 'valkey_src', 'Testing', '.vs', 'out')

    # Create a custom environment to prioritize Windows System32 over MSYS/Git bash bins
    # Also create a dummy timeout.bat to intercept timeout.exe calls, because Windows timeout.exe
    # fails with "Input redirection is not supported" inside CI/subprocess hooks.
    dummy_bin_dir = os.path.join(tempfile.gettempdir(), 'dummy_bin')
    os.makedirs(dummy_bin_dir, exist_ok=True)
    timeout_bat_path = os.path.join(dummy_bin_dir, 'timeout.bat')
    with open(timeout_bat_path, 'w') as f:
        f.write('@echo off\npython -c "import time; time.sleep(3)"\nexit /b 0\n')

    env = os.environ.copy()
    env['PATH'] = f"{dummy_bin_dir};C:\\Windows\\System32;{env.get('PATH', '')}"

    # 3. Rsync
    rsync_path, rsync_is_tmp = setup_repo('rsync', 'https://github.com/SamuelMarks/rsync.git', branch='windows')
    # Rsync CMakeLists.txt looks for ../auto-win-msvc
    rsync_parent = os.path.dirname(rsync_path)
    rsync_awm = os.path.join(rsync_parent, 'auto-win-msvc')
    if os.path.abspath(rsync_awm) != os.path.abspath(current_awm_dir):
        if os.path.exists(rsync_awm):
            shutil.rmtree(rsync_awm, ignore_errors=True)
        shutil.copytree(current_awm_dir, rsync_awm, ignore=ignore_pats, dirs_exist_ok=True)

    # Configure and build rsync
    with open(os.path.join(rsync_path, 'CMakeLists.txt'), 'a') as f:
        f.write('\nif(TARGET popt)\n  target_compile_definitions(popt PRIVATE HAVE_CONFIG_H=1)\nendif()\n')
        f.write('file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/rounding.h "#define EXTRA_ROUNDING 0\\n")\n')
        f.write('file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/git-version.h "#define RSYNC_GITOBJ \\"\\"\\n")\n')

    vcpkg_path = os.environ.get('VCPKG_INSTALLATION_ROOT') or os.environ.get('VCPKG_ROOT') or r'C:\Users\samue\repos\vcpkg'
    if not os.path.exists(vcpkg_path):
        print("Skipping rsync downstream test because VCPKG is not set in the environment.")
    else:
        cmake_cmd = ['cmake', '-G', 'Visual Studio 17 2022', '-A', 'x64', '-B', 'build', '-S', '.', '--fresh', '-DFETCHCONTENT_SOURCE_DIR_AUTO_WIN_MSVC=../auto-win-msvc']
        cmake_cmd.append(f"-DCMAKE_TOOLCHAIN_FILE={vcpkg_path}/scripts/buildsystems/vcpkg.cmake")
        run_cmd(cmake_cmd, cwd=rsync_path)
        run_cmd(['cmake', '--build', 'build', '--config', 'Release'], cwd=rsync_path)

    print("All downstream tests passed!")

if __name__ == '__main__':
    main()
