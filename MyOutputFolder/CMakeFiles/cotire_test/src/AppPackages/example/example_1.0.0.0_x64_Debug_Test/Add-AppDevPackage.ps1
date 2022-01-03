#
# Add-AppxDevPackage.ps1 is a PowerShell script designed to install app
# packages created by Visual Studio for developers.  To run this script from
# Explorer, right-click on its icon and choose "Run with PowerShell".
#
# Visual Studio supplies this script in the folder generated with its
# "Prepare Package" command.  The same folder will also contain the app
# package (a .appx file), the signing certificate (a .cer file), and a
# "Dependencies" subfolder containing all the framework packages used by the
# app.
#
# This script simplifies installing these packages by automating the
# following functions:
#   1. Find the app package and signing certificate in the script directory
#   2. Prompt the user to acquire a developer license and to install the
#      certificate if necessary
#   3. Find dependency packages that are applicable to the operating system's
#      CPU architecture
#   4. Install the package along with all applicable dependencies
#
# All command line parameters are reserved for use internally by the script.
# Users should launch this script from Explorer.
#

# .Link
# http://go.microsoft.com/fwlink/?LinkId=243053

param(
    [switch]$Force = $false,
    [switch]$GetDeveloperLicense = $false,
    [switch]$SkipLoggingTelemetry = $false,
    [string]$CertificatePath = $null
)

$ErrorActionPreference = "Stop"

# The language resources for this script are placed in the
# "Add-AppDevPackage.resources" subfolder alongside the script.  Since the
# current working directory might not be the directory that contains the
# script, we need to create the full path of the resources directory to
# pass into Import-LocalizedData
$ScriptPath = $null
try
{
    $ScriptPath = (Get-Variable MyInvocation).Value.MyCommand.Path
    $ScriptDir = Split-Path -Parent $ScriptPath
}
catch {}

if (!$ScriptPath)
{
    PrintMessageAndExit $UiStrings.ErrorNoScriptPath $ErrorCodes.NoScriptPath
}

$LocalizedResourcePath = Join-Path $ScriptDir "Add-AppDevPackage.resources"
Import-LocalizedData -BindingVariable UiStrings -BaseDirectory $LocalizedResourcePath

$ErrorCodes = Data {
    ConvertFrom-StringData @'
    Success = 0
    NoScriptPath = 1
    NoPackageFound = 2
    ManyPackagesFound = 3
    NoCertificateFound = 4
    ManyCertificatesFound = 5
    BadCertificate = 6
    PackageUnsigned = 7
    CertificateMismatch = 8
    ForceElevate = 9
    LaunchAdminFailed = 10
    GetDeveloperLicenseFailed = 11
    InstallCertificateFailed = 12
    AddPackageFailed = 13
    ForceDeveloperLicense = 14
    CertUtilInstallFailed = 17
    CertIsCA = 18
    BannedEKU = 19
    NoBasicConstraints = 20
    NoCodeSigningEku = 21
    InstallCertificateCancelled = 22
    BannedKeyUsage = 23
    ExpiredCertificate = 24
'@
}

$IMAGE_FILE_MACHINE_i386  = 0x014c
$IMAGE_FILE_MACHINE_AMD64 = 0x8664
$IMAGE_FILE_MACHINE_ARM64 = 0xAA64
$IMAGE_FILE_MACHINE_ARM   = 0x01c0
$IMAGE_FILE_MACHINE_THUMB = 0x01c2
$IMAGE_FILE_MACHINE_ARMNT = 0x01c4

# First try to use IsWow64Process2 to determine the OS architecture
try
{
    $IsWow64Process2_MethodDefinition = @"
[DllImport("kernel32.dll", SetLastError = true)]
public static extern bool IsWow64Process2(IntPtr process, out ushort processMachine, out ushort nativeMachine);
"@

    $Kernel32 = Add-Type -MemberDefinition $IsWow64Process2_MethodDefinition -Name "Kernel32" -Namespace "Win32" -PassThru

    $Proc = Get-Process -id $pid
    $processMachine = New-Object uint16
    $nativeMachine = New-Object uint16

    $Res = $Kernel32::IsWow64Process2($Proc.Handle, [ref] $processMachine, [ref] $nativeMachine)
    if ($Res -eq $True)
    {
        switch ($nativeMachine)
        {
            $IMAGE_FILE_MACHINE_i386  { $ProcessorArchitecture = "x86" }
            $IMAGE_FILE_MACHINE_AMD64 { $ProcessorArchitecture = "amd64" }
            $IMAGE_FILE_MACHINE_ARM64 { $ProcessorArchitecture = "arm64" }
            $IMAGE_FILE_MACHINE_ARM   { $ProcessorArchitecture = "arm" }
            $IMAGE_FILE_MACHINE_THUMB { $ProcessorArchitecture = "arm" }
            $IMAGE_FILE_MACHINE_ARMNT { $ProcessorArchitecture = "arm" }
        }
    }
}
catch
{
    # Ignore exception and fall back to using environment variables to determine the OS architecture
}

if ($null -eq $ProcessorArchitecture)
{
    $ProcessorArchitecture = $Env:Processor_Architecture

    # Getting $Env:Processor_Architecture on arm64 machines will return x86.  So check if the environment
    # variable "ProgramFiles(Arm)" is also set, if it is we know the actual processor architecture is arm64.
    # The value will also be x86 on amd64 machines when running the x86 version of PowerShell.
    if ($ProcessorArchitecture -eq "x86")
    {
        if ($null -ne ${Env:ProgramFiles(Arm)})
        {
            $ProcessorArchitecture = "arm64"
        }
        elseif ($null -ne ${Env:ProgramFiles(x86)})
        {
            $ProcessorArchitecture = "amd64"
        }
    }
}

function PrintMessageAndExit($ErrorMessage, $ReturnCode)
{
    Write-Host $ErrorMessage
    try
    {
        # Log telemetry data regarding the use of the script if possible.
        # There are three ways that this can be disabled:
        #   1. If the "TelemetryDependencies" folder isn't present.  This can be excluded at build time by setting the MSBuild property AppxLogTelemetryFromSideloadingScript to false
        #   2. If the SkipLoggingTelemetry switch is passed to this script.
        #   3. If Visual Studio telemetry is disabled via the registry.
        $TelemetryAssembliesFolder = (Join-Path $PSScriptRoot "TelemetryDependencies")
        if (!$SkipLoggingTelemetry -And (Test-Path $TelemetryAssembliesFolder))
        {
            $job = Start-Job -FilePath (Join-Path $TelemetryAssembliesFolder "LogSideloadingTelemetry.ps1") -ArgumentList $TelemetryAssembliesFolder, "VS/DesignTools/SideLoadingScript/AddAppDevPackage", $ReturnCode, $ProcessorArchitecture
            Wait-Job -Job $job -Timeout 60 | Out-Null
        }
    }
    catch
    {
        # Ignore telemetry errors
    }

    if (!$Force)
    {
        Pause
    }
    
    exit $ReturnCode
}

#
# Warns the user about installing certificates, and presents a Yes/No prompt
# to confirm the action.  The default is set to No.
#
function ConfirmCertificateInstall
{
    $Answer = $host.UI.PromptForChoice(
                    "", 
                    $UiStrings.WarningInstallCert, 
                    [System.Management.Automation.Host.ChoiceDescription[]]@($UiStrings.PromptYesString, $UiStrings.PromptNoString), 
                    1)
    
    return $Answer -eq 0
}

#
# Validates whether a file is a valid certificate using CertUtil.
# This needs to be done before calling Get-PfxCertificate on the file, otherwise
# the user will get a cryptic "Password: " prompt for invalid certs.
#
function ValidateCertificateFormat($FilePath)
{
    # certutil -verify prints a lot of text that we don't need, so it's redirected to $null here
    certutil.exe -verify $FilePath > $null
    if ($LastExitCode -lt 0)
    {
        PrintMessageAndExit ($UiStrings.ErrorBadCertificate -f $FilePath, $LastExitCode) $ErrorCodes.BadCertificate
    }
    
    # Check if certificate is expired
    $cert = Get-PfxCertificate $FilePath
    if (($cert.NotBefore -gt (Get-Date)) -or ($cert.NotAfter -lt (Get-Date)))
    {
        PrintMessageAndExit ($UiStrings.ErrorExpiredCertificate -f $FilePath) $ErrorCodes.ExpiredCertificate
    }
}

#
# Verify that the developer certificate meets the following restrictions:
#   - The certificate must contain a Basic Constraints extension, and its
#     Certificate Authority (CA) property must be false.
#   - The certificate's Key Usage extension must be either absent, or set to
#     only DigitalSignature.
#   - The certificate must contain an Extended Key Usage (EKU) extension with
#     Code Signing usage.
#   - The certificate must NOT contain any other EKU except Code Signing and
#     Lifetime Signing.
#
# These restrictions are enforced to decrease security risks that arise from
# trusting digital certificates.
#
function CheckCertificateRestrictions
{
    Set-Variable -Name BasicConstraintsExtensionOid -Value "2.5.29.19" -Option Constant
    Set-Variable -Name KeyUsageExtensionOid -Value "2.5.29.15" -Option Constant
    Set-Variable -Name EkuExtensionOid -Value "2.5.29.37" -Option Constant
    Set-Variable -Name CodeSigningEkuOid -Value "1.3.6.1.5.5.7.3.3" -Option Constant
    Set-Variable -Name LifetimeSigningEkuOid -Value "1.3.6.1.4.1.311.10.3.13" -Option Constant
    Set-Variable -Name UwpSigningEkuOid -Value "1.3.6.1.4.1.311.84.3.1" -Option Constant
    Set-Variable -Name DisposableSigningEkuOid -Value "1.3.6.1.4.1.311.84.3.2" -Option Constant

    $CertificateExtensions = (Get-PfxCertificate $CertificatePath).Extensions
    $HasBasicConstraints = $false
    $HasCodeSigningEku = $false

    foreach ($Extension in $CertificateExtensions)
    {
        # Certificate must contain the Basic Constraints extension
        if ($Extension.oid.value -eq $BasicConstraintsExtensionOid)
        {
            # CA property must be false
            if ($Extension.CertificateAuthority)
            {
                PrintMessageAndExit $UiStrings.ErrorCertIsCA $ErrorCodes.CertIsCA
            }
            $HasBasicConstraints = $true
        }

        # If key usage is present, it must be set to digital signature
        elseif ($Extension.oid.value -eq $KeyUsageExtensionOid)
        {
            if ($Extension.KeyUsages -ne "DigitalSignature")
            {
                PrintMessageAndExit ($UiStrings.ErrorBannedKeyUsage -f $Extension.KeyUsages) $ErrorCodes.BannedKeyUsage
            }
        }

        elseif ($Extension.oid.value -eq $EkuExtensionOid)
        {
            # Certificate must contain the Code Signing EKU
            $EKUs = $Extension.EnhancedKeyUsages.Value
            if ($EKUs -contains $CodeSigningEkuOid)
            {
                $HasCodeSigningEKU = $True
            }

            # EKUs other than code signing and lifetime signing are not allowed
            foreach ($EKU in $EKUs)
            {
                if ($EKU -ne $CodeSigningEkuOid -and $EKU -ne $LifetimeSigningEkuOid -and $EKU -ne $UwpSigningEkuOid -and $EKU -ne $DisposableSigningEkuOid)
                {
                    PrintMessageAndExit ($UiStrings.ErrorBannedEKU -f $EKU) $ErrorCodes.BannedEKU
                }
            }
        }
    }

    if (!$HasBasicConstraints)
    {
        PrintMessageAndExit $UiStrings.ErrorNoBasicConstraints $ErrorCodes.NoBasicConstraints
    }
    if (!$HasCodeSigningEKU)
    {
        PrintMessageAndExit $UiStrings.ErrorNoCodeSigningEku $ErrorCodes.NoCodeSigningEku
    }
}

#
# Performs operations that require administrative privileges:
#   - Prompt the user to obtain a developer license
#   - Install the developer certificate (if -Force is not specified, also prompts the user to confirm)
#
function DoElevatedOperations
{
    if ($GetDeveloperLicense)
    {
        Write-Host $UiStrings.GettingDeveloperLicense

        if ($Force)
        {
            PrintMessageAndExit $UiStrings.ErrorForceDeveloperLicense $ErrorCodes.ForceDeveloperLicense
        }
        try
        {
            Show-WindowsDeveloperLicenseRegistration
        }
        catch
        {
            $Error[0] # Dump details about the last error
            PrintMessageAndExit $UiStrings.ErrorGetDeveloperLicenseFailed $ErrorCodes.GetDeveloperLicenseFailed
        }
    }

    if ($CertificatePath)
    {
        Write-Host $UiStrings.InstallingCertificate

        # Make sure certificate format is valid and usage constraints are followed
        ValidateCertificateFormat $CertificatePath
        CheckCertificateRestrictions

        # If -Force is not specified, warn the user and get consent
        if ($Force -or (ConfirmCertificateInstall))
        {
            # Add cert to store
            certutil.exe -addstore TrustedPeople $CertificatePath
            if ($LastExitCode -lt 0)
            {
                PrintMessageAndExit ($UiStrings.ErrorCertUtilInstallFailed -f $LastExitCode) $ErrorCodes.CertUtilInstallFailed
            }
        }
        else
        {
            PrintMessageAndExit $UiStrings.ErrorInstallCertificateCancelled $ErrorCodes.InstallCertificateCancelled
        }
    }
}

#
# Checks whether the machine is missing a valid developer license.
#
function CheckIfNeedDeveloperLicense
{
    $Result = $true
    try
    {
        $Result = (Get-WindowsDeveloperLicense | Where-Object { $_.IsValid } | Measure-Object).Count -eq 0
    }
    catch {}

    return $Result
}

#
# Launches an elevated process running the current script to perform tasks
# that require administrative privileges.  This function waits until the
# elevated process terminates, and checks whether those tasks were successful.
#
function LaunchElevated
{
    # Set up command line arguments to the elevated process
    $RelaunchArgs = '-ExecutionPolicy Unrestricted -file "' + $ScriptPath + '"'

    if ($Force)
    {
        $RelaunchArgs += ' -Force'
    }
    if ($NeedDeveloperLicense)
    {
        $RelaunchArgs += ' -GetDeveloperLicense'
    }
    if ($SkipLoggingTelemetry)
    {
        $RelaunchArgs += ' -SkipLoggingTelemetry'
    }
    if ($NeedInstallCertificate)
    {
        $RelaunchArgs += ' -CertificatePath "' + $DeveloperCertificatePath.FullName + '"'
    }

    # Launch the process and wait for it to finish
    try
    {
        $PowerShellExePath = (Get-Process -Id $PID).Path
        $AdminProcess = Start-Process $PowerShellExePath -Verb RunAs -ArgumentList $RelaunchArgs -PassThru
    }
    catch
    {
        $Error[0] # Dump details about the last error
        PrintMessageAndExit $UiStrings.ErrorLaunchAdminFailed $ErrorCodes.LaunchAdminFailed
    }

    while (!($AdminProcess.HasExited))
    {
        Start-Sleep -Seconds 2
    }

    # Check if all elevated operations were successful
    if ($NeedDeveloperLicense)
    {
        if (CheckIfNeedDeveloperLicense)
        {
            PrintMessageAndExit $UiStrings.ErrorGetDeveloperLicenseFailed $ErrorCodes.GetDeveloperLicenseFailed
        }
        else
        {
            Write-Host $UiStrings.AcquireLicenseSuccessful
        }
    }
    if ($NeedInstallCertificate)
    {
        $Signature = Get-AuthenticodeSignature $DeveloperPackagePath -Verbose
        if ($Signature.Status -ne "Valid")
        {
            PrintMessageAndExit ($UiStrings.ErrorInstallCertificateFailed -f $Signature.Status) $ErrorCodes.InstallCertificateFailed
        }
        else
        {
            Write-Host $UiStrings.InstallCertificateSuccessful
        }
    }
}

#
# Finds all applicable dependency packages according to OS architecture, and
# installs the developer package with its dependencies.  The expected layout
# of dependencies is:
#
# <current dir>
#     \Dependencies
#         <Architecture neutral dependencies>.appx\.msix
#         \x86
#             <x86 dependencies>.appx\.msix
#         \x64
#             <x64 dependencies>.appx\.msix
#         \arm
#             <arm dependencies>.appx\.msix
#         \arm64
#             <arm64 dependencies>.appx\.msix
#
function InstallPackageWithDependencies
{
    $DependencyPackagesDir = (Join-Path $ScriptDir "Dependencies")
    $DependencyPackages = @()
    if (Test-Path $DependencyPackagesDir)
    {
        # Get architecture-neutral dependencies
        $DependencyPackages += Get-ChildItem (Join-Path $DependencyPackagesDir "*.appx") | Where-Object { $_.Mode -NotMatch "d" }
        $DependencyPackages += Get-ChildItem (Join-Path $DependencyPackagesDir "*.msix") | Where-Object { $_.Mode -NotMatch "d" }

        # Get architecture-specific dependencies
        if (($ProcessorArchitecture -eq "x86" -or $ProcessorArchitecture -eq "amd64" -or $ProcessorArchitecture -eq "arm64") -and (Test-Path (Join-Path $DependencyPackagesDir "x86")))
        {
            $DependencyPackages += Get-ChildItem (Join-Path $DependencyPackagesDir "x86\*.appx") | Where-Object { $_.Mode -NotMatch "d" }
            $DependencyPackages += Get-ChildItem (Join-Path $DependencyPackagesDir "x86\*.msix") | Where-Object { $_.Mode -NotMatch "d" }
        }
        if (($ProcessorArchitecture -eq "amd64") -and (Test-Path (Join-Path $DependencyPackagesDir "x64")))
        {
            $DependencyPackages += Get-ChildItem (Join-Path $DependencyPackagesDir "x64\*.appx") | Where-Object { $_.Mode -NotMatch "d" }
            $DependencyPackages += Get-ChildItem (Join-Path $DependencyPackagesDir "x64\*.msix") | Where-Object { $_.Mode -NotMatch "d" }
        }
        if (($ProcessorArchitecture -eq "arm" -or $ProcessorArchitecture -eq "arm64") -and (Test-Path (Join-Path $DependencyPackagesDir "arm")))
        {
            $DependencyPackages += Get-ChildItem (Join-Path $DependencyPackagesDir "arm\*.appx") | Where-Object { $_.Mode -NotMatch "d" }
            $DependencyPackages += Get-ChildItem (Join-Path $DependencyPackagesDir "arm\*.msix") | Where-Object { $_.Mode -NotMatch "d" }
        }
        if (($ProcessorArchitecture -eq "arm64") -and (Test-Path (Join-Path $DependencyPackagesDir "arm64")))
        {
            $DependencyPackages += Get-ChildItem (Join-Path $DependencyPackagesDir "arm64\*.appx") | Where-Object { $_.Mode -NotMatch "d" }
            $DependencyPackages += Get-ChildItem (Join-Path $DependencyPackagesDir "arm64\*.msix") | Where-Object { $_.Mode -NotMatch "d" }
        }
    }
    Write-Host $UiStrings.InstallingPackage

    $AddPackageSucceeded = $False
    try
    {
        if ($DependencyPackages.FullName.Count -gt 0)
        {
            Write-Host $UiStrings.DependenciesFound
            $DependencyPackages.FullName
            Add-AppxPackage -Path $DeveloperPackagePath.FullName -DependencyPath $DependencyPackages.FullName -ForceApplicationShutdown
        }
        else
        {
            Add-AppxPackage -Path $DeveloperPackagePath.FullName -ForceApplicationShutdown
        }
        $AddPackageSucceeded = $?
    }
    catch
    {
        $Error[0] # Dump details about the last error
    }

    if (!$AddPackageSucceeded)
    {
        if ($NeedInstallCertificate)
        {
            PrintMessageAndExit $UiStrings.ErrorAddPackageFailedWithCert $ErrorCodes.AddPackageFailed
        }
        else
        {
            PrintMessageAndExit $UiStrings.ErrorAddPackageFailed $ErrorCodes.AddPackageFailed
        }
    }
}

#
# Main script logic when the user launches the script without parameters.
#
function DoStandardOperations
{
    # Check for an .appx or .msix file in the script directory
    $PackagePath = Get-ChildItem (Join-Path $ScriptDir "*.appx") | Where-Object { $_.Mode -NotMatch "d" }
    if ($PackagePath -eq $null)
    {
        $PackagePath = Get-ChildItem (Join-Path $ScriptDir "*.msix") | Where-Object { $_.Mode -NotMatch "d" }
    }
    $PackageCount = ($PackagePath | Measure-Object).Count

    # Check for an .appxbundle or .msixbundle file in the script directory
    $BundlePath = Get-ChildItem (Join-Path $ScriptDir "*.appxbundle") | Where-Object { $_.Mode -NotMatch "d" }
    if ($BundlePath -eq $null)
    {
        $BundlePath = Get-ChildItem (Join-Path $ScriptDir "*.msixbundle") | Where-Object { $_.Mode -NotMatch "d" }
    }
    $BundleCount = ($BundlePath | Measure-Object).Count

    # Check for an .eappx or .emsix file in the script directory
    $EncryptedPackagePath = Get-ChildItem (Join-Path $ScriptDir "*.eappx") | Where-Object { $_.Mode -NotMatch "d" }
    if ($EncryptedPackagePath -eq $null)
    {
        $EncryptedPackagePath = Get-ChildItem (Join-Path $ScriptDir "*.emsix") | Where-Object { $_.Mode -NotMatch "d" }
    }
    $EncryptedPackageCount = ($EncryptedPackagePath | Measure-Object).Count

    # Check for an .eappxbundle or .emsixbundle file in the script directory
    $EncryptedBundlePath = Get-ChildItem (Join-Path $ScriptDir "*.eappxbundle") | Where-Object { $_.Mode -NotMatch "d" }
    if ($EncryptedBundlePath -eq $null)
    {
        $EncryptedBundlePath = Get-ChildItem (Join-Path $ScriptDir "*.emsixbundle") | Where-Object { $_.Mode -NotMatch "d" }
    }
    $EncryptedBundleCount = ($EncryptedBundlePath | Measure-Object).Count

    $NumberOfPackages = $PackageCount + $EncryptedPackageCount
    $NumberOfBundles = $BundleCount + $EncryptedBundleCount

    # There must be at least one package or bundle
    if ($NumberOfPackages + $NumberOfBundles -lt 1)
    {
        PrintMessageAndExit $UiStrings.ErrorNoPackageFound $ErrorCodes.NoPackageFound
    }
    # We must have exactly one bundle OR no bundle and exactly one package
    elseif ($NumberOfBundles -gt 1 -or
            ($NumberOfBundles -eq 0 -and $NumberOfpackages -gt 1))
    {
        PrintMessageAndExit $UiStrings.ErrorManyPackagesFound $ErrorCodes.ManyPackagesFound
    }

    # First attempt to install a bundle or encrypted bundle. If neither exists, fall back to packages and then encrypted packages
    if ($BundleCount -eq 1)
    {
        $DeveloperPackagePath = $BundlePath
        Write-Host ($UiStrings.BundleFound -f $DeveloperPackagePath.FullName)
    }
    elseif ($EncryptedBundleCount -eq 1)
    {
        $DeveloperPackagePath = $EncryptedBundlePath
        Write-Host ($UiStrings.EncryptedBundleFound -f $DeveloperPackagePath.FullName)
    }
    elseif ($PackageCount -eq 1)
    {
        $DeveloperPackagePath = $PackagePath
        Write-Host ($UiStrings.PackageFound -f $DeveloperPackagePath.FullName)
    }
    elseif ($EncryptedPackageCount -eq 1)
    {
        $DeveloperPackagePath = $EncryptedPackagePath
        Write-Host ($UiStrings.EncryptedPackageFound -f $DeveloperPackagePath.FullName)
    }
    
    # The package must be signed
    $PackageSignature = Get-AuthenticodeSignature $DeveloperPackagePath
    $PackageCertificate = $PackageSignature.SignerCertificate
    if (!$PackageCertificate)
    {
        PrintMessageAndExit $UiStrings.ErrorPackageUnsigned $ErrorCodes.PackageUnsigned
    }

    # Test if the package signature is trusted.  If not, the corresponding certificate
    # needs to be present in the current directory and needs to be installed.
    $NeedInstallCertificate = ($PackageSignature.Status -ne "Valid")

    if ($NeedInstallCertificate)
    {
        # List all .cer files in the script directory
        $DeveloperCertificatePath = Get-ChildItem (Join-Path $ScriptDir "*.cer") | Where-Object { $_.Mode -NotMatch "d" }
        $DeveloperCertificateCount = ($DeveloperCertificatePath | Measure-Object).Count

        # There must be exactly 1 certificate
        if ($DeveloperCertificateCount -lt 1)
        {
            PrintMessageAndExit $UiStrings.ErrorNoCertificateFound $ErrorCodes.NoCertificateFound
        }
        elseif ($DeveloperCertificateCount -gt 1)
        {
            PrintMessageAndExit $UiStrings.ErrorManyCertificatesFound $ErrorCodes.ManyCertificatesFound
        }

        Write-Host ($UiStrings.CertificateFound -f $DeveloperCertificatePath.FullName)

        # The .cer file must have the format of a valid certificate
        ValidateCertificateFormat $DeveloperCertificatePath

        # The package signature must match the certificate file
        if ($PackageCertificate -ne (Get-PfxCertificate $DeveloperCertificatePath))
        {
            PrintMessageAndExit $UiStrings.ErrorCertificateMismatch $ErrorCodes.CertificateMismatch
        }
    }

    $NeedDeveloperLicense = CheckIfNeedDeveloperLicense

    # Relaunch the script elevated with the necessary parameters if needed
    if ($NeedDeveloperLicense -or $NeedInstallCertificate)
    {
        Write-Host $UiStrings.ElevateActions
        if ($NeedDeveloperLicense)
        {
            Write-Host $UiStrings.ElevateActionDevLicense
        }
        if ($NeedInstallCertificate)
        {
            Write-Host $UiStrings.ElevateActionCertificate
        }

        $IsAlreadyElevated = ([Security.Principal.WindowsIdentity]::GetCurrent().Groups.Value -contains "S-1-5-32-544")
        if ($IsAlreadyElevated)
        {
            if ($Force -and $NeedDeveloperLicense)
            {
                PrintMessageAndExit $UiStrings.ErrorForceDeveloperLicense $ErrorCodes.ForceDeveloperLicense
            }
            if ($Force -and $NeedInstallCertificate)
            {
                Write-Warning $UiStrings.WarningInstallCert
            }
        }
        else
        {
            if ($Force)
            {
                PrintMessageAndExit $UiStrings.ErrorForceElevate $ErrorCodes.ForceElevate
            }
            else
            {
                Write-Host $UiStrings.ElevateActionsContinue
                Pause
            }
        }

        LaunchElevated
    }

    InstallPackageWithDependencies
}

#
# Main script entry point
#
if ($GetDeveloperLicense -or $CertificatePath)
{
    DoElevatedOperations
}
else
{
    DoStandardOperations
    PrintMessageAndExit $UiStrings.Success $ErrorCodes.Success
}

# SIG # Begin signature block
# MIIhdAYJKoZIhvcNAQcCoIIhZTCCIWECAQExDzANBglghkgBZQMEAgEFADB5Bgor
# BgEEAYI3AgEEoGswaTA0BgorBgEEAYI3AgEeMCYCAwEAAAQQH8w7YFlLCE63JNLG
# KX7zUQIBAAIBAAIBAAIBAAIBADAxMA0GCWCGSAFlAwQCAQUABCAaovVnr86uRLc7
# aU4MWMaEBdFBUDgUiMNw07NprqcvsqCCC3IwggT6MIID4qADAgECAhMzAAAEOfYf
# emdtoACvAAAAAAQ5MA0GCSqGSIb3DQEBCwUAMH4xCzAJBgNVBAYTAlVTMRMwEQYD
# VQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNy
# b3NvZnQgQ29ycG9yYXRpb24xKDAmBgNVBAMTH01pY3Jvc29mdCBDb2RlIFNpZ25p
# bmcgUENBIDIwMTAwHhcNMjEwOTAyMTgyNTU4WhcNMjIwOTAxMTgyNTU4WjB0MQsw
# CQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9u
# ZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMR4wHAYDVQQDExVNaWNy
# b3NvZnQgQ29ycG9yYXRpb24wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIB
# AQDYxnPFVXLjCNZotpu2pA/klQnh61TVmOwkp46L2lhfjh3H1JisbpZfdR7PSIOy
# thfERueQRQM4cYwlCHxZs2PJgVAWT1A09MgvyOnUu8+TP3rMJux8XpgfjbT1QY9W
# NvAV+9T/3+JaRgW+L/IarOJQ+fQx6fwoO8U1UDJykFo5fQIbgCGXO/uz69B0z6LE
# VrJP+qibVhromVIQ0vaip2Rh+EMlHNN3jDpuYJOfcI9iClLffv30NDVa7LNdr5S8
# 5uFW7WD6aVLd5Y4vytrD477um9drb3Xe/gXmBKUZ2JLMv+xZG39Xw/UbA1lQTN/t
# bof2MgifNoRRRRELlcOForTtAgMBAAGjggF5MIIBdTAfBgNVHSUEGDAWBgorBgEE
# AYI3PQYBBggrBgEFBQcDAzAdBgNVHQ4EFgQUxfAmBmr7eiyHypaAy6/f8G8lQsUw
# UAYDVR0RBEkwR6RFMEMxKTAnBgNVBAsTIE1pY3Jvc29mdCBPcGVyYXRpb25zIFB1
# ZXJ0byBSaWNvMRYwFAYDVQQFEw0yMzA4NjUrNDY3Mzk4MB8GA1UdIwQYMBaAFOb8
# X3u7IgBY5HJOtfQhdCMy5u+sMFYGA1UdHwRPME0wS6BJoEeGRWh0dHA6Ly9jcmwu
# bWljcm9zb2Z0LmNvbS9wa2kvY3JsL3Byb2R1Y3RzL01pY0NvZFNpZ1BDQV8yMDEw
# LTA3LTA2LmNybDBaBggrBgEFBQcBAQROMEwwSgYIKwYBBQUHMAKGPmh0dHA6Ly93
# d3cubWljcm9zb2Z0LmNvbS9wa2kvY2VydHMvTWljQ29kU2lnUENBXzIwMTAtMDct
# MDYuY3J0MAwGA1UdEwEB/wQCMAAwDQYJKoZIhvcNAQELBQADggEBAGaOsNHOxecF
# hmUQiipJkW1uEeTTuKdpftxfnqFzxAqNngYLPDHQb3Ja8CnFNwCN5BFh21p4TM15
# Pv1aO+HCA3mYRAexP5LM9mTTBEoC5WFMNVG+6x138G/BnafTHRIj5UjgZHWR3t2s
# /uWoNBRtTYVUKTdwuvh+2bCeJrEebuWi4cOOkHd3eBwaD+Dh/iJinmdUoYoAA8cN
# AnZ+4jsirVYsvnfHeYtzEPVUPFtRVsHSRhs+zMpm+66oju2d8z2HHS3Q+OVgbCXq
# BAg1c+BTzV9+9oaMXuq7klKeRNj1quZae0jisxP+fxQx3iWB7I8YVx0EmGg67aQS
# pjH84cst2PswggZwMIIEWKADAgECAgphDFJMAAAAAAADMA0GCSqGSIb3DQEBCwUA
# MIGIMQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMH
# UmVkbW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMTIwMAYDVQQD
# EylNaWNyb3NvZnQgUm9vdCBDZXJ0aWZpY2F0ZSBBdXRob3JpdHkgMjAxMDAeFw0x
# MDA3MDYyMDQwMTdaFw0yNTA3MDYyMDUwMTdaMH4xCzAJBgNVBAYTAlVTMRMwEQYD
# VQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNy
# b3NvZnQgQ29ycG9yYXRpb24xKDAmBgNVBAMTH01pY3Jvc29mdCBDb2RlIFNpZ25p
# bmcgUENBIDIwMTAwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDpDmRQ
# eWe1xOP9CQBMnpSs91Zo6kTYz8VYT6mldnxtRbrTOZK0pB75+WWC5BfSj/1EnAjo
# ZZPOLFWEv30I4y4rqEErGLeiS25JTGsVB97R0sKJHnGUzbV/S7SvCNjMiNZrF5Q6
# k84mP+zm/jSYV9UdXUn2siou1YW7WT/4kLQrg3TKK7M7RuPwRknBF2ZUyRy9HcRV
# Yldy+Ge5JSA03l2mpZVeqyiAzdWynuUDtWPTshTIwciKJgpZfwfs/w7tgBI1TBKm
# vlJb9aba4IsLSHfWhUfVELnG6Krui2otBVxgxrQqW5wjHF9F4xoUHm83yxkzgGqJ
# TaNqZmN4k9Uwz5UfAgMBAAGjggHjMIIB3zAQBgkrBgEEAYI3FQEEAwIBADAdBgNV
# HQ4EFgQU5vxfe7siAFjkck619CF0IzLm76wwGQYJKwYBBAGCNxQCBAweCgBTAHUA
# YgBDAEEwCwYDVR0PBAQDAgGGMA8GA1UdEwEB/wQFMAMBAf8wHwYDVR0jBBgwFoAU
# 1fZWy4/oolxiaNE9lJBb186aGMQwVgYDVR0fBE8wTTBLoEmgR4ZFaHR0cDovL2Ny
# bC5taWNyb3NvZnQuY29tL3BraS9jcmwvcHJvZHVjdHMvTWljUm9vQ2VyQXV0XzIw
# MTAtMDYtMjMuY3JsMFoGCCsGAQUFBwEBBE4wTDBKBggrBgEFBQcwAoY+aHR0cDov
# L3d3dy5taWNyb3NvZnQuY29tL3BraS9jZXJ0cy9NaWNSb29DZXJBdXRfMjAxMC0w
# Ni0yMy5jcnQwgZ0GA1UdIASBlTCBkjCBjwYJKwYBBAGCNy4DMIGBMD0GCCsGAQUF
# BwIBFjFodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20vUEtJL2RvY3MvQ1BTL2RlZmF1
# bHQuaHRtMEAGCCsGAQUFBwICMDQeMiAdAEwAZQBnAGEAbABfAFAAbwBsAGkAYwB5
# AF8AUwB0AGEAdABlAG0AZQBuAHQALiAdMA0GCSqGSIb3DQEBCwUAA4ICAQAadO9X
# Tyl7xBaFeLhQ0yL8CZ2sgpf4NP8qLJeVEuXkv8+/k8jjNKnbgbjcHgC+0jVvr+V/
# eZV35QLU8evYzU4eG2GiwlojGvCMqGJRRWcI4z88HpP4MIUXyDlAptcOsyEp5aWh
# aYwik8x0mOehR0PyU6zADzBpf/7SJSBtb2HT3wfV2XIALGmGdj1R26Y5SMk3YW0H
# 3VMZy6fWYcK/4oOrD+Brm5XWfShRsIlKUaSabMi3H0oaDmmp19zBftFJcKq2rbty
# R2MX+qbWoqaG7KgQRJtjtrJpiQbHRoZ6GD/oxR0h1Xv5AiMtxUHLvx1MyBbvsZx/
# /CJLSYpuFeOmf3Zb0VN5kYWd1dLbPXM18zyuVLJSR2rAqhOV0o4R2plnXjKM+zeF
# 0dx1hZyHxlpXhcK/3Q2PjJst67TuzyfTtV5p+qQWBAGnJGdzz01Ptt4FVpd69+lS
# TfR3BU+FxtgL8Y7tQgnRDXbjI1Z4IiY2vsqxjG6qHeSF2kczYo+kyZEzX3EeQK+Y
# Zcki6EIhJYocLWDZN4lBiSoWD9dhPJRoYFLv1keZoIBA7hWBdz6c4FMYGlAdOJWb
# HmYzEyc5F3iHNs5Ow1+y9T1HU7bg5dsLYT0q15IszjdaPkBCMaQfEAjCVpy/JF1R
# Ap1qedIX09rBlI4HeyVxRKsGaubUxt8jmpZ1xTGCFVgwghVUAgEBMIGVMH4xCzAJ
# BgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25k
# MR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xKDAmBgNVBAMTH01pY3Jv
# c29mdCBDb2RlIFNpZ25pbmcgUENBIDIwMTACEzMAAAQ59h96Z22gAK8AAAAABDkw
# DQYJYIZIAWUDBAIBBQCgga4wGQYJKoZIhvcNAQkDMQwGCisGAQQBgjcCAQQwHAYK
# KwYBBAGCNwIBCzEOMAwGCisGAQQBgjcCARUwLwYJKoZIhvcNAQkEMSIEIEopdEf3
# vaw+bknrxifr7wCqVwSSQ1Hlz/MHRULiG6hkMEIGCisGAQQBgjcCAQwxNDAyoBSA
# EgBNAGkAYwByAG8AcwBvAGYAdKEagBhodHRwOi8vd3d3Lm1pY3Jvc29mdC5jb20w
# DQYJKoZIhvcNAQEBBQAEggEAWvBiWDKKMqwsXHtT1lIYi5SNVa0nfEIFuXgAAT5f
# UPNvaNxyczg5Afr+9TabkEQD4OL6WgiBmBWGgWgafzb97xvSYTfdjns2wrLAs45J
# 0SzWoBBOAmuRL4mOj+V2/BNGtSN56sUJiCnZI1SKLRc53e30LGUe5nnLlI6yBPoi
# FUMoxNKkdpgmuHmE6b7SdmD8yTBUUS3kR6X8tHXMQkAW7kyD8rBOjTeYtKHvLdTX
# 1YH/m8zNKeI8khLlDbCQrvCxRaXCIt32bQJ2eeEgwB+LUctUv39aA5cphjoP0KHg
# 8XQp46jOpRn7zq15g1TQZI7sBlC9wOEmbhG4TjC0CyMAHaGCEuIwghLeBgorBgEE
# AYI3AwMBMYISzjCCEsoGCSqGSIb3DQEHAqCCErswghK3AgEDMQ8wDQYJYIZIAWUD
# BAIBBQAwggFRBgsqhkiG9w0BCRABBKCCAUAEggE8MIIBOAIBAQYKKwYBBAGEWQoD
# ATAxMA0GCWCGSAFlAwQCAQUABCDh9sFfcTp7jxi/gPkhW9eN874VsSyDcCgzV28t
# KuMB3QIGYYASxPVMGBMyMDIxMTExMjE4NDYxOC43NjJaMASAAgH0oIHQpIHNMIHK
# MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVk
# bW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSUwIwYDVQQLExxN
# aWNyb3NvZnQgQW1lcmljYSBPcGVyYXRpb25zMSYwJAYDVQQLEx1UaGFsZXMgVFNT
# IEVTTjo3QkYxLUUzRUEtQjgwODElMCMGA1UEAxMcTWljcm9zb2Z0IFRpbWUtU3Rh
# bXAgU2VydmljZaCCDjkwggTxMIID2aADAgECAhMzAAABUcNQ51lsqsanAAAAAAFR
# MA0GCSqGSIb3DQEBCwUAMHwxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5n
# dG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9y
# YXRpb24xJjAkBgNVBAMTHU1pY3Jvc29mdCBUaW1lLVN0YW1wIFBDQSAyMDEwMB4X
# DTIwMTExMjE4MjYwNFoXDTIyMDIxMTE4MjYwNFowgcoxCzAJBgNVBAYTAlVTMRMw
# EQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVN
# aWNyb3NvZnQgQ29ycG9yYXRpb24xJTAjBgNVBAsTHE1pY3Jvc29mdCBBbWVyaWNh
# IE9wZXJhdGlvbnMxJjAkBgNVBAsTHVRoYWxlcyBUU1MgRVNOOjdCRjEtRTNFQS1C
# ODA4MSUwIwYDVQQDExxNaWNyb3NvZnQgVGltZS1TdGFtcCBTZXJ2aWNlMIIBIjAN
# BgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAn9KH76qErjvvOIkjWbHptMkYDjmG
# +JEmzguyr/VxjZgZ/ig8Mk47jqSJP5RxH/sDyqhYu7jPSO86siZh8u7DBX9L8I+A
# B+8fPPvD4uoLKD22BpoFl4B8Fw5K7SuibvbxGN7adL1/zW+sWXlVvpDhEPIKDICv
# EdNjGTLhktfftjefg9lumBMUBJ2G4/g4ad0dDvRNmKiMZXXe/Ll4Qg/oPSzXCUEY
# oSSqa5D+5MRimVe5/YTLj0jVr8iF45V0hT7VH8OJO4YImcnZhq6Dw1G+w6ACRGeP
# FmOWqW8tEZ13SMmOquJrTkwyy8zyNtVttJAX7diFLbR0SvMlbJZWK0KHdwIDAQAB
# o4IBGzCCARcwHQYDVR0OBBYEFMV3/+NoUGKTNGg6OMyE6fN1ROptMB8GA1UdIwQY
# MBaAFNVjOlyKMZDzQ3t8RhvFM2hahW1VMFYGA1UdHwRPME0wS6BJoEeGRWh0dHA6
# Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvY3JsL3Byb2R1Y3RzL01pY1RpbVN0YVBD
# QV8yMDEwLTA3LTAxLmNybDBaBggrBgEFBQcBAQROMEwwSgYIKwYBBQUHMAKGPmh0
# dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9wa2kvY2VydHMvTWljVGltU3RhUENBXzIw
# MTAtMDctMDEuY3J0MAwGA1UdEwEB/wQCMAAwEwYDVR0lBAwwCgYIKwYBBQUHAwgw
# DQYJKoZIhvcNAQELBQADggEBACv99cAVg5nx0SqjvLfQzmugMj5cJ9NE60duSH1L
# pxHYim9Ls3UfiYd7t0JvyEw/rRTEKHbznV6LFLlX++lHJMGKzZnHtTe2OI6ZHFnN
# iFhtgyWuYDJrm7KQykNi1G1LbuVie9MehmoK+hBiZnnrcfZSnBSokrvO2QEWHC1x
# nZ5wM82UEjprFYOkchU+6RcoCjjmIFGfgSzNj1MIbf4lcJ5FoV1Mg6FwF45CijOX
# HVXrzkisMZ9puDpFjjEV6TAY6INgMkhLev/AVow0sF8MfQztJIlFYdFEkZ5NF/Iy
# zoC2Yb9iw4bCKdBrdD3As6mvoGSNjCC6lOdz6EerJK3NhFgwggZxMIIEWaADAgEC
# AgphCYEqAAAAAAACMA0GCSqGSIb3DQEBCwUAMIGIMQswCQYDVQQGEwJVUzETMBEG
# A1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVkbW9uZDEeMBwGA1UEChMVTWlj
# cm9zb2Z0IENvcnBvcmF0aW9uMTIwMAYDVQQDEylNaWNyb3NvZnQgUm9vdCBDZXJ0
# aWZpY2F0ZSBBdXRob3JpdHkgMjAxMDAeFw0xMDA3MDEyMTM2NTVaFw0yNTA3MDEy
# MTQ2NTVaMHwxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpXYXNoaW5ndG9uMRAwDgYD
# VQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQgQ29ycG9yYXRpb24xJjAk
# BgNVBAMTHU1pY3Jvc29mdCBUaW1lLVN0YW1wIFBDQSAyMDEwMIIBIjANBgkqhkiG
# 9w0BAQEFAAOCAQ8AMIIBCgKCAQEAqR0NvHcRijog7PwTl/X6f2mUa3RUENWlCgCC
# hfvtfGhLLF/Fw+Vhwna3PmYrW/AVUycEMR9BGxqVHc4JE458YTBZsTBED/FgiIRU
# QwzXTbg4CLNC3ZOs1nMwVyaCo0UN0Or1R4HNvyRgMlhgRvJYR4YyhB50YWeRX4FU
# sc+TTJLBxKZd0WETbijGGvmGgLvfYfxGwScdJGcSchohiq9LZIlQYrFd/XcfPfBX
# day9ikJNQFHRD5wGPmd/9WbAA5ZEfu/QS/1u5ZrKsajyeioKMfDaTgaRtogINeh4
# HLDpmc085y9Euqf03GS9pAHBIAmTeM38vMDJRF1eFpwBBU8iTQIDAQABo4IB5jCC
# AeIwEAYJKwYBBAGCNxUBBAMCAQAwHQYDVR0OBBYEFNVjOlyKMZDzQ3t8RhvFM2ha
# hW1VMBkGCSsGAQQBgjcUAgQMHgoAUwB1AGIAQwBBMAsGA1UdDwQEAwIBhjAPBgNV
# HRMBAf8EBTADAQH/MB8GA1UdIwQYMBaAFNX2VsuP6KJcYmjRPZSQW9fOmhjEMFYG
# A1UdHwRPME0wS6BJoEeGRWh0dHA6Ly9jcmwubWljcm9zb2Z0LmNvbS9wa2kvY3Js
# L3Byb2R1Y3RzL01pY1Jvb0NlckF1dF8yMDEwLTA2LTIzLmNybDBaBggrBgEFBQcB
# AQROMEwwSgYIKwYBBQUHMAKGPmh0dHA6Ly93d3cubWljcm9zb2Z0LmNvbS9wa2kv
# Y2VydHMvTWljUm9vQ2VyQXV0XzIwMTAtMDYtMjMuY3J0MIGgBgNVHSABAf8EgZUw
# gZIwgY8GCSsGAQQBgjcuAzCBgTA9BggrBgEFBQcCARYxaHR0cDovL3d3dy5taWNy
# b3NvZnQuY29tL1BLSS9kb2NzL0NQUy9kZWZhdWx0Lmh0bTBABggrBgEFBQcCAjA0
# HjIgHQBMAGUAZwBhAGwAXwBQAG8AbABpAGMAeQBfAFMAdABhAHQAZQBtAGUAbgB0
# AC4gHTANBgkqhkiG9w0BAQsFAAOCAgEAB+aIUQ3ixuCYP4FxAz2do6Ehb7Prpsz1
# Mb7PBeKp/vpXbRkws8LFZslq3/Xn8Hi9x6ieJeP5vO1rVFcIK1GCRBL7uVOMzPRg
# Eop2zEBAQZvcXBf/XPleFzWYJFZLdO9CEMivv3/Gf/I3fVo/HPKZeUqRUgCvOA8X
# 9S95gWXZqbVr5MfO9sp6AG9LMEQkIjzP7QOllo9ZKby2/QThcJ8ySif9Va8v/rbl
# jjO7Yl+a21dA6fHOmWaQjP9qYn/dxUoLkSbiOewZSnFjnXshbcOco6I8+n99lmqQ
# eKZt0uGc+R38ONiU9MalCpaGpL2eGq4EQoO4tYCbIjggtSXlZOz39L9+Y1klD3ou
# OVd2onGqBooPiRa6YacRy5rYDkeagMXQzafQ732D8OE7cQnfXXSYIghh2rBQHm+9
# 8eEA3+cxB6STOvdlR3jo+KhIq/fecn5ha293qYHLpwmsObvsxsvYgrRyzR30uIUB
# HoD7G4kqVDmyW9rIDVWZeodzOwjmmC3qjeAzLhIp9cAvVCch98isTtoouLGp25ay
# p0Kiyc8ZQU3ghvkqmqMRZjDTu3QyS99je/WZii8bxyGvWbWu3EQ8l1Bx16HSxVXj
# ad5XwdHeMMD9zOZN+w2/XU/pnR4ZOC+8z1gFLu8NoFA12u8JJxzVs341Hgi62jbb
# 01+P3nSISRKhggLLMIICNAIBATCB+KGB0KSBzTCByjELMAkGA1UEBhMCVVMxEzAR
# BgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAcBgNVBAoTFU1p
# Y3Jvc29mdCBDb3Jwb3JhdGlvbjElMCMGA1UECxMcTWljcm9zb2Z0IEFtZXJpY2Eg
# T3BlcmF0aW9uczEmMCQGA1UECxMdVGhhbGVzIFRTUyBFU046N0JGMS1FM0VBLUI4
# MDgxJTAjBgNVBAMTHE1pY3Jvc29mdCBUaW1lLVN0YW1wIFNlcnZpY2WiIwoBATAH
# BgUrDgMCGgMVAKCir3PxP6RCCyVMJSAVoMV61yNeoIGDMIGApH4wfDELMAkGA1UE
# BhMCVVMxEzARBgNVBAgTCldhc2hpbmd0b24xEDAOBgNVBAcTB1JlZG1vbmQxHjAc
# BgNVBAoTFU1pY3Jvc29mdCBDb3Jwb3JhdGlvbjEmMCQGA1UEAxMdTWljcm9zb2Z0
# IFRpbWUtU3RhbXAgUENBIDIwMTAwDQYJKoZIhvcNAQEFBQACBQDlORF4MCIYDzIw
# MjExMTEzMDAxNDQ4WhgPMjAyMTExMTQwMDE0NDhaMHQwOgYKKwYBBAGEWQoEATEs
# MCowCgIFAOU5EXgCAQAwBwIBAAICCG0wBwIBAAICEXEwCgIFAOU6YvgCAQAwNgYK
# KwYBBAGEWQoEAjEoMCYwDAYKKwYBBAGEWQoDAqAKMAgCAQACAwehIKEKMAgCAQAC
# AwGGoDANBgkqhkiG9w0BAQUFAAOBgQBEVzAGaoO9zID4cCx/eoX1CLyom48tzLtW
# hA5lFYu8Q2fJbuNoUizn4BZssVWz24Yyc4/4lPg0HffOrUbM+FVDpDuQ7XUn+Ylf
# jNvAK5EblcAvHi5bJ9DXgWTCN3nD/39tQwk2ufhlpGMXeh5+KBiqNlsPh4ihchhw
# OyfVnAN0PDGCAw0wggMJAgEBMIGTMHwxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpX
# YXNoaW5ndG9uMRAwDgYDVQQHEwdSZWRtb25kMR4wHAYDVQQKExVNaWNyb3NvZnQg
# Q29ycG9yYXRpb24xJjAkBgNVBAMTHU1pY3Jvc29mdCBUaW1lLVN0YW1wIFBDQSAy
# MDEwAhMzAAABUcNQ51lsqsanAAAAAAFRMA0GCWCGSAFlAwQCAQUAoIIBSjAaBgkq
# hkiG9w0BCQMxDQYLKoZIhvcNAQkQAQQwLwYJKoZIhvcNAQkEMSIEIKHZ/lGBXobp
# YJBveag1cyqTQjkRZaL4tee8HYrS0VzlMIH6BgsqhkiG9w0BCRACLzGB6jCB5zCB
# 5DCBvQQgLs1cmYj41sFZBwCmFvv9ScP5tuuUhxsv/t0B9XF65UEwgZgwgYCkfjB8
# MQswCQYDVQQGEwJVUzETMBEGA1UECBMKV2FzaGluZ3RvbjEQMA4GA1UEBxMHUmVk
# bW9uZDEeMBwGA1UEChMVTWljcm9zb2Z0IENvcnBvcmF0aW9uMSYwJAYDVQQDEx1N
# aWNyb3NvZnQgVGltZS1TdGFtcCBQQ0EgMjAxMAITMwAAAVHDUOdZbKrGpwAAAAAB
# UTAiBCAB7saHneNCM9WBz+NT6Mlm11hSHTIfrVzlsb9O+THHMTANBgkqhkiG9w0B
# AQsFAASCAQAiGZGXY0BNbh8R9nEmo5318mortZH8wxW5VJsWjk/+f/Nab+h+jMKv
# dVjFBOsfqS0bvjQSYvtV4/OrcBM+ku7dYtD7gxrE1cZPMmrpg67DWEcKzJFDu0hl
# o6+dScQ4kue5XA2vE3mkfY88bJOnMrQW5MygjcKEoFA3JVgStmA4uYeu7B/KzbnZ
# 4D2q2gLwcZQbxHODo959JjEGcPiRii33BjDcd6uSDn/2rEQv13QialYHd9uvRmOb
# +bocWDt9omytzNoqbRKR0P/vI9kfR56B8R/WgrHGzsn9Tib+Txf6JNkzPgAKk41r
# yEbYLUfgBAlz+eYaVkuG4PiStxDgsE3S
# SIG # End signature block
