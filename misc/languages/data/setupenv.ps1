# Add tool paths to environment PATH variable

# It is necessary to add path of the Lua installation,
# because it is not properly set by the Chocolatey installation
$SYSPATH = [System.Environment]::GetEnvironmentVariable('PATH','machine')
$NEWPATH = "c:\ProgramData\chocolatey\lib\lua53\tools;$SYSPATH"
$env:path = $NEWPATH

# Define environment variable for the Lua shell
$env:luashell = "lua53"
echo "env:luashell is $env:luashell"
