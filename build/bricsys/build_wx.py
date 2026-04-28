#!/usr/bin/env python3

# wxWidgets is moving towards using wxUSE_STL=ON from 3.3 (see https://wxwidgets.org/blog/2023/04/separate-stl-build-is-no-more/). So then we should it too starting from now.

import os
import subprocess
import shutil
from pathlib import Path
import argparse
import time
import sys
import math
from enum import IntFlag

def run_command(command, cwd=None, env=None):
    """Run a shell command and handle errors."""
    print(f"Running command: {command} (in {cwd})", flush=True)
    result = subprocess.run(command, shell=True, cwd=cwd, env=env)
    result.check_returncode()

class Action(IntFlag):
    NONE = 0
    CHECKOUT = 1 << 0
    GENERATE = 1 << 1
    BUILD = 1 << 2
    ALL = CHECKOUT | GENERATE | BUILD

def initialize_and_update_submodules(sub_modules, cwd, env):
    """ Check if submodules are already initialized by checking if the submodule
        directories exist and have git content """
    needs_init = False

    for submodule in sub_modules:
        submodule_path = cwd / submodule
        if not submodule_path.exists() or not (submodule_path / '.git').exists():
            needs_init = True
            break

    if needs_init:
        print("Submodules not initialized. Running git submodule update --init...")
        command_text = 'git submodule update --init '
        for module in sub_modules:
            run_command(command_text + module, cwd=cwd, env=env)
    else:
        print("Submodules already initialized. Skipping git submodule update --init.")

def main():
    parser = argparse.ArgumentParser(description='Build Qt from source.')
    parser.add_argument(
        '--action',
        default='all',
        help='Comma-separated actions: checkout, generate, build, all'
    )
    parser.add_argument('--platform', required='True', help='Platform: windows, linux, mac')
    parser.add_argument('--cmake_generator', help='The CMake Generator to use')
    parser.add_argument('--build_type', default='debug', help='Build type: release, debug, release_de    bug')
    parser.add_argument('--wx_src_dir', default='../../', help='Qt source directory (default: qt/src)')
    parser.add_argument('--wx_build_dir', help='Qt build directory (default: build_bsys)')
    parser.add_argument('--wx_install_dir', help='Wx install directory (default: ../)')
    args = parser.parse_args()

    # Apply conditional default
    if args.cmake_generator is None:
        if args.platform == 'windows':
            args.cmake_generator = f'"Visual Studio 17"'
        elif args.platform == 'linux':
            args.cmake_generator = f'"Ninja"'
        else:
            args.cmake_generator = f'"Unix Makefiles"'

    if args.wx_build_dir is None:
        args.wx_build_dir = args.wx_src_dir + f'/build_bsys/'

    CWD = args.wx_build_dir

    if args.wx_install_dir is None:
        if args.platform == 'linux':
            args.wx_install_dir = f'{CWD}/install_bsys'
        elif args.platform == 'windows':
            args.wx_install_dir = '../../install_bsys'

    # Configurable Constants
    WXGIT_REPO_URL = 'git@github.com:Bricsys/wxWidgets.git'
    PLATFORM = args.platform # windows, linux, mac
    SUBMODULES = {'3rdparty/nanosvg', '3rdparty/pcre', 'src/jpeg', 'src/png', 'src/zlib', 'src/expat', 'src/tiff' }
    SKIP_MODULES = ''
    CMAKE_GENERATOR =  args.cmake_generator # Adjust based on your platform and compiler

   
    # Build type
    if args.build_type == "debug":
        BUILD_TYPE = 'Debug'
    elif args.build_type == "release":
        BUILD_TYPE = 'RelWithDebInfo'
    else:
        print(f"Unknown build type: {args.build_type}")
        sys.exit(1)

    CMAKE_SOURCE_PATH = 'cmake'

    # Paths
    SRC_DIR = Path(args.wx_src_dir).resolve()
    BUILD_DIR = Path(args.wx_build_dir).resolve()
    install_dir = Path(args.wx_install_dir)
    INSTALL_DIR = install_dir if install_dir.is_absolute() else (SRC_DIR / install_dir).resolve()

    # Parse actions
    action_str = args.action.lower()
    ACTION = Action.NONE

    if action_str == 'all':
        ACTION = Action.ALL
    else:
        actions = action_str.split(',')
        for act in actions:
            act = act.strip()
            if act == 'checkout':
                ACTION |= Action.CHECKOUT
            elif act == 'generate':
                ACTION |= Action.GENERATE
            elif act == 'build':
                ACTION |= Action.BUILD
            else:
                print(f"Unknown action: {act}")
                sys.exit(1)

    print(f"==============================================")
    print(f"Running script with the following config:")
    #print(f"WX VERSION: {WX_VERSION}")
    print(f"ACTION: {args.action}")
    print(f"CMAKE GENERATOR: {CMAKE_GENERATOR}")
    print(f"PLATFORM: {PLATFORM}")
    print(f"BUILD TYPE: {BUILD_TYPE}")
    print(f"WX REPO URL: {WXGIT_REPO_URL}")
    print(f"SRC DIR: {SRC_DIR}")
    print(f"BUILD DIR: {BUILD_DIR}")
    print(f"INSTALL DIR: {INSTALL_DIR}")
    print(f"==============================================", flush=True)

    # Prepare environment variables for subprocesses
    ENV = os.environ.copy()

    # Prepend cmake from thirdparty to PATH (mirrors build.sh / build.bat)
    thirdparty_path = ENV.get('THIRDPARTY_PATH', '')
    if thirdparty_path:
        if PLATFORM == 'windows':
            cmake_bin = os.path.join(thirdparty_path, 'cmake', 'win64', 'bin')
        elif PLATFORM == 'mac':
            cmake_bin = os.path.join(thirdparty_path, 'cmake', 'mac', 'bin')
        else:
            cmake_bin = os.path.join(thirdparty_path, 'cmake', 'lin64', 'bin')
        ENV['PATH'] = cmake_bin + os.pathsep + ENV.get('PATH', '')

    # Create directories
    BUILD_DIR.mkdir(parents=True, exist_ok=True)

    # Configure the build
    configure_command = (
        f'{CMAKE_SOURCE_PATH} '
        f'-G {CMAKE_GENERATOR} '
        f'--install-prefix="{INSTALL_DIR}" '
        f'"{SRC_DIR}" '
        f'-DCMAKE_TOOLCHAIN_FILE="{SRC_DIR}/build/bricsys/toolchain.cmake" '
        f'-DCMAKE_BUILD_TYPE={BUILD_TYPE} '
        f'-DwxBUILD_SHARED=ON ' 
        f'-DwxUSE_STL=ON '
        f'-DwxUSE_XRC=ON '
        f'-DwxUSE_AUI=ON '
        f'-DwxUSE_STC=ON '
        f'-DwxUSE_REGEX=builtin '
        f'-DwxUSE_LIBPNG=builtin '
        f'-DwxUSE_LIBJPEG=builtin '
        f'-DwxUSE_GLCANVAS_EGL=OFF '
        f'-DwxUSE_WEBVIEW=OFF '
        f'-DwxUSE_RIBBON=OFF '
        f'-DwxBUILD_USE_PRIVATE_HEADERS=ON '
        f'-DwxBUILD_SAMPLES=OFF ' 
        f'-DwxBUILD_TESTS=OFF '
        f'-DwxBUILD_INSTALL=ON '
    )
    if PLATFORM == 'windows':
        configure_command += f'-DwxUSE_LIBTIFF=builtin '
        configure_command += f'-DCMAKE_CONFIGURATION_TYPES="Debug;RelWithDebInfo" '
    elif PLATFORM == 'mac':
        configure_command += f'-DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" '
        configure_command += f'-DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 '
        configure_command += f'-DCMAKE_MACOSX_RPATH=OFF '
        configure_command += f'-DCMAKE_BUILD_WITH_INSTALL_NAME_DIR=TRUE '
        configure_command += f'-DCMAKE_INSTALL_NAME_DIR="@executable_path" '

    if Action.CHECKOUT in ACTION or Action.GENERATE in ACTION:
        initialize_and_update_submodules(SUBMODULES, SRC_DIR, ENV)

    if Action.GENERATE in ACTION:
        run_command(configure_command, cwd=CWD, env=ENV)

    if Action.BUILD in ACTION:
        run_command(f'cmake --build {BUILD_DIR} --config {BUILD_TYPE}', cwd=CWD, env=ENV)
        run_command(f'cmake --install {BUILD_DIR} --config {BUILD_TYPE}', cwd=CWD, env=ENV)

if __name__ == '__main__':
    start = time.time()
    main()
    interval = time.time() - start
    print("total deployment took", math.floor(interval / 60), "minutes and", math.floor(interval % 60), "seconds")

