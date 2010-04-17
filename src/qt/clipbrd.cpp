/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/clipbrd.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/clipbrd.h"

wxClipboard::wxClipboard()
{
}


bool wxClipboard::Open()
{
    return false;
}

void wxClipboard::Close()
{
}

bool wxClipboard::IsOpened() const
{
    return false;
}


bool wxClipboard::AddData( wxDataObject *data )
{
    return false;
}

bool wxClipboard::SetData( wxDataObject *data )
{
    return false;
}

bool wxClipboard::GetData( wxDataObject& data )
{
    return false;
}

void wxClipboard::Clear()
{
}

bool wxClipboard::IsSupported( const wxDataFormat& format )
{
    return false;
}

