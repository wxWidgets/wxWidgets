#ifdef __GNUG__
#pragma implementation "serext.h"
#endif

#include <wx/splitter.h>
#include <wx/objstrm.h>
#include <wx/datstrm.h>
#include "serext.h"

IMPLEMENT_SERIAL_CLASS(wxSplitterWindow, wxWindow)

void WXSERIAL(wxSplitterWindow)::StoreObject(wxObjectOutputStream& s)
{
  wxSplitterWindow *splitter = (wxSplitterWindow *)Object();
  WXSERIAL(wxWindow)::StoreObject(s);

  if (s.FirstStage()) {
    s.AddChild( splitter->GetWindow1() );
    s.AddChild( splitter->GetWindow2() );
    return;
  }

  wxDataOutputStream data_s(s);
  data_s.Write8( splitter->GetSplitMode() );
  data_s.Write32( splitter->GetSashSize() );
  data_s.Write8( splitter->GetBorderSize() );
  data_s.Write32( splitter->GetSashPosition() );
  data_s.Write32( splitter->GetMinimumPaneSize() );
}

void WXSERIAL(wxSplitterWindow)::LoadObject(wxObjectInputStream& s)
{
  wxSplitterWindow *splitter = (wxSplitterWindow *)Object();
  WXSERIAL(wxWindow)::LoadObject(s);

  wxDataInputStream data_s(s);
  int split_mode, sash_size, border_size, sash_position, min_pane_size;

  split_mode    = data_s.Read8();
  sash_size     = data_s.Read32();
  border_size   = data_s.Read8();
  sash_position = data_s.Read32();
  min_pane_size = data_s.Read32();

  splitter->Create(m_parent, m_id, wxPoint(m_x, m_y), wxSize(m_w, m_h), m_style,
                   m_name);
  
  if (s.GetChild(1)) {
    if (data_s.Read8() == wxSPLIT_VERTICAL)
      splitter->SplitVertically((wxWindow *)s.GetChild(0),
                                (wxWindow *)s.GetChild(1), sash_position);
    else
      splitter->SplitHorizontally((wxWindow *)s.GetChild(0),
                                  (wxWindow *)s.GetChild(1), sash_position);
  }

  splitter->SetSashSize(sash_size);
  splitter->SetBorderSize(border_size);
  splitter->SetMinimumPaneSize(min_pane_size);
}
