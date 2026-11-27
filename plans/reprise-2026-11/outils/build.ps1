# Build the WinUI tree. A demo left running (yours, while you test by hand)
# keeps the DLLs and its own exe open, which makes the linker fail; a file in
# use can still be renamed, so move it aside and link again. The running
# program is not disturbed; the "*.old" files are removed on a later build,
# once nothing uses them any more.
param(
    [string]$Tree = 'F:\wxb\reprise-dll',
    [string[]]$Target = @()
)

# The compiler and the linker write large temporary files: keep them on the
# drive of the build tree. The system drive is short on space, and a build
# which fills it fails with LNK1108 or MSB6003.
$tmp = Join-Path (Split-Path $Tree -Parent) 'tmp'
New-Item -ItemType Directory -Force $tmp | Out-Null
$previousTmp = $env:TMP
$previousTemp = $env:TEMP
$env:TMP = $tmp
$env:TEMP = $tmp

$bin = Join-Path $Tree 'lib\vc_x64_dll'
Get-ChildItem $bin -Filter '*.old' -ErrorAction SilentlyContinue |
    ForEach-Object { Remove-Item $_.FullName -Force -ErrorAction SilentlyContinue }

$cmakeArgs = @('--build', $Tree, '--config', 'Release')
if ($Target.Count) { $cmakeArgs += '--target'; $cmakeArgs += $Target }
$cmakeArgs += '--', '/m:6', '/nr:false', '/v:m', '/clp:ErrorsOnly;Summary'

for ($pass = 1; $pass -le 30; $pass++)
{
    $output = & cmake @cmakeArgs 2>&1 | ForEach-Object { "$_" }
    $locked = $output | Select-String -Pattern "LNK1104.*'([^']+\.(dll|exe))'" |
        ForEach-Object { $_.Matches[0].Groups[1].Value } | Sort-Object -Unique
    if (-not $locked) { break }
    foreach ($file in $locked)
    {
        if (Test-Path -LiteralPath $file)
        {
            $aside = '{0}.{1:HHmmssfff}.old' -f $file, (Get-Date)
            Rename-Item -LiteralPath $file -NewName (Split-Path $aside -Leaf)
            "in use, moved aside: $file"
        }
    }
}
$env:TMP = $previousTmp
$env:TEMP = $previousTemp
$output | Select-Object -Last 12
