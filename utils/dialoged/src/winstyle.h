/////////////////////////////////////////////////////////////////////////////
// Name:        winstyle.h
// Purpose:     Window styles
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _DE_WINSTYLE_H_
#define _DE_WINSTYLE_H_

#ifdef __GNUG__
#pragma interface "winstyle.h"
#endif

#include "wx/wx.h"

/*
 * A class for storing/generating window styles.
 */

class wxWindowStyleClass;

class wxWindowStylePair
{
    friend class wxWindowStyleClass;
public:
    char*               m_styleName;
    long                m_styleId;
};

class wxWindowStyleTable: public wxObject
{
public:
    wxWindowStyleTable();
    ~wxWindowStyleTable();

// Operations
    void ClearTable();
    void AddStyles(const wxString& className, int n, wxWindowStylePair *styles);
    wxWindowStyleClass* FindClass(const wxString& className) ;
    bool GenerateStyleStrings(const wxString& className, long windowStyle, char *buf);

    // Initialise with all possible styles
    void Init();

// Members
protected:
    wxList      m_classes; // A list of wxWindowStyleClass objects, indexed by class name

};

/*
 * Classes for storing all the window style identifiers associated with a particular class
 */

class wxWindowStyleClass: public wxObject
{
public:
    wxWindowStyleClass(int n, wxWindowStylePair *styles);
    ~wxWindowStyleClass();

// Operations
    void GenerateStyleStrings(long windowStyle, char *buf);
    bool GenerateStyle(char *buf, long windowStyle, long flag, const wxString& strStyle);

// Members
protected:
    wxWindowStylePair*  m_styles; // An array of wxWindowStylePair objects
    int                 m_styleCount;
};

#endif
    // _DE_WINSTYLE_H_
