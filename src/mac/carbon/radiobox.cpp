/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.cpp
// Purpose:     wxRadioBox
// Author:      AUTHOR
// Modified by: JS Lair (99/11/15) first implementation
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------
// 		headers
//-------------------------------------------------------------------------------------

#ifdef __GNUG__
#pragma implementation "radiobox.h"
#endif

#include "wx/radiobox.h"
#include <wx/mac/uma.h>

#if !USE_SHARED_LIBRARY
IMPLEMENT_DYNAMIC_CLASS(wxRadioBox, wxControl)
#endif

#pragma mark -
#pragma mark ### Constructors & destructor ###

//-------------------------------------------------------------------------------------
// 		¥ wxRadioBox()
//-------------------------------------------------------------------------------------
// Default constructor

wxRadioBox::wxRadioBox()
{
    m_noItems = 0;
    m_noRowsOrCols = 0;
    m_majorDim = 0 ;
    m_radioButtonCycle = NULL;
}

//-------------------------------------------------------------------------------------
// 		¥ wxRadioBox(wxWindow*, wxWindowID, const wxString&, const wxPoint&, 
//						const wxSize&, int, const wxString[], int, long, 
//						const wxValidator&, const wxString&)
//-------------------------------------------------------------------------------------
// Contructor, creating and showing a radiobox
//
// inline defined
//

//-------------------------------------------------------------------------------------
// 		¥ ~wxRadioBox
//-------------------------------------------------------------------------------------
// Destructor, destroying the radiobox item

wxRadioBox::~wxRadioBox()
{
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
// 		¥ Create
//-------------------------------------------------------------------------------------
// Create the radiobox for two-step construction

bool wxRadioBox::Create(wxWindow *parent, wxWindowID id, const wxString& label,
             const wxPoint& pos, const wxSize& size,
             int n, const wxString choices[],
             int majorDim, long style,
             const wxValidator& val, const wxString& name)
{
    int i;
    
    m_noItems = n;
    m_noRowsOrCols = majorDim;
    m_radioButtonCycle = NULL;

    if (majorDim==0)
        m_majorDim = n ;
    else
        m_majorDim = majorDim ;


	Rect bounds ;
	Str255 title ;
	
	MacPreControlCreate( parent , id ,  label , pos , size ,style, *((wxValidator*)NULL) , name , &bounds , title ) ;

	m_macControl = UMANewControl( parent->GetMacRootWindow() , &bounds , title , true , 0 , 0 , 1, 
	  	kControlGroupBoxTextTitleProc , (long) this ) ;
	
	MacPostControlCreate() ;

    for (i = 0; i < n; i++)
    {
        wxRadioButton *radBtn = new wxRadioButton(this, NewControlId(),choices[i],wxPoint(5,20*i+10));
        m_radioButtonCycle=radBtn->AddInCycle(m_radioButtonCycle);
    }

	SetSelection(0);
	SetSize(pos.x,pos.y,size.x,size.y);

  return TRUE;
}


#pragma mark -
#pragma mark ### Specific functions (reference v2) ###

//-------------------------------------------------------------------------------------
// 		¥ Enable(bool)
//-------------------------------------------------------------------------------------
// Enables or disables the entire radiobox

bool wxRadioBox::Enable(bool enable)
{
   	int i;
    wxRadioButton *current;
    
    if (!wxControl::Enable(enable))
    	return (false);

    current=m_radioButtonCycle;
   	for (i=0;i<m_noItems;i++) {
    	current->Enable(enable);
    	current=current->NextInCycle();
    	}
    return (true);
}

//-------------------------------------------------------------------------------------
// 		¥ Enable(int, bool)
//-------------------------------------------------------------------------------------
// Enables or disables an given button

void wxRadioBox::Enable(int item, bool enable)
{
   	int i;
    wxRadioButton *current;
    
    if ((item < 0) || (item >= m_noItems))
        return;
    i=0;
    current=m_radioButtonCycle;
    while (i!=item) {
    	i++;
    	current=current->NextInCycle();
    	}
}


//-------------------------------------------------------------------------------------
// 		¥ FindString
//-------------------------------------------------------------------------------------
// Finds a button matching the given string, returning the position if found 
// or -1 if not found

int wxRadioBox::FindString(const wxString& s) const
{
    int i;
    wxRadioButton *current;
    
    current=m_radioButtonCycle;
    for (i = 0; i < m_noItems; i++)
    {
        if (s == current->GetLabel())
            return i;
    	current=current->NextInCycle();
    }
    return -1;
}

//-------------------------------------------------------------------------------------
// 		¥ GetLabel()
//-------------------------------------------------------------------------------------
// Returns the radiobox label

wxString wxRadioBox::GetLabel() const
{
    return wxControl::GetLabel();
}

//-------------------------------------------------------------------------------------
// 		¥ GetLabel(int)
//-------------------------------------------------------------------------------------
// Returns the label for the given button

wxString wxRadioBox::GetLabel(int item) const
{
   	int i;
    wxRadioButton *current;
    
    if ((item < 0) || (item >= m_noItems))
        return wxString("");
    i=0;
    current=m_radioButtonCycle;
    while (i!=item) {
    	i++;
    	current=current->NextInCycle();
    	}
    return current->GetLabel();
}

//-------------------------------------------------------------------------------------
// 		¥ GetSelection
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
// 		¥ GetString
//-------------------------------------------------------------------------------------
// Find string for position

wxString wxRadioBox::GetString(int item) const
{

    return GetLabel(item);
}

//-------------------------------------------------------------------------------------
// 		¥ GetStringSelection
//-------------------------------------------------------------------------------------
// Returns the selected string

wxString wxRadioBox::GetStringSelection () const
{
    int sel = GetSelection ();
    if (sel > -1)
        return this->GetString (sel);
    else
        return wxString("");
}

//-------------------------------------------------------------------------------------
// 		¥ Number
//-------------------------------------------------------------------------------------
// Returns the number of buttons in the radiobox
//
// inline defined
//

//-------------------------------------------------------------------------------------
// 		¥ SetLabel(const wxString&)
//-------------------------------------------------------------------------------------
// Sets the radiobox label

void wxRadioBox::SetLabel(const wxString& label)
{
    return wxControl::SetLabel(label);
}

//-------------------------------------------------------------------------------------
// 		¥ SetLabel(int, const wxString&)
//-------------------------------------------------------------------------------------
// Sets the label of a given button

void wxRadioBox::SetLabel(int item,const wxString& label)
{
   	int i;
    wxRadioButton *current;
    
    if ((item < 0) || (item >= m_noItems))
        return;
	i=0;
    current=m_radioButtonCycle;
    while (i!=item) {
    	i++;
    	current=current->NextInCycle();
    	}
    return current->SetLabel(label);
}

//-------------------------------------------------------------------------------------
// 		¥ SetSelection
//-------------------------------------------------------------------------------------
// Sets a button by passing the desired position. This does not cause 
// wxEVT_COMMAND_RADIOBOX_SELECTED event to get emitted

void wxRadioBox::SetSelection(int item)
{
    int i;
    wxRadioButton *current;
    
    if ((item < 0) || (item >= m_noItems))
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
// 		¥ SetStringSelection
//-------------------------------------------------------------------------------------
// Sets a button by passing the desired string. This does not cause 
// wxEVT_COMMAND_RADIOBOX_SELECTED event to get emitted

bool wxRadioBox::SetStringSelection (const wxString& s)
{
    int sel = FindString (s);
    if (sel > -1)
    {
        SetSelection (sel);
        return TRUE;
    }
    else
        return FALSE;
}

//-------------------------------------------------------------------------------------
// 		¥ Show(bool)
//-------------------------------------------------------------------------------------
// Shows or hides the entire radiobox 

bool wxRadioBox::Show(bool show)
{
    int i;
    wxRadioButton *current;
    
    wxControl::Show(show);

    current=m_radioButtonCycle;
   	for (i=0;i<m_noItems;i++) {
    	current->Show(show);
    	current=current->NextInCycle();
    	}
    return true;
}

//-------------------------------------------------------------------------------------
// 		¥ Show(int, bool)
//-------------------------------------------------------------------------------------
// Shows or hides the given button 

void wxRadioBox::Show(int item, bool show)
{
   	int i;
    wxRadioButton *current;
    
    if ((item < 0) || (item >= m_noItems))
        return;
    i=0;
    current=m_radioButtonCycle;
    while (i!=item) {
    	i++;
    	current=current->NextInCycle();
    	}
    current->Show(show);
}

#pragma mark -
#pragma mark ### Other external functions ###

//-------------------------------------------------------------------------------------
// 		¥ Command
//-------------------------------------------------------------------------------------
// Simulates the effect of the user issuing a command to the item

void wxRadioBox::Command (wxCommandEvent & event)
{
    SetSelection (event.GetInt());
    ProcessCommand (event);
}

//-------------------------------------------------------------------------------------
// 		¥ SetFocus
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


#pragma mark -
#pragma mark ### Internal functions ###

//-------------------------------------------------------------------------------------
// 		¥ DoSetSize
//-------------------------------------------------------------------------------------
// Simulates the effect of the user issuing a command to the item

#define RADIO_SIZE 20

void wxRadioBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
	int i;
	wxRadioButton *current;

// define the position

	int x_current, y_current;
	int x_offset,y_offset;

	x_offset = x;
	y_offset = y;
	GetPosition(&x_current, &y_current);
	if ((x == -1) || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
		x_offset = x_current;
	if ((y == -1) || (sizeFlags & wxSIZE_ALLOW_MINUS_ONE))
		y_offset = y_current;

// define size

	int charWidth,charHeight;
	int maxWidth,maxHeight;
	int eachWidth[128],eachHeight[128];
    int totWidth,totHeight;

	SetFont(GetParent()->GetFont());
	GetTextExtent(wxString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"), &charWidth, &charHeight);
	charWidth/=52;

	maxWidth=-1;
	maxHeight=-1;
	for (i = 0 ; i < m_noItems; i++)
		{
		GetTextExtent(GetLabel(i), &eachWidth[i], &eachHeight[i]);
		eachWidth[i] = (int)(eachWidth[i] + RADIO_SIZE);
		eachHeight[i] = (int)((3*eachHeight[i])/2);
		if (maxWidth<eachWidth[i]) maxWidth = eachWidth[i];
		if (maxHeight<eachHeight[i]) maxHeight = eachHeight[i];
  		}

	totHeight = GetNumVer() * (maxHeight + charHeight/2) + charHeight*3/2;
	totWidth  = GetNumHor() * (maxWidth + charWidth) + charWidth;

	wxControl::DoSetSize(x_offset,y_offset,totWidth,totHeight,wxSIZE_AUTO);

// arrange radiobuttons

	int x_start,y_start;
	
	
	x_start = charWidth;
	y_start = charHeight*3/2;
	x_offset = x_start;
	y_offset = y_start;
	
	current=m_radioButtonCycle;  
	for ( i = 0 ; i < m_noItems; i++)
		{
		if (i&&((i%m_majorDim)==0)) // not to do for the zero button!
    		{
      		if (m_windowStyle & wxRA_VERTICAL)
      			{
 			    x_offset += maxWidth + charWidth;
        		y_offset = y_start;
				}
      		else
      			{
				x_offset = x_start;
        		y_offset += maxHeight + charHeight/2;
				}
			}

		current->SetSize(x_offset,y_offset,eachWidth[i],eachHeight[i]);
		current=current->NextInCycle();
		
		if (m_windowStyle & wxRA_SPECIFY_ROWS)
			y_offset += maxHeight + charHeight/2;
		else
			x_offset += maxWidth + charWidth;
		}
}

//-------------------------------------------------------------------------------------
// 		¥ GetNumVer
//-------------------------------------------------------------------------------------
// return the number of buttons in the vertical direction

int wxRadioBox::GetNumVer() const
{
    if ( m_windowStyle & wxRA_SPECIFY_ROWS )
    {
        return m_majorDim;
    }
    else
    {
        return (m_noItems + m_majorDim - 1)/m_majorDim;
    }
}

//-------------------------------------------------------------------------------------
// 		¥ GetNumHor
//-------------------------------------------------------------------------------------
// return the number of buttons in the horizontal direction

int wxRadioBox::GetNumHor() const
{
    if ( m_windowStyle & wxRA_SPECIFY_ROWS )
    {
        return (m_noItems + m_majorDim - 1)/m_majorDim;
    }
    else
    {
        return m_majorDim;
    }
}





