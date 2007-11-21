///////////////////////////////////////////////////////////////////////////////
// Name:        splitwise.cpp
// Purpose:     Use this to split wxwin2.wse in the distrib/msw directory
//              into wisetop.txt, wisebott.txt. This allows you to do the
//              following:
//
//              1) Edit the existing wxwin2.wse in WISE Install.
//              2) Call splitwise.exe to split off the non-file bits (i.e.
//                 preserve everything except the "item: Install File" lines).
//              3) Call makewise.bat to generate a new wxwin2.wse from
//                 wisetop.txt, wisebott.txt and the file list generated
//                 from the files in deliver\wx (which themselves have been
//                 put there by zipdist.bat).
//
//              If you don't wish to change the WISE settings, then there's no
//              need to use splitwise, but it's very likely that settings will
//              be altered, e.g. to change the version number, what's installed,
//              etc.
//
// Author:      Julian Smart
// Modified by:
// Created:     13/10/99
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char g_Line[1024];

int ReadLine(FILE* fd, char* buf)
{
    int ch;
    int i = 0;
    while (((ch = getc(fd)) != EOF) && (ch != '\n'))
    {
        buf[i] = ch;
        i ++;
    }
    buf[i] = 0;

    if (ch == EOF && (i == 0))
        return 0;
    else
        return 1;
}

void WriteLine(FILE* fd, char* buf)
{
    int len = strlen(buf);
    int i;
    for (i = 0; i < len; i++)
        putc(buf[i], fd);
    putc('\n', fd);
}

int main(int argc, char* argv[])
{
    FILE* wiseTop = fopen("wisetop.txt", "w");
    if (!wiseTop)
        return 1;

    FILE* wiseBottom = fopen("wisebott.txt", "w");
    if (!wiseBottom)
        return 1;

    FILE* wiseWhole = fopen("wxwin2.wse", "r");
    if (!wiseWhole)
        return 1;

    // Write out the top of the file
    g_Line[0] = 0;
    while (ReadLine(wiseWhole, g_Line))
    {
        if (strcmp(g_Line, "item: Install File") == 0)
            break;
        else
            WriteLine(wiseTop, g_Line);
    }
    // Skip to the end of the file items
    while (ReadLine(wiseWhole, g_Line))
    {
        if ((strncmp(g_Line, "item:", 5) == 0) && (strcmp(g_Line, "item: Install File") != 0))
        {
            WriteLine(wiseBottom, g_Line);
            break;
        }
    }
    // Write the rest of the bottom
    while (ReadLine(wiseWhole, g_Line))
    {
        WriteLine(wiseBottom, g_Line);
    }

    fclose(wiseTop);
    fclose(wiseBottom);
    fclose(wiseWhole);

    return 0;
}


