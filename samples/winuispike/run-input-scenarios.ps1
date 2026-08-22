# Run the deterministic WinUI root-pointer scenario suite repeatedly.
#
# Each iteration is a new, watchdog-bounded process. The executable creates
# only an off-screen WS_EX_NOACTIVATE TLW and drives normalized samples through
# the production root-input seam: no SendInput, cursor movement, foreground
# activation, or timer-driven animation is used.

[CmdletBinding()]
param(
    [string] $BuildDir = (Join-Path $PSScriptRoot '..\..\build-winui-clean'),
    [string] $BinaryDir,
    [string] $Executable = 'winuispike.exe',
    [string] $OutputDir,
    [ValidateRange(1, 10000)]
    [int] $Iterations = 100,
    [ValidateRange(2, 300)]
    [int] $TimeoutSeconds = 15,
    [ValidateRange(1000, 60000)]
    [int] $ScenarioTimeoutMs = 8000
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

function Get-TextSha256
{
    param([Parameter(Mandatory = $true)][string] $Text)

    $sha = [Security.Cryptography.SHA256]::Create()
    try
    {
        $bytes = [Text.Encoding]::UTF8.GetBytes($Text)
        return ([BitConverter]::ToString(
            $sha.ComputeHash($bytes))).Replace('-', '').ToLowerInvariant()
    }
    finally
    {
        $sha.Dispose()
    }
}

function Test-JsonInteger
{
    param($Value)

    return $Value -is [int] -or $Value -is [long] -or
           $Value -is [uint32] -or $Value -is [uint64]
}

function Stop-ExactProcess
{
    param(
        [Parameter(Mandatory = $true)]
        [Diagnostics.Process] $Process
    )

    for ($attempt = 1; $attempt -le 3; ++$attempt)
    {
        try
        {
            $Process.Refresh()
            if ($Process.HasExited)
            {
                return $true
            }
            # Kill via the retained Process object/handle. Stop-Process -Id
            # would leave a narrow PID-reuse race.
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
                "WARNING: failed to terminate the exact scenario child " +
                "(attempt $attempt): $killError")
        }
        try
        {
            if ($Process.WaitForExit(2000))
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

$watchdogMarginMs = 5000
if ($TimeoutSeconds * 1000 -lt $ScenarioTimeoutMs + $watchdogMarginMs)
{
    throw "TimeoutSeconds must leave at least ${watchdogMarginMs}ms after " +
          'ScenarioTimeoutMs for process/XAML startup and result publication.'
}

$resolvedBuildDir = [IO.Path]::GetFullPath($BuildDir)
if (-not (Test-Path -LiteralPath $resolvedBuildDir -PathType Container))
{
    throw "Build directory not found: $resolvedBuildDir"
}

$resolvedBinaryDir =
    if ($BinaryDir)
    {
        [IO.Path]::GetFullPath($BinaryDir)
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
if (-not (Test-PathUnder -Path $resolvedBinaryDir `
                         -Directory $resolvedBuildDir))
{
    throw "Binary directory must belong to this build: $resolvedBuildDir"
}

$executablePath =
    if ([IO.Path]::IsPathRooted($Executable))
    {
        [IO.Path]::GetFullPath($Executable)
    }
    else
    {
        [IO.Path]::GetFullPath(
            (Join-Path $resolvedBinaryDir $Executable))
    }
if (-not (Test-Path -LiteralPath $executablePath -PathType Leaf))
{
    throw "Executable not found: $executablePath"
}
$executablePath = (Resolve-Path -LiteralPath $executablePath).Path
if (-not (Test-PathUnder -Path $executablePath `
                         -Directory $resolvedBinaryDir))
{
    throw "Executable must belong to this binary directory: $resolvedBinaryDir"
}

$campaignId =
    (Get-Date -Format 'yyyyMMdd-HHmmss-fff') + '-' +
    [guid]::NewGuid().ToString('N').Substring(0, 8)
if (-not $OutputDir)
{
    $OutputDir = Join-Path $resolvedBuildDir `
        "logs\winui-input-scenarios\$campaignId"
}
$resolvedOutputDir = [IO.Path]::GetFullPath($OutputDir)
if (Test-Path -LiteralPath $resolvedOutputDir)
{
    if (-not (Test-Path -LiteralPath $resolvedOutputDir -PathType Container))
    {
        throw "Output path is not a directory: $resolvedOutputDir"
    }
    if (Get-ChildItem -LiteralPath $resolvedOutputDir -Force |
            Select-Object -First 1)
    {
        throw "Output directory is not empty; select a fresh directory: $resolvedOutputDir"
    }
}
else
{
    New-Item -ItemType Directory -Path $resolvedOutputDir -Force |
        Out-Null
}

$binaryHashBefore = (Get-FileHash -LiteralPath $executablePath `
    -Algorithm SHA256).Hash.ToLowerInvariant()
$campaignStarted = Get-Date
$records = [Collections.Generic.List[object]]::new()
$aggregateLines = [Collections.Generic.List[string]]::new()
$activeProcess = $null
$orphanProcessIds = [Collections.Generic.List[int]]::new()
$infrastructureFailure = $null
$sawFailure = $false
$sawTimeout = $false
$cleanupFailed = $false
$stopCampaign = $false
$expectedScenarioNames = @(
    'move-fixed-pixel',
    'move-two-pixel-oscillation',
    'client-nonclient-crossing',
    'native-xaml-logical-crossing',
    'root-boundary-enter-exit',
    'post-callback-refresh-failure',
    'setcursor-reentry',
    'release-outside-under-native-capture',
    'native-capture-lost-late-release'
)

# Keep opt-in freeze instrumentation out of this deterministic campaign even
# if the calling shell has it enabled. Only the child environment is changed.
$diagnosticVariables = @(
    'WX_WINUI_DIAGNOSTICS',
    'WX_WINUI_SPIKE_DIAGNOSTICS',
    'WX_WINUI_DIAGNOSTICS_DIR',
    'WX_WINUI_INPUT_LOG'
)

try
{
    for ($iteration = 1; $iteration -le $Iterations; ++$iteration)
    {
        $tag = '{0:D4}' -f $iteration
        $resultPath = Join-Path $resolvedOutputDir "result-$tag.json"
        $stdoutPath = Join-Path $resolvedOutputDir "stdout-$tag.txt"
        $stderrPath = Join-Path $resolvedOutputDir "stderr-$tag.txt"
        $arguments = @(
            '--input-scenarios',
            "--scenario-result=$resultPath",
            "--scenario-timeout-ms=$ScenarioTimeoutMs"
        )

        $startInfo = [Diagnostics.ProcessStartInfo]::new()
        $startInfo.FileName = $executablePath
        $startInfo.WorkingDirectory =
            [IO.Path]::GetDirectoryName($executablePath)
        $startInfo.Arguments =
            (($arguments |
                ForEach-Object { ConvertTo-NativeArgument $_ }) -join ' ')
        $startInfo.UseShellExecute = $false
        $startInfo.CreateNoWindow = $true
        $startInfo.WindowStyle =
            [Diagnostics.ProcessWindowStyle]::Hidden
        $startInfo.RedirectStandardOutput = $true
        $startInfo.RedirectStandardError = $true
        foreach ($name in $diagnosticVariables)
        {
            [void]$startInfo.EnvironmentVariables.Remove($name)
        }

        $record = [ordered]@{
            Iteration = $iteration
            ProcessId = $null
            StartedAt = (Get-Date -Format o)
            ElapsedMs = $null
            TimedOut = $false
            ProcessExitCode = $null
            ResultExitCode = $null
            ResultStatus = $null
            ScenarioCount = $null
            WindowActivated = $null
            ResultSha256 = $null
            SemanticSha256 = $null
            StdoutSha256 = $null
            StderrSha256 = $null
            ExecutableSha256Before = $null
            ExecutableSha256After = $null
            Status = 'Running'
            Error = $null
        }

        $stopwatch = [Diagnostics.Stopwatch]::StartNew()
        $processStarted = $false
        try
        {
            $record.ExecutableSha256Before =
                (Get-FileHash -LiteralPath $executablePath `
                    -Algorithm SHA256).Hash.ToLowerInvariant()
            if ($record.ExecutableSha256Before -ne $binaryHashBefore)
            {
                throw 'winuispike.exe changed before this iteration'
            }

            $activeProcess = [Diagnostics.Process]::new()
            $activeProcess.StartInfo = $startInfo
            if (-not $activeProcess.Start())
            {
                throw 'Process.Start() returned false.'
            }
            $processStarted = $true
            $record.ProcessId = $activeProcess.Id
            $stdoutTask = $activeProcess.StandardOutput.ReadToEndAsync()
            $stderrTask = $activeProcess.StandardError.ReadToEndAsync()

            if (-not $activeProcess.WaitForExit($TimeoutSeconds * 1000))
            {
                $record.TimedOut = $true
                $record.Status = 'Timeout'
                $record.Error =
                    "process exceeded ${TimeoutSeconds}s watchdog"
                $sawTimeout = $true
                if (-not (Stop-ExactProcess -Process $activeProcess))
                {
                    $cleanupFailed = $true
                    $record.Status = 'CleanupFailed'
                    $record.Error =
                        'exact process is still alive after three forced stops'
                    $orphanProcessIds.Add($activeProcess.Id)
                }
            }

            $activeProcess.Refresh()
            if ($activeProcess.HasExited)
            {
                # Parameterless WaitForExit also drains redirected async data.
                $activeProcess.WaitForExit()
                $record.ProcessExitCode = $activeProcess.ExitCode
                Set-Content -LiteralPath $stdoutPath `
                    -Value $stdoutTask.Result -NoNewline -Encoding UTF8
                Set-Content -LiteralPath $stderrPath `
                    -Value $stderrTask.Result -NoNewline -Encoding UTF8
                $record.StdoutSha256 =
                    (Get-FileHash -LiteralPath $stdoutPath `
                        -Algorithm SHA256).Hash.ToLowerInvariant()
                $record.StderrSha256 =
                    (Get-FileHash -LiteralPath $stderrPath `
                        -Algorithm SHA256).Hash.ToLowerInvariant()
            }

            if (-not $record.TimedOut)
            {
                if (-not (Test-Path -LiteralPath $resultPath -PathType Leaf))
                {
                    $record.Status = 'InvalidResult'
                    $record.Error = 'scenario process wrote no result file'
                    $sawFailure = $true
                }
                else
                {
                    $record.ResultSha256 =
                        (Get-FileHash -LiteralPath $resultPath `
                            -Algorithm SHA256).Hash.ToLowerInvariant()
                    try
                    {
                        $result = Get-Content -LiteralPath $resultPath -Raw |
                            ConvertFrom-Json
                        $record.ResultExitCode = $result.exit_code
                        $record.ResultStatus = $result.status
                        $record.ScenarioCount = $result.scenario_count
                        $record.WindowActivated = $result.window_activated

                        $actualScenarios = @($result.scenarios)
                        $scenarioNamesMatch =
                            $actualScenarios.Count -eq
                                $expectedScenarioNames.Count
                        if ($scenarioNamesMatch)
                        {
                            for ($scenarioIndex = 0;
                                 $scenarioIndex -lt
                                    $expectedScenarioNames.Count;
                                 ++$scenarioIndex)
                            {
                                if (-not [string]::Equals(
                                        [string]$actualScenarios[
                                            $scenarioIndex].name,
                                        $expectedScenarioNames[$scenarioIndex],
                                        [StringComparison]::Ordinal))
                                {
                                    $scenarioNamesMatch = $false
                                    break
                                }
                            }
                        }

                        # Exclude PID/timestamps/elapsed values from this
                        # canonical digest. Equal semantic hashes across the
                        # 100 fresh processes prove identical transitions and
                        # native message counts, not merely valid JSON.
                        $semanticScenarios = @(
                            foreach ($scenario in $actualScenarios)
                            {
                                [ordered]@{
                                    name = $scenario.name
                                    status = $scenario.status
                                    failure = $scenario.failure
                                    samples = $scenario.samples
                                    prepared_actions =
                                        $scenario.prepared_actions
                                    outcomes = $scenario.outcomes
                                    messages = $scenario.messages
                                }
                            }
                        )
                        $semanticJson =
                            $semanticScenarios |
                                ConvertTo-Json -Depth 6 -Compress
                        $record.SemanticSha256 =
                            Get-TextSha256 -Text $semanticJson

                        $failedScenarios = @(
                            $actualScenarios |
                                Where-Object { $_.status -ne 'pass' }
                        )
                        $valid =
                            (Test-JsonInteger $result.schema_version) -and
                            $result.schema_version -eq 1 -and
                            $result.suite -eq
                                'winui-root-pointer-scenarios' -and
                            (Test-JsonInteger $result.pid) -and
                            $result.pid -eq $record.ProcessId -and
                            (Test-JsonInteger $result.exit_code) -and
                            $result.exit_code -eq
                                $record.ProcessExitCode -and
                            $result.exit_code -eq 0 -and
                            $result.status -eq 'pass' -and
                            (Test-JsonInteger $result.scenario_count) -and
                            $result.scenario_count -eq
                                $expectedScenarioNames.Count -and
                            $actualScenarios.Count -eq
                                $expectedScenarioNames.Count -and
                            $scenarioNamesMatch -and
                            $result.window_activated -is [bool] -and
                            $result.window_activated -eq $false -and
                            $failedScenarios.Count -eq 0
                        if ($valid)
                        {
                            $record.Status = 'Pass'
                        }
                        else
                        {
                            $record.Status = 'Fail'
                            $record.Error =
                                'process/result/schema/scenario gate mismatch'
                            $sawFailure = $true
                        }
                    }
                    catch
                    {
                        $record.Status = 'InvalidResult'
                        $record.Error =
                            "result JSON invalid: $($_.Exception.Message)"
                        $sawFailure = $true
                    }
                }
            }

        }
        catch
        {
            $record.Status = 'RunnerError'
            $record.Error = $_.Exception.Message
            $sawFailure = $true
            $infrastructureFailure = $_.Exception.Message
            $stopCampaign = $true
            if ($processStarted -and $activeProcess)
            {
                try
                {
                    $activeProcess.Refresh()
                    if (-not $activeProcess.HasExited -and
                        -not (Stop-ExactProcess -Process $activeProcess))
                    {
                        $cleanupFailed = $true
                        $orphanProcessIds.Add($activeProcess.Id)
                    }
                }
                catch
                {
                    $cleanupFailed = $true
                    $orphanProcessIds.Add($record.ProcessId)
                }
            }
        }
        finally
        {
            $stopwatch.Stop()
            $record.ElapsedMs = $stopwatch.ElapsedMilliseconds

            try
            {
                $record.ExecutableSha256After =
                    (Get-FileHash -LiteralPath $executablePath `
                        -Algorithm SHA256).Hash.ToLowerInvariant()
                if ($record.ExecutableSha256After -ne $binaryHashBefore)
                {
                    $infrastructureFailure =
                        'winuispike.exe changed during an iteration'
                    $sawFailure = $true
                    $stopCampaign = $true
                    if ($record.Status -eq 'Pass')
                    {
                        $record.Status = 'BinaryDrift'
                        $record.Error = $infrastructureFailure
                    }
                }
            }
            catch
            {
                $infrastructureFailure =
                    "cannot hash winuispike.exe after iteration: " +
                    $_.Exception.Message
                $sawFailure = $true
                $stopCampaign = $true
                if (-not $record.Error)
                {
                    $record.Error = $infrastructureFailure
                }
                if ($record.Status -eq 'Pass')
                {
                    $record.Status = 'RunnerError'
                }
            }

            $resultHash =
                if ($record.ResultSha256)
                {
                    $record.ResultSha256
                }
                else
                {
                    '-'
                }
            $aggregateLines.Add(
                "$iteration|$($record.Status)|" +
                "$($record.ProcessExitCode)|$resultHash|" +
                "$($record.SemanticSha256)|" +
                "$($record.ExecutableSha256Before)|" +
                "$($record.ExecutableSha256After)")
            $records.Add([pscustomobject]$record)
            if ($activeProcess)
            {
                $activeProcess.Dispose()
                $activeProcess = $null
            }
        }

        Write-Progress -Activity 'WinUI input scenarios' `
            -Status "$iteration / $Iterations : $($record.Status)" `
            -PercentComplete (100 * $iteration / $Iterations)

        if ($cleanupFailed -or $stopCampaign)
        {
            break
        }
    }
}
catch
{
    $infrastructureFailure = $_.Exception.Message
    $sawFailure = $true
}
finally
{
    Write-Progress -Activity 'WinUI input scenarios' -Completed
    if ($activeProcess)
    {
        try
        {
            $activeProcess.Refresh()
            if (-not $activeProcess.HasExited -and
                -not (Stop-ExactProcess -Process $activeProcess))
            {
                $cleanupFailed = $true
                $orphanProcessIds.Add($activeProcess.Id)
            }
        }
        catch
        {
            $cleanupFailed = $true
            try { $orphanProcessIds.Add($activeProcess.Id) } catch {}
        }
    }
}

$binaryHashAfter = $null
try
{
    $binaryHashAfter = (Get-FileHash -LiteralPath $executablePath `
        -Algorithm SHA256).Hash.ToLowerInvariant()
    if ($binaryHashAfter -ne $binaryHashBefore)
    {
        $infrastructureFailure =
            'winuispike.exe changed during the campaign'
    }
}
catch
{
    $infrastructureFailure =
        "cannot hash winuispike.exe after the campaign: " +
        $_.Exception.Message
}

$aggregateMaterial = $aggregateLines -join "`n"
$aggregateHash = Get-TextSha256 -Text $aggregateMaterial
$passedCount = @($records | Where-Object Status -eq 'Pass').Count
$timedOutCount = @($records | Where-Object TimedOut).Count
$failedCount = $records.Count - $passedCount
$semanticHashes = @(
    $records |
        Where-Object Status -eq 'Pass' |
        Select-Object -ExpandProperty SemanticSha256 -Unique
)
if ($passedCount -gt 0 -and $semanticHashes.Count -ne 1)
{
    $infrastructureFailure =
        'passing runs produced different semantic scenario hashes'
}

$exitCode =
    if ($cleanupFailed)
    {
        125
    }
    elseif ($sawTimeout)
    {
        124
    }
    elseif ($infrastructureFailure)
    {
        2
    }
    elseif ($sawFailure -or $failedCount)
    {
        1
    }
    else
    {
        0
    }

$summary = [ordered]@{
    SchemaVersion = 1
    CampaignId = $campaignId
    Status = if ($exitCode -eq 0) { 'Pass' } else { 'Fail' }
    ExitCode = $exitCode
    BuildDir = $resolvedBuildDir
    BinaryDir = $resolvedBinaryDir
    Executable = $executablePath
    ExecutableSha256Before = $binaryHashBefore
    ExecutableSha256After = $binaryHashAfter
    IterationsRequested = $Iterations
    IterationsCompleted = $records.Count
    Passed = $passedCount
    Failed = $failedCount
    TimedOut = $timedOutCount
    TimeoutSeconds = $TimeoutSeconds
    ScenarioTimeoutMs = $ScenarioTimeoutMs
    WatchdogMarginMs = $watchdogMarginMs
    AggregateInput = $aggregateMaterial
    AggregateSha256 = $aggregateHash
    SemanticSha256 = if ($semanticHashes.Count -eq 1) {
        $semanticHashes[0]
    } else {
        $null
    }
    DistinctSemanticHashes = $semanticHashes
    AllProcessesExited = -not $cleanupFailed
    OrphanProcessIds = @($orphanProcessIds)
    InfrastructureFailure = $infrastructureFailure
    StartedAt = $campaignStarted.ToString('o')
    EndedAt = (Get-Date -Format o)
    Runs = @($records)
}
$summaryPath = Join-Path $resolvedOutputDir 'summary.json'
$summary | ConvertTo-Json -Depth 8 |
    Set-Content -LiteralPath $summaryPath -Encoding UTF8
$summaryHash = (Get-FileHash -LiteralPath $summaryPath `
    -Algorithm SHA256).Hash.ToLowerInvariant()

Write-Host "Scenario campaign: $($summary.Status)"
Write-Host "Runs: $passedCount passed / $failedCount failed / $timedOutCount timed out"
Write-Host "Executable SHA-256: $binaryHashBefore"
Write-Host "Aggregate SHA-256:  $aggregateHash"
Write-Host "Summary SHA-256:    $summaryHash"
Write-Host "Output: $resolvedOutputDir"
if ($orphanProcessIds.Count)
{
    [Console]::Error.WriteLine(
        "ERROR: exact process cleanup failed for PID(s): " +
        ($orphanProcessIds -join ', '))
}

exit $exitCode
