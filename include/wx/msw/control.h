/////////////////////////////////////////////////////////////////////////////
// Name:        control.h
// Purpose:     wxControl class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __CONTROLH__
#define __CONTROLH__

#ifdef __GNUG__
#pragma interface "control.h"
#endif

#include "wx/window.h"
#include "wx/list.h"
#include "wx/validate.h"

// General item class
class WXDLLEXPORT wxControl: public wxWindow
{
  DECLARE_ABSTRACT_CLASS(wxControl)
public:
   wxControl(void);
   ~wxControl(void);

   virtual void Command(wxCommandEvent& WXUNUSED(event)) = 0;        // Simulates an event
   virtual void ProcessCommand(wxCommandEvent& event); // Calls the callback and
                                                                 // appropriate event handlers
   virtual void SetClientSize(const int width, const int height);
   virtual void SetLabel(const wxString& label);
   virtual wxString GetLabel(void) const ;

#if WXWIN_COMPATIBILITY
   inline virtual void SetButtonColour(const wxColour& WXUNUSED(col)) { }
   inline wxColour*GetButtonColour(void) const { return NULL; }

   inline virtual void SetLabelFont(const wxFont& font);
   inline virtual void SetButtonFont(const wxFont& font);
   inline wxFont  *GetLabelFont(void) const ;
   inline wxFont  *GetButtonFont(void) const ;
#endif

   // Places item in centre of panel - so can't be used BEFORE panel->Fit()
   void Centre(const int direction = wxHORIZONTAL);
   inline void Callback(const wxFunction function);           // Adds callback

   // MSW-specific
   
   // Window procedure
   virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
   virtual void MSWOnMouseMove(const int x, const int y, const WXUINT flags);
   virtual bool MSWNotify(const WXWPARAM wParam, const WXLPARAM lParam);

   void OnEraseBackground(wxEraseEvent& event);

   // For ownerdraw items
   virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *WXUNUSED(item)) { return FALSE; };
   virtual bool MSWOnMeasure(WXMEASUREITEMSTRUCT *WXUNUSED(item)) { return FALSE; };

   inline wxFunction GetCallback(void) { return m_callback; }
   inline wxList& GetSubcontrols(void) { return m_subControls; }
protected:
   wxFunction       m_callback;     // Callback associated with the window
 
   // MSW implementation
   wxList           m_subControls; // For controls like radiobuttons which are really composite

DECLARE_EVENT_TABLE()
};

inline void wxControl::Callback(const wxFunction function) { m_callback = function; };           // Adds callback

#if WXWIN_COMPATIBILITY
inline wxFont  *wxControl::GetLabelFont(void) const         { return GetFont() ; }
inline wxFont  *wxControl::GetButtonFont(void) const        { return GetFont() ; }
inline void wxControl::SetLabelFont(const wxFont& font) { SetFont(font); }
inline void wxControl::SetButtonFont(const wxFont& font) { SetFont(font); }
#endif

#endif
    // __CONTROLH__
