/////////////////////////////////////////////////////////////////////////////
// Name:        accel.cpp
// Purpose:
// Author:      Robert Roebling
// Id:
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "accel.h"
#endif

#include "wx/accel.h"

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
  m_refData = new wxAccelRefData();
}

wxAcceleratorTable::wxAcceleratorTable( int n, wxAcceleratorEntry entries[] )
{
  m_refData = new wxAccelRefData();
  for (int i = 0; i < n; i++)
  {
    M_ACCELDATA->m_accels.Append( (wxObject*)
      new wxAcceleratorEntry( entries[n].GetFlags(), entries[n].GetKeyCode(), entries[n].GetCommand() ) );
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
  wxNode *node = M_ACCELDATA->m_accels.First();
  while (node)
  {
    wxAcceleratorEntry *entry = (wxAcceleratorEntry*)node->Data();
    if ((event.m_keyCode == entry->GetKeyCode()) &&
        (((entry->GetFlags() & wxACCEL_CTRL) == 0) || event.ControlDown()) &&
        (((entry->GetFlags() & wxACCEL_SHIFT) == 0) || event.ShiftDown()) &&
        (((entry->GetFlags() & wxACCEL_ALT) == 0) || event.AltDown() || event.MetaDown()))
      return entry->GetCommand();
    node = node->Next();
  }
  
  return -1;
}

