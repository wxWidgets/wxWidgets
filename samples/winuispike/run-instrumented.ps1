# Start one WinUI executable with all diagnostics pointing at one run directory.

[CmdletBinding()]
param(
    [string] $BuildDir = (Join-Path $PSScriptRoot '..\..\build-winui-clean'),
    [string] $BinaryDir,
    [string] $Executable = 'winuispike.exe',
    [string] $OutputDir,
    [string[]] $ArgumentList,
    [ValidateRange(1, 86400)]
    [int] $TimeoutSeconds = 1800
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function ConvertTo-NativeArgument
{
    param([AllowEmptyString()][string] $Value)

    if ($Value.Length -gt 0 -and $Value -notmatch '[\s"]')
    {
        return $Value
    }

    # CommandLineToArgvW-compatible quoting, including runs of backslashes
    # immediately before a quote or the closing quote.
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

function Test-PathUnder
{
    param(
        [Parameter(Mandatory = $true)][string] $Path,
        [Parameter(Mandatory = $true)][string] $Directory
    )

    $resolvedPath = [IO.Path]::GetFullPath($Path)
    $resolvedDirectory = [IO.Path]::GetFullPath($Directory)
    $resolvedDirectory =
        $resolvedDirectory.TrimEnd([IO.Path]::DirectorySeparatorChar) +
            [IO.Path]::DirectorySeparatorChar
    return $resolvedPath.StartsWith(
        $resolvedDirectory, [StringComparison]::OrdinalIgnoreCase)
}

function Resolve-FullPath
{
    param(
        [Parameter(Mandatory = $true)][string] $Path,
        [Parameter(Mandatory = $true)][string] $Base
    )

    if ([IO.Path]::IsPathRooted($Path))
    {
        return [IO.Path]::GetFullPath($Path)
    }
    return [IO.Path]::GetFullPath((Join-Path $Base $Path))
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

function Write-TextAtomically
{
    param(
        [Parameter(Mandatory = $true)][string] $Path,
        [Parameter(Mandatory = $true)][AllowEmptyString()][string] $Contents
    )

    $resolvedPath = [IO.Path]::GetFullPath($Path)
    $directory = [IO.Path]::GetDirectoryName($resolvedPath)
    if (-not (Test-Path -LiteralPath $directory -PathType Container))
    {
        throw "Atomic-write directory not found: $directory"
    }

    $temporary = Join-Path $directory (
        '.' + [IO.Path]::GetFileName($resolvedPath) +
        ".tmp-$PID-$([guid]::NewGuid().ToString('N'))")
    $backup = Join-Path $directory (
        '.' + [IO.Path]::GetFileName($resolvedPath) +
        ".bak-$PID-$([guid]::NewGuid().ToString('N'))")
    try
    {
        $utf8 = [Text.UTF8Encoding]::new($false)
        [IO.File]::WriteAllText($temporary, $Contents, $utf8)
        $published = $false
        for ($attempt = 1; $attempt -le 5 -and -not $published; ++$attempt)
        {
            try
            {
                if ([IO.File]::Exists($resolvedPath))
                {
                    [IO.File]::Replace(
                        $temporary, $resolvedPath, $backup, $true)
                }
                else
                {
                    [IO.File]::Move($temporary, $resolvedPath)
                }
                $published = $true
            }
            catch [IO.IOException]
            {
                if ($attempt -eq 5)
                {
                    throw
                }
                # Get-Content may hold a short-lived non-delete-sharing handle
                # while the collector reads the previous complete manifest.
                Start-Sleep -Milliseconds 50
            }
        }
    }
    finally
    {
        if (Test-Path -LiteralPath $temporary -PathType Leaf)
        {
            Remove-Item -LiteralPath $temporary -Force
        }
        if (Test-Path -LiteralPath $backup -PathType Leaf)
        {
            Remove-Item -LiteralPath $backup -Force
        }
    }
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
            # Kill through the retained Process object/handle, never by a
            # numeric PID that could have been recycled.
            $Process.Kill()
        }
        catch
        {
            $killError = $_.Exception.Message
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
            [Console]::Error.WriteLine(
                "WARNING: failed to terminate child process " +
                "(attempt $attempt): $killError")
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
        Join-Path $resolvedBuildDir 'lib\vc_x64_dll'
    }
if (-not (Test-Path -LiteralPath $binaryDir -PathType Container))
{
    throw "Binary directory not found: $binaryDir"
}
if (-not (Test-PathUnder -Path $binaryDir -Directory $resolvedBuildDir))
{
    throw "Binary directory must belong to this build: $resolvedBuildDir"
}
if ([IO.Path]::IsPathRooted($Executable))
{
    $executablePath = [IO.Path]::GetFullPath($Executable)
}
else
{
    $executablePath = Join-Path $binaryDir $Executable
}

if (-not (Test-Path -LiteralPath $executablePath -PathType Leaf))
{
    throw "Executable not found: $executablePath"
}
$executablePath = (Resolve-Path -LiteralPath $executablePath).Path

if (-not (Test-PathUnder -Path $executablePath -Directory $binaryDir))
{
    throw "Executable must belong to this build: $binaryDir"
}

$runId =
    (Get-Date -Format 'yyyyMMdd-HHmmss-fff') + '-' +
    [guid]::NewGuid().ToString('N').Substring(0, 8)
if ([string]::IsNullOrWhiteSpace($OutputDir))
{
    $OutputDir =
        Join-Path $resolvedBuildDir "logs\winui-freeze\$runId"
}

$resolvedOutputDir =
    Resolve-FullPath -Path $OutputDir -Base $resolvedBuildDir
if (Test-Path -LiteralPath $resolvedOutputDir)
{
    if (-not (Test-Path -LiteralPath $resolvedOutputDir -PathType Container))
    {
        throw "Output path is not a directory: $resolvedOutputDir"
    }
    if (Get-ChildItem -LiteralPath $resolvedOutputDir -Force |
            Select-Object -First 1)
    {
        throw "Output directory is not empty; select a fresh run directory: $resolvedOutputDir"
    }
}
else
{
    New-Item -ItemType Directory -Path $resolvedOutputDir | Out-Null
}

$runsRoot = Join-Path $resolvedBuildDir 'logs\winui-freeze'
New-Item -ItemType Directory -Path $runsRoot -Force | Out-Null

$executableHash = (Get-FileHash -LiteralPath $executablePath `
    -Algorithm SHA256).Hash.ToLowerInvariant()
$variables = @{
    WX_WINUI_DIAGNOSTICS = '1'
    WX_WINUI_SPIKE_DIAGNOSTICS = '1'
    WX_WINUI_DIAGNOSTICS_DIR = $resolvedOutputDir
    WX_WINUI_INPUT_LOG = (Join-Path $resolvedOutputDir 'input-router.txt')
}
$activeRunPath = $null
$manifestPath = Join-Path $resolvedOutputDir 'run-manifest.json'
$manifest = $null
$manifestArguments = if ($ArgumentList) { @($ArgumentList) } else { @() }
$exitCode = 0
$process = $null
$processStarted = $false
$runnerError = $null
$finalizationError = $null
$outputLock =
    Enter-OutputDirectoryLock `
        -Directory $resolvedOutputDir -Owner 'campaign'

try
{
    $startInfo = [Diagnostics.ProcessStartInfo]::new()
    $startInfo.FileName = $executablePath
    $startInfo.WorkingDirectory =
        [IO.Path]::GetDirectoryName($executablePath)
    $startInfo.UseShellExecute = $false
    $startInfo.CreateNoWindow = $true
    if ($ArgumentList)
    {
        $startInfo.Arguments =
            (($ArgumentList |
                ForEach-Object { ConvertTo-NativeArgument $_ }) -join ' ')
    }
    foreach ($entry in $variables.GetEnumerator())
    {
        $startInfo.EnvironmentVariables[$entry.Key] = $entry.Value
    }

    Write-Host "Diagnostic run directory: $resolvedOutputDir"
    $process = [Diagnostics.Process]::new()
    $process.StartInfo = $startInfo
    if (-not $process.Start())
    {
        throw 'Process.Start() returned false.'
    }
    $processStarted = $true
    $processStartTime = [DateTimeOffset]$process.StartTime

    $manifest = [ordered]@{
        SchemaVersion = 2
        RunId = $runId
        ProcessId = $process.Id
        BuildDir = $resolvedBuildDir
        OutputDir = $resolvedOutputDir
        Executable = $executablePath
        ExecutableSha256 = $executableHash
        Arguments = $manifestArguments
        TimeoutSeconds = $TimeoutSeconds
        StartedAt = $processStartTime.ToString(
            'o', [Globalization.CultureInfo]::InvariantCulture)
        EndedAt = $null
        ExitCode = $null
        ProcessExitCode = $null
        Outcome = 'Running'
        WatchdogAction = $null
        RunnerError = $null
    }

    $activeRunPath =
        Join-Path $runsRoot "active-run-$($process.Id).json"
    $manifestJson = $manifest | ConvertTo-Json -Depth 4
    Write-TextAtomically -Path $manifestPath -Contents $manifestJson
    Write-TextAtomically -Path $activeRunPath -Contents $manifestJson

    # The validated maximum (86,400 s) remains within Int32 milliseconds.
    [int] $waitMilliseconds = $TimeoutSeconds * 1000
    if ($process.WaitForExit($waitMilliseconds))
    {
        $exitCode = $process.ExitCode
        $manifest.ProcessExitCode = $process.ExitCode
        $manifest.Outcome = 'Exited'
    }
    else
    {
        # 124 is the conventional timeout status. First request a normal
        # close, then force termination after a separate, bounded grace
        # period. Never leave the runner blocked forever in WaitForExit().
        $exitCode = 124
        $manifest.Outcome = 'TimedOut'
        $manifest.WatchdogAction = 'CloseMainWindow'
        [Console]::Error.WriteLine(
            "ERROR: process $($process.Id) exceeded the " +
            "$TimeoutSeconds-second watchdog; requesting a clean close. " +
            "Diagnostics: $resolvedOutputDir")

        $closeRequested = $false
        try
        {
            $closeRequested = $process.CloseMainWindow()
        }
        catch
        {
            [Console]::Error.WriteLine(
                "WARNING: CloseMainWindow failed for process " +
                "$($process.Id): $($_.Exception.Message)")
        }

        if ($closeRequested)
        {
            [void]$process.WaitForExit(5000)
        }

        if (-not $process.HasExited)
        {
            $manifest.WatchdogAction = 'KillProcess'
            if (-not (Stop-ExactProcess -Process $process))
            {
                [Console]::Error.WriteLine(
                    "ERROR: forced termination failed for process " +
                    "$($process.Id) after three bounded attempts.")
            }
        }

        if ($process.HasExited)
        {
            $manifest.ProcessExitCode = $process.ExitCode
            [Console]::Error.WriteLine(
                "ERROR: watchdog terminated process $($process.Id); " +
                "runner exit code is 124.")
        }
        else
        {
            $exitCode = 125
            $manifest.Outcome = 'WatchdogFailed'
            $manifest.WatchdogAction = 'ProcessStillRunning'
            [Console]::Error.WriteLine(
                "ERROR: process $($process.Id) is still running after " +
                "the watchdog; runner exit code is 125. Collect the run " +
                "directory, then verify the manifest path/start identity " +
                "before manually terminating PID $($process.Id).")
        }
    }

    $manifest.EndedAt = Get-Date -Format o
    $manifest.ExitCode = $exitCode
}
catch
{
    $runnerError = $_.Exception.Message
    $exitCode = 1
    [Console]::Error.WriteLine("ERROR: diagnostic runner failed: $runnerError")

    if ($manifest)
    {
        $manifest.Outcome = 'RunnerFailed'
        $manifest.RunnerError = $runnerError
    }

    if ($processStarted)
    {
        $stillRunning = $false
        try
        {
            $process.Refresh()
            $stillRunning = -not $process.HasExited
        }
        catch
        {
            $stillRunning = $true
        }

        if ($stillRunning)
        {
            if ($manifest)
            {
                $manifest.WatchdogAction = 'RunnerFailureKill'
            }

            if (-not (Stop-ExactProcess -Process $process))
            {
                $exitCode = 125
                if ($manifest)
                {
                    $manifest.Outcome = 'WatchdogFailed'
                    $manifest.WatchdogAction = 'ProcessStillRunning'
                }
                [Console]::Error.WriteLine(
                    "ERROR: runner failure cleanup could not terminate exact " +
                    "process $($process.Id); exit code is 125.")
            }
            elseif ($manifest)
            {
                try
                {
                    $manifest.ProcessExitCode = $process.ExitCode
                }
                catch
                {
                }
            }
        }
        elseif ($manifest)
        {
            try
            {
                $manifest.ProcessExitCode = $process.ExitCode
            }
            catch
            {
            }
        }
    }
}
finally
{
    try
    {
    # PowerShell pipeline interruption (for example Ctrl+C) is not guaranteed
    # to enter the catch block, so make the outer finally independently uphold
    # the no-orphan contract.
    if ($processStarted -and $exitCode -ne 125)
    {
        $processAlive = $false
        try
        {
            $process.Refresh()
            $processAlive = -not $process.HasExited
        }
        catch
        {
            $processAlive = $true
        }

        if ($processAlive)
        {
            if ($manifest)
            {
                $manifest.Outcome = 'RunnerFailed'
                $manifest.WatchdogAction = 'FinallyKill'
                if (-not $manifest.RunnerError)
                {
                    $manifest.RunnerError =
                        'runner interrupted while the child was still alive'
                }
            }
            if (-not (Stop-ExactProcess -Process $process))
            {
                $exitCode = 125
                if ($manifest)
                {
                    $manifest.Outcome = 'WatchdogFailed'
                    $manifest.WatchdogAction = 'ProcessStillRunning'
                }
                [Console]::Error.WriteLine(
                    "ERROR: outer-finally cleanup could not terminate exact " +
                    "process $($process.Id); exit code is 125.")
            }
            else
            {
                if ($manifest)
                {
                    try
                    {
                        $manifest.ProcessExitCode = $process.ExitCode
                    }
                    catch
                    {
                    }
                }
                if ($exitCode -eq 0)
                {
                    $exitCode = 1
                }
            }
        }
    }

    if ($manifest)
    {
        if (-not $manifest.EndedAt)
        {
            $manifest.EndedAt = Get-Date -Format o
        }
        $manifest.ExitCode = $exitCode
        if ($manifest.Outcome -eq 'Running')
        {
            $manifest.Outcome = 'RunnerFailed'
        }
        $manifestJson = $manifest | ConvertTo-Json -Depth 4
        try
        {
            Write-TextAtomically -Path $manifestPath -Contents $manifestJson

            $processAlive = $false
            if ($processStarted)
            {
                try
                {
                    $process.Refresh()
                    $processAlive = -not $process.HasExited
                }
                catch
                {
                    $processAlive = $exitCode -eq 125
                }
            }

            if ($processAlive)
            {
                Write-TextAtomically `
                    -Path $activeRunPath -Contents $manifestJson
            }
            elseif ($activeRunPath -and
                    (Test-Path -LiteralPath $activeRunPath -PathType Leaf))
            {
                Remove-Item -LiteralPath $activeRunPath -Force
            }
        }
        catch
        {
            $finalizationError = $_.Exception.Message
            [Console]::Error.WriteLine(
                "ERROR: manifest finalization failed: $finalizationError")
            if ($exitCode -eq 0)
            {
                $exitCode = 1
                $manifest.Outcome = 'RunnerFailed'
            }
            $manifest.ExitCode = $exitCode
            $manifest.RunnerError = $finalizationError
            try
            {
                $manifestJson = $manifest | ConvertTo-Json -Depth 4
                Write-TextAtomically `
                    -Path $manifestPath -Contents $manifestJson
            }
            catch
            {
            }
        }
    }

    if ($process)
    {
        $process.Dispose()
    }
    }
    finally
    {
        Exit-OutputDirectoryLock -Lock $outputLock
    }
}

exit $exitCode
