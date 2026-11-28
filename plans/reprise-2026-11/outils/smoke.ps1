# Start each program on a separate, invisible Win32 desktop, check that it is
# still alive and has shown a top-level window after a few seconds, then end it.
# Nothing appears on the interactive desktop and no input is injected.
param(
    [string]$BinDir,
    [string[]]$Programs,
    [int]$Seconds = 6
)

Add-Type -TypeDefinition @'
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

public static class WxSmoke
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

    delegate bool EnumProc(IntPtr hwnd, IntPtr lParam);

    [DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern IntPtr CreateDesktop(string name, IntPtr device, IntPtr devmode, int flags, uint access, IntPtr sa);
    [DllImport("user32.dll")] static extern bool CloseDesktop(IntPtr h);
    [DllImport("user32.dll")] static extern bool EnumDesktopWindows(IntPtr desk, EnumProc proc, IntPtr lParam);
    [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr hwnd, out int pid);
    [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr hwnd);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetWindowText(IntPtr hwnd, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetClassName(IntPtr hwnd, StringBuilder sb, int max);
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern bool CreateProcess(string app, StringBuilder cmd, IntPtr pa, IntPtr ta, bool inherit, uint flags,
                                     IntPtr env, string cwd, ref STARTUPINFO si, out PROCESS_INFORMATION pi);
    [DllImport("kernel32.dll")] static extern uint WaitForSingleObject(IntPtr h, uint ms);
    [DllImport("kernel32.dll")] static extern bool GetExitCodeProcess(IntPtr h, out uint code);
    [DllImport("kernel32.dll")] static extern bool TerminateProcess(IntPtr h, uint code);
    [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);

    public static string Run(string exe, string cwd, int seconds)
    {
        const uint GENERIC_ALL = 0x10000000;
        IntPtr desk = CreateDesktop("wxwinui-smoke", IntPtr.Zero, IntPtr.Zero, 0, GENERIC_ALL, IntPtr.Zero);
        if (desk == IntPtr.Zero) return "cannot create desktop, error " + Marshal.GetLastWin32Error();
        try
        {
            STARTUPINFO si = new STARTUPINFO();
            si.cb = Marshal.SizeOf(typeof(STARTUPINFO));
            si.lpDesktop = "wxwinui-smoke";
            PROCESS_INFORMATION pi;
            StringBuilder cmd = new StringBuilder("\"" + exe + "\"");
            if (!CreateProcess(null, cmd, IntPtr.Zero, IntPtr.Zero, false, 0, IntPtr.Zero, cwd, ref si, out pi))
                return "cannot start, error " + Marshal.GetLastWin32Error();
            try
            {
                uint wait = WaitForSingleObject(pi.hProcess, (uint)(seconds * 1000));
                if (wait == 0)
                {
                    uint code; GetExitCodeProcess(pi.hProcess, out code);
                    return "EXITED EARLY with code 0x" + code.ToString("X8");
                }
                int pid = pi.dwProcessId;
                List<string> windows = new List<string>();
                EnumDesktopWindows(desk, delegate(IntPtr hwnd, IntPtr l)
                {
                    int owner; GetWindowThreadProcessId(hwnd, out owner);
                    if (owner == pid && IsWindowVisible(hwnd))
                    {
                        StringBuilder t = new StringBuilder(200), c = new StringBuilder(100);
                        GetWindowText(hwnd, t, 200); GetClassName(hwnd, c, 100);
                        windows.Add("\"" + t + "\" [" + c + "]");
                    }
                    return true;
                }, IntPtr.Zero);
                TerminateProcess(pi.hProcess, 0);
                return "alive after " + seconds + " s, visible windows: " +
                       (windows.Count == 0 ? "NONE" : string.Join("; ", windows.ToArray()));
            }
            finally { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); }
        }
        finally { CloseDesktop(desk); }
    }
}
'@

$dumpDir = Join-Path $env:LOCALAPPDATA 'CrashDumps'
$before = @(Get-ChildItem $dumpDir -ErrorAction SilentlyContinue).Count
foreach ($p in $Programs) {
    $exe = Join-Path $BinDir "$p.exe"
    if (-not (Test-Path $exe)) { "{0,-14} MISSING" -f $p; continue }
    "{0,-14} {1}" -f $p, [WxSmoke]::Run($exe, $BinDir, $Seconds)
}
$after = @(Get-ChildItem $dumpDir -ErrorAction SilentlyContinue).Count
"new crash dumps: $($after - $before)"
