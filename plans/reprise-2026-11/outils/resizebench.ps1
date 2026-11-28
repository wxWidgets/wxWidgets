# Resize benchmark: start a program on a separate, invisible Win32 desktop,
# resize its main window N times with SetWindowPos and time each step.
# Nothing appears on the interactive desktop and no input is injected.
# The in-process flush cost is written by the program itself when
# WX_WINUI_INPUT_PROFILE names a file.
param(
    [string]$Exe,
    [int]$Steps = 300,
    [int]$WarmupSeconds = 5,
    [string]$ProfileFile = ''
)

Add-Type -TypeDefinition @'
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

public static class WxResizeBench
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
    [StructLayout(LayoutKind.Sequential)]
    struct RECT { public int left, top, right, bottom; }

    delegate bool EnumProc(IntPtr hwnd, IntPtr lParam);

    [DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern IntPtr CreateDesktop(string name, IntPtr device, IntPtr devmode, int flags, uint access, IntPtr sa);
    [DllImport("user32.dll")] static extern bool CloseDesktop(IntPtr h);
    [DllImport("user32.dll", SetLastError = true)] static extern bool SetThreadDesktop(IntPtr h);
    [DllImport("user32.dll")] static extern bool EnumDesktopWindows(IntPtr desk, EnumProc proc, IntPtr lParam);
    [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr hwnd, out int pid);
    [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr hwnd);
    [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr hwnd, out RECT rc);
    [DllImport("user32.dll")] static extern IntPtr GetWindow(IntPtr hwnd, uint cmd);
    [DllImport("user32.dll")] static extern bool SetWindowPos(IntPtr hwnd, IntPtr after, int x, int y, int cx, int cy, uint flags);
    [DllImport("user32.dll")] static extern IntPtr SendMessageTimeout(IntPtr hwnd, uint msg, IntPtr wp, IntPtr lp, uint flags, uint ms, out IntPtr result);
    [DllImport("user32.dll")] static extern bool PostMessage(IntPtr hwnd, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] static extern bool EnumChildWindows(IntPtr parent, EnumProc proc, IntPtr lParam);
    [DllImport("user32.dll")] static extern IntPtr GetDC(IntPtr hwnd);
    [DllImport("user32.dll")] static extern int ReleaseDC(IntPtr hwnd, IntPtr dc);

    // Cost of GetDC on the program's child windows, measured from outside.
    static string MeasureGetDC(IntPtr main)
    {
        List<IntPtr> kids = new List<IntPtr>();
        int all = 0;
        EnumChildWindows(main, delegate(IntPtr h, IntPtr l) { all++; if (IsWindowVisible(h)) kids.Add(h); return true; }, IntPtr.Zero);
        List<double> us = new List<double>();
        foreach (IntPtr h in kids)
        {
            Stopwatch sw = Stopwatch.StartNew();
            for (int i = 0; i < 5; i++) { IntPtr dc = GetDC(h); if (dc != IntPtr.Zero) ReleaseDC(h, dc); }
            us.Add(sw.Elapsed.TotalMilliseconds * 1000.0 / 5);
        }
        if (us.Count == 0) return "no child windows";
        us.Sort();
        double sum = 0; foreach (double v in us) sum += v;
        return string.Format(System.Globalization.CultureInfo.InvariantCulture,
            "child HWNDs: {0} ({1} visible) | GetDC+ReleaseDC per call: median {2:F0} us, p95 {3:F0} us, max {4:F0} us, mean {5:F0} us",
            all, us.Count, us[us.Count / 2], us[(int)(us.Count * 0.95)], us[us.Count - 1], sum / us.Count);
    }
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern bool CreateProcess(string app, StringBuilder cmd, IntPtr pa, IntPtr ta, bool inherit, uint flags,
                                     IntPtr env, string cwd, ref STARTUPINFO si, out PROCESS_INFORMATION pi);
    [DllImport("kernel32.dll")] static extern uint WaitForSingleObject(IntPtr h, uint ms);
    [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);

    const uint WM_NULL = 0, WM_CLOSE = 0x10;
    const uint SWP_NOMOVE = 2, SWP_NOZORDER = 4, SWP_NOACTIVATE = 0x10;

    public static string Run(string exe, string cwd, int steps, int warmupSeconds)
    {
        string result = "no result";
        Thread worker = new Thread(delegate() { result = RunOnDesktop(exe, cwd, steps, warmupSeconds); });
        worker.Start();
        worker.Join();
        return result;
    }

    static string RunOnDesktop(string exe, string cwd, int steps, int warmupSeconds)
    {
        IntPtr desk = CreateDesktop("wxwinui-bench", IntPtr.Zero, IntPtr.Zero, 0, 0x10000000, IntPtr.Zero);
        if (desk == IntPtr.Zero) return "cannot create desktop";
        if (!SetThreadDesktop(desk)) return "cannot enter desktop, error " + Marshal.GetLastWin32Error();

        STARTUPINFO si = new STARTUPINFO();
        si.cb = Marshal.SizeOf(typeof(STARTUPINFO));
        si.lpDesktop = "wxwinui-bench";
        PROCESS_INFORMATION pi;
        if (!CreateProcess(null, new StringBuilder("\"" + exe + "\""), IntPtr.Zero, IntPtr.Zero, false, 0,
                           IntPtr.Zero, cwd, ref si, out pi))
            return "cannot start, error " + Marshal.GetLastWin32Error();
        try
        {
            if (WaitForSingleObject(pi.hProcess, (uint)(warmupSeconds * 1000)) == 0)
                return "program exited during warm-up";

            int pid = pi.dwProcessId;
            IntPtr main = IntPtr.Zero; int bestArea = 0;
            EnumDesktopWindows(desk, delegate(IntPtr hwnd, IntPtr l)
            {
                int owner; GetWindowThreadProcessId(hwnd, out owner);
                RECT rc;
                if (owner == pid && IsWindowVisible(hwnd) && GetWindow(hwnd, 4) == IntPtr.Zero && GetWindowRect(hwnd, out rc))
                {
                    int area = (rc.right - rc.left) * (rc.bottom - rc.top);
                    if (area > bestArea) { bestArea = area; main = hwnd; }
                }
                return true;
            }, IntPtr.Zero);
            if (main == IntPtr.Zero) return "no main window found";

            RECT start; GetWindowRect(main, out start);
            int w0 = 1100, h0 = 760;
            IntPtr ignored;
            SetWindowPos(main, IntPtr.Zero, 0, 0, w0, h0, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
            SendMessageTimeout(main, WM_NULL, IntPtr.Zero, IntPtr.Zero, 0, 5000, out ignored);
            Thread.Sleep(500);

            string dcCost = MeasureGetDC(main);
            List<double> sync = new List<double>(), settled = new List<double>();
            Stopwatch total = Stopwatch.StartNew();
            for (int i = 0; i < steps; i++)
            {
                // Triangle wave: grow then shrink, 6 px per step on both axes.
                int phase = i % 80; int delta = (phase < 40 ? phase : 80 - phase) * 6;
                Stopwatch sw = Stopwatch.StartNew();
                SetWindowPos(main, IntPtr.Zero, 0, 0, w0 + delta, h0 + delta / 2, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
                double tSync = sw.Elapsed.TotalMilliseconds;
                // Two round trips through the message loop let deferred work run.
                SendMessageTimeout(main, WM_NULL, IntPtr.Zero, IntPtr.Zero, 0, 5000, out ignored);
                SendMessageTimeout(main, WM_NULL, IntPtr.Zero, IntPtr.Zero, 0, 5000, out ignored);
                sync.Add(tSync); settled.Add(sw.Elapsed.TotalMilliseconds);
            }
            total.Stop();
            PostMessage(main, WM_CLOSE, IntPtr.Zero, IntPtr.Zero);
            bool closed = WaitForSingleObject(pi.hProcess, 8000) == 0;

            sync.Sort(); settled.Sort();
            return string.Format(System.Globalization.CultureInfo.InvariantCulture,
                "{9}\nsteps={0} total={1:F0} ms | resize call: median {2:F2} p95 {3:F2} max {4:F2} ms | settled: median {5:F2} p95 {6:F2} max {7:F2} ms | closed cleanly: {8}",
                steps, total.Elapsed.TotalMilliseconds,
                sync[sync.Count / 2], sync[(int)(sync.Count * 0.95)], sync[sync.Count - 1],
                settled[settled.Count / 2], settled[(int)(settled.Count * 0.95)], settled[settled.Count - 1], closed, dcCost);
        }
        finally { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); }
    }
}
'@

if ($ProfileFile) { Remove-Item -LiteralPath $ProfileFile -ErrorAction SilentlyContinue; $env:WX_WINUI_INPUT_PROFILE = $ProfileFile }
$out = [WxResizeBench]::Run($Exe, (Split-Path $Exe), $Steps, $WarmupSeconds)
if ($ProfileFile) { Remove-Item Env:\WX_WINUI_INPUT_PROFILE }
"{0}: {1}" -f (Split-Path $Exe -Leaf), $out
if ($ProfileFile -and (Test-Path -LiteralPath $ProfileFile)) { Select-String -LiteralPath $ProfileFile -Pattern '^flush:' | ForEach-Object { $_.Line } | Select-Object -Last 4 }
