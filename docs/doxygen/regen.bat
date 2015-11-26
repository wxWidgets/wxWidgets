@echo off

REM This bash script regenerates the HTML doxygen version of the
REM wxWidgets manual and adjusts the doxygen log to make it more
REM readable.

where /q doxygen
if %ERRORLEVEL% neq 0 (
    echo Error: Doxygen was not found in your PATH.
    exit /b 1
)

if not exist out (mkdir out)
if not exist out\html (mkdir out\html)
if not exist out\html\generic (mkdir out\html\generic)

REM These not automatically copied by Doxygen because they're not
REM used in doxygen documentation, only in our html footer and by our
REM custom aliases
copy images\generic\*.png out\html\generic 2>&1 >NUL

pushd ..\..
set WXWIDGETS=%CD%
popd

REM Defaults for settings controlled by this script
set GENERATE_DOCSET=NO
set GENERATE_HTML=NO
set GENERATE_HTMLHELP=NO
set GENERATE_LATEX=NO
set GENERATE_QHP=NO
set GENERATE_XML=NO
set SEARCHENGINE=NO
set SERVER_BASED_SEARCH=NO

IF "%1" == "all" (
  set GENERATE_HTML=YES
  set GENERATE_HTMLHELP=YES
  set GENERATE_XML=YES
) ELSE (
  IF "%1" == "chm" (
    set GENERATE_HTML=YES
    set GENERATE_HTMLHELP=YES
  ) ELSE (
    IF "%1" == "docset" (
      set GENERATE_DOCSET=YES
      set GENERATE_HTML=YES
    ) ELSE (
      IF "%1" == "latex" (
        set GENERATE_LATEX=YES
      ) ELSE (
        IF "%1" == "php" (
          set GENERATE_HTML=YES
          set SEARCHENGINE=YES
          set SERVER_BASED_SEARCH=YES
        ) ELSE (
          IF "%1" == "qch" (
            set GENERATE_HTML=YES
            set GENERATE_QHP=YES
          ) ELSE (
            IF "%1" == "xml" (
              set GENERATE_XML=YES
            ) ELSE (
              REM Default to HTML format.
              set GENERATE_HTML=YES
              set SEARCHENGINE=YES
            )
          )
        )
      )
    )
  )
)

REM Check for Graphviz (its location should be in %PATH%).
REM
REM NB: Always do this check because it looks like errorlevel is not set
REM     when the test is done inside the IF block.
dot <NUL >NUL 2>&1
IF %GENERATE_HTML%==YES (
    IF NOT %errorlevel%==0 (
        IF %errorlevel%==9009 (
            echo Error: dot was not found in PATH, please install Graphviz!
        ) ELSE (
            echo Error: %errorlevel% error code when running dot, please check Graphviz installation.
        )

        EXIT /B
    )
)

REM
REM NOW RUN DOXYGEN
REM
REM NB: we do this _after_ copying the required files to the output folders
REM     otherwise when generating the CHM file with Doxygen, those files are
REM     not included!
REM
set PATH=%PATH%;%HHC_PATH%
doxygen Doxyfile
if %errorlevel% neq 0 exit /b %errorlevel%

REM Check that class inheritance diagram images are present for html/chm docs.
REM
REM NB: Set the file to check outside the IF block,
REM     otherwise the second check does not always pick its value.
set filetofind=out\html\classwx_app_console__inherit__graph.png
IF %GENERATE_HTML%==YES (
    IF NOT EXIST %~dp0%filetofind% (
        echo Warning: Class inheritance diagram images are missing, please check Graphviz installation.
    )
)
