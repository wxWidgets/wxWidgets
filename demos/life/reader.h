/////////////////////////////////////////////////////////////////////////////
// Name:        reader.h
// Purpose:     Life! pattern reader (writer coming soon)
// Author:      Guillermo Rodriguez Garcia, <guille@iies.es>
// Modified by:
// Created:     Jan/2000
// RCS-ID:      $Id$
// Copyright:   (c) 2000, Guillermo Rodriguez Garcia
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _LIFE_READER_H_
#define _LIFE_READER_H_

#ifdef __GNUG__
    #pragma interface "reader.h"
#endif

// for compilers that support precompilation, includes "wx/wx.h"
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "game.h"

// --------------------------------------------------------------------------
// LifeReader
// --------------------------------------------------------------------------

class LifeReader
{
public:
    LifeReader(wxInputStream& is);

    inline bool          IsOk() const           { return m_ok; };
    inline wxString      GetDescription() const { return m_description; };
    inline wxString      GetRules() const       { return m_rules; };
    inline wxArrayString GetShape() const       { return m_shape; };
    inline LifePattern   GetPattern() const
    {
        return LifePattern(_(""), m_description, m_rules, m_shape);
    };

private:
    bool             m_ok;
    wxString         m_description;
    wxString         m_rules;
    wxArrayString    m_shape;
};

#endif // _LIFE_READER_H_
