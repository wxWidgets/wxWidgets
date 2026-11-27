# Visual review: save a picture of every Showcase page and of every dialog it
# can open, on an invisible desktop (see drive.ps1). Compare two output
# folders to see what a change did.
# Run with Windows PowerShell 5.1 (powershell.exe).
#
#   powershell.exe -File revue.ps1 -Out C:\tmp\apres [-Exe ...\showcase.exe]
param(
    [string]$Exe = 'F:\wxb\reprise-dll\lib\vc_x64_dll\showcase.exe',
    [Parameter(Mandatory = $true)][string]$Out,
    [int]$Width = 1200,
    [int]$Height = 900
)

$drive = Join-Path $PSScriptRoot 'drive.ps1'
New-Item -ItemType Directory -Force $Out | Out-Null
$script = Join-Path $Out '_script.txt'

function Invoke-Drive([string[]]$lines, [string]$exeArgs)
{
    Set-Content -LiteralPath $script -Value $lines -Encoding ASCII
    # Called in this process: a nested powershell.exe would lose the quotes
    # around an action label containing spaces.
    & $drive -Exe $Exe -Script $script -WarmupSeconds 4 -ExeArgs $exeArgs
}

$pages = 'overview', 'buttons', 'text', 'choices', 'containers', 'pickers',
         'data', 'advanced', 'dialogs', 'events'
for ($i = 0; $i -lt $pages.Count; $i++)
{
    Invoke-Drive @(
        'wait 1000', 'top main Showcase', "size main $Width $Height", 'wait 1500',
        "shot main $Out\page$i-$($pages[$i]).png") "--page=$i"
}

# Dialog name -> action label, then the titles to close to reach the next
# dialog of the same action.
$dialogs = @(
    @('message',   'wxMessageDialog',     @()),
    @('text',      'Text/number dialogs', @('wxTextEntryDialog')),
    @('choice',    'Choice dialogs',      @('wxSingleChoiceDialog')),
    @('find',      'Find/replace',        @()),
    @('wizard',    'Wizard',              @()),
    @('rearrange', 'Rearrange dialog',    @()),
    @('colour',    'wxColourDialog',      @()),
    @('font',      'wxFontDialog',        @()),
    @('tooltip',   'Tooltip windows',     @())
)
foreach ($d in $dialogs)
{
    $lines = @('wait 2500', "shotall $Out\dlg-$($d[0])-1")
    $n = 2
    foreach ($title in $d[2])
    {
        $lines += "closetop $title", 'wait 1500', "shotall $Out\dlg-$($d[0])-$n"
        $n++
    }
    Invoke-Drive $lines "--page=8 --action=`"$($d[1])`""
}

# The main frame and the system's own helper windows are captured with every
# dialog: keep only the dialogs themselves.
Get-ChildItem $Out -Filter 'dlg-*.png' | ForEach-Object {
    Add-Type -AssemblyName System.Drawing
    $img = [System.Drawing.Image]::FromFile($_.FullName)
    $drop = ($img.Width -le 60 -and $img.Height -le 60) -or
            ($img.Width -ge 1100 -and $img.Height -ge 700)
    $img.Dispose()
    if ($drop) { Remove-Item $_.FullName }
}
Remove-Item $script -ErrorAction SilentlyContinue
Get-ChildItem $Out -Filter '*.png' | Select-Object Name, Length
