/****************************************************************************
*
*                       wxWindows HTML Applet Package
*
*               Copyright (C) 1991-2001 SciTech Software, Inc.
*                            All rights reserved.
*
*  ========================================================================
*
*    The contents of this file are subject to the wxWindows License
*    Version 3.0 (the "License"); you may not use this file except in
*    compliance with the License. You may obtain a copy of the License at
*    http://www.wxwindows.org/licence3.txt
*
*    Software distributed under the License is distributed on an
*    "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
*    implied. See the License for the specific language governing
*    rights and limitations under the License.
*
*  ========================================================================
*
* Language:     ANSI C++
* Environment:  Any
*
* Description:  Combobox wrapper. This header file defines the custom
*               combo boxes used for this sample program.
*
****************************************************************************/

#ifndef __COMBOBOX_H
#define __COMBOBOX_H

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
REMARKS:
Defines a Custom ComboBox. This combobox is a portable implementation of
the msw combobox control. It is made of the wxWindows textctrl primitive and
the listbox primitive. This object does not create or display the controls,
it provides the relationship and underlying behavior layer for the primitives
allready created via wxDesigner.
****************************************************************************/
class ComboBox {
private:
    int         m_ListBoxId;
    int         m_TextCtrlId;
    wxWindow    *m_Parent;
    wxListBox   *m_ListBox;
    wxTextCtrl  *m_TextCtrl;

public:
            // Constructor
            ComboBox(wxWindow *parent, int,int);

            // Returns the id of the listbox: listBoxId.
            int GetListBoxId();

            // Inserts: Used to insert items into the listbox
            void Insert(const wxString& item, int pos);
            void Insert(const wxString& item, int pos, void *clientData);
            void Insert(const wxString& item, int pos, wxClientData *clientData);
            void InsertItems(int nItems, const wxString *items, int pos);
            void InsertItems(const wxArrayString& items, int pos);

            // Sets: Used to set items in the combo box
            void Set(int n, const wxString* items, void **clientData );
            void Set(const wxArrayString& items, void **clientData);
            int FindString(const wxString &s);

            // Selections: Used to get/de/select items in the listbox
            void Select(int n);
            void Deselect(int n);
            int GetSelection();
            wxString GetStringSelection();
            bool SetStringSelection(const wxString& s, bool select);

            // Set the specified item at the first visible item or scroll to max
            // range.
            void SetFirstItem(int n);
            void SetFirstItem(const wxString& s);

            // Append items to the listbox
            void Append(const wxString& item);
            void Append(const wxString& item, void *clientData);
            void Append(const wxString& item, wxClientData *clientData);

            // Deleting items from the list box
            void Clear();
            void Delete(int n);

            // OnChange event function (called from SDD dialog box code, see: dialog.h) Mimic
            // msw combobox behavior: Click on listbox item it shows in textbox.
            void OnChange(wxCommandEvent &event);
    };

#endif  // __COMBOBOX_H

