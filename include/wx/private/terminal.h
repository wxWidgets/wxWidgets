///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/terminal.h
// Purpose:     Helpers for working with terminal output
// Author:      Vadim Zeitlin
// Created:     2024-11-22
// Copyright:   (c) 2024 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_TERMINAL_H_
#define _WX_PRIVATE_TERMINAL_H_

#ifdef __WINDOWS__
    #include "wx/utils.h"

    #include "wx/msw/wrapwin.h"
#endif

#ifdef __UNIX__
    #include <stdio.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
#endif

namespace wxTerminal
{

// Return the current terminal width or 0 if we couldn't find it.
inline int GetWidth()
{
#ifdef __WINDOWS__
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if ( ::GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi) )
      return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#elif defined TIOCGWINSZ
    winsize w;
    int fd = fileno(stdout);
    if ( fd != -1 && ioctl(fd, TIOCGWINSZ, &w) == 0 )
      return w.ws_col;
#endif

    return 0;
}

} // namespace wxTerminal

#endif // _WX_PRIVATE_TERMINAL_H_
