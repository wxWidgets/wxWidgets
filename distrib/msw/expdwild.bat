@echo off
rem Expands wildcards in response file (arg 1) into output file (arg 2)
rem Note: requires ls.exe from GNU-WIN32 distribution, renamed to ls2.exe.
rem Correction: this is too slow, so we're using the built in 'dir'.

set newname=%temp\temp.tmp
sed -e "s/\//\\/g" %1 > %newname

set len=%@LINES[%newname]
rem set len=%@DEC[%len]
do i = 0 to %len by 1
  set line=%@LINE[%newname,%i]
  if NOT "%line" == "" dir /FB %line >> %2
enddo

