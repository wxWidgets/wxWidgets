
SET PATH=%PATH%;%THIRDPARTY_PATH%\cmake\win64\bin

:: Default build type
set BUILD_TYPE=release
set ACTION_TYPE=build

:: Parse command line arguments
:parse_args
if not "%~1" == "" set ACTION_TYPE=%~1
if not "%~2" == "" set BUILD_TYPE=%~2

if "%ACTION_TYPE%" == "generate" (
    python build/bricsys/build_wx.py --wx_src_dir=. --wx_install_dir=. --platform=windows --action=generate
)

if "%ACTION_TYPE%" == "build" (
    python build/bricsys/build_wx.py --wx_src_dir=. --wx_install_dir=. --platform=windows --action=build --build_type=%BUILD_TYPE%
)

