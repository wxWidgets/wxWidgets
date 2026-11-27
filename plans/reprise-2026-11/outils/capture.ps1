# Start a program on a separate, invisible Win32 desktop and save a picture of
# its main window with PrintWindow. Nothing appears on the interactive desktop
# and no input is injected.
# Run with Windows PowerShell 5.1 (powershell.exe), which has System.Drawing.
param(
    [string]$Exe,
    [string]$Out,
    [int]$WarmupSeconds = 5,
    [int]$Width = 1000,
    [int]$Height = 700
)

Add-Type -ReferencedAssemblies System.Drawing -TypeDefinition @'
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

public static class WxCapture
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
    [DllImport("user32.dll", SetLastError = true)] static extern bool SetThreadDesktop(IntPtr h);
    [DllImport("user32.dll")] static extern bool EnumDesktopWindows(IntPtr desk, EnumProc proc, IntPtr lParam);
    [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr hwnd, out int pid);
    [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr hwnd);
    [DllImport("user32.dll")] static extern IntPtr GetWindow(IntPtr hwnd, uint cmd);
    [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr hwnd, out RECT rc);
    [DllImport("user32.dll")] static extern bool SetWindowPos(IntPtr hwnd, IntPtr after, int x, int y, int cx, int cy, uint flags);
    [DllImport("user32.dll")] static extern bool PrintWindow(IntPtr hwnd, IntPtr hdc, uint flags);
    [DllImport("user32.dll")] static extern IntPtr SendMessageTimeout(IntPtr hwnd, uint msg, IntPtr wp, IntPtr lp, uint flags, uint ms, out IntPtr result);
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern bool CreateProcess(string app, StringBuilder cmd, IntPtr pa, IntPtr ta, bool inherit, uint flags,
                                     IntPtr env, string cwd, ref STARTUPINFO si, out PROCESS_INFORMATION pi);
    [DllImport("kernel32.dll")] static extern uint WaitForSingleObject(IntPtr h, uint ms);
    [DllImport("kernel32.dll")] static extern bool TerminateProcess(IntPtr h, uint code);
    [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);

    public static string Run(string exe, string cwd, string outBase, int warmup, int width, int height)
    {
        string result = "no result";
        Thread t = new Thread(delegate() { result = Work(exe, cwd, outBase, warmup, width, height); });
        t.Start(); t.Join();
        return result;
    }

    static string Work(string exe, string cwd, string outBase, int warmup, int width, int height)
    {
        IntPtr desk = CreateDesktop("wxwinui-capture", IntPtr.Zero, IntPtr.Zero, 0, 0x10000000, IntPtr.Zero);
        if (desk == IntPtr.Zero || !SetThreadDesktop(desk)) return "cannot use desktop";
        STARTUPINFO si = new STARTUPINFO(); si.cb = Marshal.SizeOf(typeof(STARTUPINFO)); si.lpDesktop = "wxwinui-capture";
        PROCESS_INFORMATION pi;
        if (!CreateProcess(null, new StringBuilder("\"" + exe + "\""), IntPtr.Zero, IntPtr.Zero, false, 0, IntPtr.Zero, cwd, ref si, out pi))
            return "cannot start";
        try
        {
            if (WaitForSingleObject(pi.hProcess, (uint)(warmup * 1000)) == 0) return "exited during warm-up";
            int pid = pi.dwProcessId; IntPtr main = IntPtr.Zero; int best = 0;
            EnumDesktopWindows(desk, delegate(IntPtr h, IntPtr l)
            {
                int owner; GetWindowThreadProcessId(h, out owner); RECT rc;
                if (owner == pid && IsWindowVisible(h) && GetWindow(h, 4) == IntPtr.Zero && GetWindowRect(h, out rc))
                { int a = (rc.right - rc.left) * (rc.bottom - rc.top); if (a > best) { best = a; main = h; } }
                return true;
            }, IntPtr.Zero);
            if (main == IntPtr.Zero) return "no main window";
            IntPtr ignored;
            SetWindowPos(main, IntPtr.Zero, 40, 40, width, height, 0x4 | 0x10);
            SendMessageTimeout(main, 0, IntPtr.Zero, IntPtr.Zero, 0, 5000, out ignored);
            Thread.Sleep(1500);
            RECT r; GetWindowRect(main, out r);
            string report = "";
            foreach (uint flag in new uint[] { 2, 0 })
            {
                using (Bitmap bmp = new Bitmap(r.right - r.left, r.bottom - r.top, PixelFormat.Format32bppArgb))
                {
                    using (Graphics g = Graphics.FromImage(bmp))
                    {
                        IntPtr hdc = g.GetHdc();
                        bool ok = PrintWindow(main, hdc, flag);
                        g.ReleaseHdc(hdc);
                        report += "flag " + flag + ": PrintWindow=" + ok + "; ";
                    }
                    bmp.Save(outBase + "-flag" + flag + ".png", ImageFormat.Png);
                }
            }
            TerminateProcess(pi.hProcess, 0);
            return report;
        }
        finally { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); }
    }
}
'@

[WxCapture]::Run($Exe, (Split-Path $Exe), $Out, $WarmupSeconds, $Width, $Height)

