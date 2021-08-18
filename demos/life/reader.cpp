/////////////////////////////////////////////////////////////////////////////
// Name:        reader.cpp
// Purpose:     Life! pattern reader (writer coming soon)
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ==========================================================================
// headers, declarations, constants
// ==========================================================================

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/txtstrm.h"
#include "reader.h"


// ==========================================================================
// LifeReader
// ==========================================================================

#define LIFE_CHECKVAL( msg )                                        \
    if (!m_ok)                                                      \
    {                                                               \
        wxMessageBox( msg, _("Error"), wxICON_EXCLAMATION | wxOK ); \
        return;                                                     \
    }

LifeReader::LifeReader(wxInputStream& is)
{
    wxBufferedInputStream buff_is(is);
    wxTextInputStream     text_is(buff_is);
    wxString              line, rest;

    // check stream
    m_ok = is.IsOk();
    LIFE_CHECKVAL(_("Couldn't read any data"));

    // read signature
    m_ok = text_is.ReadLine().Contains(wxT("#Life 1.05"));
    LIFE_CHECKVAL(_("Error reading signature. Not a Life pattern?"));

    // read description
    m_description = wxEmptyString;
    line = text_is.ReadLine();
    while (buff_is.IsOk() && line.StartsWith(wxT("#D"), &rest))
    {
        m_description += rest.Trim(false);
        m_description += wxT("\n");
        line = text_is.ReadLine();
    }

    m_ok = buff_is.IsOk();
    LIFE_CHECKVAL(_("Unexpected EOF while reading description"));

    // read rules
    m_ok = line.StartsWith(wxT("#N"));
    LIFE_CHECKVAL(_("Sorry, non-conway rules not supported yet"));

    // read shape
    while (buff_is.IsOk())
    {
        line = ( text_is.ReadLine() ).Trim();

        if (!line.empty())
        {
            if (line.StartsWith(wxT("#P "), &rest))
                m_shape.Add(rest);
            else
                m_shape.Add(line);
        }
    }
}

