/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/uiactioncmn.cpp
// Purpose:     wxUIActionSimulator common implementation
// Author:      Kevin Ollivier
// Modified by:
// Created:     2010-03-06
// RCS-ID:      $Id: menu.cpp 54129 2008-06-11 19:30:52Z SC $
// Copyright:   (c) Kevin Ollivier
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_UIACTIONSIMULATOR

#include "wx/uiaction.h"

wxUIActionSimulator::wxUIActionSimulator()
{
}

wxUIActionSimulator::~wxUIActionSimulator()
{
}


bool wxUIActionSimulator::MouseClick(int button)
{
    MouseDown(button);
    MouseUp(button);

    return true;
}

bool wxUIActionSimulator::MouseDblClick(int button)
{
    MouseDown(button);
    MouseUp(button);
    MouseDown(button);
    MouseUp(button);

    return true;
}

bool wxUIActionSimulator::MouseDragDrop(long x1, long y1, long x2, long y2, int button)
{
    MouseMove(x1, y1);
    MouseDown(button);
    MouseMove(x2, y2);
    MouseUp(button);

    return true;
}

bool  wxUIActionSimulator::Char(int keycode, int modifiers)
{
    Key(keycode, true, modifiers);
    Key(keycode, false, modifiers);

    return true;
}

bool wxUIActionSimulator::Text(const wxString& test)
{
    for ( unsigned int i = 0; i < test.length(); i++ )
    {
        wxUniChar uchar = test[i];
        char achar = 0;
        if ( uchar.GetAsChar(&achar) && isalpha(achar) )
        {
            int mod = 0;
            if ( isupper(achar) )
                mod = wxMOD_SHIFT;
            Char(achar, mod);
        }
        else
        {
            wxFAIL_MSG( "Only characters a-z & A-Z are supported" );
            return false;
        }
    }
    return true;
}

#endif // wxUSE_UIACTIONSIMULATOR
