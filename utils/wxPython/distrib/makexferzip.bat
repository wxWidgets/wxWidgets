

cd %WXWIN%\utils\wxPython
find . -name "*.py"  > filelist
find . -name "*.i"  >> filelist
find . -name "*.h"  >> filelist
find . -name "*.cpp"  >> filelist

cat filelist | zip -r -u -@ xfer.zip
