/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     wxControl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CONTROL_H_
#define _WX_CONTROL_H_

#ifdef __GNUG__
    #pragma interface "control.h"
#endif

#include "wx/window.h"
#include "wx/list.h"

// General item class
class WXDLLEXPORT wxControl : public wxWindow
{
    DECLARE_ABSTRACT_CLASS(wxControl)

public:
   wxControl();
   virtual ~wxControl();

   // Simulates an event
   virtual void Command(wxCommandEvent& WXUNUSED(event)) { }
   // Calls the callback and appropriate event handlers
   virtual void ProcessCommand(wxCommandEvent& event); 

   virtual void SetClientSize(int width, int height);
   virtual void SetClientSize(const wxSize& sz) { wxWindow::SetClientSize(sz); }

   virtual void SetLabel(const wxString& label);
   virtual wxString GetLabel() const;

#if WXWIN_COMPATIBILITY
   virtual void SetButtonColour(const wxColour& WXUNUSED(col)) { }
   wxColour* GetButtonColour() const { return NULL; }

   inline virtual void SetLabelFont(const wxFont& font);
   inline virtual void SetButtonFont(const wxFont& font);
   inline wxFont& GetLabelFont() const;
   inline wxFont& GetButtonFont() const;
#endif

   // Places item in centre of panel - so can't be used BEFORE panel->Fit()
   void Centre(int direction = wxHORIZONTAL);

   // Adds callback
   inline void Callback(const wxFunction function);

   // MSW-specific
   
   // Window procedure
   virtual void MSWOnMouseMove(int x, int y, WXUINT flags);
   virtual bool MSWNotify(WXWPARAM wParam, WXLPARAM lParam, WXLPARAM *result);

   void OnEraseBackground(wxEraseEvent& event);

   // For ownerdraw items
   virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *WXUNUSED(item)) { return FALSE; };
   virtual bool MSWOnMeasure(WXMEASUREITEMSTRUCT *WXUNUSED(item)) { return FALSE; };

   wxFunction GetCallback() { return m_callback; }
   wxList& GetSubcontrols() { return m_subControls; }

protected:
   wxFunction       m_callback;     // Callback associated with the window
 
   // MSW implementation
   wxList           m_subControls; // For controls like radiobuttons which are really composite

private:
   DECLARE_EVENT_TABLE()
};

// Adds callback
inline void wxControl::Callback(const wxFunction function)
{
    m_callback = function;
};

#if WXWIN_COMPATIBILITY
    inline wxFont& wxControl::GetLabelFont() const { return GetFont() ; }
    inline wxFont& wxControl::GetButtonFont() const { return GetFont() ; }
    inline void wxControl::SetLabelFont(const wxFont& font) { SetFont(font); }
    inline void wxControl::SetButtonFont(const wxFont& font) { SetFont(font); }
#endif

#endif
    // _WX_CONTROL_H_
