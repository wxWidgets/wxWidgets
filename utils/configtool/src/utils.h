/////////////////////////////////////////////////////////////////////////////
// Name:        utils.h
// Purpose:     Utility functions and classes
// Author:      Julian Smart
// Modified by:
// Created:     2002-09-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

/*!
 * \file
 * \brief A file of utility functions and classes.
 */

#ifdef __GNUG__
// #pragma interface
#endif

#ifndef _AP_UTILS_H_
#define _AP_UTILS_H_

#include "wx/imaglist.h"

#ifndef DOXYGEN_SKIP

/*!
 * \defgroup ForwardDeclarations Forward Declations
 */

/*@{*/

class WXDLLEXPORT wxImage;
class WXDLLEXPORT wxNotebook;
class WXDLLEXPORT wxInputStream;
class WXDLLEXPORT wxOutputStream;
class WXDLLEXPORT wxFileInputStream;
class WXDLLEXPORT wxFileOutputStream;
class WXDLLEXPORT wxDataInputStream;
class WXDLLEXPORT wxDataOutputStream;
class WXDLLEXPORT wxSplitterWindow;
class WXDLLEXPORT wxVariant;
class WXDLLEXPORT wxListCtrl;

 /* \endif
  */

/*@}*/

#endif

#ifdef __WXMSW__
#define wxNEWLINE wxT("\r\n")
#else
#define wxNEWLINE wxT("\n")
#endif

/// Returns the image type, or -1, determined from the extension.
int apDetermineImageType(const wxString& filename);

/// Convert a colour to a 6-digit hex string
wxString apColourToHexString(const wxColour& col);

/// Convert 6-digit hex string to a colour
wxColour apHexStringToColour(const wxString& hex);

/// Convert a wxFont to a string
wxString apFontToString(const wxFont& font);

/// Convert a string to a wxFont
wxFont apStringToFont(const wxString& str);

/// Get the index of the given named wxNotebook page
int apFindNotebookPage(wxNotebook* notebook, const wxString& name);

/// View the given URL
void apViewHTMLFile(const wxString& url);

/// Returns the system temporary directory.
wxString wxGetTempDir();

/// Launch the application associated with the filename's extension
bool apInvokeAppForFile(const wxString& filename);

/// \brief Find the absolute path where this application has been run from.
///
/// \param argv0            wxTheApp->argv[0]
/// \param cwd              The current working directory (at startup)
/// \param appVariableName  The name of a variable containing the directory for this app, e.g.
/// MYAPPDIR. This is used as a last resort.
wxString apFindAppPath(const wxString& argv0, const wxString& cwd, const wxString& appVariableName = wxEmptyString);

/// Adds a context-sensitive help button, for non-Windows platforms
void apAddContextHelpButton(wxWindow* parent, wxSizer* sizer, int sizerFlags = wxALIGN_CENTRE|wxALL, int sizerBorder = 5);

/// Get selected wxNotebook page
wxWindow* apNotebookGetSelectedPage(wxNotebook* notebook);

#define wxMAX_ICON_STATES 4

/*

wxIconInfo, wxIconTable
associate multiple state icons with items in tree controls
(and potentially other controls).

So instead of having to remember a lot of image list ids,
you have a named state info object which contains up to 4 different states
(identified by the integers 0 - 3). Each of these states can
be in a further 2 sub-states - enabled or disabled.

wxIconTable holds a list of these state info objects
and has a convenient API. For example, the following adds
icons for a checkbox item that can be: on/enabled, off/enabled,
on/disabled,off/disabled.

    m_iconTable.AddInfo("Checkbox", wxICON(checked), 0, true);
    m_iconTable.AddInfo("Checkbox", wxICON(checked_dis), 0, false);
    m_iconTable.AddInfo("Checkbox", wxICON(unchecked), 1, true);
    m_iconTable.AddInfo("Checkbox", wxICON(unchecked_dis), 1, false);

When you update the item image in response to (e.g.) user interaction,
you can say something like this:

    int iconId = m_iconTable.GetIconId("Checkbox", 0, false);

    treeCtrl.SetItemImage(itemId, iconId, wxTreeItemIcon_Normal);
    treeCtrl.SetItemImage(itemId, iconId, wxTreeItemIcon_Selected);

 */

/*
 * wxIconInfo
 * Stores information about the visual state of an item in a tree control
 */

class wxIconInfo: public wxObject
{
public:
    wxIconInfo(const wxString& name);
    
    // How many states? (Each state
    //  has enabled/disabled state)
    // Max (say) 4 states, each with
    // enabled/disabled
    int GetStateCount() const { return m_maxStates; };

    void SetStateCount(int count) { m_maxStates = count; }
    int GetIconId(int state, bool enabled = true) const;
    void SetIconId(int state, bool enabled, int iconId);

    const wxString& GetName() const { return m_name; }
    
protected:
    int             m_maxStates;
    int             m_states[wxMAX_ICON_STATES * 2]; // Enabled/disabled
    wxString        m_name; // Name of icon, e.g. "Package"
};

/*!
 * wxIconTable
 * Contains a list of wxIconInfos
 */

class wxIconTable: public wxList
{
public:
    wxIconTable(wxImageList* imageList = NULL);
    
    void AppendInfo(wxIconInfo* info);
    
    // Easy way of initialising both the image list and the
    // info db. It will generate image ids itself while appending the icon.
    // 'state' is an integer from 0 up to the max allowed, representing a different
    // state. There may be only one, or (for a checkbox) there may be two.
    // A folder that can be open or closed would have two states.
    // Enabled/disabled is taken as a special case.
    bool AddInfo(const wxString& name, const wxIcon& icon, int state, bool enabled);
    
    wxIconInfo* FindInfo(const wxString& name) const;
    
    int GetIconId(const wxString& name, int state, bool enabled = true) const;
    bool SetIconId(const wxString& name, int state, bool enabled, int iconId) ;
    
    void SetImageList(wxImageList* imageList) { m_imageList = imageList; }
    wxImageList* GetImageList() const { return m_imageList; }
    
protected:
    wxImageList*    m_imageList;    
};

/// Useful insertion operators for wxOutputStream.
wxOutputStream& operator <<(wxOutputStream&, const wxString& s);
wxOutputStream& operator <<(wxOutputStream&, const char c);
wxOutputStream& operator <<(wxOutputStream&, long l);

// Convert characters to HTML equivalents
wxString ctEscapeHTMLCharacters(const wxString& str);

// Match 'matchText' against 'matchAgainst', optionally constraining to
// whole-word only.
bool ctMatchString(const wxString& matchAgainst, const wxString& matchText, bool wholeWordOnly);


#endif
    // _AP_UTILS_H_
