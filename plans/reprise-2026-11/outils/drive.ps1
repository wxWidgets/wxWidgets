# Drive a program on a separate, invisible Win32 desktop with a small script:
# post window messages, inspect the window tree, read the capture/focus state
# and save pictures of its windows. Nothing appears on the interactive desktop
# and no real input is injected.
# Run with Windows PowerShell 5.1 (powershell.exe), which has System.Drawing.
#
# Script commands, one per line:
#   wait <ms>
#   tree [var]                      list the visible windows (of var, or all top-level ones)
#   top <name> <title-substring>    bind a visible top-level window of the program
#   topbig <name>                   bind the largest visible top-level wx window of the program
#   child <name> <parent> <class-substring> [index]   bind a descendant window by class
#   size <var> <w> <h>              resize a window
#   command <var> <id>              post WM_COMMAND
#   move|down|up|rdown|rup <var> <x> <y>   post a client mouse message
#   ncmove <var> <hit> <sx> <sy>    post WM_NCMOUSEMOVE with screen coordinates
#   key <var> <vk>                  post WM_KEYDOWN + WM_KEYUP
#   post <var> <msg-hex> <wparam> <lparam>
#   state <var>                     print capture / focus / active of the window's thread
#   bindfocus <name> <var>          bind the window which has the focus in the thread of <var>
#   shot <var> <file.png>           save a picture of the window
#       On the invisible desktop Windows delivers WM_PAINT to a window only
#       when something asks for it, which the picture does: what a window
#       paints after a resize is painted during the shot, and XAML shows it
#       a frame later. After "size", take the picture twice ("shot", "wait
#       500", "shot") and keep the second one.
#   shotall <prefix>                save a picture of every visible top-level window (prefix-N.png)
#   closetop <title-substring>      post WM_CLOSE to the matching top-level windows
#   close <var>                     post WM_CLOSE
param(
    [string]$Exe,
    [string]$Script,
    [int]$WarmupSeconds = 5,
    [string[]]$Env = @(),
    [string]$ExeArgs = ''
)

Add-Type -ReferencedAssemblies System.Drawing, UIAutomationClient, UIAutomationTypes, WindowsBase -TypeDefinition @'
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows.Automation;

public static class WxDrive
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
    [StructLayout(LayoutKind.Sequential)]
    struct GUITHREADINFO
    {
        public int cbSize, flags; public IntPtr hwndActive, hwndFocus, hwndCapture, hwndMenuOwner, hwndMoveSize, hwndCaret;
        public RECT rcCaret;
    }
    delegate bool EnumProc(IntPtr hwnd, IntPtr lParam);

    [DllImport("user32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern IntPtr CreateDesktop(string name, IntPtr device, IntPtr devmode, int flags, uint access, IntPtr sa);
    [DllImport("user32.dll", SetLastError = true)] static extern bool SetThreadDesktop(IntPtr h);
    [DllImport("user32.dll")] static extern IntPtr SetThreadDpiAwarenessContext(IntPtr context);
    [DllImport("user32.dll")] static extern bool EnumDesktopWindows(IntPtr desk, EnumProc proc, IntPtr lParam);
    [DllImport("user32.dll")] static extern bool EnumChildWindows(IntPtr parent, EnumProc proc, IntPtr lParam);
    [DllImport("user32.dll")] static extern uint GetWindowThreadProcessId(IntPtr hwnd, out int pid);
    [DllImport("user32.dll")] static extern bool IsWindowVisible(IntPtr hwnd);
    [DllImport("user32.dll")] static extern bool IsWindowEnabled(IntPtr hwnd);
    [DllImport("user32.dll")] static extern bool IsWindow(IntPtr hwnd);
    [DllImport("user32.dll")] static extern IntPtr GetParent(IntPtr hwnd);
    [DllImport("user32.dll")] static extern bool GetWindowRect(IntPtr hwnd, out RECT rc);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetWindowText(IntPtr hwnd, StringBuilder sb, int max);
    [DllImport("user32.dll", CharSet = CharSet.Unicode)] static extern int GetClassName(IntPtr hwnd, StringBuilder sb, int max);
    [DllImport("user32.dll")] static extern bool SetWindowPos(IntPtr hwnd, IntPtr after, int x, int y, int cx, int cy, uint flags);
    [DllImport("user32.dll")] static extern bool PrintWindow(IntPtr hwnd, IntPtr hdc, uint flags);
    [StructLayout(LayoutKind.Sequential)]
    struct SCROLLBARINFO { public int cbSize; public RECT rcScrollBar; public int dxyLineButton, xyThumbTop, xyThumbBottom, reserved;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 6)] public int[] rgstate; }
    [DllImport("user32.dll")] static extern bool GetScrollBarInfo(IntPtr hwnd, int idObject, ref SCROLLBARINFO info);
    [DllImport("user32.dll")] static extern IntPtr GetWindowLongPtr(IntPtr hwnd, int index);
    [DllImport("user32.dll")] static extern int GetWindowRgn(IntPtr hwnd, IntPtr rgn);
    [DllImport("gdi32.dll")] static extern IntPtr CreateRectRgn(int l, int t, int r, int b);
    [DllImport("gdi32.dll")] static extern int GetRegionData(IntPtr rgn, int count, byte[] data);
    [DllImport("gdi32.dll")] static extern bool DeleteObject(IntPtr h);
    [DllImport("user32.dll")] static extern bool PostMessage(IntPtr hwnd, uint msg, IntPtr wp, IntPtr lp);
    [DllImport("user32.dll")] static extern IntPtr SendMessageTimeout(IntPtr hwnd, uint msg, IntPtr wp, IntPtr lp, uint flags, uint ms, out IntPtr result);
    [DllImport("user32.dll")] static extern bool GetGUIThreadInfo(uint thread, ref GUITHREADINFO info);
    [DllImport("kernel32.dll", CharSet = CharSet.Unicode, SetLastError = true)]
    static extern bool CreateProcess(string app, StringBuilder cmd, IntPtr pa, IntPtr ta, bool inherit, uint flags,
                                     IntPtr env, string cwd, ref STARTUPINFO si, out PROCESS_INFORMATION pi);
    [DllImport("kernel32.dll")] static extern uint WaitForSingleObject(IntPtr h, uint ms);
    [DllImport("kernel32.dll")] static extern bool GetExitCodeProcess(IntPtr h, out uint code);
    [DllImport("kernel32.dll")] static extern bool TerminateProcess(IntPtr h, uint code);
    [DllImport("kernel32.dll")] static extern bool CloseHandle(IntPtr h);

    static string Describe(IntPtr h)
    {
        StringBuilder t = new StringBuilder(200), c = new StringBuilder(200);
        GetWindowText(h, t, 200); GetClassName(h, c, 200);
        RECT r; GetWindowRect(h, out r);
        return string.Format("0x{0:X} [{1}] \"{2}\" ({3},{4} {5}x{6}){7}", h.ToInt64(), c, t, r.left, r.top,
                             r.right - r.left, r.bottom - r.top, IsWindowEnabled(h) ? "" : " DISABLED");
    }

    static IntPtr LParam(int x, int y) { return new IntPtr((int)(((uint)(ushort)y << 16) | (uint)(ushort)x)); }

    public static string Run(string exe, string exeArgs, string cwd, string[] script, int warmupSeconds)
    {
        StringBuilder log = new StringBuilder();
        Thread t = new Thread(delegate() { try { Work(exe, exeArgs, cwd, script, warmupSeconds, log); } catch (Exception e) { log.AppendLine("EXCEPTION " + e); } });
        t.Start(); t.Join();
        return log.ToString();
    }

    static void Work(string exe, string exeArgs, string cwd, string[] script, int warmupSeconds, StringBuilder log)
    {
        // Per-monitor aware, as the programs driven are: the sizes read and
        // the pictures taken are then in real pixels. Unaware, a picture of
        // a window on a screen scaled to 150% shows its top left part only.
        SetThreadDpiAwarenessContext(new IntPtr(-4));
        IntPtr desk = CreateDesktop("wxwinui-drive", IntPtr.Zero, IntPtr.Zero, 0, 0x10000000, IntPtr.Zero);
        if (desk == IntPtr.Zero || !SetThreadDesktop(desk)) { log.AppendLine("cannot use desktop"); return; }
        STARTUPINFO si = new STARTUPINFO(); si.cb = Marshal.SizeOf(typeof(STARTUPINFO)); si.lpDesktop = "wxwinui-drive";
        PROCESS_INFORMATION pi;
        if (!CreateProcess(null, new StringBuilder("\"" + exe + "\"" + (string.IsNullOrEmpty(exeArgs) ? "" : " " + exeArgs)), IntPtr.Zero, IntPtr.Zero, false, 0, IntPtr.Zero, cwd, ref si, out pi))
        { log.AppendLine("cannot start"); return; }
        try
        {
            if (WaitForSingleObject(pi.hProcess, (uint)(warmupSeconds * 1000)) == 0) { log.AppendLine("exited during warm-up"); return; }
            int pid = pi.dwProcessId;
            Dictionary<string, IntPtr> vars = new Dictionary<string, IntPtr>();
            foreach (string raw in script)
            {
                string line = raw.Trim();
                if (line.Length == 0 || line.StartsWith("#")) continue;
                string[] a = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
                uint exited; if (GetExitCodeProcess(pi.hProcess, out exited) && exited != 259)
                { log.AppendLine("PROGRAM EXITED with code 0x" + exited.ToString("X8") + " before: " + line); return; }
                switch (a[0])
                {
                    case "wait": Thread.Sleep(int.Parse(a[1])); break;
                    case "uia":
                    {
                        // The elements UI Automation shows under a window:
                        // type, name and identifier, to know what "invoke"
                        // can be given. An optional text keeps the lines
                        // which contain it.
                        string filter = a.Length > 2 ? line.Substring(line.IndexOf(a[2], line.IndexOf(a[1]) + a[1].Length)).Trim() : "";
                        AutomationElement root = AutomationElement.FromHandle(vars[a[1]]);
                        AutomationElementCollection all = root.FindAll(TreeScope.Descendants, Condition.TrueCondition);
                        int shown = 0;
                        foreach (AutomationElement e in all)
                        {
                            string text;
                            try
                            {
                                System.Windows.Rect r = e.Current.BoundingRectangle;
                                text = e.Current.ControlType.ProgrammaticName.Replace("ControlType.", "") + " name='" + e.Current.Name + "' id='" + e.Current.AutomationId + "' " +
                                    (r.IsEmpty ? "(no rect)" : ((int)r.X + "," + (int)r.Y + " " + (int)r.Width + "x" + (int)r.Height));
                            }
                            catch (Exception) { continue; }
                            if (filter.Length > 0 && text.IndexOf(filter, StringComparison.OrdinalIgnoreCase) < 0) continue;
                            if (++shown > 250) { log.AppendLine("  ..."); break; }
                            log.AppendLine("  " + text);
                        }
                        break;
                    }
                    case "invoke":
                    {
                        // Acts on a XAML control as a screen reader does: the
                        // pointer cannot be simulated on this desktop, UI
                        // Automation can. The element is the first whose name
                        // or identifier is the text (or contains it).
                        string want = line.Substring(line.IndexOf(a[2], line.IndexOf(a[1]) + a[1].Length)).Trim();
                        AutomationElement root = AutomationElement.FromHandle(vars[a[1]]);
                        AutomationElement found = null, partial = null;
                        foreach (AutomationElement e in root.FindAll(TreeScope.Descendants, Condition.TrueCondition))
                        {
                            string n, id;
                            try { n = e.Current.Name; id = e.Current.AutomationId; } catch (Exception) { continue; }
                            if (n == want || id == want) { found = e; break; }
                            if (partial == null && (n.IndexOf(want, StringComparison.OrdinalIgnoreCase) >= 0 || id.IndexOf(want, StringComparison.OrdinalIgnoreCase) >= 0)) partial = e;
                        }
                        // "#n:Type" is the n-th element of that control type
                        // (ComboBox, Button...), for those without a name.
                        if (want.StartsWith("#") && want.Contains(":"))
                        {
                            int nth = int.Parse(want.Substring(1, want.IndexOf(':') - 1));
                            string type = "ControlType." + want.Substring(want.IndexOf(':') + 1);
                            int seen = 0;
                            foreach (AutomationElement e in root.FindAll(TreeScope.Descendants, Condition.TrueCondition))
                            {
                                string t;
                                try { t = e.Current.ControlType.ProgrammaticName; } catch (Exception) { continue; }
                                if (t == type && seen++ == nth) { found = e; break; }
                            }
                        }
                        if (found == null) found = partial;
                        if (found == null) { log.AppendLine("invoke: NOT FOUND " + want); break; }
                        object pattern;
                        string done;
                        if (found.TryGetCurrentPattern(InvokePattern.Pattern, out pattern)) { ((InvokePattern)pattern).Invoke(); done = "invoked"; }
                        else if (found.TryGetCurrentPattern(TogglePattern.Pattern, out pattern)) { ((TogglePattern)pattern).Toggle(); done = "toggled"; }
                        else if (found.TryGetCurrentPattern(ExpandCollapsePattern.Pattern, out pattern))
                        {
                            ExpandCollapsePattern ec = (ExpandCollapsePattern)pattern;
                            if (ec.Current.ExpandCollapseState == ExpandCollapseState.Collapsed) ec.Expand(); else ec.Collapse();
                            done = "expanded or collapsed";
                        }
                        else if (found.TryGetCurrentPattern(SelectionItemPattern.Pattern, out pattern)) { ((SelectionItemPattern)pattern).Select(); done = "selected"; }
                        else done = "has no pattern to act on";
                        log.AppendLine("invoke: " + found.Current.ControlType.ProgrammaticName + " '" + found.Current.Name + "' " + done);
                        break;
                    }
                    case "uiascroll":
                    {
                        // Scrolls, as the wheel would, every scrollable
                        // element under a window whose rectangle contains
                        // the point given in screen pixels: uiascroll <var>
                        // <x> <y> <up|down> [count].
                        int sx = int.Parse(a[2]), sy = int.Parse(a[3]);
                        int times = a.Length > 5 ? int.Parse(a[5]) : 1;
                        AutomationElement root = AutomationElement.FromHandle(vars[a[1]]);
                        foreach (AutomationElement e in root.FindAll(TreeScope.Descendants, Condition.TrueCondition))
                        {
                            object pattern;
                            try
                            {
                                System.Windows.Rect r = e.Current.BoundingRectangle;
                                if (r.IsEmpty || !r.Contains(new System.Windows.Point(sx, sy))) continue;
                                if (!e.TryGetCurrentPattern(ScrollPattern.Pattern, out pattern)) continue;
                                ScrollPattern sp = (ScrollPattern)pattern;
                                if (!sp.Current.VerticallyScrollable) continue;
                                for (int i = 0; i < times; i++)
                                    sp.ScrollVertical(a[4] == "up" ? ScrollAmount.SmallDecrement : ScrollAmount.SmallIncrement);
                                log.AppendLine("uiascroll: " + e.Current.ControlType.ProgrammaticName + " '" + e.Current.Name + "' id='" + e.Current.AutomationId + "' at " + sp.Current.VerticalScrollPercent.ToString("0.###") + " %");
                            }
                            catch (Exception) { continue; }
                        }
                        break;
                    }
                    case "scrollbars":
                    {
                        // The descendants which have a native scrollbar band,
                        // with its rect on screen and its state.
                        EnumChildWindows(vars[a[1]], delegate(IntPtr h, IntPtr l)
                        {
                            long style = GetWindowLongPtr(h, -16).ToInt64();
                            int[] ids = { -5, -6 };   // OBJID_VSCROLL, OBJID_HSCROLL
                            long[] bits = { 0x00200000L, 0x00100000L };
                            for (int i = 0; i < 2; i++)
                            {
                                SCROLLBARINFO info = new SCROLLBARINFO(); info.cbSize = Marshal.SizeOf(typeof(SCROLLBARINFO));
                                bool got = GetScrollBarInfo(h, ids[i], ref info);
                                if ((style & bits[i]) == 0 && !(got && (info.rgstate[0] & 0x8000) == 0)) continue;
                                log.AppendLine("  " + (i == 0 ? "V" : "H") + " style=" + ((style & bits[i]) != 0) + " " +
                                    (got ? "rect " + info.rcScrollBar.left + "," + info.rcScrollBar.top + " " + (info.rcScrollBar.right - info.rcScrollBar.left) + "x" + (info.rcScrollBar.bottom - info.rcScrollBar.top) + " state 0x" + info.rgstate[0].ToString("X") : "no info") +
                                    " : " + Describe(h));
                            }
                            return true;
                        }, IntPtr.Zero);
                        break;
                    }
                    case "region":
                    {
                        // The rectangles of the region of a window (the island
                        // is cut where a native scrollbar must show).
                        IntPtr rgn = CreateRectRgn(0, 0, 0, 0);
                        int kind = GetWindowRgn(vars[a[1]], rgn);
                        if (kind == 0) log.AppendLine("  region: none");
                        else
                        {
                            int size = GetRegionData(rgn, 0, null);
                            byte[] data = new byte[size];
                            GetRegionData(rgn, size, data);
                            int count = BitConverter.ToInt32(data, 8);
                            log.AppendLine("  region: " + count + " rects");
                            for (int i = 0; i < count && i < 40; i++)
                            {
                                int o = 32 + i * 16;
                                int l0 = BitConverter.ToInt32(data, o), t0 = BitConverter.ToInt32(data, o + 4), r0 = BitConverter.ToInt32(data, o + 8), b0 = BitConverter.ToInt32(data, o + 12);
                                log.AppendLine("    " + l0 + "," + t0 + " " + (r0 - l0) + "x" + (b0 - t0));
                            }
                        }
                        DeleteObject(rgn);
                        break;
                    }
                    case "treeall":
                    {
                        // The children of a window, the hidden ones too.
                        EnumChildWindows(vars[a[1]], delegate(IntPtr h, IntPtr l)
                        { log.AppendLine((IsWindowVisible(h) ? "  " : "  (hidden) ") + Describe(h)); return true; }, IntPtr.Zero);
                        break;
                    }
                    case "tree":
                    {
                        EnumProc print = delegate(IntPtr h, IntPtr l) { if (IsWindowVisible(h)) log.AppendLine("  " + Describe(h)); return true; };
                        if (a.Length > 1) EnumChildWindows(vars[a[1]], print, IntPtr.Zero);
                        else EnumDesktopWindows(desk, delegate(IntPtr h, IntPtr l)
                        { int owner; GetWindowThreadProcessId(h, out owner); if (owner == pid && IsWindowVisible(h)) log.AppendLine("  " + Describe(h)); return true; }, IntPtr.Zero);
                        break;
                    }
                    case "topbig":
                    {
                        // The largest visible top-level wx window of the program.
                        IntPtr found = IntPtr.Zero; long best = 0;
                        EnumDesktopWindows(desk, delegate(IntPtr h, IntPtr l)
                        {
                            int owner; GetWindowThreadProcessId(h, out owner);
                            StringBuilder c = new StringBuilder(200); GetClassName(h, c, 200);
                            RECT r; GetWindowRect(h, out r);
                            long area = (long)(r.right - r.left) * (r.bottom - r.top);
                            if (owner == pid && IsWindowVisible(h) && c.ToString().StartsWith("wx") && area > best) { best = area; found = h; }
                            return true;
                        }, IntPtr.Zero);
                        vars[a[1]] = found; log.AppendLine(a[1] + " = " + (found == IntPtr.Zero ? "NOT FOUND" : Describe(found)));
                        break;
                    }
                    case "top":
                    {
                        IntPtr found = IntPtr.Zero; string want = line.Substring(line.IndexOf(a[1]) + a[1].Length).Trim();
                        EnumDesktopWindows(desk, delegate(IntPtr h, IntPtr l)
                        {
                            int owner; GetWindowThreadProcessId(h, out owner);
                            StringBuilder t = new StringBuilder(200); GetWindowText(h, t, 200);
                            if (owner == pid && IsWindowVisible(h) && found == IntPtr.Zero && t.ToString().Contains(want)) found = h;
                            return true;
                        }, IntPtr.Zero);
                        vars[a[1]] = found; log.AppendLine(a[1] + " = " + (found == IntPtr.Zero ? "NOT FOUND" : Describe(found)));
                        break;
                    }
                    case "child":
                    {
                        List<IntPtr> matches = new List<IntPtr>();
                        EnumChildWindows(vars[a[2]], delegate(IntPtr h, IntPtr l)
                        { StringBuilder c = new StringBuilder(200); GetClassName(h, c, 200); if (c.ToString().Contains(a[3])) matches.Add(h); return true; }, IntPtr.Zero);
                        int index = a.Length > 4 ? int.Parse(a[4]) : 0;
                        IntPtr found = index < matches.Count ? matches[index] : IntPtr.Zero;
                        vars[a[1]] = found; log.AppendLine(a[1] + " = " + (found == IntPtr.Zero ? "NOT FOUND" : Describe(found)) + " (" + matches.Count + " matches)");
                        break;
                    }
                    case "childt":
                    {
                        // Descendant whose window text contains the rest of the line.
                        string want = line.Substring(line.IndexOf(a[2], line.IndexOf(a[1]) + a[1].Length) + a[2].Length).Trim();
                        IntPtr found = IntPtr.Zero;
                        EnumChildWindows(vars[a[2]], delegate(IntPtr h, IntPtr l)
                        { StringBuilder t = new StringBuilder(200); GetWindowText(h, t, 200); if (found == IntPtr.Zero && IsWindowVisible(h) && t.ToString().Contains(want)) found = h; return true; }, IntPtr.Zero);
                        vars[a[1]] = found; log.AppendLine(a[1] + " = " + (found == IntPtr.Zero ? "NOT FOUND" : Describe(found)));
                        break;
                    }
                    case "childx":
                    {
                        // Descendant whose window text is exactly the rest of
                        // the line, hidden or not; an index may come first as
                        // "#n".
                        string rest = line.Substring(line.IndexOf(a[2], line.IndexOf(a[1]) + a[1].Length) + a[2].Length).Trim();
                        int nth = 0;
                        if (rest.StartsWith("#")) { int sp = rest.IndexOf(' '); nth = int.Parse(rest.Substring(1, sp - 1)); rest = rest.Substring(sp + 1).Trim(); }
                        List<IntPtr> same = new List<IntPtr>();
                        EnumChildWindows(vars[a[2]], delegate(IntPtr h, IntPtr l)
                        { StringBuilder t = new StringBuilder(200); GetWindowText(h, t, 200); if (t.ToString() == rest) same.Add(h); return true; }, IntPtr.Zero);
                        IntPtr got = nth < same.Count ? same[nth] : IntPtr.Zero;
                        vars[a[1]] = got; log.AppendLine(a[1] + " = " + (got == IntPtr.Zero ? "NOT FOUND" : Describe(got)));
                        break;
                    }
                    // Hides or shows a window from outside, to find which one
                    // something on screen belongs to.
                    case "hide": SetWindowPos(vars[a[1]], IntPtr.Zero, 0, 0, 0, 0, 0x80 | 0x1 | 0x2 | 0x4 | 0x10); break;
                    case "unhide": SetWindowPos(vars[a[1]], IntPtr.Zero, 0, 0, 0, 0, 0x40 | 0x1 | 0x2 | 0x4 | 0x10); break;
                    // Moves and sizes a window. Keep it inside the screen:
                    // what is beyond its edge is not painted by GDI, and
                    // a capture shows it cut there.
                    case "place": SetWindowPos(vars[a[1]], IntPtr.Zero, int.Parse(a[2]), int.Parse(a[3]), int.Parse(a[4]), int.Parse(a[5]), 0x4 | 0x10); break;
                    case "size": SetWindowPos(vars[a[1]], IntPtr.Zero, 0, 0, int.Parse(a[2]), int.Parse(a[3]), 0x2 | 0x4 | 0x10); break;
                    case "command": PostMessage(vars[a[1]], 0x0111, new IntPtr(int.Parse(a[2])), IntPtr.Zero); break;
                    case "move": PostMessage(vars[a[1]], 0x0200, IntPtr.Zero, LParam(int.Parse(a[2]), int.Parse(a[3]))); break;
                    case "down": PostMessage(vars[a[1]], 0x0201, new IntPtr(1), LParam(int.Parse(a[2]), int.Parse(a[3]))); break;
                    case "drag": PostMessage(vars[a[1]], 0x0200, new IntPtr(1), LParam(int.Parse(a[2]), int.Parse(a[3]))); break;
                    case "up": PostMessage(vars[a[1]], 0x0202, IntPtr.Zero, LParam(int.Parse(a[2]), int.Parse(a[3]))); break;
                    case "rdown": PostMessage(vars[a[1]], 0x0204, new IntPtr(2), LParam(int.Parse(a[2]), int.Parse(a[3]))); break;
                    case "rup": PostMessage(vars[a[1]], 0x0205, IntPtr.Zero, LParam(int.Parse(a[2]), int.Parse(a[3]))); break;
                    case "ncmove": PostMessage(vars[a[1]], 0x00A0, new IntPtr(int.Parse(a[2])), LParam(int.Parse(a[3]), int.Parse(a[4]))); break;
                    case "key": PostMessage(vars[a[1]], 0x0100, new IntPtr(int.Parse(a[2])), IntPtr.Zero); PostMessage(vars[a[1]], 0x0101, new IntPtr(int.Parse(a[2])), new IntPtr(unchecked((int)0xC0000001))); break;
                    case "post": PostMessage(vars[a[1]], Convert.ToUInt32(a[2], 16), new IntPtr(long.Parse(a[3])), new IntPtr(long.Parse(a[4]))); break;
                    case "close": PostMessage(vars[a[1]], 0x0010, IntPtr.Zero, IntPtr.Zero); break;
                    case "state":
                    {
                        int owner; uint thread = GetWindowThreadProcessId(vars[a[1]], out owner);
                        GUITHREADINFO g = new GUITHREADINFO(); g.cbSize = Marshal.SizeOf(typeof(GUITHREADINFO));
                        if (GetGUIThreadInfo(thread, ref g))
                            log.AppendLine("state: capture=" + (g.hwndCapture == IntPtr.Zero ? "none" : Describe(g.hwndCapture)) +
                                           " | focus=" + (g.hwndFocus == IntPtr.Zero ? "none" : Describe(g.hwndFocus)) +
                                           " | active=" + (g.hwndActive == IntPtr.Zero ? "none" : Describe(g.hwndActive)) +
                                           " | movesize=" + (g.hwndMoveSize == IntPtr.Zero ? "none" : "yes"));
                        else log.AppendLine("state: unavailable");
                        break;
                    }
                    case "bindfocus":
                    {
                        // bindfocus <new-var> <var>: the window which has the
                        // focus in the thread of <var>, to post keys to it.
                        int owner; uint thread = GetWindowThreadProcessId(vars[a[2]], out owner);
                        GUITHREADINFO g = new GUITHREADINFO(); g.cbSize = Marshal.SizeOf(typeof(GUITHREADINFO));
                        IntPtr focus = GetGUIThreadInfo(thread, ref g) ? g.hwndFocus : IntPtr.Zero;
                        vars[a[1]] = focus; log.AppendLine(a[1] + " = " + (focus == IntPtr.Zero ? "NO FOCUS" : Describe(focus)));
                        break;
                    }
                    case "shot":
                    {
                        IntPtr h = vars[a[1]]; IntPtr ignored;
                        SendMessageTimeout(h, 0, IntPtr.Zero, IntPtr.Zero, 0, 3000, out ignored);
                        RECT r; GetWindowRect(h, out r);
                        using (Bitmap bmp = new Bitmap(Math.Max(1, r.right - r.left), Math.Max(1, r.bottom - r.top), PixelFormat.Format32bppArgb))
                        {
                            using (Graphics g = Graphics.FromImage(bmp)) { IntPtr hdc = g.GetHdc(); PrintWindow(h, hdc, 2); g.ReleaseHdc(hdc); }
                            bmp.Save(a[2], ImageFormat.Png);
                        }
                        log.AppendLine("shot " + a[2] + " " + (r.right - r.left) + "x" + (r.bottom - r.top));
                        break;
                    }
                    case "shotall":
                    {
                        // One picture per visible top-level window of the program.
                        List<IntPtr> tops = new List<IntPtr>();
                        EnumDesktopWindows(desk, delegate(IntPtr h, IntPtr l)
                        { int owner; GetWindowThreadProcessId(h, out owner); if (owner == pid && IsWindowVisible(h)) tops.Add(h); return true; }, IntPtr.Zero);
                        int n = 0;
                        foreach (IntPtr h in tops)
                        {
                            RECT r; GetWindowRect(h, out r);
                            if (r.right - r.left < 8 || r.bottom - r.top < 8) continue;
                            IntPtr ignored; SendMessageTimeout(h, 0, IntPtr.Zero, IntPtr.Zero, 0, 3000, out ignored);
                            string file = a[1] + "-" + n++ + ".png";
                            using (Bitmap bmp = new Bitmap(r.right - r.left, r.bottom - r.top, PixelFormat.Format32bppArgb))
                            {
                                using (Graphics g = Graphics.FromImage(bmp)) { IntPtr hdc = g.GetHdc(); PrintWindow(h, hdc, 2); g.ReleaseHdc(hdc); }
                                bmp.Save(file, ImageFormat.Png);
                            }
                            log.AppendLine("shot " + file + " " + Describe(h));
                        }
                        break;
                    }
                    case "closetop":
                    {
                        // Close the visible top-level window whose title contains the rest of the line.
                        string want = line.Substring(a[0].Length).Trim();
                        EnumDesktopWindows(desk, delegate(IntPtr h, IntPtr l)
                        {
                            int owner; GetWindowThreadProcessId(h, out owner);
                            StringBuilder t = new StringBuilder(200); GetWindowText(h, t, 200);
                            if (owner == pid && IsWindowVisible(h) && t.ToString().Contains(want)) PostMessage(h, 0x0010, IntPtr.Zero, IntPtr.Zero);
                            return true;
                        }, IntPtr.Zero);
                        break;
                    }
                    default: log.AppendLine("unknown command: " + line); break;
                }
            }
            uint code; if (GetExitCodeProcess(pi.hProcess, out code) && code != 259) log.AppendLine("PROGRAM EXITED with code 0x" + code.ToString("X8"));
            else TerminateProcess(pi.hProcess, 0);
        }
        finally { CloseHandle(pi.hThread); CloseHandle(pi.hProcess); }
    }
}
'@

foreach ($pair in $Env) { $name, $value = $pair -split '=', 2; Set-Item -Path "Env:\$name" -Value $value }
[WxDrive]::Run($Exe, $ExeArgs, (Split-Path $Exe), (Get-Content -LiteralPath $Script), $WarmupSeconds)
