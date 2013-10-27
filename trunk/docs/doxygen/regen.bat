@echo off

REM This bash script regenerates the HTML doxygen version of the
REM wxWidgets manual and adjusts the doxygen log to make it more
REM readable.

mkdir out 2>&1 >NUL
mkdir out\html 2>&1 >NUL
mkdir out\html\generic 2>&1 >NUL

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

REM
REM NOW RUN DOXYGEN
REM
REM NB: we do this _after_ copying the required files to the output folders
REM     otherwise when generating the CHM file with Doxygen, those files are
REM     not included!
REM
set PATH=%PATH%;%HHC_PATH%
doxygen Doxyfile
