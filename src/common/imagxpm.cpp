/////////////////////////////////////////////////////////////////////////////
// Name:        imagxpm.cpp
// Purpose:     wxXPMHandler
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*

This file is partially based on source code of ImageMagick by John Cristy. Its
license is as follows:

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                            X   X  PPPP   M   M                              %
%                             X X   P   P  MM MM                              %
%                              X    PPPP   M M M                              %
%                             X X   P      M   M                              %
%                            X   X  P      M   M                              %
%                                                                             %
%                                                                             %
%                    Read/Write ImageMagick Image Format.                     %
%                                                                             %
%                                                                             %
%                              Software Design                                %
%                                John Cristy                                  %
%                                 July 1992                                   %
%                                                                             %
%                                                                             %
%  Copyright (C) 2001 ImageMagick Studio, a non-profit organization dedicated %
%  to making software imaging solutions freely available.                     %
%                                                                             %
%  Permission is hereby granted, free of charge, to any person obtaining a    %
%  copy of this software and associated documentation files ("ImageMagick"),  %
%  to deal in ImageMagick without restriction, including without limitation   %
%  the rights to use, copy, modify, merge, publish, distribute, sublicense,   %
%  and/or sell copies of ImageMagick, and to permit persons to whom the       %
%  ImageMagick is furnished to do so, subject to the following conditions:    %
%                                                                             %
%  The above copyright notice and this permission notice shall be included in %
%  all copies or substantial portions of ImageMagick.                         %
%                                                                             %
%  The software is provided "as is", without warranty of any kind, express or %
%  implied, including but not limited to the warranties of merchantability,   %
%  fitness for a particular purpose and noninfringement.  In no event shall   %
%  ImageMagick Studio be liable for any claim, damages or other liability,    %
%  whether in an action of contract, tort or otherwise, arising from, out of  %
%  or in connection with ImageMagick or the use or other dealings in          %
%  ImageMagick.                                                               %
%                                                                             %
%  Except as contained in this notice, the name of the ImageMagick Studio     %
%  shall not be used in advertising or otherwise to promote the sale, use or  %
%  other dealings in ImageMagick without prior written authorization from the %
%  ImageMagick Studio.                                                        %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
*/

#ifdef __GNUG__
#pragma implementation "imagxpm.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#  pragma hdrstop
#endif

#ifndef WX_PRECOMP
#  include "wx/defs.h"
#endif

#if wxUSE_IMAGE && wxUSE_XPM

#include "wx/imagxpm.h"
#include "wx/wfstream.h"
#include "wx/log.h"
#include "wx/intl.h"
#include "wx/utils.h"


IMPLEMENT_DYNAMIC_CLASS(wxXPMHandler,wxImageHandler)

//-----------------------------------------------------------------------------
// wxXPMHandler
//-----------------------------------------------------------------------------

#if wxUSE_STREAMS

bool wxXPMHandler::LoadFile(wxImage *WXUNUSED(image), 
                            wxInputStream& WXUNUSED(stream), 
                            bool verbose, int WXUNUSED(index))
{
    if (verbose)
        wxLogDebug(_("XPM: the handler is write-only!"));
    return FALSE;
}

bool wxXPMHandler::SaveFile(wxImage * image,
                            wxOutputStream& stream, bool verbose)
{
    wxString tmp;
    char tmp_c;
    
    // 1. count colours:
    #define MaxCixels  92
    static const char Cixel[MaxCixels+1] = 
                         " .XoO+@#$%&*=-;:>,<1234567890qwertyuipasdfghjk"
                         "lzxcvbnmMNBVCZASDFGHJKLPIUYTREWQ!~^/()_`'][{}|";
    int chars_per_pixel;
    int cols;
    int i, j, k;
    
    cols = image->CountColours();
    chars_per_pixel = 1;
    for ( k = MaxCixels; cols > k; k *= MaxCixels)
        chars_per_pixel++;

    // 2. write the header:
    tmp.Printf("/* XPM */\n"
               "static char *xpm_data[] = {\n"
               "/* columns rows colors chars-per-pixel */\n"
               "\"%i %i %i %i\",\n", 
               image->GetWidth(), image->GetHeight(), cols, chars_per_pixel);
    stream.Write(tmp.mb_str(), tmp.Length());

    // 3. create color symbols table:
    wxHashTable table(wxKEY_INTEGER);
    image->ComputeHistogram(table);

    char *symbols_data = new char[cols * (chars_per_pixel+1)];
    char **symbols = new char*[cols];

    // 2a. find mask colour:
    long mask_key = -1;
    if (image->HasMask())
        mask_key = (image->GetMaskRed() << 16) |
                   (image->GetMaskGreen() << 8) | image->GetMaskBlue();
    
    // 2b. generate colour table:
    table.BeginFind();
    wxNode *node = NULL;
    while ((node = table.Next()) != NULL)
    {
        wxHNode *hnode = (wxHNode*) node->GetData();
        long index = hnode->index;
        symbols[index] = symbols_data + index * (chars_per_pixel+1);
        char *sym = symbols[index];

        k = index % MaxCixels;
        sym[0] = Cixel[k];
        for (j = 1; j < chars_per_pixel; j++)
        {
            k = ((index - k) / MaxCixels) % MaxCixels;
            sym[j] = Cixel[k];
        }
        sym[j] = '\0';

        long key = node->GetKeyInteger();

        if (key == 0)
            tmp.Printf(wxT("\"%s c Black\",\n"), sym);
        else if (key == mask_key)
            tmp.Printf(wxT("\"%s c None\",\n"), sym);
        else
            tmp.Printf(wxT("\"%s c #%s%s%s\",\n"), sym, 
                       wxDecToHex((unsigned char)(key >> 16)).c_str(),
                       wxDecToHex((unsigned char)(key >> 8)).c_str(),
                       wxDecToHex((unsigned char)(key)).c_str());
        stream.Write(tmp.mb_str(), tmp.Length());
    }

    tmp = wxT("/* pixels */\n");
    stream.Write(tmp.mb_str(), tmp.Length());

    unsigned char *data = image->GetData();
    for (j = 0; j < image->GetHeight(); j++)
    {
        tmp_c = '\"'; stream.Write(&tmp_c, 1);
        for (i = 0; i < image->GetWidth(); i++, data += 3)
        {
            unsigned long key = (data[0] << 16) | (data[1] << 8) | (data[2]);
            wxHNode *hnode = (wxHNode*) table.Get(key);
            stream.Write(symbols[hnode->index], chars_per_pixel);
        }
        tmp_c = '\"'; stream.Write(&tmp_c, 1);
        if ( j + 1 < image->GetHeight() )
        {
            tmp_c = ','; stream.Write(&tmp_c, 1);
        }
        tmp_c = '\n'; stream.Write(&tmp_c, 1);
    }
    tmp = wxT("};\n");
    stream.Write(tmp.mb_str(), 3);
    
    delete[] symbols;
    delete[] symbols_data;

    return TRUE;
}

bool wxXPMHandler::DoCanRead(wxInputStream& stream)
{
    unsigned char buf[9];

    stream.Read(buf, 9);
    stream.SeekI(-9, wxFromCurrent);

    return (memcmp(buf, "/* XPM */", 9) == 0);
}

#endif  // wxUSE_STREAMS

#endif // wxUSE_XPM && wxUSE_IMAGE
