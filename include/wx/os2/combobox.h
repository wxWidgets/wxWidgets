/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.h
// Purpose:     wxComboBox class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMBOBOX_H_
#define _WX_COMBOBOX_H_

#include "wx/choice.h"

#if wxUSE_COMBOBOX

// Combobox item
class WXDLLEXPORT wxComboBox : public wxChoice
{

 public:
  inline wxComboBox() {}

  inline wxComboBox( wxWindow*          pParent
                    ,wxWindowID         vId
                    ,const wxString&    rsValue = wxEmptyString
                    ,const wxPoint&     rPos = wxDefaultPosition
                    ,const wxSize&      rSize = wxDefaultSize
                    ,int                n = 0
                    ,const wxString     asChoices[] = NULL
                    ,long               lStyle = 0
                    ,const wxValidator& rValidator = wxDefaultValidator
                    ,const wxString&    rsName = wxComboBoxNameStr
                   )
    {
        Create( pParent
               ,vId
               ,rsValue
               ,rPos
               ,rSize
               ,n
               ,asChoices
               ,lStyle
               ,rValidator
               ,rsName
              );
    }

  inline wxComboBox( wxWindow*            pParent
                    ,wxWindowID           vId
                    ,const wxString&      rsValue
                    ,const wxPoint&       rPos
                    ,const wxSize&        rSize
                    ,const wxArrayString& asChoices
                    ,long                 lStyle = 0
                    ,const wxValidator&   rValidator = wxDefaultValidator
                    ,const wxString&      rsName = wxComboBoxNameStr
                   )
    {
        Create( pParent
               ,vId
               ,rsValue
               ,rPos
               ,rSize
               ,asChoices
               ,lStyle
               ,rValidator
               ,rsName
              );
    }

    bool Create( wxWindow*          pParent
                ,wxWindowID         vId
                ,const wxString&    rsValue = wxEmptyString
                ,const wxPoint&     rPos = wxDefaultPosition
                ,const wxSize&      rSize = wxDefaultSize
                ,int                n = 0
                ,const wxString     asChoices[] = NULL
                ,long               lStyle = 0
                ,const wxValidator& rValidator = wxDefaultValidator
                ,const wxString&    rsName = wxComboBoxNameStr
               );

    bool Create( wxWindow*            pParent
                ,wxWindowID           vId
                ,const wxString&      rsValue
                ,const wxPoint&       rPos
                ,const wxSize&        rSize
                ,const wxArrayString& asChoices
                ,long                 lStyle = 0
                ,const wxValidator&   rValidator = wxDefaultValidator
                ,const wxString&      rsName = wxComboBoxNameStr
               );

    //
    // List functions: see wxChoice
    //
    virtual wxString GetValue(void) const;
    virtual void     SetValue(const wxString& rsValue);

    //
    // Clipboard operations
    //
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();

    virtual void        SetInsertionPoint(long lPos);
    virtual void        SetInsertionPointEnd(void);
    virtual long        GetInsertionPoint(void) const;
    virtual wxTextPos   GetLastPosition(void) const;
    virtual void        Replace( long            lFrom
                                ,long            lTo
                                ,const wxString& rsValue
                               );
    virtual void        Remove( long lFrom
                               ,long lTo
                              );
    inline virtual void SetSelection(int n) { wxChoice::SetSelection(n); }
    virtual void        SetSelection( long lFrom
                                     ,long lTo
                                    );
    virtual void        SetEditable(bool bEditable);

    virtual bool        OS2Command( WXUINT uParam
                                   ,WXWORD wId
                                  );
    bool                ProcessEditMsg( WXUINT   uMsg
                                       ,WXWPARAM wParam
                                       ,WXLPARAM lParam
                                      );

private:
    DECLARE_DYNAMIC_CLASS(wxComboBox)
}; // end of CLASS wxComboBox

#endif // wxUSE_COMBOBOX
#endif
    // _WX_COMBOBOX_H_
