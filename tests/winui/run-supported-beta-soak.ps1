# Run the automated wxWinUI Supported V0 gate repeatedly in fresh processes.
#
# This is supplemental automation evidence only. It deliberately excludes
# hidden and physical tests and cannot replace the manual, accessibility,
# multi-DPI, real-input, or unlocked-local-session qualification gates.

[CmdletBinding()]
param(
    [string] $BuildDir = (Join-Path $PSScriptRoot '..\..\build-winui-clean'),
    [string] $BinaryDir,
    [string] $RunnerPath,
    [string] $TestExecutable,
    [string] $OutputDir,
    [Alias('WerDumpDir')]
    [string[]] $WerDumpDirectory,
    [ValidateRange(0, 10080)]
    [int] $DurationMinutes = 60,
    [ValidateRange(0, 1000000)]
    [int] $Iterations = 0,
    [ValidateRange(2, 600)]
    [int] $TimeoutSeconds = 240,
    [ValidateRange(1, 60)]
    [int] $WerSettleSeconds = 10
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$testFilter = '[winui-v0-supported]~[.]~[physical]'
$expectedTestCaseCount = 50
$outerWatchdogMarginSeconds = 15
$utf8NoBom = [Text.UTF8Encoding]::new($false)

function ConvertTo-NativeArgument
{
    param([AllowEmptyString()][string] $Value)

    if ($Value.Length -gt 0 -and $Value -notmatch '[\s"]')
    {
        return $Value
    }

    # CommandLineToArgvW-compatible quoting, including backslashes directly
    # before a quote and before the terminating quote.
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

function Resolve-ChildPath
{
    param(
        [Parameter(Mandatory = $true)][string] $Path,
        [Parameter(Mandatory = $true)][string] $Parent
    )

    if ([IO.Path]::IsPathRooted($Path))
    {
        return [IO.Path]::GetFullPath($Path)
    }
    return [IO.Path]::GetFullPath((Join-Path $Parent $Path))
}

function Get-TextSha256
{
    param([Parameter(Mandatory = $true)][string] $Text)

    $algorithm = [Security.Cryptography.SHA256]::Create()
    try
    {
        $bytes = [Text.Encoding]::UTF8.GetBytes($Text)
        return ([BitConverter]::ToString(
            $algorithm.ComputeHash($bytes))).Replace(
                '-', '').ToLowerInvariant()
    }
    finally
    {
        $algorithm.Dispose()
    }
}

function Get-FileSha256
{
    param([Parameter(Mandatory = $true)][string] $Path)

    return (Get-FileHash -LiteralPath $Path -Algorithm SHA256).Hash.
        ToLowerInvariant()
}

function Write-NewUtf8File
{
    param(
        [Parameter(Mandatory = $true)][string] $Path,
        [AllowEmptyString()][Parameter(Mandatory = $true)][string] $Text
    )

    # CreateNew is intentional: evidence files are never silently replaced.
    $stream = [IO.File]::Open(
        $Path,
        [IO.FileMode]::CreateNew,
        [IO.FileAccess]::Write,
        [IO.FileShare]::Read)
    try
    {
        $writer = [IO.StreamWriter]::new($stream, $utf8NoBom)
        try
        {
            $writer.Write($Text)
            $writer.Flush()
        }
        finally
        {
            $writer.Dispose()
        }
    }
    finally
    {
        $stream.Dispose()
    }
}

function Stop-ExactProcess
{
    param([Parameter(Mandatory = $true)][Diagnostics.Process] $Process)

    for ($attempt = 1; $attempt -le 3; ++$attempt)
    {
        try
        {
            $Process.Refresh()
            if ($Process.HasExited)
            {
                return $true
            }

            # Retain and terminate the exact Process object to avoid PID reuse.
            # Closing this runner also closes its kill-on-close job, so the
            # isolated test process and any descendants are terminated too.
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
            if ($Process.WaitForExit(5000))
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

function Add-NormalizedDirectory
{
    param(
        [Parameter(Mandatory = $true)]
        [AllowEmptyCollection()]
        [Collections.Generic.HashSet[string]] $Set,
        [AllowEmptyString()][string] $Path
    )

    if ([string]::IsNullOrWhiteSpace($Path))
    {
        return
    }

    $expanded = [Environment]::ExpandEnvironmentVariables($Path.Trim())
    [void]$Set.Add([IO.Path]::GetFullPath($expanded))
}

function Get-RegistryDumpFolder
{
    param([Parameter(Mandatory = $true)][string] $RegistryPath)

    if (-not (Test-Path -LiteralPath $RegistryPath))
    {
        return $null
    }

    $properties = Get-ItemProperty -LiteralPath $RegistryPath
    $dumpFolder = $properties.PSObject.Properties['DumpFolder']
    if (-not $dumpFolder -or
        [string]::IsNullOrWhiteSpace([string]$dumpFolder.Value))
    {
        return $null
    }
    return [Environment]::ExpandEnvironmentVariables(
        [string]$dumpFolder.Value)
}

function Get-WerDumpDirectories
{
    param(
        [Parameter(Mandatory = $true)][string[]] $ExecutableNames,
        [string[]] $ExplicitDirectories
    )

    $directories = [Collections.Generic.HashSet[string]]::new(
        [StringComparer]::OrdinalIgnoreCase)

    if ($ExplicitDirectories -and $ExplicitDirectories.Count -gt 0)
    {
        foreach ($directory in $ExplicitDirectories)
        {
            Add-NormalizedDirectory -Set $directories -Path $directory
        }
        return @($directories | Sort-Object)
    }

    Add-NormalizedDirectory -Set $directories -Path $env:WX_WINUI_DUMP_DIR
    if (-not [string]::IsNullOrWhiteSpace($env:LOCALAPPDATA))
    {
        Add-NormalizedDirectory -Set $directories -Path (
            Join-Path $env:LOCALAPPDATA 'CrashDumps')
    }

    $registryRoot =
        'Registry::HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\' +
        'Windows Error Reporting\LocalDumps'
    foreach ($executableName in $ExecutableNames)
    {
        # An executable-specific LocalDumps key overrides the global key.
        $specificFolder = Get-RegistryDumpFolder -RegistryPath (
            Join-Path $registryRoot $executableName)
        if ($specificFolder)
        {
            Add-NormalizedDirectory -Set $directories -Path $specificFolder
            continue
        }

        $globalFolder = Get-RegistryDumpFolder -RegistryPath $registryRoot
        if ($globalFolder)
        {
            Add-NormalizedDirectory -Set $directories -Path $globalFolder
        }
    }

    return @($directories | Sort-Object)
}

function Get-WerDumpFingerprints
{
    param(
        [Parameter(Mandatory = $true)][string[]] $Directories,
        [Parameter(Mandatory = $true)][string[]] $ExecutableBaseNames
    )

    $fingerprints = [Collections.Generic.List[object]]::new()
    foreach ($directory in $Directories)
    {
        if (-not (Test-Path -LiteralPath $directory))
        {
            continue
        }
        if (-not (Test-Path -LiteralPath $directory -PathType Container))
        {
            throw "WER dump path is not a directory: $directory"
        }

        foreach ($dump in Get-ChildItem -LiteralPath $directory -File `
                                      -Filter '*.dmp')
        {
            $matchesExecutable = $false
            foreach ($baseName in $ExecutableBaseNames)
            {
                if ($dump.Name.StartsWith(
                        $baseName, [StringComparison]::OrdinalIgnoreCase))
                {
                    $matchesExecutable = $true
                    break
                }
            }
            if (-not $matchesExecutable)
            {
                continue
            }

            $fingerprints.Add([pscustomobject][ordered]@{
                Path = $dump.FullName
                Length = $dump.Length
                LastWriteUtc = $dump.LastWriteTimeUtc.ToString('o')
                Sha256 = Get-FileSha256 -Path $dump.FullName
            })
        }
    }

    return @($fingerprints | Sort-Object Path, LastWriteUtc, Sha256)
}

function Get-DumpFingerprintKey
{
    param([Parameter(Mandatory = $true)] $Fingerprint)

    return "$($Fingerprint.Path)|$($Fingerprint.Length)|" +
           "$($Fingerprint.LastWriteUtc)|$($Fingerprint.Sha256)"
}

function Find-NewDumpFingerprints
{
    param(
        [AllowEmptyCollection()]
        [Parameter(Mandatory = $true)][object[]] $Before,
        [AllowEmptyCollection()]
        [Parameter(Mandatory = $true)][object[]] $After
    )

    $known = [Collections.Generic.HashSet[string]]::new(
        [StringComparer]::OrdinalIgnoreCase)
    foreach ($fingerprint in $Before)
    {
        [void]$known.Add((Get-DumpFingerprintKey $fingerprint))
    }

    return @(
        foreach ($fingerprint in $After)
        {
            if (-not $known.Contains(
                    (Get-DumpFingerprintKey $fingerprint)))
            {
                $fingerprint
            }
        }
    )
}

function Get-SupportedSourceManifest
{
    param([Parameter(Mandatory = $true)][string] $SourceDirectory)

    $sourceRoot = [IO.Path]::GetFullPath($SourceDirectory)
    $sourcePrefix = $sourceRoot.TrimEnd(
        [IO.Path]::DirectorySeparatorChar,
        [IO.Path]::AltDirectorySeparatorChar) +
            [IO.Path]::DirectorySeparatorChar
    $paths = [Collections.Generic.HashSet[string]]::new(
        [StringComparer]::OrdinalIgnoreCase)

    foreach ($relativeRoot in @('include\wx\winui', 'src\winui'))
    {
        $absoluteRoot = Join-Path $sourceRoot $relativeRoot
        if (-not (Test-Path -LiteralPath $absoluteRoot -PathType Container))
        {
            throw "Supported V0 source directory not found: $absoluteRoot"
        }
        foreach ($file in Get-ChildItem -LiteralPath $absoluteRoot -File `
                                       -Recurse)
        {
            [void]$paths.Add($file.FullName)
        }
    }

    foreach ($relativeFile in @(
        'build\cmake\functions.cmake',
        'build\cmake\tests\gui\CMakeLists.txt',
        'tests\winui\desktop-test-runner.cpp'))
    {
        $absoluteFile = Join-Path $sourceRoot $relativeFile
        if (-not (Test-Path -LiteralPath $absoluteFile -PathType Leaf))
        {
            throw "Supported V0 source file not found: $absoluteFile"
        }
        [void]$paths.Add([IO.Path]::GetFullPath($absoluteFile))
    }

    $testsRoot = Join-Path $sourceRoot 'tests'
    foreach ($testSource in Get-ChildItem -LiteralPath $testsRoot -File `
                                         -Recurse -Include '*.cpp', '*.h')
    {
        if (Select-String -LiteralPath $testSource.FullName -SimpleMatch `
                          '[winui-v0-supported]' -Quiet)
        {
            [void]$paths.Add($testSource.FullName)
        }
    }

    [void]$paths.Add([IO.Path]::GetFullPath($PSCommandPath))

    $entries = [Collections.Generic.List[object]]::new()
    foreach ($path in $paths)
    {
        $fullPath = [IO.Path]::GetFullPath($path)
        if (-not $fullPath.StartsWith(
                $sourcePrefix, [StringComparison]::OrdinalIgnoreCase))
        {
            throw "Source manifest path escaped its source root: $fullPath"
        }
        $item = Get-Item -LiteralPath $fullPath
        $entries.Add([pscustomobject][ordered]@{
            Path = $fullPath.Substring($sourcePrefix.Length).
                Replace('\', '/')
            Length = $item.Length
            Sha256 = Get-FileSha256 -Path $fullPath
        })
    }

    $sortedEntries = @($entries | Sort-Object Path)
    $canonicalLines = @(
        foreach ($entry in $sortedEntries)
        {
            "$($entry.Path)|$($entry.Length)|$($entry.Sha256)"
        }
    )
    $canonicalText = ($canonicalLines -join "`n") + "`n"
    return [pscustomobject][ordered]@{
        FileCount = $sortedEntries.Count
        AggregateSha256 = Get-TextSha256 -Text $canonicalText
        Files = $sortedEntries
    }
}

if ($DurationMinutes -eq 0 -and $Iterations -eq 0)
{
    throw 'DurationMinutes and Iterations cannot both be zero.'
}

$sourceDirectory = [IO.Path]::GetFullPath((Join-Path $PSScriptRoot '..\..'))
$resolvedBuildDir = [IO.Path]::GetFullPath($BuildDir)
if (-not (Test-Path -LiteralPath $resolvedBuildDir -PathType Container))
{
    throw "Build directory not found: $resolvedBuildDir"
}

$resolvedBinaryDir =
    if ($BinaryDir)
    {
        Resolve-ChildPath -Path $BinaryDir -Parent $resolvedBuildDir
    }
    else
    {
        [IO.Path]::GetFullPath(
            (Join-Path $resolvedBuildDir 'lib\vc_x64_dll'))
    }
if (-not (Test-Path -LiteralPath $resolvedBinaryDir -PathType Container))
{
    throw "Binary directory not found: $resolvedBinaryDir"
}

$resolvedTestExecutable =
    if ($TestExecutable)
    {
        Resolve-ChildPath -Path $TestExecutable -Parent $resolvedBinaryDir
    }
    else
    {
        Join-Path $resolvedBinaryDir 'test_gui.exe'
    }
$resolvedTestExecutable = [IO.Path]::GetFullPath($resolvedTestExecutable)
if (-not (Test-Path -LiteralPath $resolvedTestExecutable -PathType Leaf))
{
    throw "test_gui executable not found: $resolvedTestExecutable"
}
$resolvedTestExecutable =
    (Resolve-Path -LiteralPath $resolvedTestExecutable).Path

$resolvedRunnerPath =
    if ($RunnerPath)
    {
        Resolve-ChildPath -Path $RunnerPath -Parent $resolvedBuildDir
    }
    else
    {
        Join-Path $resolvedBuildDir (
            'winui\test-tools\Release\' +
            'wx_winui_desktop_test_runner.exe')
    }
$resolvedRunnerPath = [IO.Path]::GetFullPath($resolvedRunnerPath)
if (-not (Test-Path -LiteralPath $resolvedRunnerPath -PathType Leaf))
{
    throw "Desktop test runner not found: $resolvedRunnerPath"
}
$resolvedRunnerPath = (Resolve-Path -LiteralPath $resolvedRunnerPath).Path

$workingDirectory = [IO.Path]::GetDirectoryName($resolvedTestExecutable)
$runnerName = [IO.Path]::GetFileName($resolvedRunnerPath)
$testName = [IO.Path]::GetFileName($resolvedTestExecutable)
$werExecutableBaseNames = @(
    [IO.Path]::GetFileNameWithoutExtension($runnerName),
    [IO.Path]::GetFileNameWithoutExtension($testName)
)
$werDirectoryMode =
    if ($WerDumpDirectory -and $WerDumpDirectory.Count -gt 0)
    {
        'explicit'
    }
    else
    {
        'auto-discovered'
    }
$werDirectoryCandidates = @(Get-WerDumpDirectories `
    -ExecutableNames @($runnerName, $testName) `
    -ExplicitDirectories $WerDumpDirectory)
$observableWerDirectories = [Collections.Generic.List[string]]::new()
foreach ($directory in $werDirectoryCandidates)
{
    if (-not (Test-Path -LiteralPath $directory))
    {
        if ($werDirectoryMode -eq 'explicit')
        {
            throw "Explicit WER dump directory does not exist: $directory"
        }
        continue
    }
    if (-not (Test-Path -LiteralPath $directory -PathType Container))
    {
        throw "WER dump path is not a directory: $directory"
    }

    # Enumeration is the observability check: a configured but unreadable
    # directory cannot support a no-new-dumps qualification claim.
    try
    {
        [void](Get-ChildItem -LiteralPath $directory -Force `
                            -ErrorAction Stop | Select-Object -First 1)
    }
    catch
    {
        throw "WER dump directory is not observable: $directory ($($_.Exception.Message))"
    }
    $observableWerDirectories.Add(
        (Resolve-Path -LiteralPath $directory).Path)
}
if ($observableWerDirectories.Count -eq 0)
{
    throw ('No existing, observable WER dump directory was found. ' +
           'Configure LocalDumps and pass -WerDumpDirectory explicitly.')
}
$werDumpDirectories = @($observableWerDirectories | Sort-Object -Unique)

$campaignId =
    (Get-Date -Format 'yyyyMMdd-HHmmss-fff') + '-' +
    [guid]::NewGuid().ToString('N').Substring(0, 8)
if (-not $OutputDir)
{
    $OutputDir = Join-Path $resolvedBuildDir (
        "logs\winui-supported-beta-soak\$campaignId")
}
$resolvedOutputDir = [IO.Path]::GetFullPath($OutputDir)
if (Test-Path -LiteralPath $resolvedOutputDir)
{
    if (-not (Test-Path -LiteralPath $resolvedOutputDir `
                            -PathType Container))
    {
        throw "Output path is not a directory: $resolvedOutputDir"
    }
    if (Get-ChildItem -LiteralPath $resolvedOutputDir -Force |
            Select-Object -First 1)
    {
        throw "Output directory is not empty: $resolvedOutputDir"
    }
}
else
{
    [void][IO.Directory]::CreateDirectory($resolvedOutputDir)
}

$lockPath = Join-Path $resolvedOutputDir 'campaign.lock'
$campaignLock = $null
$activeProcess = $null
$scriptExitCode = 4

try
{
$campaignLock = [IO.File]::Open(
    $lockPath,
    [IO.FileMode]::CreateNew,
    [IO.FileAccess]::Write,
    [IO.FileShare]::Read)

$runnerArguments = @(
    "--timeout-ms=$($TimeoutSeconds * 1000)",
    '--',
    $resolvedTestExecutable,
    $testFilter,
    '--warn',
    'NoTests'
)
$nativeArgumentLine =
    (($runnerArguments |
        ForEach-Object { ConvertTo-NativeArgument $_ }) -join ' ')
$exactCommandLine =
    (ConvertTo-NativeArgument $resolvedRunnerPath) + ' ' +
    $nativeArgumentLine
$preflightArguments = @(
    "--timeout-ms=$($TimeoutSeconds * 1000)",
    '--',
    $resolvedTestExecutable,
    $testFilter,
    '--list-tests'
)
$preflightNativeArgumentLine =
    (($preflightArguments |
        ForEach-Object { ConvertTo-NativeArgument $_ }) -join ' ')
$preflightExactCommandLine =
    (ConvertTo-NativeArgument $resolvedRunnerPath) + ' ' +
    $preflightNativeArgumentLine

$campaignStartedUtc = $null
$campaignStopwatch = [Diagnostics.Stopwatch]::new()
$records = [Collections.Generic.List[object]]::new()
$recordEvidence = [Collections.Generic.List[object]]::new()
$campaignFailures = [Collections.Generic.List[string]]::new()
$stopReason = $null

$scriptHashBefore = Get-FileSha256 -Path $PSCommandPath
$sourceManifestBefore =
    Get-SupportedSourceManifest -SourceDirectory $sourceDirectory
$runnerHashBefore = Get-FileSha256 -Path $resolvedRunnerPath
$testHashBefore = Get-FileSha256 -Path $resolvedTestExecutable
$werCampaignBaseline = @(Get-WerDumpFingerprints `
    -Directories $werDumpDirectories `
    -ExecutableBaseNames $werExecutableBaseNames)

$preflightStdoutPath = Join-Path $resolvedOutputDir 'preflight-stdout.txt'
$preflightStderrPath = Join-Path $resolvedOutputDir 'preflight-stderr.txt'
$preflightJsonPath = Join-Path $resolvedOutputDir 'preflight.json'
$preflightFailures = [Collections.Generic.List[string]]::new()
$preflightWerBefore = $werCampaignBaseline
$preflightWerAfter = @()
$preflightNewWerDumps = [Collections.Generic.List[object]]::new()
$preflightNewWerDumpKeys = [Collections.Generic.HashSet[string]]::new(
    [StringComparer]::OrdinalIgnoreCase)
$preflightStdout = ''
$preflightStderr = ''
$preflightExitCode = $null
$preflightOuterTimedOut = $false
$preflightProcessId = $null
$preflightProcessStarted = $false
$preflightCleanupSucceeded = $true
$preflightStdoutTask = $null
$preflightStderrTask = $null
$preflightTimer = [Diagnostics.Stopwatch]::StartNew()
$preflightStartedUtc = [DateTime]::UtcNow
$preflightRecord = [ordered]@{
    StartedUtc = $preflightStartedUtc.ToString('o')
    EndedUtc = $null
    ElapsedMs = $null
    ProcessId = $null
    ExactCommand = $preflightExactCommandLine
    WorkingDirectory = $workingDirectory
    Filter = $testFilter
    ExpectedTestCaseCount = $expectedTestCaseCount
    ObservedTestCaseCount = $null
    RunnerExitCode = $null
    OuterWatchdogTimeout = $false
    CleanupSucceeded = $true
    StdoutFile = [IO.Path]::GetFileName($preflightStdoutPath)
    StdoutSha256 = $null
    StderrFile = [IO.Path]::GetFileName($preflightStderrPath)
    StderrSha256 = $null
    WerBefore = $preflightWerBefore
    WerAfter = @()
    NewWerDumps = @()
    FailureReasons = @()
    Status = 'Running'
}

try
{
    try
    {
        $startInfo = [Diagnostics.ProcessStartInfo]::new()
        $startInfo.FileName = $resolvedRunnerPath
        $startInfo.Arguments = $preflightNativeArgumentLine
        $startInfo.WorkingDirectory = $workingDirectory
        $startInfo.UseShellExecute = $false
        $startInfo.CreateNoWindow = $true
        $startInfo.WindowStyle =
            [Diagnostics.ProcessWindowStyle]::Hidden
        $startInfo.RedirectStandardOutput = $true
        $startInfo.RedirectStandardError = $true

        $activeProcess = [Diagnostics.Process]::new()
        $activeProcess.StartInfo = $startInfo
        $preflightProcessStarted = $activeProcess.Start()
        if (-not $preflightProcessStarted)
        {
            throw 'Preflight Process.Start() returned false.'
        }
        $preflightProcessId = $activeProcess.Id
        $preflightRecord.ProcessId = $preflightProcessId
        $preflightStdoutTask =
            $activeProcess.StandardOutput.ReadToEndAsync()
        $preflightStderrTask =
            $activeProcess.StandardError.ReadToEndAsync()

        $outerTimeoutMs =
            ($TimeoutSeconds + $outerWatchdogMarginSeconds) * 1000
        if (-not $activeProcess.WaitForExit($outerTimeoutMs))
        {
            $preflightOuterTimedOut = $true
            $preflightFailures.Add(
                "preflight outer watchdog exceeded ${outerTimeoutMs}ms")
            $preflightCleanupSucceeded =
                Stop-ExactProcess -Process $activeProcess
        }

        $activeProcess.Refresh()
        if (-not $activeProcess.HasExited)
        {
            $preflightCleanupSucceeded = $false
            $preflightFailures.Add(
                "preflight runner PID $preflightProcessId survived forced termination")
        }
    }
    catch
    {
        $preflightFailures.Add(
            "infrastructure failure: preflight $($_.Exception.Message)")
    }
    finally
    {
        # This is also the cancellation path. Keep the exact Process object
        # alive unless its handle confirms exit, so outer cleanup can retry it.
        if ($activeProcess)
        {
            try
            {
                $activeProcess.Refresh()
                $preflightProcessStarted = $true
            }
            catch [InvalidOperationException]
            {
                # No process is associated only when Start() never succeeded.
                $preflightProcessStarted = $false
            }
            if ($preflightProcessStarted)
            {
                try
                {
                    $activeProcess.Refresh()
                    if (-not $activeProcess.HasExited)
                    {
                        $preflightCleanupSucceeded =
                            (Stop-ExactProcess -Process $activeProcess) -and
                            $preflightCleanupSucceeded
                    }
                    $activeProcess.Refresh()
                    if ($activeProcess.HasExited)
                    {
                        $activeProcess.WaitForExit()
                        $preflightExitCode = $activeProcess.ExitCode
                        if ($preflightStdoutTask)
                        {
                            $preflightStdout =
                                $preflightStdoutTask.GetAwaiter().GetResult()
                        }
                        if ($preflightStderrTask)
                        {
                            $preflightStderr =
                                $preflightStderrTask.GetAwaiter().GetResult()
                        }
                        $activeProcess.Dispose()
                        $activeProcess = $null
                    }
                    else
                    {
                        $preflightCleanupSucceeded = $false
                    }
                }
                catch
                {
                    $preflightCleanupSucceeded = $false
                    $preflightFailures.Add(
                    "infrastructure failure: preflight cleanup $($_.Exception.Message)")
                }
            }
            else
            {
                $activeProcess.Dispose()
                $activeProcess = $null
            }
        }

        $preflightTimer.Stop()
        $preflightRecord.EndedUtc = [DateTime]::UtcNow.ToString('o')
        $preflightRecord.ElapsedMs = $preflightTimer.ElapsedMilliseconds
        $preflightRecord.ProcessId = $preflightProcessId
        $preflightRecord.RunnerExitCode = $preflightExitCode
        $preflightRecord.OuterWatchdogTimeout = $preflightOuterTimedOut
        $preflightRecord.CleanupSucceeded = $preflightCleanupSucceeded
    }

    if (-not $preflightCleanupSucceeded)
    {
        $preflightFailures.Add('exact preflight runner cleanup failed')
    }
    if (-not $preflightOuterTimedOut)
    {
        if ($preflightExitCode -eq 125)
        {
            $preflightFailures.Add(
                'infrastructure failure: desktop runner setup failed (exit 125)')
        }
        elseif ($preflightExitCode -ne $expectedTestCaseCount)
        {
            $preflightFailures.Add(
                ('preflight list exit was {0}; expected inventory exit {1}' -f
                    $preflightExitCode, $expectedTestCaseCount))
        }
    }

    $inventoryMatches = [regex]::Matches(
        $preflightStdout,
        '(?m)^\s*(?<Count>\d+)\s+matching test cases?\s*$')
    if ($inventoryMatches.Count -ne 1)
    {
        $preflightFailures.Add(
            'preflight output did not contain exactly one test inventory total')
    }
    else
    {
        $observedTestCaseCount =
            [int]$inventoryMatches[0].Groups['Count'].Value
        $preflightRecord.ObservedTestCaseCount = $observedTestCaseCount
        if ($observedTestCaseCount -ne $expectedTestCaseCount)
        {
            $preflightFailures.Add(
                ("Supported V0 inventory is $observedTestCaseCount; " +
                 "expected $expectedTestCaseCount"))
        }
    }

    # Account for a dump created by the inventory process itself. The timed
    # campaign begins only after this complete preflight settle and freeze.
    for ($poll = 0; $poll -le $WerSettleSeconds; ++$poll)
    {
        if ($poll -gt 0)
        {
            Start-Sleep -Seconds 1
        }
        $preflightWerAfter = @(Get-WerDumpFingerprints `
            -Directories $werDumpDirectories `
            -ExecutableBaseNames $werExecutableBaseNames)
        foreach ($newDump in @(Find-NewDumpFingerprints `
            -Before $preflightWerBefore -After $preflightWerAfter))
        {
            $newDumpKey = Get-DumpFingerprintKey $newDump
            if ($preflightNewWerDumpKeys.Add($newDumpKey))
            {
                $preflightNewWerDumps.Add($newDump)
            }
        }
    }
    if ($preflightNewWerDumps.Count -gt 0)
    {
        $preflightFailures.Add(
            "$($preflightNewWerDumps.Count) preflight WER dump fingerprint(s)")
    }

    if ((Get-FileSha256 -Path $PSCommandPath) -ne $scriptHashBefore -or
        (Get-SupportedSourceManifest -SourceDirectory $sourceDirectory).
            AggregateSha256 -ne $sourceManifestBefore.AggregateSha256 -or
        (Get-FileSha256 -Path $resolvedRunnerPath) -ne $runnerHashBefore -or
        (Get-FileSha256 -Path $resolvedTestExecutable) -ne $testHashBefore)
    {
        $preflightFailures.Add(
            'source, harness, or test binaries changed during preflight')
    }

    $preflightRecord.WerAfter = $preflightWerAfter
    $preflightRecord.NewWerDumps = @($preflightNewWerDumps)
    $preflightRecord.FailureReasons = @($preflightFailures)
    $preflightRecord.Status =
        if ($preflightFailures.Count -eq 0) { 'Pass' } else { 'Fail' }
    Write-NewUtf8File -Path $preflightStdoutPath -Text $preflightStdout
    Write-NewUtf8File -Path $preflightStderrPath -Text $preflightStderr
    $preflightRecord.StdoutSha256 =
        Get-FileSha256 -Path $preflightStdoutPath
    $preflightRecord.StderrSha256 =
        Get-FileSha256 -Path $preflightStderrPath
    Write-NewUtf8File -Path $preflightJsonPath -Text (
        (([pscustomobject]$preflightRecord | ConvertTo-Json -Depth 8) +
         "`n"))

    if ($preflightFailures.Count -gt 0)
    {
        throw ('Supported V0 inventory preflight failed: ' +
               ($preflightFailures -join '; '))
    }

    $werCampaignBaseline = $preflightWerAfter
    $campaignStartedUtc = [DateTime]::UtcNow
    $campaignStopwatch.Start()

    while ($true)
    {
        if ($records.Count -gt 0)
        {
            if ($Iterations -gt 0 -and $records.Count -ge $Iterations)
            {
                $stopReason = 'iteration-limit'
                break
            }
            if ($DurationMinutes -gt 0 -and
                $campaignStopwatch.Elapsed.TotalMinutes -ge
                    $DurationMinutes)
            {
                $stopReason = 'duration-limit'
                break
            }
        }

        $iteration = $records.Count + 1
        $tag = '{0:D6}' -f $iteration
        $stdoutPath = Join-Path $resolvedOutputDir "stdout-$tag.txt"
        $stderrPath = Join-Path $resolvedOutputDir "stderr-$tag.txt"
        $iterationJsonPath =
            Join-Path $resolvedOutputDir "iteration-$tag.json"
        $failureReasons = [Collections.Generic.List[string]]::new()
        $werBefore = @()
        $werAfter = @()
        $newWerDumps = [Collections.Generic.List[object]]::new()
        $newWerDumpKeys = [Collections.Generic.HashSet[string]]::new(
            [StringComparer]::OrdinalIgnoreCase)
        $stdout = ''
        $stderr = ''
        $runnerExitCode = $null
        $outerTimedOut = $false
        $processId = $null
        $processStarted = $false
        $cleanupSucceeded = $true
        $stdoutTask = $null
        $stderrTask = $null
        $iterationTimer = [Diagnostics.Stopwatch]::StartNew()

        $record = [ordered]@{
            Iteration = $iteration
            StartedUtc = [DateTime]::UtcNow.ToString('o')
            EndedUtc = $null
            ElapsedMs = $null
            ProcessId = $null
            ExactCommand = $exactCommandLine
            WorkingDirectory = $workingDirectory
            Filter = $testFilter
            RunnerExitCode = $null
            OuterWatchdogTimeout = $false
            CleanupSucceeded = $true
            RunnerSha256Before = $null
            RunnerSha256After = $null
            TestGuiSha256Before = $null
            TestGuiSha256After = $null
            StdoutFile = [IO.Path]::GetFileName($stdoutPath)
            StdoutSha256 = $null
            StderrFile = [IO.Path]::GetFileName($stderrPath)
            StderrSha256 = $null
            WerBefore = @()
            WerAfter = @()
            NewWerDumps = @()
            FailureReasons = @()
            Status = 'Running'
        }

        try
        {
            $record.RunnerSha256Before =
                Get-FileSha256 -Path $resolvedRunnerPath
            $record.TestGuiSha256Before =
                Get-FileSha256 -Path $resolvedTestExecutable
            if ($record.RunnerSha256Before -ne $runnerHashBefore -or
                $record.TestGuiSha256Before -ne $testHashBefore)
            {
                throw 'A test binary changed before the iteration started.'
            }

            $werBefore = @(Get-WerDumpFingerprints `
                -Directories $werDumpDirectories `
                -ExecutableBaseNames $werExecutableBaseNames)
            $record.WerBefore = $werBefore

            $startInfo = [Diagnostics.ProcessStartInfo]::new()
            $startInfo.FileName = $resolvedRunnerPath
            $startInfo.Arguments = $nativeArgumentLine
            $startInfo.WorkingDirectory = $workingDirectory
            $startInfo.UseShellExecute = $false
            $startInfo.CreateNoWindow = $true
            $startInfo.WindowStyle =
                [Diagnostics.ProcessWindowStyle]::Hidden
            $startInfo.RedirectStandardOutput = $true
            $startInfo.RedirectStandardError = $true

            $activeProcess = [Diagnostics.Process]::new()
            $activeProcess.StartInfo = $startInfo
            $processStarted = $activeProcess.Start()
            if (-not $processStarted)
            {
                throw 'Process.Start() returned false.'
            }
            $processId = $activeProcess.Id
            $record.ProcessId = $processId
            $stdoutTask = $activeProcess.StandardOutput.ReadToEndAsync()
            $stderrTask = $activeProcess.StandardError.ReadToEndAsync()

            $outerTimeoutMs =
                ($TimeoutSeconds + $outerWatchdogMarginSeconds) * 1000
            if (-not $activeProcess.WaitForExit($outerTimeoutMs))
            {
                $outerTimedOut = $true
                $failureReasons.Add(
                    "outer watchdog exceeded ${outerTimeoutMs}ms")
                $cleanupSucceeded =
                    Stop-ExactProcess -Process $activeProcess
            }

            $activeProcess.Refresh()
            if ($activeProcess.HasExited)
            {
                # The parameterless wait drains both redirected streams.
                $activeProcess.WaitForExit()
                $runnerExitCode = $activeProcess.ExitCode
                $stdout = $stdoutTask.GetAwaiter().GetResult()
                $stderr = $stderrTask.GetAwaiter().GetResult()
            }
            else
            {
                $cleanupSucceeded = $false
                $failureReasons.Add(
                    "runner PID $processId survived forced termination")
            }

            if (-not $outerTimedOut)
            {
                if ($runnerExitCode -eq 124)
                {
                    $failureReasons.Add(
                        "desktop runner timed out after ${TimeoutSeconds}s")
                }
                elseif ($runnerExitCode -eq 125)
                {
                    $failureReasons.Add(
                        'infrastructure failure: desktop runner setup failed (exit 125)')
                }
                elseif ($runnerExitCode -ne 0)
                {
                    $failureReasons.Add(
                        "desktop runner/test_gui exited $runnerExitCode")
                }
            }

            # WER may publish LocalDumps asynchronously. Inventory immediately
            # and for a bounded settle window. Dumps are never moved or deleted.
            for ($poll = 0; $poll -le $WerSettleSeconds; ++$poll)
            {
                if ($poll -gt 0)
                {
                    Start-Sleep -Seconds 1
                }
                $werAfter = @(Get-WerDumpFingerprints `
                    -Directories $werDumpDirectories `
                    -ExecutableBaseNames $werExecutableBaseNames)
                foreach ($newDump in @(Find-NewDumpFingerprints `
                    -Before $werBefore -After $werAfter))
                {
                    $newDumpKey = Get-DumpFingerprintKey $newDump
                    if ($newWerDumpKeys.Add($newDumpKey))
                    {
                        $newWerDumps.Add($newDump)
                    }
                }
            }
            if ($newWerDumps.Count -gt 0)
            {
                $failureReasons.Add(
                    "$($newWerDumps.Count) new WER dump fingerprint(s)")
            }

            $record.RunnerSha256After =
                Get-FileSha256 -Path $resolvedRunnerPath
            $record.TestGuiSha256After =
                Get-FileSha256 -Path $resolvedTestExecutable
            if ($record.RunnerSha256After -ne $runnerHashBefore -or
                $record.TestGuiSha256After -ne $testHashBefore)
            {
                $failureReasons.Add(
                    'A test binary changed during the iteration.')
            }
        }
        catch
        {
            $failureReasons.Add(
                "infrastructure failure: $($_.Exception.Message)")
            if ($processStarted -and $activeProcess)
            {
                try
                {
                    $activeProcess.Refresh()
                    if (-not $activeProcess.HasExited)
                    {
                        $cleanupSucceeded =
                            (Stop-ExactProcess -Process $activeProcess) -and
                            $cleanupSucceeded
                    }
                }
                catch
                {
                    $cleanupSucceeded = $false
                }
            }

            # Make a best-effort settled inventory, but never hide the original
            # infrastructure failure if WER itself cannot be enumerated.
            try
            {
                if ($processStarted)
                {
                    for ($poll = 0;
                         $poll -le $WerSettleSeconds;
                         ++$poll)
                    {
                        if ($poll -gt 0)
                        {
                            Start-Sleep -Seconds 1
                        }
                        $werAfter = @(Get-WerDumpFingerprints `
                            -Directories $werDumpDirectories `
                            -ExecutableBaseNames $werExecutableBaseNames)
                        foreach ($newDump in @(Find-NewDumpFingerprints `
                            -Before $werBefore -After $werAfter))
                        {
                            $newDumpKey = Get-DumpFingerprintKey $newDump
                            if ($newWerDumpKeys.Add($newDumpKey))
                            {
                                $newWerDumps.Add($newDump)
                            }
                        }
                    }
                }
            }
            catch
            {
                $failureReasons.Add(
                    "final WER inventory failed: $($_.Exception.Message)")
            }
        }
        finally
        {
            # Always close the exact process started by this iteration before
            # recording it. On a failed stop, retain the same Process object so
            # campaign-level and outermost cleanup can retry its live handle.
            if ($activeProcess)
            {
                try
                {
                    $activeProcess.Refresh()
                    $processStarted = $true
                }
                catch [InvalidOperationException]
                {
                    # No process is associated only when Start() never succeeded.
                    $processStarted = $false
                }
                if ($processStarted)
                {
                    try
                    {
                        $activeProcess.Refresh()
                        if (-not $activeProcess.HasExited)
                        {
                            $cleanupSucceeded =
                                (Stop-ExactProcess -Process $activeProcess) -and
                                $cleanupSucceeded
                        }
                        $activeProcess.Refresh()
                        if ($activeProcess.HasExited)
                        {
                            $activeProcess.WaitForExit()
                            if ($null -eq $runnerExitCode)
                            {
                                $runnerExitCode = $activeProcess.ExitCode
                            }
                            if ($stdoutTask)
                            {
                                $stdout =
                                    $stdoutTask.GetAwaiter().GetResult()
                            }
                            if ($stderrTask)
                            {
                                $stderr =
                                    $stderrTask.GetAwaiter().GetResult()
                            }
                            $activeProcess.Dispose()
                            $activeProcess = $null
                        }
                        else
                        {
                            $cleanupSucceeded = $false
                        }
                    }
                    catch
                    {
                        $cleanupSucceeded = $false
                        $failureReasons.Add(
                            "infrastructure failure: exact runner cleanup $($_.Exception.Message)")
                    }
                }
                else
                {
                    $activeProcess.Dispose()
                    $activeProcess = $null
                }
            }

            $iterationTimer.Stop()
            $record.EndedUtc = [DateTime]::UtcNow.ToString('o')
            $record.ElapsedMs = $iterationTimer.ElapsedMilliseconds
            $record.ProcessId = $processId
            $record.RunnerExitCode = $runnerExitCode
            $record.OuterWatchdogTimeout = $outerTimedOut
            $record.CleanupSucceeded = $cleanupSucceeded
            $record.WerAfter = $werAfter
            $record.NewWerDumps = @($newWerDumps)

            Write-NewUtf8File -Path $stdoutPath -Text $stdout
            Write-NewUtf8File -Path $stderrPath -Text $stderr
            $record.StdoutSha256 = Get-FileSha256 -Path $stdoutPath
            $record.StderrSha256 = Get-FileSha256 -Path $stderrPath

            if (-not $cleanupSucceeded)
            {
                $failureReasons.Add('exact runner cleanup failed')
            }
            $record.FailureReasons = @($failureReasons)

            if ($failureReasons.Count -eq 0)
            {
                $record.Status = 'Pass'
            }
            elseif (-not $cleanupSucceeded -or
                    @($failureReasons | Where-Object {
                        $_ -like 'infrastructure failure:*' -or
                        $_ -like 'final WER inventory failed:*' -or
                        $_ -like 'A test binary changed*'
                    }).Count -gt 0)
            {
                $record.Status = 'InfrastructureFailure'
            }
            elseif ($newWerDumps.Count -gt 0)
            {
                $record.Status = 'WerDumpDetected'
            }
            elseif ($outerTimedOut -or $runnerExitCode -eq 124)
            {
                $record.Status = 'Timeout'
            }
            else
            {
                $record.Status = 'TestFailure'
            }

            $iterationJson =
                ([pscustomobject]$record | ConvertTo-Json -Depth 8)
            Write-NewUtf8File -Path $iterationJsonPath `
                              -Text ($iterationJson + "`n")
            $recordEvidence.Add([pscustomobject][ordered]@{
                File = [IO.Path]::GetFileName($iterationJsonPath)
                Sha256 = Get-FileSha256 -Path $iterationJsonPath
            })
            $records.Add([pscustomobject]$record)

        }

        if ($record.Status -ne 'Pass')
        {
            foreach ($reason in $record.FailureReasons)
            {
                $campaignFailures.Add("iteration ${iteration}: $reason")
            }
            $stopReason = 'fail-closed'
            break
        }
    }
}
catch
{
    $campaignFailures.Add(
        "campaign infrastructure failure: $($_.Exception.Message)")
    $stopReason = 'fail-closed'
}
finally
{
    if ($activeProcess)
    {
        try
        {
            $activeProcess.Refresh()
            if (-not $activeProcess.HasExited)
            {
                [void](Stop-ExactProcess -Process $activeProcess)
            }
            $activeProcess.Refresh()
            if ($activeProcess.HasExited)
            {
                $activeProcess.Dispose()
                $activeProcess = $null
            }
            else
            {
                $campaignFailures.Add(
                    'campaign cleanup could not stop the exact runner process')
            }
        }
        catch
        {
            $campaignFailures.Add(
                "campaign exact-process cleanup failed: $($_.Exception.Message)")
        }
    }
    $campaignStopwatch.Stop()
}

if (-not $stopReason)
{
    $stopReason = 'fail-closed'
    $campaignFailures.Add('campaign ended without a stop reason')
}
if ($records.Count -eq 0)
{
    $campaignFailures.Add('campaign completed no test iteration')
}

$scriptHashAfter = $null
$sourceManifestAfter = $null
$runnerHashAfter = $null
$testHashAfter = $null
try
{
    $scriptHashAfter = Get-FileSha256 -Path $PSCommandPath
    $sourceManifestAfter =
        Get-SupportedSourceManifest -SourceDirectory $sourceDirectory
    $runnerHashAfter = Get-FileSha256 -Path $resolvedRunnerPath
    $testHashAfter = Get-FileSha256 -Path $resolvedTestExecutable
}
catch
{
    $campaignFailures.Add(
        "final source/binary inventory failed: $($_.Exception.Message)")
}

if ($scriptHashAfter -and $scriptHashAfter -ne $scriptHashBefore)
{
    $campaignFailures.Add('soak harness changed during the campaign')
}
if ($sourceManifestAfter -and
    $sourceManifestAfter.AggregateSha256 -ne
        $sourceManifestBefore.AggregateSha256)
{
    $campaignFailures.Add('Supported V0 source manifest changed during campaign')
}
if (($runnerHashAfter -and $runnerHashAfter -ne $runnerHashBefore) -or
    $testHashAfter -ne $testHashBefore)
{
    $campaignFailures.Add('test binaries changed during campaign')
}

$werCampaignFinal = @()
$newCampaignWerDumps = [Collections.Generic.List[object]]::new()
$newCampaignWerDumpKeys = [Collections.Generic.HashSet[string]]::new(
    [StringComparer]::OrdinalIgnoreCase)
try
{
    # A final full settle is required even though each process has its own
    # settle window: LocalDumps publication can lag the last iteration record.
    for ($poll = 0; $poll -le $WerSettleSeconds; ++$poll)
    {
        if ($poll -gt 0)
        {
            Start-Sleep -Seconds 1
        }
        $werCampaignFinal = @(Get-WerDumpFingerprints `
            -Directories $werDumpDirectories `
            -ExecutableBaseNames $werExecutableBaseNames)
        foreach ($newDump in @(Find-NewDumpFingerprints `
            -Before $werCampaignBaseline -After $werCampaignFinal))
        {
            $newDumpKey = Get-DumpFingerprintKey $newDump
            if ($newCampaignWerDumpKeys.Add($newDumpKey))
            {
                $newCampaignWerDumps.Add($newDump)
            }
        }
    }
}
catch
{
    $campaignFailures.Add(
        "final campaign WER inventory failed: $($_.Exception.Message)")
}
if ($newCampaignWerDumps.Count -gt 0 -and
    -not @($records | Where-Object {
        $_.NewWerDumps.Count -gt 0
    }).Count)
{
    $campaignFailures.Add(
        "$($newCampaignWerDumps.Count) late campaign WER dump fingerprint(s)")
}

$campaignEndedUtc = [DateTime]::UtcNow
$passedCount = @($records | Where-Object Status -eq 'Pass').Count
$failedCount = $records.Count - $passedCount
$campaignPassed =
    $preflightRecord.Status -eq 'Pass' -and
    $records.Count -gt 0 -and
    $failedCount -eq 0 -and
    $campaignFailures.Count -eq 0

$werDirectoryRecords = @(
    foreach ($directory in $werDumpDirectories)
    {
        [pscustomobject][ordered]@{
            Path = $directory
            Exists = Test-Path -LiteralPath $directory -PathType Container
        }
    }
)

$summary = [pscustomobject][ordered]@{
    SchemaVersion = 1
    Suite = 'wxwinui-supported-v0-supplemental-soak'
    QualificationClaim = 'supplemental-automation-only'
    PhysicalOrManualQualification = $false
    Status = if ($campaignPassed) { 'Pass' } else { 'Fail' }
    FailureReasons = @($campaignFailures)
    CampaignId = $campaignId
    StartedUtc = if ($campaignStartedUtc) {
        $campaignStartedUtc.ToString('o')
    } else {
        $null
    }
    EndedUtc = $campaignEndedUtc.ToString('o')
    ElapsedMs = [long]$campaignStopwatch.ElapsedMilliseconds
    StopReason = $stopReason
    Limits = [pscustomobject][ordered]@{
        DurationMinutes = $DurationMinutes
        Iterations = $Iterations
        PerIterationTimeoutSeconds = $TimeoutSeconds
        OuterWatchdogMarginSeconds = $outerWatchdogMarginSeconds
        WerSettleSeconds = $WerSettleSeconds
    }
    InventoryPreflight = [pscustomobject]$preflightRecord
    IterationCount = $records.Count
    PassedCount = $passedCount
    FailedCount = $failedCount
    SourceDirectory = $sourceDirectory
    BuildDirectory = $resolvedBuildDir
    BinaryDirectory = $resolvedBinaryDir
    OutputDirectory = $resolvedOutputDir
    WorkingDirectory = $workingDirectory
    TestFilter = $testFilter
    ExactCommand = $exactCommandLine
    Runner = [pscustomobject][ordered]@{
        Path = $resolvedRunnerPath
        Sha256Before = $runnerHashBefore
        Sha256After = $runnerHashAfter
    }
    TestGui = [pscustomobject][ordered]@{
        Path = $resolvedTestExecutable
        Sha256Before = $testHashBefore
        Sha256After = $testHashAfter
    }
    Harness = [pscustomobject][ordered]@{
        Path = $PSCommandPath
        Sha256Before = $scriptHashBefore
        Sha256After = $scriptHashAfter
    }
    SupportedSourceManifest = [pscustomobject][ordered]@{
        FileCount = $sourceManifestBefore.FileCount
        AggregateSha256Before = $sourceManifestBefore.AggregateSha256
        AggregateSha256After = if ($sourceManifestAfter) {
            $sourceManifestAfter.AggregateSha256
        } else {
            $null
        }
        Files = $sourceManifestBefore.Files
    }
    Wer = [pscustomobject][ordered]@{
        Handling = 'inventory-only; dumps are never moved or deleted'
        DirectoryMode = $werDirectoryMode
        ExecutableBaseNames = $werExecutableBaseNames
        Directories = $werDirectoryRecords
        CampaignBaseline = $werCampaignBaseline
        CampaignFinal = $werCampaignFinal
        NewCampaignDumps = $newCampaignWerDumps
    }
    IterationEvidence = @($recordEvidence)
    Iterations = @($records)
}

$summaryJsonPath = Join-Path $resolvedOutputDir 'summary.json'
$summaryJson = $summary | ConvertTo-Json -Depth 10
Write-NewUtf8File -Path $summaryJsonPath -Text ($summaryJson + "`n")
$summaryJsonHash = Get-FileSha256 -Path $summaryJsonPath

$textLines = [Collections.Generic.List[string]]::new()
$textLines.Add('wxWinUI Supported V0 supplemental automated soak')
$textLines.Add(
    'Claim: supplemental automation only; no physical/manual/production sign-off')
$textLines.Add("Status: $($summary.Status)")
$textLines.Add("Campaign: $campaignId")
$textLines.Add("Started UTC: $($summary.StartedUtc)")
$textLines.Add("Ended UTC: $($summary.EndedUtc)")
$textLines.Add("Elapsed ms: $($summary.ElapsedMs)")
$textLines.Add("Stop reason: $stopReason")
$textLines.Add(
    "Iterations: $($records.Count); pass=$passedCount; fail=$failedCount")
$textLines.Add("Filter: $testFilter")
$textLines.Add(
    ("Inventory preflight: {0}; expected={1}; observed={2}; exit={3}" -f
        $preflightRecord.Status,
        $preflightRecord.ExpectedTestCaseCount,
        $preflightRecord.ObservedTestCaseCount,
        $preflightRecord.RunnerExitCode))
$textLines.Add("Preflight command: $preflightExactCommandLine")
$textLines.Add("Working directory: $workingDirectory")
$textLines.Add("Exact command: $exactCommandLine")
$textLines.Add(
    "Runner SHA-256 before/after: $runnerHashBefore / $runnerHashAfter")
$textLines.Add(
    "test_gui SHA-256 before/after: $testHashBefore / $testHashAfter")
$textLines.Add(
    "Harness SHA-256 before/after: $scriptHashBefore / $scriptHashAfter")
$textLines.Add(
    'Supported source manifest SHA-256 before/after: ' +
    "$($sourceManifestBefore.AggregateSha256) / " +
    $(if ($sourceManifestAfter) {
        $sourceManifestAfter.AggregateSha256
    } else {
        '<unavailable>'
    }))
$textLines.Add("summary.json SHA-256: $summaryJsonHash")
$textLines.Add('WER handling: inventory only; no dump was moved or deleted')
$textLines.Add("WER directory mode: $werDirectoryMode")
foreach ($directory in $werDirectoryRecords)
{
    $textLines.Add(
        "WER directory (exists=$($directory.Exists)): $($directory.Path)")
}
if ($campaignFailures.Count -gt 0)
{
    $textLines.Add('Failures:')
    foreach ($failure in $campaignFailures)
    {
        $textLines.Add("  $failure")
    }
}
$textLines.Add('Iteration results:')
foreach ($record in $records)
{
    $textLines.Add(
        (('  {0:D6}: {1}; exit={2}; elapsed_ms={3}; new_wer={4}; ' +
          'stdout_sha256={5}; stderr_sha256={6}') -f
            $record.Iteration,
            $record.Status,
            $record.RunnerExitCode,
            $record.ElapsedMs,
            $record.NewWerDumps.Count,
            $record.StdoutSha256,
            $record.StderrSha256))
}

$summaryTextPath = Join-Path $resolvedOutputDir 'summary.txt'
Write-NewUtf8File -Path $summaryTextPath -Text (
    ($textLines -join [Environment]::NewLine) + [Environment]::NewLine)
$summaryTextHash = Get-FileSha256 -Path $summaryTextPath

Write-Host "Supported V0 supplemental soak: $($summary.Status)"
Write-Host "Iterations: $($records.Count) ($passedCount pass, $failedCount fail)"
Write-Host "Evidence: $resolvedOutputDir"
Write-Host "summary.json SHA-256: $summaryJsonHash"
Write-Host "summary.txt SHA-256:  $summaryTextHash"

if ($campaignPassed)
{
    $scriptExitCode = 0
}
elseif ($preflightRecord.Status -ne 'Pass' -or
        @($records | Where-Object Status -eq 'InfrastructureFailure').Count `
            -gt 0)
{
    $scriptExitCode = 4
}
elseif ($newCampaignWerDumps.Count -gt 0 -or
    @($records | Where-Object Status -eq 'WerDumpDetected').Count -gt 0)
{
    $scriptExitCode = 3
}
elseif (@($records | Where-Object Status -eq 'Timeout').Count -gt 0)
{
    $scriptExitCode = 2
}
elseif (@($records | Where-Object Status -eq 'TestFailure').Count -gt 0)
{
    $scriptExitCode = 1
}
}
catch
{
    $scriptExitCode = 4
    [Console]::Error.WriteLine(
        "Supported V0 soak infrastructure failure: $($_.Exception.Message)")
}
finally
{
    # This outermost guard runs for terminating errors and cancellation too.
    # Never discard a live Process object: retry the exact handle and only
    # dispose it after HasExited is observable.
    if ($activeProcess)
    {
        try
        {
            $activeProcess.Refresh()
            if (-not $activeProcess.HasExited)
            {
                [void](Stop-ExactProcess -Process $activeProcess)
            }
            $activeProcess.Refresh()
            if ($activeProcess.HasExited)
            {
                $activeProcess.Dispose()
                $activeProcess = $null
            }
        }
        catch
        {
            # Keep the exact handle referenced until script teardown.
        }
    }
    if ($campaignLock)
    {
        $campaignLock.Dispose()
        $campaignLock = $null
    }
}

exit $scriptExitCode
