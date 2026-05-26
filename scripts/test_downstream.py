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
    current_awm_dir = os.getcwd()

    ignore_pats = shutil.ignore_patterns('.git', 'build', '__pycache__', 'valkey_src', 'Testing')

    # 1. Valkey Windows
    valkey_path, valkey_is_tmp = setup_repo('valkey-windows', 'https://github.com/SamuelMarks/valkey-windows.git')
    valkey_awm = os.path.join(valkey_path, 'auto-win-msvc')
    if os.path.exists(valkey_awm):
        shutil.rmtree(valkey_awm, ignore_errors=True)
    shutil.copytree(current_awm_dir, valkey_awm, ignore=ignore_pats, dirs_exist_ok=True)
    run_cmd(['cmd.exe', '/c', 'release.bat', '9.1.0', '--local-only'], cwd=valkey_path)

    # 2. Redis Windows
    redis_path, redis_is_tmp = setup_repo('redis-windows', 'https://github.com/SamuelMarks/redis-windows.git')
    redis_build_work = os.path.join(redis_path, 'build_work')
    os.makedirs(redis_build_work, exist_ok=True)
    redis_awm = os.path.join(redis_build_work, 'auto-win-msvc')
    if os.path.exists(redis_awm):
        shutil.rmtree(redis_awm, ignore_errors=True)
    shutil.copytree(current_awm_dir, redis_awm, ignore=ignore_pats, dirs_exist_ok=True)
    run_cmd(['cmd.exe', '/c', 'build-and-release.bat', '8.8-rc1', 'redis', '--local-only'], cwd=redis_path)

    # 3. Rsync
    rsync_path, rsync_is_tmp = setup_repo('rsync', 'https://github.com/SamuelMarks/rsync.git', branch='windows')
    if rsync_is_tmp:
        # Rsync CMakeLists.txt looks for ../auto-win-msvc
        rsync_parent = os.path.dirname(rsync_path)
        rsync_awm = os.path.join(rsync_parent, 'auto-win-msvc')
        shutil.copytree(current_awm_dir, rsync_awm, ignore=ignore_pats, dirs_exist_ok=True)

    # Configure and build rsync
    # We attempt to find vcpkg via VCPKG_INSTALLATION_ROOT or local vcpkg if present in parent.
    cmake_cmd = ['cmake', '-G', 'Visual Studio 17 2022', '-A', 'x64', '-B', 'build', '-S', '.']
    if 'VCPKG_INSTALLATION_ROOT' in os.environ:
        cmake_cmd.append(f"-DCMAKE_TOOLCHAIN_FILE={os.environ['VCPKG_INSTALLATION_ROOT']}/scripts/buildsystems/vcpkg.cmake")
    elif 'VCPKG_ROOT' in os.environ:
        cmake_cmd.append(f"-DCMAKE_TOOLCHAIN_FILE={os.environ['VCPKG_ROOT']}/scripts/buildsystems/vcpkg.cmake")
    run_cmd(cmake_cmd, cwd=rsync_path)
    run_cmd(['cmake', '--build', 'build', '--config', 'Release'], cwd=rsync_path)

    print("All downstream tests passed!")

if __name__ == '__main__':
    main()
