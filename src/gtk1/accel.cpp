/////////////////////////////////////////////////////////////////////////////
// Name:        accel.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "accel.h"
#endif

#include "wx/accel.h"

#include <ctype.h>

#ifdef __WXDEBUG__
#ifdef NULL
#undef NULL
#endif
#define NULL ((void*)0L)
#endif

//-----------------------------------------------------------------------------
// wxAcceleratorTable
//-----------------------------------------------------------------------------

class wxAccelRefData: public wxObjectRefData
{
public:
  
  wxAccelRefData(void);
  
  wxList m_accels;
};

wxAccelRefData::wxAccelRefData(void)
{
    m_accels.DeleteContents( TRUE );
}

//-----------------------------------------------------------------------------

#define M_ACCELDATA ((wxAccelRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxAcceleratorTable,wxObject)
  
wxAcceleratorTable::wxAcceleratorTable()
{
}

wxAcceleratorTable::wxAcceleratorTable( int n, wxAcceleratorEntry entries[] )
{
    m_refData = new wxAccelRefData();
  
    for (int i = 0; i < n; i++)
    {
        int flag    = entries[i].GetFlags();
        int keycode = entries[i].GetKeyCode();
        int command = entries[i].GetCommand();
        if ((keycode >= (int)'A') && (keycode <= (int)'Z')) keycode = (int)tolower( (char)keycode );
        M_ACCELDATA->m_accels.Append( new wxAcceleratorEntry( flag, keycode, command ) );
    }
}

wxAcceleratorTable::~wxAcceleratorTable()
{
}

bool wxAcceleratorTable::Ok() const
{
    return (m_refData != NULL);
}

int wxAcceleratorTable::GetCommand( wxKeyEvent &event )
{
    if (!Ok()) return -1;

    wxNode *node = M_ACCELDATA->m_accels.First();
    while (node)
    {
        wxAcceleratorEntry *entry = (wxAcceleratorEntry*)node->Data();
        if ((event.m_keyCode == entry->GetKeyCode()) &&
           (((entry->GetFlags() & wxACCEL_CTRL) == 0) || event.ControlDown()) &&
           (((entry->GetFlags() & wxACCEL_SHIFT) == 0) || event.ShiftDown()) &&
           (((entry->GetFlags() & wxACCEL_ALT) == 0) || event.AltDown() || event.MetaDown()))
	{
            return entry->GetCommand();
	}
        node = node->Next();
    }
  
    return -1;
}

