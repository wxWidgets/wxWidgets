/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/radiobox.cpp
// Purpose:     wxRadioBox
// Author:      Stefan Csomor
// Modified by: JS Lair (99/11/15) first implementation
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------
//         headers
//-------------------------------------------------------------------------------------

#include "wx/wxprec.h"

#if wxUSE_RADIOBOX

#include "wx/radiobox.h"

#ifndef WX_PRECOMP
    #include "wx/radiobut.h"
    #include "wx/arrstr.h"
#endif

#include "wx/mac/uma.h"

IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)

//-------------------------------------------------------------------------------------
//         ¥ wxRadioBox()
//-------------------------------------------------------------------------------------
// Default constructor
BEGIN_EVENT_TABLE(wxRadioBox, wxControl)
    EVT_RADIOBUTTON( wxID_ANY , wxRadioBox::OnRadioButton )
END_EVENT_TABLE()

void wxRadioBox::OnRadioButton( wxCommandEvent &outer )
{
    if ( outer.IsChecked() )
    {
        wxCommandEvent event(wxEVT_COMMAND_RADIOBOX_SELECTED, m_windowId);
        int i = GetSelection() ;
        event.SetInt( i );
        event.SetString(GetString(i));
        event.SetEventObject( this );
        ProcessCommand(event);
    }
}

wxRadioBox::wxRadioBox()
{
    m_noItems = 0;
    m_noRowsOrCols = 0;
    m_radioButtonCycle = NULL;
}

//-------------------------------------------------------------------------------------
//         ¥ wxRadioBox(wxWindow*, wxWindowID, const wxString&, const wxPoint&,
//                        const wxSize&, int, const wxString[], int, long,
//                        const wxValidator&, const wxString&)
//-------------------------------------------------------------------------------------
// Contructor, creating and showing a radiobox
//
// inline defined
//

//-------------------------------------------------------------------------------------
//         ¥ ~wxRadioBox
//-------------------------------------------------------------------------------------
// Destructor, destroying the radiobox item

wxRadioBox::~wxRadioBox()
{
    m_isBeingDeleted = true;

    wxRadioButton *next,*current;

    current=m_radioButtonCycle->NextInCycle();
    next=current->NextInCycle();
    while (current!=m_radioButtonCycle) {
        delete current;
        current=next;
        next=current->NextInCycle();
    }
    delete current;
}

//-------------------------------------------------------------------------------------
//         ¥ Create
//-------------------------------------------------------------------------------------
// Create the radiobox for two-step construction

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
                        const wxPoint& pos, const wxSize& size,
                        const wxArrayString& choices,
                        int majorDim, long style,
                        const wxValidator& val, const wxString& name)
{
    wxCArrayString chs(choices);

    return Create(parent, id, label, pos, size, chs.GetCount(),
                  chs.GetStrings(), majorDim, style, val, name);
}

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
                        const wxPoint& pos, const wxSize& size,
                        int n, const wxString choices[],
                        int majorDim, long style,
                        const wxValidator& val, const wxString& name)
{
    if ( !wxControl::Create(parent, id, pos, size, style, val, name) )
        return false;

    int i;

    m_noItems = (unsigned int)n;
    m_noRowsOrCols = majorDim;
    m_radioButtonCycle = NULL;

    SetMajorDim(majorDim == 0 ? n : majorDim, style);

    Rect bounds ;
    Str255 title ;

    MacPreControlCreate( parent , id ,  wxStripMenuCodes(label) , pos , size ,style, val , name , &bounds , title ) ;

    m_macControl = (WXWidget) ::NewControl( MAC_WXHWND(parent->MacGetRootWindow()) , &bounds , title , false , 0 , 0 , 1,
        kControlGroupBoxTextTitleProc , (long) this ) ;

    for (i = 0; i < n; i++)
    {
        wxRadioButton *radBtn = new wxRadioButton
                                    (
                                        this,
                                        wxID_ANY,
                                        wxStripMenuCodes(choices[i]),
                                        wxPoint(5,20*i+10),
                                        wxDefaultSize,
                                        i == 0 ? wxRB_GROUP : 0
                                    );
        if ( i == 0 )
            m_radioButtonCycle = radBtn ;
        //        m_radioButtonCycle=radBtn->AddInCycle(m_radioButtonCycle);
    }

    SetSelection(0);
    MacPostControlCreate() ;

    return true;
}


//-------------------------------------------------------------------------------------
//         ¥ Enable(bool)
//-------------------------------------------------------------------------------------
// Enables or disables the entire radiobox

bool wxRadioBox::Enable(bool enable)
{
    if (!wxControl::Enable(enable))
        return false;

    wxRadioButton *current = m_radioButtonCycle;
    for (unsigned int i = 0; i < m_noItems; i++)
    {
        current->Enable(enable);
        current = current->NextInCycle();
    }
    return true;
}

//-------------------------------------------------------------------------------------
//         ¥ Enable(unsigned int, bool)
//-------------------------------------------------------------------------------------
// Enables or disables an given button

bool wxRadioBox::Enable(unsigned int item, bool enable)
{
    if (!IsValid(item))
        return false;

    unsigned int i = 0;
    wxRadioButton *current = m_radioButtonCycle;
    while (i != item)
    {
        i++;
        current = current->NextInCycle();
    }
    return current->Enable(enable);
}

//-------------------------------------------------------------------------------------
//         ¥ GetLabel()
//-------------------------------------------------------------------------------------
// Returns the radiobox label

wxString wxRadioBox::GetLabel() const
{
    return wxControl::GetLabel();
}

//-------------------------------------------------------------------------------------
//         ¥ GetLabel(int)
//-------------------------------------------------------------------------------------
// Returns the label for the given button

wxString wxRadioBox::GetString(unsigned int item) const
{
    wxRadioButton *current;

    if (!IsValid(item))
        return wxEmptyString;

    unsigned int i = 0;
    current = m_radioButtonCycle;
    while (i != item) {
        i++;
        current = current->NextInCycle();
    }
    return current->GetLabel();
}

//-------------------------------------------------------------------------------------
//         ¥ GetSelection
//-------------------------------------------------------------------------------------
// Returns the zero-based position of the selected button

int wxRadioBox::GetSelection() const
{
    int i;
    wxRadioButton *current;

    i=0;
    current=m_radioButtonCycle;
    while (!current->GetValue()) {
        i++;
        current=current->NextInCycle();
    }

    return i;
}

//-------------------------------------------------------------------------------------
//         ¥ Number
//-------------------------------------------------------------------------------------
// Returns the number of buttons in the radiobox
//
// inline defined
//

//-------------------------------------------------------------------------------------
//         ¥ SetLabel(const wxString&)
//-------------------------------------------------------------------------------------
// Sets the radiobox label

void wxRadioBox::SetLabel(const wxString& label)
{
    return wxControl::SetLabel(label);
}

//-------------------------------------------------------------------------------------
//         ¥ SetLabel(int, const wxString&)
//-------------------------------------------------------------------------------------
// Sets the label of a given button

void wxRadioBox::SetString(unsigned int item,const wxString& label)
{
    if (!IsValid(item))
        return;

    unsigned int i=0;
    wxRadioButton *current=m_radioButtonCycle;
    while (i!=item)
    {
        i++;
        current=current->NextInCycle();
    }
    return current->SetLabel(label);
}

//-------------------------------------------------------------------------------------
//         ¥ SetSelection
//-------------------------------------------------------------------------------------
// Sets a button by passing the desired position. This does not cause
// wxEVT_COMMAND_RADIOBOX_SELECTED event to get emitted

void wxRadioBox::SetSelection(int item)
{
    int i;
    wxRadioButton *current;

    if (!IsValid(item))
        return;
    i=0;
    current=m_radioButtonCycle;
    while (i!=item) {
        i++;
        current=current->NextInCycle();
    }
    current->SetValue(true);

}

//-------------------------------------------------------------------------------------
//         ¥ Show(bool)
//-------------------------------------------------------------------------------------
// Shows or hides the entire radiobox

bool wxRadioBox::Show(bool show)
{
    wxRadioButton *current;

    wxControl::Show(show);

    current=m_radioButtonCycle;
    for (unsigned int i=0; i<m_noItems; i++)
    {
        current->Show(show);
        current=current->NextInCycle();
    }
    return true;
}

//-------------------------------------------------------------------------------------
//         ¥ Show(unsigned int, bool)
//-------------------------------------------------------------------------------------
// Shows or hides the given button

bool wxRadioBox::Show(unsigned int item, bool show)
{
    if (!IsValid(item))
        return false;

    unsigned int i = 0;
    wxRadioButton *current=m_radioButtonCycle;
    while (i!=item) {
        i++;
        current=current->NextInCycle();
    }
    return current->Show(show);
}

//-------------------------------------------------------------------------------------
//         ¥ Command
//-------------------------------------------------------------------------------------
// Simulates the effect of the user issuing a command to the item

void wxRadioBox::Command (wxCommandEvent & event)
{
    SetSelection (event.GetInt());
    ProcessCommand (event);
}

//-------------------------------------------------------------------------------------
//         ¥ SetFocus
//-------------------------------------------------------------------------------------
// Sets the selected button to receive keyboard input

void wxRadioBox::SetFocus()
{
    int i;
    wxRadioButton *current;

    i=0;
    current=m_radioButtonCycle;
    while (!current->GetValue()) {
        i++;
        current=current->NextInCycle();
    }
    current->SetFocus();
}


//-------------------------------------------------------------------------------------
//         ¥ DoSetSize
//-------------------------------------------------------------------------------------
// Simulates the effect of the user issuing a command to the item

#define RADIO_SIZE 20

void wxRadioBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    unsigned int i;
    wxRadioButton *current;

    // define the position

    int x_current, y_current;
    int x_offset,y_offset;
    int widthOld, heightOld;
    GetSize(&widthOld, &heightOld);

    x_offset = x;
    y_offset = y;
    GetPosition(&x_current, &y_current);
    if ((x == wxDefaultCoord) && !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        x_offset = x_current;
    if ((y == wxDefaultCoord)&& !(sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
        y_offset = y_current;

    // define size

    int charWidth,charHeight;
    int maxWidth,maxHeight;
    int eachWidth[128],eachHeight[128];
    int totWidth,totHeight;

    SetFont(GetParent()->GetFont());
    GetTextExtent(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"), &charWidth, &charHeight);
    charWidth/=52;

    maxWidth=-1;
    maxHeight=-1;
    for (i = 0 ; i < m_noItems; i++)
    {
        GetTextExtent(GetString(i), &eachWidth[i], &eachHeight[i]);
        eachWidth[i] = (int)(eachWidth[i] + RADIO_SIZE);
        eachHeight[i] = (int)((3*eachHeight[i])/2);
        if (maxWidth<eachWidth[i]) maxWidth = eachWidth[i];
        if (maxHeight<eachHeight[i]) maxHeight = eachHeight[i];
    }

    totHeight = GetRowCount() * (maxHeight + charHeight/2) + charHeight ;
    totWidth  = GetColumnCount() * (maxWidth + charWidth) + charWidth;

    // only change our width/height if asked for
    if ( width == wxDefaultCoord )
    {
        if ( sizeFlags & wxSIZE_AUTO_WIDTH )
            width = totWidth ;
        else
            width = widthOld;
    }

    if ( height == wxDefaultCoord )
    {
        if ( sizeFlags & wxSIZE_AUTO_HEIGHT )
            height = totHeight ;
        else
            height = heightOld;
    }

    wxControl::DoSetSize(x_offset,y_offset,width,height,wxSIZE_AUTO);

    // arrange radiobuttons

    int x_start,y_start;


    x_start = charWidth;
    y_start = 15 ;
    if ( UMAGetSystemVersion() >= 0x1030 )
    {
        //need to add a few more pixels for the top border on panther
        y_start = y_start + 5; //how many exactly should this be to meet the HIG?
    }
    x_offset = x_start;
    y_offset = y_start;

    current=m_radioButtonCycle;
    for ( i = 0 ; i < m_noItems; i++)
    {
        if (i&&((i%GetMajorDim())==0)) // not to do for the zero button!
        {
            if (m_windowStyle & wxRA_VERTICAL)
            {
                x_offset += maxWidth + charWidth;
                y_offset = y_start;
            }
            else
            {
                x_offset = x_start;
                y_offset += maxHeight ; /*+ charHeight/2;*/
            }
        }

        current->SetSize(x_offset,y_offset,eachWidth[i],eachHeight[i]);
        current=current->NextInCycle();

        if (m_windowStyle & wxRA_SPECIFY_ROWS)
            y_offset += maxHeight ; /*+ charHeight/2;*/
        else
            x_offset += maxWidth + charWidth;
    }
}

wxSize wxRadioBox::DoGetBestSize() const
{
    int charWidth, charHeight;
    int maxWidth, maxHeight;
    int eachWidth, eachHeight;
    int totWidth, totHeight;

    wxFont font = GetParent()->GetFont();
    GetTextExtent(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"),
        &charWidth, &charHeight, NULL, NULL, &font);
    charWidth /= 52;

    maxWidth = -1;
    maxHeight = -1;

    for (unsigned int i = 0 ; i < m_noItems; i++)
    {
        GetTextExtent(GetString(i), &eachWidth, &eachHeight);
        eachWidth  = (int)(eachWidth + RADIO_SIZE) ;
        eachHeight = (int)((3 * eachHeight) / 2);
        if (maxWidth < eachWidth)     maxWidth = eachWidth;
        if (maxHeight < eachHeight)   maxHeight = eachHeight;
    }

    totHeight = GetRowCount() * (maxHeight + charHeight/2) + charHeight ;
    totWidth  = GetColumnCount() * (maxWidth + charWidth) + charWidth;

    if ( UMAGetSystemVersion() >= 0x1030 )
    {
        //need to add a few more pixels for the static boxborder on panther
        totHeight = totHeight + 10; //how many exactly should this be to meet the HIG?
    }
    // handle radio box title as well
    GetTextExtent(GetLabel(), &eachWidth, NULL);
    eachWidth  = (int)(eachWidth + RADIO_SIZE) + 3 * charWidth ;
    if (totWidth < eachWidth)
        totWidth = eachWidth;

    return wxSize(totWidth, totHeight);
}

#endif // wxUSE_RADIOBOX
