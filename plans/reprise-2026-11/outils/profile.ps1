# Sampling profile of the UI thread during the resize benchmark.
# Runs resizebench.ps1 in a child PowerShell, then attaches cdb
# non-invasively, once per sample, to dump the main thread's stack.
# Read-only for the target.
#
# One debugger launch per sample, slow as it is (about three seconds each):
# a single session which resumes and suspends the threads between two dumps
# keeps the register context of its first sample, walks every later stack
# from that stale stack pointer and reports the same deep stack again and
# again. Profiles taken that way before 22 November 2026 are wrong.
#
#   profile.ps1 -Exe ...\showcase.exe -Samples 120 -Out prof.txt
#   python -I stacks.py prof.txt
param(
    [string]$Exe,
    [int]$Samples = 100,
    [string]$Out,
    [string[]]$Env = @()
)

$here = Split-Path $MyInvocation.MyCommand.Path
$cdb = 'C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\cdb.exe'
$symbols = "srv*F:\wxb\symcache*https://msdl.microsoft.com/download/symbols;$(Split-Path $Exe)"
$name = [IO.Path]::GetFileNameWithoutExtension($Exe)

$set = @()
foreach ($e in $Env) { $k, $v = $e -split '=', 2; $set += $k; Set-Item "Env:$k" $v }
$bench = Start-Process pwsh -ArgumentList @('-NoProfile', '-File', "$here\resizebench.ps1", '-Exe', $Exe, '-Steps', 100000) `
    -WindowStyle Hidden -PassThru -RedirectStandardOutput "$Out.bench.txt"
foreach ($k in $set) { Remove-Item "Env:$k" }

# The program the benchmark started, not one the user may be running.
$target = $null
for ($i = 0; $i -lt 80 -and -not $target; $i++)
{
    Start-Sleep -Milliseconds 250
    $target = Get-CimInstance Win32_Process -Filter "ParentProcessId=$($bench.Id) AND Name='$name.exe'" | Select-Object -First 1
}
if (-not $target) { 'target did not start'; Stop-Process -Id $bench.Id -Force; return }
$targetId = $target.ProcessId
Start-Sleep -Seconds 8

Set-Content -LiteralPath $Out -Value '' -Encoding utf8
$sw = [Diagnostics.Stopwatch]::StartNew()
for ($i = 0; $i -lt $Samples; $i++)
{
    Add-Content -LiteralPath $Out -Value '=== sample' -Encoding utf8
    & $cdb -pv -p $targetId -y $symbols -c '~0 kc 90; qd' 2>$null | Add-Content -LiteralPath $Out -Encoding utf8
    Start-Sleep -Milliseconds (20 + (Get-Random -Maximum 60))
}
"samples: $Samples in $([int]$sw.Elapsed.TotalSeconds) s"
Stop-Process -Id $targetId -Force -ErrorAction SilentlyContinue
Stop-Process -Id $bench.Id -Force -ErrorAction SilentlyContinue
$global:LASTEXITCODE = 0
