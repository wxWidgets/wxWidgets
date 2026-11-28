# Run a command line on a separate, invisible Win32 desktop and wait for it.
# Used for GUI tests: their windows never appear on the interactive desktop
# and they cannot take the user's focus or input.
param(
    [string]$CommandLine,
    [string]$WorkDir,
    [string]$OutFile,
    [int]$TimeoutSeconds = 300
)

Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;
using System.Text;

public static class WxDeskRun
{
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
    struct STARTUPINFO
    {
        public int cb; public string lpReserved; public string lpDesktop; public string lpTitle;
        public int dwX, dwY, dwXSize, dwYSize, dwXCountChars, dwYCountChars, dwFillAttribute, dwFlags;
        public short wShowWindow, cbReserved2; public IntPtr lpReserved2, hStdInput, hStdOutput, hStdError;
    }
    [StructLayout(LayoutKind.Sequential)]
    struct PROCESS_INFORMATION { public IntPtr hProcess, hThread; public int dwProcessId, dwThreadId; }

    [DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern IntPtr CreateDesktop(string name, IntPtr device, IntPtr devmode, int flags, uint access, IntPtr sa);
    [DllImport("user32.dll")] static extern bool CloseDesktop(IntPtr h);
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern bool CreateProcess(string app, StringBuilder cmd, IntPtr pa, IntPtr ta, bool inherit, uint flags,
                                     IntPtr env, string cwd, ref STARTUPINFO si, out PROCESS_INFORMATION pi);
    [DllImport("kernel32.dll")] static extern uint WaitForSingleObject(IntPtr h, uint ms);
    [DllImport("kernel32.dll")] static extern bool GetExitCodeProcess(IntPtr h, out uint code);
    [DllImport("kernel32.dll")] static extern bool TerminateProcess(IntPtr h, uint code);
    [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);

    public static string Run(string commandLine, string cwd, int timeoutSeconds)
    {
        IntPtr desk = CreateDesktop("wxwinui-tests", IntPtr.Zero, IntPtr.Zero, 0, 0x10000000, IntPtr.Zero);
        if (desk == IntPtr.Zero) return "cannot create desktop";
        try
        {
            STARTUPINFO si = new STARTUPINFO();
            si.cb = Marshal.SizeOf(typeof(STARTUPINFO));
            si.lpDesktop = "wxwinui-tests";
            PROCESS_INFORMATION pi;
            const uint CREATE_NO_WINDOW = 0x08000000;
            if (!CreateProcess(null, new StringBuilder(commandLine), IntPtr.Zero, IntPtr.Zero, false, CREATE_NO_WINDOW,
                               IntPtr.Zero, cwd, ref si, out pi))
                return "cannot start, error " + Marshal.GetLastWin32Error();
            try
            {
                if (WaitForSingleObject(pi.hProcess, (uint)(timeoutSeconds * 1000)) != 0)
                {
                    TerminateProcess(pi.hProcess, 99);
                    return "TIMEOUT after " + timeoutSeconds + " s";
                }
                uint code; GetExitCodeProcess(pi.hProcess, out code);
                return "exit code " + (int)code;
            }
            finally { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); }
        }
        finally { CloseDesktop(desk); }
    }
}
'@

Remove-Item -LiteralPath $OutFile -ErrorAction SilentlyContinue
$wrapped = 'cmd.exe /d /c "' + $CommandLine + ' > "' + $OutFile + '" 2>&1"'
[WxDeskRun]::Run($wrapped, $WorkDir, $TimeoutSeconds)
