#ifdef __GNUG__
#pragma implementation "serctrl.h"
#endif

#include <wx/window.h>
#include <wx/control.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/slider.h>
#include <wx/gauge.h>
#include <wx/choice.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/radiobox.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/objstrm.h>
#include <wx/datstrm.h>
#include <wx/serbase.h>
#include "serwnd.h"
#include "serctrl.h"

IMPLEMENT_ALIAS_SERIAL_CLASS(wxControl, wxWindow)
IMPLEMENT_SERIAL_CLASS(wxSlider, wxControl)
IMPLEMENT_SERIAL_CLASS(wxCheckBox, wxControl)
IMPLEMENT_SERIAL_CLASS(wxChoice, wxControl)
IMPLEMENT_SERIAL_CLASS(wxComboBox, wxControl)
IMPLEMENT_SERIAL_CLASS(wxGauge, wxControl)
IMPLEMENT_SERIAL_CLASS(wxListBox, wxControl)
IMPLEMENT_SERIAL_CLASS(wxNotebook, wxControl)
IMPLEMENT_SERIAL_CLASS(wxRadioBox, wxControl)

IMPLEMENT_SERIAL_CLASS(wxButton, wxControl)
IMPLEMENT_SERIAL_CLASS(wxStaticText, wxControl)

void WXSERIAL(wxButton)::StoreObject(wxObjectOutputStream& s)
{
  WXSERIAL(wxControl)::StoreObject(s);
}

void WXSERIAL(wxButton)::LoadObject(wxObjectInputStream& s)
{
  WXSERIAL(wxControl)::LoadObject(s);

  wxButton *button = (wxButton *)Object();

  printf("label = %s\n", WXSTRINGCAST m_label);
  button->Create(m_parent, m_id, m_label, wxPoint(m_x, m_y), wxSize(m_w, m_h),
                 m_style, m_name);
}

void WXSERIAL(wxCheckBox)::StoreObject(wxObjectOutputStream& s)
{
  WXSERIAL(wxControl)::StoreObject(s);

  if (s.FirstStage())
    return;

  wxDataOutputStream data_s(s);
  data_s.Write8( ((wxCheckBox *)Object())->GetValue() );
}

void WXSERIAL(wxCheckBox)::LoadObject(wxObjectInputStream& s)
{
  WXSERIAL(wxControl)::LoadObject(s);

  wxDataInputStream data_s(s);
  wxCheckBox *chkbox = (wxCheckBox *)Object();

  chkbox->Create(m_parent, m_id, m_label, wxPoint(m_x, m_y), wxSize(m_w, m_h),
                 m_style, m_name);

  chkbox->SetValue(data_s.Read8());
}

void WXSERIAL(wxSlider)::StoreObject(wxObjectOutputStream& s)
{
  WXSERIAL(wxControl)::StoreObject(s);

  if (s.FirstStage())
    return;

  wxDataOutputStream data_s(s);
  wxSlider *slider = (wxSlider *)Object();

  data_s.Write32( slider->GetMin() );
  data_s.Write32( slider->GetMax() );
  data_s.Write32( slider->GetValue() );
  data_s.Write32( slider->GetTickFreq() );
  data_s.Write32( slider->GetPageSize() );
  data_s.Write32( slider->GetLineSize() );
  data_s.Write32( slider->GetSelStart() );
  data_s.Write32( slider->GetSelEnd() );
  data_s.Write32( slider->GetThumbLength() );
}

void WXSERIAL(wxSlider)::LoadObject(wxObjectInputStream& s)
{
  WXSERIAL(wxControl)::LoadObject(s);

  wxDataInputStream data_s(s);
  wxSlider *slider = (wxSlider *)Object();
  int value, min, max;

  min = data_s.Read32();
  max = data_s.Read32();
  value = data_s.Read32();

  slider->Create(m_parent, m_id, value, min, max, wxPoint(m_x, m_y),
                 wxSize(m_w, m_h), m_style, m_name);

  slider->SetTickFreq( 0, data_s.Read32() );
  slider->SetPageSize( data_s.Read32() );
  slider->SetLineSize( data_s.Read32() );
  min = data_s.Read32();
  max = data_s.Read32();
  slider->SetSelection(min, max);
  slider->SetThumbLength( data_s.Read32() );
}

void WXSERIAL(wxGauge)::StoreObject(wxObjectOutputStream& s)
{
  WXSERIAL(wxControl)::StoreObject(s);

  if (s.FirstStage())
    return;

  wxDataOutputStream data_s(s);
  wxGauge *gauge = (wxGauge *)Object();

  data_s.Write32( gauge->GetRange() );
  data_s.Write8( gauge->GetShadowWidth() );
  data_s.Write8( gauge->GetBezelFace() );
  data_s.Write32( gauge->GetValue() );
}

void WXSERIAL(wxGauge)::LoadObject(wxObjectInputStream& s)
{
  WXSERIAL(wxControl)::LoadObject(s);

  wxDataInputStream data_s(s);
  wxGauge *gauge = (wxGauge *)Object();
  int range;

  range = data_s.Read32();
  gauge->Create(m_parent, m_id, range, wxPoint(m_x, m_y), wxSize(m_w, m_h),
                m_style, m_name);

  gauge->SetShadowWidth( data_s.Read8() );
  gauge->SetBezelFace( data_s.Read8() );
  gauge->SetValue( data_s.Read32() );
}

void WXSERIAL(wxChoice)::StoreObject(wxObjectOutputStream& s)
{
  WXSERIAL(wxControl)::StoreObject(s);

  if (s.FirstStage())
    return;

  wxDataOutputStream data_s(s);
  wxChoice *choice = (wxChoice *)Object();
  int i, num = choice->Number();

  data_s.Write32(num);
  for (i=0;i<num;i++)
    data_s.WriteString( choice->GetString(i) );
}

void WXSERIAL(wxChoice)::LoadObject(wxObjectInputStream& s)
{
  WXSERIAL(wxControl)::LoadObject(s);

  wxDataInputStream data_s(s);
  wxChoice *choice = (wxChoice *)Object();
  int i,num = data_s.Read32();

  choice->Create(m_parent, m_id, wxPoint(m_x, m_y), wxSize(m_w, m_h), 0, NULL,
                 m_style, m_name);

  for (i=0;i<num;i++)
    choice->Append( data_s.ReadString() );
}

void WXSERIAL(wxListBox)::StoreObject(wxObjectOutputStream& s)
{
  WXSERIAL(wxControl)::StoreObject(s);

  if (s.FirstStage())
    return;

  wxDataOutputStream data_s(s);
  wxListBox *listbox = (wxListBox *)Object();
  int i, num = listbox->Number();

  data_s.Write32(num);
  for (i=0;i<num;i++)
    data_s.WriteString( listbox->GetString(i) );
}

void WXSERIAL(wxListBox)::LoadObject(wxObjectInputStream& s)
{
  WXSERIAL(wxListBox)::LoadObject(s);

  wxDataInputStream data_s(s);
  wxListBox *listbox = (wxListBox *)Object();
  int i, num = data_s.Read32();

  for (i=0;i<num;i++)
    listbox->Append( data_s.ReadString() );
}

void WXSERIAL(wxNotebook)::StoreObject(wxObjectOutputStream& s)
{
  wxNotebook *notebook = (wxNotebook *)Object();
  int i, pcount = notebook->GetPageCount();

  WXSERIAL(wxControl)::StoreObject(s);

  if (s.FirstStage()) {
    // Don't know how to retrieve images from wxImageList (copy to a DC ?)
    return;
  }

  wxDataOutputStream data_s(s);

  data_s.Write8( pcount );
  for (i=0;i<pcount;i++)
    data_s.WriteString( notebook->GetPageText(i) );
}

void WXSERIAL(wxNotebook)::LoadObject(wxObjectInputStream& s)
{
  wxNotebook *notebook = (wxNotebook *)Object();
  int i, pcount;

  WXSERIAL(wxControl)::LoadObject(s);

  notebook->Create(m_parent, m_id, wxPoint(m_x, m_y), wxSize(m_w, m_h),
                   m_style, m_name);

  wxDataInputStream data_s(s);

  pcount = data_s.Read8();
  for (i=0;i<pcount;i++)
    notebook->SetPageText(i, data_s.ReadString() );
}

void WXSERIAL(wxRadioBox)::StoreObject(wxObjectOutputStream& s)
{
  wxRadioBox *box = (wxRadioBox *)Object();
  WXSERIAL(wxControl)::StoreObject(s);

  if (s.FirstStage())
    return;

  wxDataOutputStream data_s(s);
  int i, n_items = box->Number();

  data_s.Write8( n_items );
  data_s.Write8( box->GetNumberOfRowsOrCols() );

  for (i=0;i<n_items;i++)
    data_s.WriteString( box->GetString(i) );
}

void WXSERIAL(wxRadioBox)::LoadObject(wxObjectInputStream& s)
{
  wxRadioBox *box = (wxRadioBox *)Object();

  WXSERIAL(wxControl)::LoadObject(s);

  wxDataInputStream data_s(s);
  int i, n_rows_cols, n_items;
  wxString *items;

  n_items = data_s.Read8();
  n_rows_cols = data_s.Read8();

  items = new wxString[n_items];
  for (i=0;i<n_items;i++)
    items[i] = data_s.ReadString();

  box->Create(m_parent, m_id, m_title, wxPoint(m_x, m_y), wxSize(m_w, m_h),
              n_items, items, 0, m_style, m_name);
}

void WXSERIAL(wxComboBox)::StoreObject(wxObjectOutputStream& s)
{
  WXSERIAL(wxControl)::StoreObject(s);

  if (s.FirstStage())
    return;

  wxDataOutputStream data_s(s);
  wxComboBox *box = (wxComboBox *)Object();
  int i, num = box->Number();

  data_s.Write8( num );
  data_s.Write8( box->GetSelection() );
  for (i=0;i<num;i++)
    data_s.WriteString( box->GetString(i) );

  data_s.WriteString( box->GetValue() );

  // TODO: Editable flag
}

void WXSERIAL(wxComboBox)::LoadObject(wxObjectInputStream& s)
{
  WXSERIAL(wxControl)::LoadObject(s);

  wxDataInputStream data_s(s);
  wxComboBox *box = (wxComboBox *)Object();
  int i, num, selection;

  box->Create(m_parent, m_id, wxEmptyString, wxPoint(m_x, m_y), wxSize(m_w, m_h),
              0, NULL, m_style, m_name);

  num       = data_s.Read8();
  selection = data_s.Read8();

  for (i=0;i<num;i++)
    box->Append( data_s.ReadString() );

  box->SetSelection( selection );
  box->SetValue( data_s.ReadString() );
}

void WXSERIAL(wxStaticText)::StoreObject(wxObjectOutputStream& s)
{
  WXSERIAL(wxControl)::StoreObject(s);
}

void WXSERIAL(wxStaticText)::LoadObject(wxObjectInputStream& s)
{
  WXSERIAL(wxControl)::LoadObject(s);

  ((wxStaticText *)Object())->Create(m_parent, m_id, m_label, wxPoint(m_x, m_y),
                                     wxSize(m_w, m_h), m_style, m_name);
}
