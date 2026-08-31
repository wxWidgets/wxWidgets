# Collect a bounded diagnostic snapshot from the running WinUI samples.
#
# The script never enables instrumentation in an already-running process.
# Prefer run-instrumented.ps1: its PID/path/hash manifest lets this collector
# select exactly one process and append to exactly one diagnostic run.

[CmdletBinding()]
param(
    [string] $BuildDir = (Join-Path $PSScriptRoot '..\..\build-winui-clean'),
    [string] $BinaryDir,
    [string] $OutputDir,
    [string] $Debugger,
    [int] $ProcessId
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function Resolve-FullPath
{
    param(
        [Parameter(Mandatory = $true)][string] $Path,
        [string] $Base = (Get-Location).Path
    )

    if ([IO.Path]::IsPathRooted($Path))
    {
        return [IO.Path]::GetFullPath($Path)
    }
    return [IO.Path]::GetFullPath((Join-Path $Base $Path))
}

function Test-PathUnder
{
    param(
        [Parameter(Mandatory = $true)][string] $Path,
        [Parameter(Mandatory = $true)][string] $Directory
    )

    $resolvedPath = Resolve-FullPath $Path
    $resolvedDirectory = Resolve-FullPath $Directory
    $resolvedDirectory =
        $resolvedDirectory.TrimEnd([IO.Path]::DirectorySeparatorChar) +
            [IO.Path]::DirectorySeparatorChar
    return $resolvedPath.StartsWith(
        $resolvedDirectory, [StringComparison]::OrdinalIgnoreCase)
}

function Enter-OutputDirectoryLock
{
    param(
        [Parameter(Mandatory = $true)][string] $Directory,
        [Parameter(Mandatory = $true)][string] $Owner
    )

    $path = Join-Path $Directory ".wxwinui-$Owner.lock"
    try
    {
        $stream = [IO.File]::Open(
            $path,
            [IO.FileMode]::CreateNew,
            [IO.FileAccess]::ReadWrite,
            [IO.FileShare]::None)
    }
    catch [IO.IOException]
    {
        throw "Output directory is already locked for ${Owner}: $Directory"
    }

    try
    {
        $identity =
            "pid=$PID`nstarted=$([DateTimeOffset]::Now.ToString('o'))`n"
        $bytes = [Text.Encoding]::UTF8.GetBytes($identity)
        $stream.Write($bytes, 0, $bytes.Length)
        $stream.Flush($true)
    }
    catch
    {
        $stream.Dispose()
        Remove-Item -LiteralPath $path -Force -ErrorAction SilentlyContinue
        throw
    }

    return [PSCustomObject]@{
        Path = $path
        Stream = $stream
    }
}

function Exit-OutputDirectoryLock
{
    param([Parameter(Mandatory = $true)] $Lock)

    $Lock.Stream.Dispose()
    Remove-Item -LiteralPath $Lock.Path -Force
}

function ConvertTo-NativeArgument
{
    param([AllowEmptyString()][string] $Value)

    if ($Value.Length -gt 0 -and $Value -notmatch '[\s"]')
    {
        return $Value
    }

    $builder = [Text.StringBuilder]::new()
    [void]$builder.Append('"')
    $backslashes = 0
    foreach ($character in $Value.ToCharArray())
    {
        if ($character -eq [char]92)
        {
            ++$backslashes
            continue
        }

        if ($character -eq '"')
        {
            [void]$builder.Append([char]92, 2 * $backslashes + 1)
            [void]$builder.Append('"')
            $backslashes = 0
            continue
        }

        if ($backslashes)
        {
            [void]$builder.Append([char]92, $backslashes)
            $backslashes = 0
        }
        [void]$builder.Append($character)
    }

    if ($backslashes)
    {
        [void]$builder.Append([char]92, 2 * $backslashes)
    }
    [void]$builder.Append('"')
    return $builder.ToString()
}

function Stop-ExactProcess
{
    param(
        [Parameter(Mandatory = $true)]
        [Diagnostics.Process] $Process,
        [int] $Attempts = 3,
        [int] $WaitMilliseconds = 2000
    )

    for ($attempt = 1; $attempt -le $Attempts; ++$attempt)
    {
        try
        {
            $Process.Refresh()
            if ($Process.HasExited)
            {
                return $true
            }
            $Process.Kill()
        }
        catch
        {
            try
            {
                $Process.Refresh()
                if ($Process.HasExited)
                {
                    return $true
                }
            }
            catch
            {
            }
        }

        try
        {
            if ($Process.WaitForExit($WaitMilliseconds))
            {
                return $true
            }
        }
        catch
        {
        }
    }

    try
    {
        $Process.Refresh()
        return $Process.HasExited
    }
    catch
    {
        return $false
    }
}

function Resolve-Cdb
{
    param([string] $Requested)

    if ($Requested)
    {
        $command = Get-Command -Name $Requested -ErrorAction SilentlyContinue
        if ($command)
        {
            return $command.Source
        }

        $candidate = Resolve-FullPath $Requested
        if (Test-Path -LiteralPath $candidate -PathType Leaf)
        {
            return $candidate
        }

        throw "Debugger not found: $Requested"
    }

    $command = Get-Command -Name 'cdb.exe' -ErrorAction SilentlyContinue
    if ($command)
    {
        return $command.Source
    }

    foreach ($root in @(${env:ProgramFiles(x86)}, $env:ProgramFiles) |
             Where-Object { $_ })
    {
        $candidate = Join-Path $root 'Windows Kits\10\Debuggers\x64\cdb.exe'
        if (Test-Path -LiteralPath $candidate -PathType Leaf)
        {
            return $candidate
        }
    }

    return $null
}

function Write-Report
{
    param([Parameter(Mandatory = $true)][string] $Text)

    Add-Content -LiteralPath $script:ReportPath -Value $Text -Encoding UTF8
}

function Test-DiagnosticLogName
{
    param([Parameter(Mandatory = $true)][string] $Name)

    return $Name -eq 'input-router.txt' -or
           $Name -eq 'spike-ticks.txt' -or
           $Name -eq 'winui-debug.txt' -or
           $Name -like 'input-log-*.txt'
}

function Copy-DiagnosticFile
{
    param([Parameter(Mandatory = $true)][string] $Path)

    if (-not (Test-Path -LiteralPath $Path -PathType Leaf))
    {
        return
    }

    $source = (Resolve-Path -LiteralPath $Path).Path
    if ($script:CopiedFiles.ContainsKey($source))
    {
        return
    }

    $destination =
        Join-Path $script:ResolvedOutputDir ([IO.Path]::GetFileName($source))
    if ([string]::Equals(
            $source, $destination, [StringComparison]::OrdinalIgnoreCase))
    {
        $script:CopiedFiles[$source] = $true
        return
    }

    if (Test-Path -LiteralPath $destination)
    {
        $base = [IO.Path]::GetFileNameWithoutExtension($destination)
        $extension = [IO.Path]::GetExtension($destination)
        $suffix = 2
        do
        {
            $destination =
                Join-Path $script:ResolvedOutputDir "$base-$suffix$extension"
            ++$suffix
        }
        while (Test-Path -LiteralPath $destination)
    }

    Copy-Item -LiteralPath $source -Destination $destination
    $script:CopiedFiles[$source] = $true
    Write-Report "copied=$source -> $([IO.Path]::GetFileName($destination))"
}

function Get-ProcessPath
{
    param([Parameter(Mandatory = $true)] $Process)

    try
    {
        return $Process.Path
    }
    catch
    {
        return $null
    }
}

function Test-ExactProcessIdentity
{
    param(
        [Parameter(Mandatory = $true)] $Process,
        [Parameter(Mandatory = $true)][string] $ExpectedPath,
        [Parameter(Mandatory = $true)][long] $ExpectedStartUtcTicks
    )

    try
    {
        $Process.Refresh()
        if ($Process.HasExited)
        {
            return $false
        }

        $currentPath = Get-ProcessPath $Process
        $currentStart = [DateTimeOffset]($Process.StartTime)
        return $currentPath -and
            [string]::Equals(
                (Resolve-FullPath $currentPath),
                (Resolve-FullPath $ExpectedPath),
                [StringComparison]::OrdinalIgnoreCase) -and
            $currentStart.UtcDateTime.Ticks -eq $ExpectedStartUtcTicks
    }
    catch
    {
        return $false
    }
}

$repoRoot = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$resolvedBuildDir =
    Resolve-FullPath -Path $BuildDir -Base $repoRoot
if (-not (Test-Path -LiteralPath $resolvedBuildDir -PathType Container))
{
    throw "Build directory not found: $resolvedBuildDir"
}

$binaryDir =
    if (-not [string]::IsNullOrWhiteSpace($BinaryDir))
    {
        Resolve-FullPath -Path $BinaryDir -Base $resolvedBuildDir
    }
    else
    {
        Resolve-FullPath (
            Join-Path $resolvedBuildDir 'lib\vc_x64_dll')
    }
if (-not (Test-Path -LiteralPath $binaryDir -PathType Container))
{
    throw "Binary directory not found: $binaryDir"
}
if (-not (Test-PathUnder -Path $binaryDir -Directory $resolvedBuildDir))
{
    throw "Binary directory must belong to this build: $resolvedBuildDir"
}
$runsRoot = Join-Path $resolvedBuildDir 'logs\winui-freeze'
$outputWasExplicit =
    -not [string]::IsNullOrWhiteSpace($OutputDir)

$activeRunList = [Collections.Generic.List[object]]::new()
$invalidLiveManifests = [Collections.Generic.List[string]]::new()
$manifestFiles = @(
    Get-ChildItem -LiteralPath $runsRoot -Filter 'active-run-*.json' -File `
        -ErrorAction SilentlyContinue |
        Sort-Object LastWriteTimeUtc -Descending
)
foreach ($manifestFile in $manifestFiles)
{
    if ($manifestFile.BaseName -notmatch '^active-run-(\d+)$')
    {
        $invalidLiveManifests.Add(
            "$($manifestFile.Name): invalid active-run filename")
        continue
    }

    $manifestProcessId = [int]$Matches[1]
    $process = Get-Process -Id $manifestProcessId `
        -ErrorAction SilentlyContinue
    if (-not $process)
    {
        Write-Warning "Ignoring stale active-run manifest '$($manifestFile.FullName)': process $manifestProcessId is no longer running"
        continue
    }

    try
    {
        $manifest =
            Get-Content -LiteralPath $manifestFile.FullName -Raw |
                ConvertFrom-Json
        # Schema 2 adds runner outcome/watchdog fields but preserves every
        # identity field consumed below. Accept schema 1 snapshots produced by
        # the earlier runner as well as the current schema 2.
        if ([int]$manifest.SchemaVersion -notin @(1, 2))
        {
            throw "unsupported schema version '$($manifest.SchemaVersion)'"
        }
        if ($manifestProcessId -ne [int]$manifest.ProcessId)
        {
            throw 'manifest filename/PID mismatch'
        }

        $processPath = Get-ProcessPath $process
        if (-not $processPath -or
            -not [string]::Equals(
                (Resolve-FullPath $processPath),
                (Resolve-FullPath $manifest.Executable),
                [StringComparison]::OrdinalIgnoreCase) -or
            -not [string]::Equals(
                (Resolve-FullPath $manifest.BuildDir),
                $resolvedBuildDir,
                [StringComparison]::OrdinalIgnoreCase) -or
            -not (Test-PathUnder `
                -Path $processPath -Directory $binaryDir) -or
            -not (Test-Path -LiteralPath $manifest.OutputDir `
                -PathType Container))
        {
            throw 'process path, build directory, or output directory mismatch'
        }

        $manifestStarted = [DateTimeOffset]::Parse(
            [string]$manifest.StartedAt,
            [Globalization.CultureInfo]::InvariantCulture)
        $processStarted = [DateTimeOffset]($process.StartTime)
        if ($processStarted.UtcDateTime.Ticks -ne
                $manifestStarted.UtcDateTime.Ticks)
        {
            throw 'process start time does not match the manifest'
        }

        $currentHash = (Get-FileHash -LiteralPath $processPath `
            -Algorithm SHA256).Hash.ToLowerInvariant()
        if (-not [string]::Equals(
                $currentHash,
                [string]$manifest.ExecutableSha256,
                [StringComparison]::OrdinalIgnoreCase))
        {
            throw 'executable SHA-256 does not match the manifest'
        }

        $activeRunList.Add([PSCustomObject]@{
            Manifest = $manifest
            ManifestPath = $manifestFile.FullName
            Process = $process
            ProcessPath = Resolve-FullPath $processPath
            StartTimeUtcTicks = $manifestStarted.UtcDateTime.Ticks
            ExecutableSha256 = $currentHash
            OutputDir = Resolve-FullPath $manifest.OutputDir
        })
    }
    catch
    {
        $invalidLiveManifests.Add(
            "$($manifestFile.Name): $($_.Exception.Message)")
        Write-Warning "Ignoring invalid active-run manifest '$($manifestFile.FullName)': $($_.Exception.Message)"
    }
}
$activeRuns = @($activeRunList)
if ($invalidLiveManifests.Count)
{
    throw "A live process has an invalid active-run manifest; refusing fallback collection: $($invalidLiveManifests -join '; ')"
}

$activeRun = $null
if ([string]::IsNullOrWhiteSpace($OutputDir))
{
    if ($activeRuns.Count -eq 1)
    {
        $activeRun = $activeRuns[0]
        $OutputDir = $activeRun.OutputDir
    }
    elseif ($activeRuns.Count -gt 1)
    {
        throw 'Multiple instrumented runs are active; pass -OutputDir for one run.'
    }
    else
    {
        $snapshotId =
            (Get-Date -Format 'yyyyMMdd-HHmmss-fff') + '-' +
            [guid]::NewGuid().ToString('N').Substring(0, 8)
        $OutputDir = Join-Path $runsRoot $snapshotId
    }
}

$script:ResolvedOutputDir =
    Resolve-FullPath -Path $OutputDir -Base $resolvedBuildDir
if (-not $activeRun)
{
    $matchingRuns = @(
        $activeRuns |
            Where-Object {
                [string]::Equals(
                    $_.OutputDir,
                    $script:ResolvedOutputDir,
                    [StringComparison]::OrdinalIgnoreCase)
            }
    )
    if ($activeRuns.Count -and $outputWasExplicit)
    {
        if ($matchingRuns.Count -ne 1)
        {
            throw 'An instrumented run is active, but -OutputDir does not select exactly that run.'
        }
        $activeRun = $matchingRuns[0]
    }
    elseif ($matchingRuns.Count -eq 1)
    {
        $activeRun = $matchingRuns[0]
    }
}

New-Item -ItemType Directory -Path $script:ResolvedOutputDir -Force |
    Out-Null
$outputLock =
    Enter-OutputDirectoryLock `
        -Directory $script:ResolvedOutputDir -Owner 'collector'
$scriptExitCode = 0
try
{
$snapshotId =
    (Get-Date -Format 'yyyyMMdd-HHmmss-fff') + '-' +
    [guid]::NewGuid().ToString('N').Substring(0, 8)
$script:ReportPath =
    Join-Path $script:ResolvedOutputDir "freeze-summary-$snapshotId.txt"
$script:CopiedFiles =
    [Collections.Generic.Dictionary[string, bool]]::new(
        [StringComparer]::OrdinalIgnoreCase)
$stackFailures = [Collections.Generic.List[string]]::new()
$matchedProcessCount = 0
$stackAttemptCount = 0
$verifiedStackCount = 0
$debuggerCleanupFailed = $false

if (-not ('WxWinUIDiagnostics.NativeMethods' -as [type]))
{
    Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;

namespace WxWinUIDiagnostics
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Rect
    {
        public int Left;
        public int Top;
        public int Right;
        public int Bottom;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct GuiThreadInfo
    {
        public uint Size;
        public uint Flags;
        public IntPtr Active;
        public IntPtr Focus;
        public IntPtr Capture;
        public IntPtr MenuOwner;
        public IntPtr MoveSize;
        public IntPtr Caret;
        public Rect CaretRect;
    }

    public static class NativeMethods
    {
        [DllImport("user32.dll")]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool GetGUIThreadInfo(
            uint threadId,
            ref GuiThreadInfo info);

        [DllImport("user32.dll")]
        public static extern IntPtr GetForegroundWindow();
    }
}
'@
}

$cdb = Resolve-Cdb $Debugger
$symbolCache = Join-Path $script:ResolvedOutputDir 'symbols'
$symbolPath =
    "srv*$symbolCache*https://msdl.microsoft.com/download/symbols;$binaryDir"

@(
    "=== wxWinUI freeze snapshot $snapshotId ==="
    "build=$resolvedBuildDir"
    "output=$script:ResolvedOutputDir"
    "activeRun=$(if ($activeRun) { $activeRun.Manifest.RunId } else { '<none>' })"
    "activePid=$(if ($activeRun) { $activeRun.Process.Id } else { '<none>' })"
    "debugger=$(if ($cdb) { $cdb } else { '<not found>' })"
    ("foreground=0x{0:X}" -f
        [WxWinUIDiagnostics.NativeMethods]::GetForegroundWindow().ToInt64())
) | Set-Content -LiteralPath $script:ReportPath -Encoding UTF8

if ($activeRun)
{
    $manifestSnapshot =
        Join-Path $script:ResolvedOutputDir (
            "active-run-$($activeRun.Process.Id)-$snapshotId.json")
    Copy-Item -LiteralPath $activeRun.ManifestPath `
        -Destination $manifestSnapshot
}
elseif ($outputWasExplicit)
{
    Write-Report 'manifest=none (manual output directory)'
}

if ($activeRun)
{
    if ($ProcessId -and $activeRun.Process.Id -ne $ProcessId)
    {
        throw "The selected active run belongs to PID $($activeRun.Process.Id), not -ProcessId $ProcessId."
    }
    $processes = @($activeRun.Process)
}
else
{
    $processes = @(
        foreach ($name in @('widgets', 'showcase', 'winuispike', 'dataview'))
        {
            Get-Process -Name $name -ErrorAction SilentlyContinue
        }
    )
    if ($ProcessId)
    {
        $processes = @(
            $processes |
                Where-Object { $_.Id -eq $ProcessId }
        )
    }
    elseif ($processes.Count -gt 1)
    {
        $candidates =
            ($processes |
                ForEach-Object { "$($_.ProcessName):$($_.Id)" }) -join ', '
        throw "Multiple matching processes are running ($candidates); pass -ProcessId."
    }
}

foreach ($process in $processes)
{
    $processPath = Get-ProcessPath $process
    if (-not $processPath -or
        -not (Test-PathUnder -Path $processPath -Directory $binaryDir))
    {
        continue
    }

    if ($activeRun -and
        -not [string]::Equals(
            (Resolve-FullPath $processPath),
            $activeRun.ProcessPath,
            [StringComparison]::OrdinalIgnoreCase))
    {
        continue
    }
    $expectedStartUtcTicks =
        if ($activeRun)
        {
            [long]$activeRun.StartTimeUtcTicks
        }
        else
        {
            ([DateTimeOffset]($process.StartTime)).UtcDateTime.Ticks
        }

    $tag = "$($process.ProcessName)-$($process.Id)"
    $hash = (Get-FileHash -LiteralPath $processPath `
        -Algorithm SHA256).Hash.ToLowerInvariant()
    Write-Report "--- $tag ---"
    Write-Report "path=$processPath"
    Write-Report "sha256=$hash"
    ++$matchedProcessCount

    $debuggerProcess = $null
    try
    {
        foreach ($thread in $process.Threads)
        {
            $info = [WxWinUIDiagnostics.GuiThreadInfo]::new()
            $info.Size =
                [Runtime.InteropServices.Marshal]::SizeOf(
                    [type][WxWinUIDiagnostics.GuiThreadInfo])

            if ([WxWinUIDiagnostics.NativeMethods]::GetGUIThreadInfo(
                    [uint32]$thread.Id, [ref]$info) -and
                ($info.Active -ne [IntPtr]::Zero -or
                 $info.Focus -ne [IntPtr]::Zero -or
                 $info.Capture -ne [IntPtr]::Zero -or
                 $info.MoveSize -ne [IntPtr]::Zero))
            {
                Write-Report ((
                    'tid={0} active=0x{1:X} focus=0x{2:X} ' +
                    'capture=0x{3:X} moveSize=0x{4:X} menu=0x{5:X}') -f
                    $thread.Id,
                    $info.Active.ToInt64(),
                    $info.Focus.ToInt64(),
                    $info.Capture.ToInt64(),
                    $info.MoveSize.ToInt64(),
                    $info.MenuOwner.ToInt64())
            }
        }
    }
    catch
    {
        Write-Report "GUI info error for ${tag}: $($_.Exception.Message)"
    }

    if (-not $cdb)
    {
        Write-Report 'stacks=skipped (cdb.exe not found; pass -Debugger)'
        continue
    }

    $stackName = "stacks-$tag-$snapshotId.txt"
    $stackPath = Join-Path $script:ResolvedOutputDir $stackName
    $errorPath = Join-Path $script:ResolvedOutputDir (
        "stacks-$tag-$snapshotId.stderr.txt")
    $debuggerArguments =
        (@(
            '-pv',
            '-p',
            $process.Id.ToString(),
            '-lines',
            '-y',
            $symbolPath,
            '-c',
            '.symopt+0x100; .echo WXSTACK_BEGIN; ~*k 40; .echo WXSTACK_END; q'
        ) | ForEach-Object { ConvertTo-NativeArgument $_ }) -join ' '

    try
    {
        if (-not (Test-ExactProcessIdentity `
                -Process $process `
                -ExpectedPath $processPath `
                -ExpectedStartUtcTicks $expectedStartUtcTicks))
        {
            $message =
                "$tag identity changed before debugger attach; refusing PID-only attach"
            Write-Report "stacks=identity-mismatch target=$tag"
            $stackFailures.Add($message)
            break
        }

        ++$stackAttemptCount
        $debuggerProcess = Start-Process `
            -FilePath $cdb `
            -ArgumentList $debuggerArguments `
            -NoNewWindow `
            -PassThru `
            -RedirectStandardOutput $stackPath `
            -RedirectStandardError $errorPath

        if (-not $debuggerProcess.WaitForExit(30000))
        {
            if (-not (Stop-ExactProcess -Process $debuggerProcess))
            {
                $debuggerCleanupFailed = $true
                Write-Report (
                    "stacks=cleanup-failed debuggerPid=" +
                    "$($debuggerProcess.Id) target=$tag")
                $stackFailures.Add(
                    "$tag left debugger PID $($debuggerProcess.Id) alive")
                break
            }

            Write-Report (
                "stacks=timeout after 30s for $tag; debugger exited")
            $stackFailures.Add("$tag timed out")
        }
        else
        {
            $stackText =
                if (Test-Path -LiteralPath $stackPath)
                {
                    Get-Content -LiteralPath $stackPath -Raw
                }
                else
                {
                    ''
                }
            $stackSection = [regex]::Match(
                $stackText,
                '(?ms)^WXSTACK_BEGIN\s*\r?\n(?<body>.*?)^WXSTACK_END\s*\r?$')
            $hasStack =
                $debuggerProcess.ExitCode -eq 0 -and
                $stackSection.Success -and
                $stackSection.Groups['body'].Value -match
                    '(?m)^\s*#?\s*Child-SP\s+RetAddr\s+Call Site\s*\r?$' -and
                $stackSection.Groups['body'].Value -match
                    '(?m)^\s*[0-9a-f`]{8,}\s+[0-9a-f`]{8,}\s+\S+' -and
                $stackText -notmatch '(?i)cdb:\s+.*(?:error|does not support)'
            if ($hasStack)
            {
                ++$verifiedStackCount
                Write-Report "stacks=$stackName exit=0 verified=true"
            }
            else
            {
                Write-Report "stacks=$stackName exit=$($debuggerProcess.ExitCode) verified=false"
                $stackFailures.Add("$tag produced no verified stack")
            }
        }
    }
    catch
    {
        Write-Report "stack capture error for ${tag}: $($_.Exception.Message)"
        $stackFailures.Add("$tag failed: $($_.Exception.Message)")
    }
    finally
    {
        if ($debuggerProcess)
        {
            if (-not $debuggerCleanupFailed)
            {
                $debuggerStillRunning = $false
                try
                {
                    $debuggerProcess.Refresh()
                    $debuggerStillRunning = -not $debuggerProcess.HasExited
                }
                catch
                {
                    $debuggerStillRunning = $true
                }

                if ($debuggerStillRunning -and
                    -not (Stop-ExactProcess -Process $debuggerProcess))
                {
                    $debuggerCleanupFailed = $true
                    Write-Report (
                        "stacks=cleanup-failed debuggerPid=" +
                        "$($debuggerProcess.Id) target=$tag")
                    $stackFailures.Add(
                        "$tag left debugger PID $($debuggerProcess.Id) alive")
                }
            }
            $debuggerProcess.Dispose()
        }
    }
    if ($debuggerCleanupFailed)
    {
        break
    }
}

$candidateLogs = [Collections.Generic.List[string]]::new()
$logDirectories =
    if ($activeRun)
    {
        @($script:ResolvedOutputDir)
    }
    else
    {
        @($resolvedBuildDir, $binaryDir, $script:ResolvedOutputDir)
        Write-Report 'logs=manual compatibility scan (build/output only)'
    }
foreach ($directory in $logDirectories)
{
    if (Test-Path -LiteralPath $directory -PathType Container)
    {
        Get-ChildItem -LiteralPath $directory -Filter '*.txt' -File `
            -ErrorAction SilentlyContinue |
            Where-Object { Test-DiagnosticLogName $_.Name } |
            ForEach-Object { $candidateLogs.Add($_.FullName) }
    }
}

if (-not $activeRun)
{
    foreach ($variableName in @(
        'WX_WINUI_INPUT_LOG',
        'WX_WINUI_DIAGNOSTICS_DIR'))
    {
        $value = [Environment]::GetEnvironmentVariable($variableName)
        if (-not $value)
        {
            continue
        }

        if (Test-Path -LiteralPath $value -PathType Leaf)
        {
            $candidateLogs.Add($value)
        }
        elseif (Test-Path -LiteralPath $value -PathType Container)
        {
            Get-ChildItem -LiteralPath $value -Filter '*.txt' -File `
                -ErrorAction SilentlyContinue |
                Where-Object { Test-DiagnosticLogName $_.Name } |
                ForEach-Object { $candidateLogs.Add($_.FullName) }
        }
    }
}

foreach ($path in $candidateLogs)
{
    Copy-DiagnosticFile $path
}

Write-Report "targets=$matchedProcessCount stackAttempts=$stackAttemptCount verifiedStacks=$verifiedStackCount"
if (-not $cdb)
{
    Write-Warning 'cdb.exe was not found: GUI state and logs were collected, stacks were skipped.'
}
elseif ($stackFailures.Count)
{
    if ($debuggerCleanupFailed)
    {
        $scriptExitCode = 125
        [Console]::Error.WriteLine(
            "ERROR: CDB cleanup failed; no further debugger was launched: " +
            ($stackFailures -join '; '))
    }
    else
    {
        throw "One or more non-invasive stack captures failed: $($stackFailures -join '; ')"
    }
}

if ($scriptExitCode -eq 0 -and $matchedProcessCount -eq 0)
{
    throw 'No running executable from the selected build was inspected.'
}

if ($scriptExitCode -eq 0)
{
    Write-Report 'done'
    Write-Host "Diagnostic snapshot: $script:ResolvedOutputDir"
    Write-Host "Summary: $script:ReportPath"
}
}
finally
{
    Exit-OutputDirectoryLock -Lock $outputLock
}

exit $scriptExitCode
