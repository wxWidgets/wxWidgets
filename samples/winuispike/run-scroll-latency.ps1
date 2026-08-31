# Measure mutation -> committed slot -> Rendering at the 60/120/165 budgets.
[CmdletBinding()]
param(
    [string]$BuildDir = 'build-winui-clean',
    [string]$BinaryDir = 'lib\vc_x64_dll',
    [string]$OutputDir,
    [Parameter(Mandatory)]
    [ValidateSet(60, 120, 165)]
    [int]$TargetHz,
    [ValidateRange(8, 200)]
    [int]$Samples = 24,
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

function Test-JsonInteger
{
    param($Value)

    return $Value -is [int] -or $Value -is [long] -or
           $Value -is [uint32] -or $Value -is [uint64]
}

function Get-IntegerPercentile95
{
    param([Parameter(Mandatory)][object[]]$Values)

    if ($Values.Count -eq 0 -or
        @($Values | Where-Object { -not (Test-JsonInteger $_) }).Count)
    {
        return $null
    }

    $ordered = @($Values | Sort-Object)
    $index = [int]([Math]::Ceiling(0.95 * $ordered.Count) - 1)
    return [long]$ordered[[Math]::Min($index, $ordered.Count - 1)]
}

function Get-IntegerUpperMedian
{
    param([Parameter(Mandatory)][object[]]$Values)

    if ($Values.Count -eq 0 -or
        @($Values | Where-Object { -not (Test-JsonInteger $_) }).Count)
    {
        return $null
    }

    $ordered = @($Values | Sort-Object)
    $index = [int][Math]::Floor($ordered.Count / 2)
    return [long]$ordered[$index]
}

function Get-LatencyContractOutcome
{
    param(
        [bool]$TechnicalFailure,
        [bool]$StructuralGatesPassed,
        [bool]$SlotBudgetPassed,
        [bool]$TargetRenderBudgetPassed,
        [bool]$CadenceSafetyPassed,
        [bool]$CadenceQualified
    )

    if ($TechnicalFailure -or
        -not $StructuralGatesPassed -or
        -not $SlotBudgetPassed -or
        -not $CadenceSafetyPassed)
    {
        return 'fail'
    }
    if (-not $CadenceQualified)
    {
        return 'unqualified'
    }
    if (-not $TargetRenderBudgetPassed)
    {
        return 'fail'
    }
    return 'pass'
}

# This pure matrix mirrors the C++ constexpr matrix. It runs before launching
# the sample so a future runner edit cannot silently reinterpret the JSON.
$contractCases = @(
    @($false, $true,  $true,  $true,  $true,  $true,  'pass'),
    @($false, $true,  $true,  $false, $true,  $false, 'unqualified'),
    @($false, $true,  $true,  $true,  $true,  $false, 'unqualified'),
    @($false, $true,  $true,  $false, $true,  $true,  'fail'),
    @($false, $true,  $true,  $true,  $false, $false, 'fail'),
    @($false, $true,  $false, $true,  $true,  $false, 'fail'),
    @($false, $false, $true,  $true,  $true,  $false, 'fail'),
    @($true,  $true,  $true,  $true,  $true,  $true,  'fail')
)
foreach ($contractCase in $contractCases)
{
    $actualOutcome = Get-LatencyContractOutcome `
        -TechnicalFailure $contractCase[0] `
        -StructuralGatesPassed $contractCase[1] `
        -SlotBudgetPassed $contractCase[2] `
        -TargetRenderBudgetPassed $contractCase[3] `
        -CadenceSafetyPassed $contractCase[4] `
        -CadenceQualified $contractCase[5]
    if ($actualOutcome -cne $contractCase[6])
    {
        throw "Internal latency-contract matrix failed: expected '$($contractCase[6])', got '$actualOutcome'"
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
        Join-Path $resolvedBuildDir "logs\winui-scroll-latency\$campaign"
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
$scriptExitCode = 1
try
{

$scenarioTimeoutMs =
    [Math]::Min(
        60000,
        [Math]::Max(1000, ($TimeoutSeconds - 2) * 1000))
$runnerScriptHashBefore =
    (Get-FileHash -LiteralPath $PSCommandPath -Algorithm SHA256).Hash
$hashBefore = (Get-FileHash -LiteralPath $executable -Algorithm SHA256).Hash
$records = [System.Collections.Generic.List[object]]::new()
$campaignTimedOut = $false
$cleanupOk = $true
$runnerFailed = $false
$orphanProcessIds = [System.Collections.Generic.List[int]]::new()
$expectedMeasurementContract =
    'after four Rendering callbacks and one unmeasured scroll batch, ' +
    'QPC mutation to committed slot observation to next ' +
    'CompositionTarget.Rendering'
$expectedRenderSafetyRule =
    '2 * max(target period, observed median period)'

foreach ($profileHz in @($TargetHz))
{
    $resultPath = Join-Path $resolvedOutputDir "result-$profileHz.json"
    $profileHashBefore =
        (Get-FileHash -LiteralPath $executable -Algorithm SHA256).Hash
    if ($profileHashBefore -ne $hashBefore)
    {
        $records.Add([ordered]@{
            target_hz = $profileHz
            status = 'fail'
            process_id = $null
            process_exit_code = $null
            orphan_process_id = $null
            watchdog_expired = $false
            result_file = $resultPath
            result_sha256 = $null
            observed_render_millihz = $null
            mutation_to_slot_p95_us = $null
            mutation_to_render_p95_us = $null
            max_scheduled_flushes_per_batch = $null
            executable_sha256_before = $profileHashBefore
            executable_sha256_after = $null
            failure = 'executable drifted before profile start'
        })
        break
    }

    $startInfo = [System.Diagnostics.ProcessStartInfo]::new()
    $startInfo.FileName = $executable
    $startInfo.WorkingDirectory = $resolvedBinaryDir
    $startInfo.UseShellExecute = $false
    $arguments = @(
        '--scroll-latency',
        "--target-hz=$profileHz",
        "--latency-samples=$Samples",
        "--scenario-result=$resultPath",
        "--scenario-timeout-ms=$scenarioTimeoutMs"
    )
    $startInfo.Arguments =
        (($arguments |
            ForEach-Object { ConvertTo-NativeArgument $_ }) -join ' ')
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
    $profileRecord = $null
    $profileHashAfter = $null
    $binaryStable = $false
    $timedOut = $false
    $exitCode = $null
    $result = $null
    $accepted = $false
    $failure = ''
    $detail = ''
    $profileCleanupOk = $true

    try
    {
        if (-not $process.Start())
        {
            throw "Failed to start winuispike.exe for $profileHz Hz"
        }
        $processStarted = $true
        $processId = $process.Id

        $timedOut = -not $process.WaitForExit($TimeoutSeconds * 1000)
        if ($timedOut)
        {
            $campaignTimedOut = $true
            $profileCleanupOk =
                Stop-ExactProcess -Process $process
        }
        $exitCode =
            if ($process.HasExited) { $process.ExitCode } else { 125 }
        $profileHashAfter =
            (Get-FileHash -LiteralPath $executable -Algorithm SHA256).Hash
        $binaryStable = $profileHashBefore -eq $profileHashAfter
    if ($timedOut)
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
            $result =
                Get-Content -LiteralPath $resultPath -Raw | ConvertFrom-Json
            # PowerShell variable names are case-insensitive: using
            # "$samples" here would overwrite the integer -Samples parameter
            # and make the schema comparison try to convert Object[] to Int32.
            $sampleRecords = @($result.samples)
            $renderIntervals = @($result.render_intervals_us)
            $slotValues =
                [System.Collections.Generic.List[long]]::new()
            $renderValues =
                [System.Collections.Generic.List[long]]::new()
            $sampleShapeValid = $sampleRecords.Count -eq $Samples
            for ($sampleIndex = 0;
                 $sampleShapeValid -and
                    $sampleIndex -lt $sampleRecords.Count;
                 ++$sampleIndex)
            {
                $sample = $sampleRecords[$sampleIndex]
                $sampleTypesValid =
                    (Test-JsonInteger $sample.index) -and
                    (Test-JsonInteger $sample.backlog) -and
                    (Test-JsonInteger $sample.scheduled_flushes) -and
                    (Test-JsonInteger $sample.mutation_to_slot_us) -and
                    (Test-JsonInteger $sample.mutation_to_render_us) -and
                    (Test-JsonInteger $sample.slot_to_render_us) -and
                    $sample.stale -is [bool] -and
                    $sample.status -is [string]
                if (-not $sampleTypesValid)
                {
                    $sampleShapeValid = $false
                    break
                }

                $derivedSlotToRender =
                    [long]$sample.mutation_to_render_us -
                    [long]$sample.mutation_to_slot_us
                $sampleShapeValid =
                    $sample.index -eq $sampleIndex -and
                    $sample.backlog -ge 0 -and
                    $sample.backlog -le 4 -and
                    $sample.scheduled_flushes -ge 0 -and
                    $sample.scheduled_flushes -le 1 -and
                    $sample.mutation_to_slot_us -ge 0 -and
                    $sample.mutation_to_render_us -ge 0 -and
                    $sample.mutation_to_render_us -le 250000 -and
                    $sample.slot_to_render_us -ge 0 -and
                    [Math]::Abs(
                        $derivedSlotToRender -
                        [long]$sample.slot_to_render_us) -le 2 -and
                    $sample.stale -eq $false -and
                    $sample.status -ceq 'pass'
                if ($sampleShapeValid)
                {
                    $slotValues.Add(
                        [long]$sample.mutation_to_slot_us)
                    $renderValues.Add(
                        [long]$sample.mutation_to_render_us)
                }
            }

            $renderIntervalTypesValid =
                $renderIntervals.Count -ge $Samples -and
                @($renderIntervals | Where-Object {
                    -not (Test-JsonInteger $_) -or $_ -le 0
                }).Count -eq 0
            $slotP95 =
                Get-IntegerPercentile95 -Values @($slotValues)
            $renderP95 =
                Get-IntegerPercentile95 -Values @($renderValues)
            $medianRender =
                Get-IntegerUpperMedian -Values @($renderIntervals)
            $computedTargetPeriod =
                [long][Math]::Floor(
                    (1000000 + $profileHz - 1) / $profileHz)
            $computedObservedMilliHz =
                if ($null -ne $medianRender -and $medianRender -gt 0) {
                    [long][Math]::Floor(1000000000 / $medianRender)
                } else {
                    0L
                }
            $targetMilliHz = [long]$profileHz * 1000
            $computedCadenceTolerance =
                [Math]::Max(
                    1000L,
                    [long][Math]::Ceiling($targetMilliHz * 0.02))
            $cadenceDifference =
                [Math]::Abs($computedObservedMilliHz - $targetMilliHz)
            $computedCadenceQualified =
                $computedObservedMilliHz -gt 0 -and
                $cadenceDifference -le $computedCadenceTolerance
            $computedMaxBacklog =
                if ($sampleShapeValid) {
                    [long]($sampleRecords.backlog |
                        Measure-Object -Maximum).Maximum
                } else {
                    -1L
                }
            $computedMaxScheduled =
                if ($sampleShapeValid) {
                    [long]($sampleRecords.scheduled_flushes |
                        Measure-Object -Maximum).Maximum
                } else {
                    -1L
                }
            $computedSafetyLimit =
                if ($null -ne $medianRender -and $medianRender -gt 0) {
                    2L * [Math]::Max(
                        $computedTargetPeriod, $medianRender)
                } else {
                    -1L
                }
            $computedStructuralGates =
                $sampleShapeValid -and
                $renderIntervalTypesValid -and
                $result.warmup.completed -is [bool] -and
                $result.warmup.completed -eq $true -and
                $result.window_activated -is [bool] -and
                $result.window_activated -eq $false -and
                $result.window_offscreen -is [bool] -and
                $result.window_offscreen -eq $true -and
                $computedMaxBacklog -le 4 -and
                $computedMaxScheduled -le 1 -and
                $null -ne $medianRender -and
                $medianRender -gt 0
            $computedSlotBudget =
                $null -ne $slotP95 -and
                $slotP95 -ge 0 -and
                $slotP95 -le $computedTargetPeriod
            $computedTargetRenderBudget =
                $null -ne $renderP95 -and
                $renderP95 -ge 0 -and
                $renderP95 -le 2L * $computedTargetPeriod
            $computedCadenceSafety =
                $null -ne $renderP95 -and
                $computedSafetyLimit -ge 0 -and
                $renderP95 -le $computedSafetyLimit
            $computedQualification =
                $computedStructuralGates -and
                $computedSlotBudget -and
                $computedTargetRenderBudget -and
                $computedCadenceSafety -and
                $computedCadenceQualified
            $derivedOutcome = Get-LatencyContractOutcome `
                -TechnicalFailure $false `
                -StructuralGatesPassed $computedStructuralGates `
                -SlotBudgetPassed $computedSlotBudget `
                -TargetRenderBudgetPassed $computedTargetRenderBudget `
                -CadenceSafetyPassed $computedCadenceSafety `
                -CadenceQualified $computedCadenceQualified
            $statusExitConsistent =
                ($result.status -ceq 'pass' -and $exitCode -eq 0) -or
                ($result.status -ceq 'unqualified' -and $exitCode -eq 2) -or
                ($result.status -ceq 'fail' -and
                    $exitCode -ne 0 -and $exitCode -ne 2)
            $strictScalarTypes =
                (Test-JsonInteger $result.schema_version) -and
                (Test-JsonInteger $result.pid) -and
                (Test-JsonInteger $result.started_tick_ms) -and
                (Test-JsonInteger $result.elapsed_ms) -and
                (Test-JsonInteger $result.exit_code) -and
                (Test-JsonInteger $result.target_hz) -and
                (Test-JsonInteger $result.target_period_us) -and
                (Test-JsonInteger `
                    $result.observed_render_median_period_us) -and
                (Test-JsonInteger $result.observed_render_millihz) -and
                (Test-JsonInteger `
                    $result.cadence_tolerance_millihz) -and
                (Test-JsonInteger $result.sample_count) -and
                (Test-JsonInteger $result.requested_sample_count) -and
                (Test-JsonInteger `
                    $result.measured_render_interval_count) -and
                (Test-JsonInteger $result.max_backlog) -and
                (Test-JsonInteger `
                    $result.max_scheduled_flushes_per_batch) -and
                (Test-JsonInteger `
                    $result.warmup.required_render_callbacks) -and
                (Test-JsonInteger `
                    $result.warmup.observed_render_callbacks) -and
                (Test-JsonInteger $result.warmup.scroll_batches) -and
                (Test-JsonInteger `
                    $result.budgets.mutation_to_slot_p95_us) -and
                (Test-JsonInteger `
                    $result.budgets.mutation_to_render_target_p95_us) -and
                (Test-JsonInteger `
                    $result.budgets.mutation_to_render_safety_p95_us) -and
                (Test-JsonInteger $result.budgets.stale_limit_us) -and
                (Test-JsonInteger $result.budgets.backlog_limit) -and
                (Test-JsonInteger `
                    $result.budgets.scheduled_flushes_per_batch_limit) -and
                (Test-JsonInteger `
                    $result.summary.mutation_to_slot_p95_us) -and
                (Test-JsonInteger `
                    $result.summary.mutation_to_render_p95_us) -and
                $result.status -is [string] -and
                $result.suite -is [string] -and
                $result.measurement_contract -is [string] -and
                $result.budgets.render_safety_rule -is [string] -and
                $result.cadence_qualified -is [bool] -and
                $result.warmup.completed -is [bool] -and
                $result.gates.structural_gates_passed -is [bool] -and
                $result.gates.slot_budget_passed -is [bool] -and
                $result.gates.target_render_budget_passed -is [bool] -and
                $result.gates.cadence_safety_passed -is [bool] -and
                $result.gates.qualification_passed -is [bool] -and
                $result.summary.all_samples_passed -is [bool]
            $shapeValid =
                $strictScalarTypes -and
                $binaryStable -and
                $result.pid -eq $process.Id -and
                $result.exit_code -eq $exitCode -and
                $result.schema_version -eq 2 -and
                $result.suite -ceq 'winui-scroll-latency' -and
                $result.measurement_contract -ceq
                    $expectedMeasurementContract -and
                $result.budgets.render_safety_rule -ceq
                    $expectedRenderSafetyRule -and
                $result.target_hz -eq $profileHz -and
                $result.sample_count -eq $Samples -and
                $result.requested_sample_count -eq $Samples -and
                $sampleRecords.Count -eq $Samples -and
                $result.measured_render_interval_count -eq
                    $renderIntervals.Count -and
                $result.status -ceq $derivedOutcome -and
                $statusExitConsistent -and
                $result.target_period_us -eq $computedTargetPeriod -and
                $result.observed_render_median_period_us -eq
                    $medianRender -and
                $result.observed_render_millihz -eq
                    $computedObservedMilliHz -and
                $result.cadence_tolerance_millihz -eq
                    $computedCadenceTolerance -and
                $result.cadence_qualified -eq
                    $computedCadenceQualified -and
                $result.warmup.required_render_callbacks -eq 4 -and
                $result.warmup.observed_render_callbacks -ge 4 -and
                $result.warmup.scroll_batches -eq 1 -and
                $result.max_backlog -eq $computedMaxBacklog -and
                $result.max_scheduled_flushes_per_batch -eq
                    $computedMaxScheduled -and
                $result.budgets.mutation_to_slot_p95_us -eq
                    $computedTargetPeriod -and
                $result.budgets.mutation_to_render_target_p95_us -eq
                    2L * $computedTargetPeriod -and
                $result.budgets.mutation_to_render_safety_p95_us -eq
                    $computedSafetyLimit -and
                $result.budgets.stale_limit_us -eq 250000 -and
                $result.budgets.backlog_limit -eq 4 -and
                $result.budgets.scheduled_flushes_per_batch_limit -eq 1 -and
                $result.summary.mutation_to_slot_p95_us -eq $slotP95 -and
                $result.summary.mutation_to_render_p95_us -eq
                    $renderP95 -and
                $result.summary.all_samples_passed -eq
                    $sampleShapeValid -and
                $result.gates.structural_gates_passed -eq
                    $computedStructuralGates -and
                $result.gates.slot_budget_passed -eq
                    $computedSlotBudget -and
                $result.gates.target_render_budget_passed -eq
                    $computedTargetRenderBudget -and
                $result.gates.cadence_safety_passed -eq
                    $computedCadenceSafety -and
                $result.gates.qualification_passed -eq
                    $computedQualification
            $qualificationConsistent =
                ($result.status -ceq 'pass' -and
                    $computedQualification) -or
                ($result.status -ceq 'unqualified' -and
                    -not $computedCadenceQualified -and
                    $computedStructuralGates -and
                    $computedSlotBudget -and
                    $computedCadenceSafety -and
                    -not $computedQualification)
            $accepted =
                $shapeValid -and
                $qualificationConsistent -and
                $result.status -cin @('pass', 'unqualified')
            if ($accepted -and $result.status -ceq 'unqualified')
            {
                $detail =
                    'integrity and safety gates passed, but observed render cadence does not qualify this target profile'
            }
            elseif (-not $accepted)
            {
                $failure =
                    if ($shapeValid -and $result.status -ceq 'fail') {
                        'probe reported a valid budget or technical failure'
                    } else {
                        'process/result/schema/status/gate contract mismatch'
                    }
            }
        }
        catch
        {
            $failure = "invalid result JSON: $($_.Exception.Message)"
            $result = $null
        }
    }

    $recordStatus =
        if ($accepted) {
            $result.status
        } elseif ($timedOut) {
            'timeout'
        } else {
            'fail'
        }
    $profileRecord = [ordered]@{
        target_hz = $profileHz
        status = $recordStatus
        process_id = $processId
        process_exit_code = $exitCode
        orphan_process_id = $null
        watchdog_expired = $timedOut
        result_file = $resultPath
        result_sha256 = if (Test-Path -LiteralPath $resultPath) {
            (Get-FileHash -LiteralPath $resultPath -Algorithm SHA256).Hash
        } else { $null }
        observed_render_millihz = if ($result) {
            $result.observed_render_millihz
        } else { $null }
        cadence_tolerance_millihz = if ($result) {
            $result.cadence_tolerance_millihz
        } else { $null }
        cadence_qualified = if ($result) {
            $result.cadence_qualified
        } else { $false }
        observed_render_median_period_us = if ($result) {
            $result.observed_render_median_period_us
        } else { $null }
        mutation_to_slot_p95_us = if ($result) {
            $result.summary.mutation_to_slot_p95_us
        } else { $null }
        mutation_to_render_p95_us = if ($result) {
            $result.summary.mutation_to_render_p95_us
        } else { $null }
        render_safety_limit_us = if ($result) {
            $result.budgets.mutation_to_render_safety_p95_us
        } else { $null }
        structural_gates_passed = if ($result) {
            $result.gates.structural_gates_passed
        } else { $false }
        slot_budget_passed = if ($result) {
            $result.gates.slot_budget_passed
        } else { $false }
        target_render_budget_passed = if ($result) {
            $result.gates.target_render_budget_passed
        } else { $false }
        cadence_safety_passed = if ($result) {
            $result.gates.cadence_safety_passed
        } else { $false }
        qualification_passed = if ($result) {
            $result.gates.qualification_passed
        } else { $false }
        max_scheduled_flushes_per_batch = if ($result) {
            $result.max_scheduled_flushes_per_batch
        } else { $null }
        executable_sha256_before = $profileHashBefore
        executable_sha256_after = $profileHashAfter
        failure = $failure
        detail = $detail
    }
    $records.Add($profileRecord)
    }
    catch
    {
        $runnerFailed = $true
        $failure = "runner exception: $($_.Exception.Message)"
        $profileRecord = [ordered]@{
            target_hz = $profileHz
            status = 'fail'
            process_id = $processId
            process_exit_code = $exitCode
            orphan_process_id = $null
            watchdog_expired = $timedOut
            result_file = $resultPath
            result_sha256 = $null
            observed_render_millihz = $null
            mutation_to_slot_p95_us = $null
            mutation_to_render_p95_us = $null
            max_scheduled_flushes_per_batch = $null
            executable_sha256_before = $profileHashBefore
            executable_sha256_after = $profileHashAfter
            failure = $failure
            detail = ''
        }
        $records.Add($profileRecord)
    }
    finally
    {
        $orphanProcessId = $null
        if ($processStarted)
        {
            try
            {
                $process.Refresh()
                if (-not $process.HasExited)
                {
                    $profileCleanupOk =
                        Stop-ExactProcess -Process $process
                    $process.Refresh()
                }
                if ($process.HasExited)
                {
                    if ($null -eq $exitCode -or $exitCode -eq 125)
                    {
                        $exitCode = $process.ExitCode
                    }
                    # The retained handle is the final authority. A kill wait
                    # may expire just before the process reports exit; don't
                    # report 125 when no child actually survives.
                    $profileCleanupOk = $true
                }
                else
                {
                    $orphanProcessId = $processId
                }
            }
            catch
            {
                $profileCleanupOk = $false
                $orphanProcessId = $processId
            }
        }

        $cleanupOk = $cleanupOk -and $profileCleanupOk
        if ($null -ne $orphanProcessId)
        {
            $orphanProcessIds.Add([int]$orphanProcessId)
        }
        if ($null -ne $profileRecord)
        {
            $profileRecord['process_exit_code'] = $exitCode
            $profileRecord['orphan_process_id'] = $orphanProcessId
            $profileRecord['executable_sha256_after'] =
                $profileHashAfter
        }
        $process.Dispose()
    }

    if ($timedOut -or -not $cleanupOk -or
        -not $binaryStable -or $runnerFailed)
    {
        break
    }
}

$hashAfter = (Get-FileHash -LiteralPath $executable -Algorithm SHA256).Hash
$runnerScriptHashAfter =
    (Get-FileHash -LiteralPath $PSCommandPath -Algorithm SHA256).Hash
$runnerScriptStable = $runnerScriptHashBefore -eq $runnerScriptHashAfter
$allPassed =
    $records.Count -eq 1 -and
    @($records | Where-Object { $_.status -cne 'pass' }).Count -eq 0 -and
    $hashBefore -eq $hashAfter -and
    $cleanupOk -and
    -not $runnerFailed -and
    $runnerScriptStable -and
    $orphanProcessIds.Count -eq 0 -and
    -not $campaignTimedOut
$allUnqualified =
    $records.Count -eq 1 -and
    @($records |
        Where-Object { $_.status -cne 'unqualified' }).Count -eq 0 -and
    $hashBefore -eq $hashAfter -and
    $cleanupOk -and
    -not $runnerFailed -and
    $runnerScriptStable -and
    $orphanProcessIds.Count -eq 0 -and
    -not $campaignTimedOut
$summary = [ordered]@{
    schema_version = 2
    suite = 'winui-scroll-latency-campaign'
    status = if ($allPassed) {
        'pass'
    } elseif ($allUnqualified) {
        'unqualified'
    } elseif ($campaignTimedOut) {
        'timeout'
    } else {
        'fail'
    }
    qualification_passed = $allPassed
    executable = $executable
    executable_sha256_before = $hashBefore
    executable_sha256_after = $hashAfter
    runner_script = $PSCommandPath
    runner_script_sha256_before = $runnerScriptHashBefore
    runner_script_sha256_after = $runnerScriptHashAfter
    watchdog_seconds = $TimeoutSeconds
    scenario_timeout_ms = $scenarioTimeoutMs
    target_hz = $TargetHz
    sample_count_per_profile = $Samples
    budgets = [ordered]@{
        mutation_to_slot_p95 = 'one target period'
        mutation_to_render_target_p95 = 'two target periods'
        mutation_to_render_safety_p95 =
            'two times max(target period, observed median render period)'
        stale_limit_us = 250000
        backlog_limit = 4
        scheduled_flushes_per_batch_limit = 1
    }
    profiles = $records
    cleanup_succeeded = $cleanupOk
    runner_failed = $runnerFailed
    orphan_process_ids = @($orphanProcessIds)
}
$summaryPath = Join-Path $resolvedOutputDir 'summary.json'
Write-JsonUtf8Atomic -Path $summaryPath -Value $summary

Write-Host "Scroll-latency summary: $summaryPath"
if ($allPassed)
{
    $scriptExitCode = 0
}
elseif (-not $cleanupOk)
{
    $scriptExitCode = 125
}
elseif ($campaignTimedOut)
{
    $scriptExitCode = 124
}
elseif ($allUnqualified)
{
    $scriptExitCode = 2
}
}
finally
{
    Exit-OutputDirectoryLock -Lock $outputLock
}

exit $scriptExitCode
