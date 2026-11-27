# Run test_gui.exe groups on the invisible desktop and print one line per
# group. With no argument, runs the groups the host subsystems rely on.
param(
    [string[]]$Tag = @('[winui-root-input]', '[winui-window-scrollbar]', '[winui-cursor]',
                       '[winui-chrome-feedback]', '[HostState]', '[HostLifecycle]',
                       '[winui-v0-supported]', '[winui-appearance]', '[winui-input-state]',
                       '[winui-zorder]'),
    [string]$Bin = 'F:\wxb\reprise-dll\lib\vc_x64_dll',
    [string]$Out = "$env:TEMP\wxwinui-tests",
    [int]$TimeoutSeconds = 600
)

New-Item -ItemType Directory -Force $Out | Out-Null
$rundesk = Join-Path $PSScriptRoot 'rundesk.ps1'
foreach ($t in $Tag)
{
    $name = ($t -replace '[^A-Za-z0-9-]', '')
    $file = Join-Path $Out "$name.txt"
    $status = & $rundesk -CommandLine "`"$Bin\test_gui.exe`" `"$t`"" -WorkDir $Bin -OutFile $file -TimeoutSeconds $TimeoutSeconds
    $summary = if (Test-Path $file) { (Get-Content $file | Where-Object { $_ -match 'test cases?:|All tests passed|No test cases matched' } | Select-Object -Last 1) } else { '' }
    '{0,-28} {1,-14} {2}' -f $t, $status, $summary
}
"details: $Out"
