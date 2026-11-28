# Run the deterministic, input-free phase-007c topology/clip/splitter gate.
[CmdletBinding()]
param(
    [string]$BuildDir = 'build-winui-clean',
    [string]$BinaryDir = 'lib\vc_x64_dll',
    [string]$OutputDir,
    [ValidateRange(5, 120)]
    [int]$TimeoutSeconds = 30
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

function ConvertTo-NativeArgument
{
    param([AllowEmptyString()][string]$Value)

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

function Resolve-FullPath
{
    param([Parameter(Mandatory)][string]$Path, [string]$Base)

    if ([System.IO.Path]::IsPathRooted($Path))
    {
        return [System.IO.Path]::GetFullPath($Path)
    }
    return [System.IO.Path]::GetFullPath((Join-Path $Base $Path))
}

function Test-PathUnder
{
    param(
        [Parameter(Mandatory)][string]$Path,
        [Parameter(Mandatory)][string]$Directory
    )

    $resolvedPath = [IO.Path]::GetFullPath($Path)
    $resolvedDirectory = [IO.Path]::GetFullPath($Directory)
    $resolvedDirectory =
        $resolvedDirectory.TrimEnd([IO.Path]::DirectorySeparatorChar) +
            [IO.Path]::DirectorySeparatorChar
    return $resolvedPath.StartsWith(
        $resolvedDirectory, [StringComparison]::OrdinalIgnoreCase)
}

function Enter-OutputDirectoryLock
{
    param(
        [Parameter(Mandatory)][string]$Directory,
        [Parameter(Mandatory)][string]$Owner
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
    param([Parameter(Mandatory)]$Lock)

    $Lock.Stream.Dispose()
    Remove-Item -LiteralPath $Lock.Path -Force
}

function Stop-ExactProcess
{
    param([Parameter(Mandatory)][System.Diagnostics.Process]$Process)

    for ($attempt = 1; $attempt -le 3; ++$attempt)
    {
        try
        {
            $Process.Refresh()
            if ($Process.HasExited)
            {
                return $true
            }
            $Process.Kill()
            if ($Process.WaitForExit(2000))
            {
                return $true
            }
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
    }
    return $false
}

function Write-JsonUtf8Atomic
{
    param(
        [Parameter(Mandatory)][string]$Path,
        [Parameter(Mandatory)]$Value
    )

    $resolvedPath = [IO.Path]::GetFullPath($Path)
    $directory = [IO.Path]::GetDirectoryName($resolvedPath)
    $temporary = Join-Path $directory (
        '.' + [IO.Path]::GetFileName($resolvedPath) +
        ".tmp-$PID-$([guid]::NewGuid().ToString('N'))")
    try
    {
        $json = $Value | ConvertTo-Json -Depth 10
        [IO.File]::WriteAllText(
            $temporary, $json, [Text.UTF8Encoding]::new($false))
        [IO.File]::Move($temporary, $resolvedPath)
    }
    finally
    {
        if (Test-Path -LiteralPath $temporary -PathType Leaf)
        {
            Remove-Item -LiteralPath $temporary -Force
        }
    }
}

function Get-TextSha256
{
    param([Parameter(Mandatory)][string]$Text)

    $algorithm = [Security.Cryptography.SHA256]::Create()
    try
    {
        $bytes = [Text.Encoding]::UTF8.GetBytes($Text)
        return ([BitConverter]::ToString(
            $algorithm.ComputeHash($bytes))).Replace('-', '').ToLowerInvariant()
    }
    finally
    {
        $algorithm.Dispose()
    }
}

$repoRoot = [System.IO.Path]::GetFullPath(
    (Join-Path $PSScriptRoot '..\..'))
$resolvedBuildDir = Resolve-FullPath -Path $BuildDir -Base $repoRoot
if (-not (Test-Path -LiteralPath $resolvedBuildDir -PathType Container))
{
    throw "BuildDir not found: '$resolvedBuildDir'"
}
$resolvedBinaryDir = Resolve-FullPath -Path $BinaryDir -Base $resolvedBuildDir
if (-not (Test-Path -LiteralPath $resolvedBinaryDir -PathType Container))
{
    throw "BinaryDir not found: '$resolvedBinaryDir'"
}
if (-not (Test-PathUnder `
        -Path $resolvedBinaryDir -Directory $resolvedBuildDir))
{
    throw "BinaryDir must belong to BuildDir: '$resolvedBuildDir'"
}
$executable = Join-Path $resolvedBinaryDir 'winuispike.exe'
if (-not (Test-Path -LiteralPath $executable -PathType Leaf))
{
    throw "winuispike.exe not found at '$executable'"
}

if ([string]::IsNullOrWhiteSpace($OutputDir))
{
    $campaign =
        '{0:yyyyMMdd-HHmmss-fff}-{1}' -f (Get-Date),
            [guid]::NewGuid().ToString('N').Substring(0, 8)
    $OutputDir =
        Join-Path $resolvedBuildDir "logs\winui-zorder-scenarios\$campaign"
}
$resolvedOutputDir =
    Resolve-FullPath -Path $OutputDir -Base $resolvedBuildDir
if (Test-Path -LiteralPath $resolvedOutputDir)
{
    if (@(Get-ChildItem -LiteralPath $resolvedOutputDir -Force).Count -ne 0)
    {
        throw "OutputDir must be new or empty: '$resolvedOutputDir'"
    }
}
else
{
    [void](New-Item -ItemType Directory -Path $resolvedOutputDir)
}
$outputLock =
    Enter-OutputDirectoryLock `
        -Directory $resolvedOutputDir -Owner 'campaign'

$process = $null
$scriptExitCode = 1
try
{
$resultPath = Join-Path $resolvedOutputDir 'result.json'
$hashBefore = (Get-FileHash -LiteralPath $executable -Algorithm SHA256).Hash
$startUtc = [DateTime]::UtcNow.ToString('o')

$startInfo = [System.Diagnostics.ProcessStartInfo]::new()
$startInfo.FileName = $executable
$startInfo.WorkingDirectory = $resolvedBinaryDir
$startInfo.UseShellExecute = $false
$arguments = @(
    '--zorder-scenarios',
    "--scenario-result=$resultPath",
    "--scenario-timeout-ms=$([Math]::Max(1000, ($TimeoutSeconds - 2) * 1000))"
)
$startInfo.Arguments =
    (($arguments | ForEach-Object { ConvertTo-NativeArgument $_ }) -join ' ')
$startInfo.CreateNoWindow = $true
$startInfo.WindowStyle = [Diagnostics.ProcessWindowStyle]::Hidden
foreach ($name in @(
    'WX_WINUI_DIAGNOSTICS',
    'WX_WINUI_SPIKE_DIAGNOSTICS',
    'WX_WINUI_DIAGNOSTICS_DIR',
    'WX_WINUI_INPUT_LOG'))
{
    [void]$startInfo.EnvironmentVariables.Remove($name)
}

$process = [System.Diagnostics.Process]::new()
$process.StartInfo = $startInfo
$processStarted = $false
$processId = $null
$timedOut = $false
$cleanupOk = $true
$childError = $null
$exitCode = 1
try
{
    if (-not $process.Start())
    {
        throw 'Failed to start winuispike.exe'
    }
    $processStarted = $true
    $processId = $process.Id

    $timedOut = -not $process.WaitForExit($TimeoutSeconds * 1000)
    if ($timedOut)
    {
        $cleanupOk = Stop-ExactProcess -Process $process
    }
}
catch
{
    $childError = $_.Exception.Message
}
finally
{
    if ($processStarted)
    {
        $childAlive = $true
        try
        {
            $process.Refresh()
            $childAlive = -not $process.HasExited
        }
        catch
        {
        }

        if ($childAlive)
        {
            $cleanupOk =
                (Stop-ExactProcess -Process $process) -and $cleanupOk
            try
            {
                $process.Refresh()
                $childAlive = -not $process.HasExited
            }
            catch
            {
                $childAlive = $true
            }
        }

        if ($childAlive)
        {
            $cleanupOk = $false
            $exitCode = 125
        }
        else
        {
            # Stop-ExactProcess can exhaust its wait just before the process
            # reports exit. The final retained-handle refresh is authoritative:
            # 125 is reserved for a child which still survives here.
            $cleanupOk = $true
            $exitCode = $process.ExitCode
        }
    }
}
$hashAfter = (Get-FileHash -LiteralPath $executable -Algorithm SHA256).Hash

$result = $null
$gatePassed = $false
$failure = ''
$semanticSha256 = $null
if (-not $cleanupOk)
{
    $failure = "exact child process $processId survived bounded cleanup"
}
elseif ($childError)
{
    $failure = "scenario runner failed: $childError"
}
elseif ($timedOut)
{
    $failure = 'process watchdog expired'
}
elseif (-not (Test-Path -LiteralPath $resultPath -PathType Leaf))
{
    $failure = 'scenario process wrote no result file'
}
else
{
    try
    {
        $result = Get-Content -LiteralPath $resultPath -Raw | ConvertFrom-Json
        $expectedChecks = @(
            'initial-xaml-sibling-order',
            'raise-follows-native-sibling-order',
            'lower-follows-native-sibling-order',
            'impossible-native-xaml-interleave-signature',
            'ancestor-clip',
            'hidden-slot-collapses',
            'scroll-viewport-clip',
            'tabview-band-clip',
            'splitter-horizontal-vertical-geometry'
        )
        $actualChecks = @($result.checks)
        $checkNamesMatch = $actualChecks.Count -eq $expectedChecks.Count
        if ($checkNamesMatch)
        {
            for ($index = 0; $index -lt $expectedChecks.Count; ++$index)
            {
                if ($actualChecks[$index].name -cne $expectedChecks[$index])
                {
                    $checkNamesMatch = $false
                    break
                }
            }
        }
        $gatePassed =
            $exitCode -eq 0 -and
            $hashBefore -eq $hashAfter -and
            $result.pid -eq $processId -and
            $result.exit_code -eq $exitCode -and
            $result.schema_version -eq 1 -and
            $result.suite -ceq 'winui-zorder-clip-splitter' -and
            $result.status -ceq 'pass' -and
            $result.window_activated -eq $false -and
            $result.window_offscreen -eq $true -and
            $checkNamesMatch -and
            $actualChecks[3].signature -ceq 'A>N>B' -and
            $actualChecks[3].diagnostic_count -eq 1 -and
            @($actualChecks |
                Where-Object { $_.status -cne 'pass' }).Count -eq 0
        if (-not $gatePassed)
        {
            $failure = 'process/result/schema/check gate mismatch'
        }
        $semantic = [ordered]@{
            schema_version = $result.schema_version
            suite = $result.suite
            scale = $result.scale
            checks = @($result.checks)
            manual_gates = @($result.manual_gates)
        }
        $semanticSha256 = Get-TextSha256 (
            $semantic | ConvertTo-Json -Depth 10 -Compress)
    }
    catch
    {
        $failure = "invalid result JSON: $($_.Exception.Message)"
    }
}

$summary = [ordered]@{
    schema_version = 1
    suite = 'winui-zorder-clip-splitter-campaign'
    status = if ($gatePassed) { 'pass' } elseif ($timedOut) { 'timeout' } else { 'fail' }
    started_utc = $startUtc
    executable = $executable
    executable_sha256_before = $hashBefore
    executable_sha256_after = $hashAfter
    process_id = $processId
    orphan_process_id = if ($cleanupOk) { $null } else { $processId }
    process_exit_code = $exitCode
    watchdog_seconds = $TimeoutSeconds
    watchdog_expired = $timedOut
    cleanup_succeeded = $cleanupOk
    result_file = $resultPath
    result_sha256 = if (Test-Path -LiteralPath $resultPath) {
        (Get-FileHash -LiteralPath $resultPath -Algorithm SHA256).Hash
    } else { $null }
    semantic_sha256 = $semanticSha256
    failure = $failure
}
$summaryPath = Join-Path $resolvedOutputDir 'summary.json'
Write-JsonUtf8Atomic -Path $summaryPath -Value $summary

Write-Host "Z-order scenario summary: $summaryPath"
if ($gatePassed)
{
    $scriptExitCode = 0
}
elseif ($timedOut -and $cleanupOk)
{
    $scriptExitCode = 124
}
elseif (-not $cleanupOk)
{
    $scriptExitCode = 125
}
}
finally
{
    if ($process)
    {
        $process.Dispose()
    }
    Exit-OutputDirectoryLock -Lock $outputLock
}

exit $scriptExitCode
