/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.h
// Purpose:     wxRadioBox class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBOX_H_
#define _WX_RADIOBOX_H_

#ifdef __GNUG__
#pragma interface "radiobox.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxRadioBoxNameStr;

// List box item
class  wxBitmap ;

class  wxRadioButton ;

class  wxRadioBox: public wxControl
{
	DECLARE_DYNAMIC_CLASS(wxRadioBox)
public:
// Constructors & destructor
	wxRadioBox();
	inline wxRadioBox(wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = NULL,
             int majorDim = 0, long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr)
		{
			Create(parent, id, title, pos, size, n, choices, majorDim, style, val, name);
		}
	~wxRadioBox();
	bool Create(wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = NULL,
             int majorDim = 0, long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr);

// Specific functions (in wxWindows2 reference)
	bool Enable(bool enable);
	void Enable(int item, bool enable);
	int FindString(const wxString& s) const;
	wxString GetLabel() const;
	wxString GetLabel(int item) const;
	int GetSelection() const;
	wxString GetString(int item) const;
	virtual wxString GetStringSelection() const;
	inline virtual int Number() const { return m_noItems; } ;
	void SetLabel(const wxString& label) ;
	void SetLabel(int item, const wxString& label) ;
	void SetSelection(int item);
	virtual bool SetStringSelection(const wxString& s);
	bool Show(bool show);
	void Show(int item, bool show) ;

// Other external functions 
	void Command(wxCommandEvent& event);
	void SetFocus();

// Other variable access functions
	inline int GetNumberOfRowsOrCols() const { return m_noRowsOrCols; }
	inline void SetNumberOfRowsOrCols(int n) { m_noRowsOrCols = n; }

protected:
	wxRadioButton		*m_radioButtonCycle;
  	
	int               m_majorDim ;
	int               m_noItems;
	int               m_noRowsOrCols;

// Internal functions 
	virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
	int GetNumHor() const;
	int GetNumVer() const;

};

#endif
    // _WX_RADIOBOX_H_
