///////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:     wxListBox
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "listbox.h"
#endif

#include "wx/listbox.h"
#include "wx/settings.h"
#include "wx/dynarray.h"
#include "wx/log.h"
#include "wx/utils.h"

#include <Xm/List.h>
#include "wx/motif/private.h"

#if !USE_SHARED_LIBRARY
    IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)
#endif

static void wxListBoxCallback(Widget w,
                              XtPointer clientData,
                              XmListCallbackStruct * cbs);

static void wxListBoxDefaultActionProc(Widget list_w,
                                XtPointer client_data,
                                XmListCallbackStruct * cbs);

// ============================================================================
// list box control implementation
// ============================================================================

// Listbox item
wxListBox::wxListBox() : m_clientDataList(wxKEY_INTEGER)
{
    m_noItems = 0;
    m_selected = 0;
}

bool wxListBox::Create(wxWindow *parent, wxWindowID id,
                       const wxPoint& pos,
                       const wxSize& size,
                       int n, const wxString choices[],
                       long style,
                       const wxValidator& validator,
                       const wxString& name)
{
    m_windowStyle = style;
    m_noItems = n;
    m_selected = 0;
    //    m_backgroundColour = parent->GetBackgroundColour();
    m_backgroundColour = * wxWHITE;
    m_foregroundColour = parent->GetForegroundColour();

    SetName(name);
    SetValidator(validator);

    if (parent) parent->AddChild(this);

    m_windowId = ( id == -1 ) ? (int)NewControlId() : id;

    Widget parentWidget = (Widget) parent->GetClientWidget();

    Arg args[3];
    int count;
    XtSetArg (args[0], XmNlistSizePolicy, XmCONSTANT);
    if (m_windowStyle & wxLB_MULTIPLE)
        XtSetArg (args[1], XmNselectionPolicy, XmMULTIPLE_SELECT);
    else if (m_windowStyle & wxLB_EXTENDED)
        XtSetArg (args[1], XmNselectionPolicy, XmEXTENDED_SELECT);
    else
        XtSetArg (args[1], XmNselectionPolicy, XmBROWSE_SELECT);
    if (m_windowStyle & wxLB_ALWAYS_SB)
    {
        XtSetArg (args[2], XmNscrollBarDisplayPolicy, XmSTATIC);
        count = 3;
    }
    else
        count = 2;

    Widget listWidget = XmCreateScrolledList (parentWidget, (char*) (const char*) name, args, count);

    m_mainWidget = (WXWidget) listWidget;

    Set(n, choices);

    XtManageChild (listWidget);

    long width = size.x;
    long height = size.y;
    if (width == -1)
        width = 150;
    if (height == -1)
        height = 80;

    XtAddCallback (listWidget, XmNbrowseSelectionCallback, (XtCallbackProc) wxListBoxCallback,
        (XtPointer) this);
    XtAddCallback (listWidget, XmNextendedSelectionCallback, (XtCallbackProc) wxListBoxCallback,
        (XtPointer) this);
    XtAddCallback (listWidget, XmNmultipleSelectionCallback, (XtCallbackProc) wxListBoxCallback,
        (XtPointer) this);

    XtAddCallback (listWidget, XmNdefaultActionCallback, (XtCallbackProc) wxListBoxDefaultActionProc,
        (XtPointer) this);

    m_font = parent->GetFont();
    ChangeFont(FALSE);

    SetCanAddEventHandler(TRUE);
    AttachWidget (parent, m_mainWidget, (WXWidget) NULL, pos.x, pos.y, width, height);

    ChangeBackgroundColour();

    return TRUE;
}

wxListBox::~wxListBox()
{
}

void wxListBox::SetFirstItem(int N)
{
    int count, length;

    if (N < 0)
        return;
    XtVaGetValues ((Widget) m_mainWidget,
                    XmNvisibleItemCount, &count,
                    XmNitemCount, &length,
                    NULL);
    if ((N + count) >= length)
        N = length - count;
    XmListSetPos ((Widget) m_mainWidget, N + 1);
}

void wxListBox::SetFirstItem(const wxString& s)
{
    int N = FindString (s);

    if (N >= 0)
        SetFirstItem (N);
}

void wxListBox::Delete(int N)
{
    int width1, height1;
    int width2, height2;
    Widget listBox = (Widget) m_mainWidget;
    GetSize (&width1, &height1);

    bool managed = XtIsManaged(listBox);

    if (managed)
        XtUnmanageChild (listBox);

    XmListDeletePos (listBox, N + 1);

    if (managed)
        XtManageChild (listBox);

    GetSize (&width2, &height2);
    // Correct for randomly resized listbox - bad boy, Motif!
    if (width1 != width2 || height1 != height2)
        SetSize (-1, -1, width1, height1);

    // (JDH) need to add code here to take care of clientDataList
    wxNode *node = m_clientDataList.Find((long)N);  // get item from list
    if (node) m_clientDataList.DeleteNode(node);    // if existed then delete from list
    node = m_clientDataList.First();                // we now have to adjust all keys that
    while (node)                                  // are >=N+1
    {
        if (node->GetKeyInteger() >= (long)(N+1))
            node->SetKeyInteger(node->GetKeyInteger() - 1);
        node = node->Next();
    }

    m_noItems --;
}

void wxListBox::Append(const wxString& item)
{
    int width1, height1;
    int width2, height2;

    Widget listBox = (Widget) m_mainWidget;
    GetSize (&width1, &height1);

    bool managed = XtIsManaged(listBox);

    if (managed)
        XtUnmanageChild (listBox);
    int n;
    XtVaGetValues (listBox, XmNitemCount, &n, NULL);
    XmString text = XmStringCreateSimple ((char*) (const char*) item);
    //  XmListAddItem(listBox, text, n + 1);
    XmListAddItemUnselected (listBox, text, 0);
    XmStringFree (text);

    // It seems that if the list is cleared, we must re-ask for
    // selection policy!!
    Arg args[3];
    XtSetArg (args[0], XmNlistSizePolicy, XmCONSTANT);
    if (m_windowStyle & wxLB_MULTIPLE)
        XtSetArg (args[1], XmNselectionPolicy, XmMULTIPLE_SELECT);
    else if (m_windowStyle & wxLB_EXTENDED)
        XtSetArg (args[1], XmNselectionPolicy, XmEXTENDED_SELECT);
    else
        XtSetArg (args[1], XmNselectionPolicy, XmBROWSE_SELECT);
    XtSetValues (listBox, args, 2);

    if (managed)
        XtManageChild (listBox);

    GetSize (&width2, &height2);
    // Correct for randomly resized listbox - bad boy, Motif!
    if (width1 != width2 || height1 != height2)
        SetSize (-1, -1, width1, height1);
    m_noItems ++;
}

void wxListBox::Append(const wxString& item, void *clientData)
{
    int width1, height1;
    int width2, height2;

    Widget listBox = (Widget) m_mainWidget;

    GetSize (&width1, &height1);
    Bool managed = XtIsManaged(listBox);

    if (managed)
        XtUnmanageChild (listBox);

    int n;
    XtVaGetValues (listBox, XmNitemCount, &n, NULL);
    XmString text = XmStringCreateSimple ((char*) (const char*) item);
    //  XmListAddItem(listBox, text, n + 1);
    XmListAddItemUnselected (listBox, text, 0);
    XmStringFree (text);

    // It seems that if the list is cleared, we must re-ask for
    // selection policy!!
    Arg args[3];
    XtSetArg (args[0], XmNlistSizePolicy, XmCONSTANT);
    if (m_windowStyle & wxLB_MULTIPLE)
        XtSetArg (args[1], XmNselectionPolicy, XmMULTIPLE_SELECT);
    else if (m_windowStyle & wxLB_EXTENDED)
        XtSetArg (args[1], XmNselectionPolicy, XmEXTENDED_SELECT);
    else
        XtSetArg (args[1], XmNselectionPolicy, XmBROWSE_SELECT);
    XtSetValues (listBox, args, 2);

    m_clientDataList.Append ((long) n, (wxObject *) clientData);

    if (managed)
        XtManageChild (listBox);

    GetSize (&width2, &height2);

    // Correct for randomly resized listbox - bad boy, Motif!
    if (width1 != width2 || height1 != height2)
        SetSize (-1, -1, width1, height1);

    m_noItems ++;
}

void wxListBox::Set(int n, const wxString *choices, void** clientData)
{
    m_clientDataList.Clear();
    int width1, height1;
    int width2, height2;

    Widget listBox = (Widget) m_mainWidget;
    GetSize (&width1, &height1);

    bool managed = XtIsManaged(listBox);

    if (managed)
        XtUnmanageChild (listBox);
        /***
        for (int i=0; i<n; i++)
        {
        XmString text = XmStringCreateSimple(choices[i]);
        XmListAddItemUnselected(listBox, text, 0);
        XmStringFree(text);
        }
    ***/
    XmString *text = new XmString[n];
    int i;
    for (i = 0; i < n; i++)
        text[i] = XmStringCreateSimple ((char*) (const char*) choices[i]);

    if ( clientData )
        for (i = 0; i < n; i++)
            m_clientDataList.Append ((long) i, (wxObject *) clientData[i]);

        XmListAddItems (listBox, text, n, 0);
        for (i = 0; i < n; i++)
            XmStringFree (text[i]);
        delete[]text;

        // It seems that if the list is cleared, we must re-ask for
        // selection policy!!
        Arg args[3];
        XtSetArg (args[0], XmNlistSizePolicy, XmCONSTANT);
        if (m_windowStyle & wxLB_MULTIPLE)
            XtSetArg (args[1], XmNselectionPolicy, XmMULTIPLE_SELECT);
        else if (m_windowStyle & wxLB_EXTENDED)
            XtSetArg (args[1], XmNselectionPolicy, XmEXTENDED_SELECT);
        else
            XtSetArg (args[1], XmNselectionPolicy, XmBROWSE_SELECT);
        XtSetValues (listBox, args, 2);

        if (managed)
            XtManageChild (listBox);

        GetSize (&width2, &height2);
        // Correct for randomly resized listbox - bad boy, Motif!
        if (width1 != width2 || height1 != height2)
            SetSize (-1, -1, width1, height1);

        m_noItems = n;
}

int wxListBox::FindString(const wxString& s) const
{
    XmString str = XmStringCreateSimple ((char*) (const char*) s);
    int *positions = NULL;
    int no_positions = 0;
    bool success = XmListGetMatchPos ((Widget) m_mainWidget, str, &positions, &no_positions);
    XmStringFree (str);
    if (success)
    {
        int pos = positions[0];
        if (positions)
            XtFree ((char *) positions);
        return pos - 1;
    }
    else
        return -1;
}

void wxListBox::Clear()
{
    if (m_noItems <= 0)
        return;

    int width1, height1;
    int width2, height2;

    Widget listBox = (Widget) m_mainWidget;
    GetSize (&width1, &height1);

    XmListDeleteAllItems (listBox);
    m_clientDataList.Clear ();
    GetSize (&width2, &height2);

    // Correct for randomly resized listbox - bad boy, Motif!
    if (width1 != width2 || height1 != height2)
        SetSize (-1, -1, width1, height1);

    m_noItems = 0;
}

void wxListBox::SetSelection(int N, bool select)
{
    m_inSetValue = TRUE;
    if (select)
    {
#if 0
        if (m_windowStyle & wxLB_MULTIPLE)
        {
            int *selections = NULL;
            int n = GetSelections (&selections);

            // This hack is supposed to work, to make it possible to select more
            // than one item, but it DOESN'T under Motif 1.1.

            XtVaSetValues ((Widget) m_mainWidget, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);

            int i;
            for (i = 0; i < n; i++)
                XmListSelectPos ((Widget) m_mainWidget, selections[i] + 1, FALSE);

            XmListSelectPos ((Widget) m_mainWidget, N + 1, FALSE);

            XtVaSetValues ((Widget) m_mainWidget, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
        }
        else
#endif // 0
            XmListSelectPos ((Widget) m_mainWidget, N + 1, FALSE);

    }
    else
        XmListDeselectPos ((Widget) m_mainWidget, N + 1);

    m_inSetValue = FALSE;
}

bool wxListBox::Selected(int N) const
{
    // In Motif, no simple way to determine if the item is selected.
    wxArrayInt theSelections;
    int count = GetSelections (theSelections);
    if (count == 0)
        return FALSE;
    else
    {
        int j;
        for (j = 0; j < count; j++)
            if (theSelections[j] == N)
                return TRUE;
    }
    return FALSE;
}

void wxListBox::Deselect(int N)
{
    XmListDeselectPos ((Widget) m_mainWidget, N + 1);
}

void *wxListBox::GetClientData(int N) const
{
    wxNode *node = m_clientDataList.Find ((long) N);
    if (node)
        return (void *) node->Data ();
    else
        return NULL;
}

void wxListBox::SetClientData(int N, void *Client_data)
{
    wxNode *node = m_clientDataList.Find ((long) N);
    if (node)
        node->SetData ((wxObject *)Client_data);
    else
        node = m_clientDataList.Append((long) N, (wxObject*) Client_data);
}

// Return number of selections and an array of selected integers
int wxListBox::GetSelections(wxArrayInt& aSelections) const
{
    aSelections.Empty();

    Widget listBox = (Widget) m_mainWidget;
    int *posList = NULL;
    int posCnt = 0;
    bool flag = XmListGetSelectedPos (listBox, &posList, &posCnt);
    if (flag)
    {
        if (posCnt > 0)
        {
            aSelections.Alloc(posCnt);

            int i;
            for (i = 0; i < posCnt; i++)
                aSelections.Add(posList[i] - 1);

            XtFree ((char *) posList);
            return posCnt;
        }
        else
            return 0;
    }
    else
        return 0;
}

// Get single selection, for single choice list items
int wxListBox::GetSelection() const
{
    Widget listBox = (Widget) m_mainWidget;
    int *posList = NULL;
    int posCnt = 0;
    bool flag = XmListGetSelectedPos (listBox, &posList, &posCnt);
    if (flag)
    {
        int id = -1;
        if (posCnt > 0)
            id = posList[0] - 1;
        XtFree ((char *) posList);
        return id;
    }
    else
        return -1;
}

// Find string for position
wxString wxListBox::GetString(int N) const
{
    Widget listBox = (Widget) m_mainWidget;
    XmString *strlist;
    int n;
    XtVaGetValues (listBox, XmNitemCount, &n, XmNitems, &strlist, NULL);
    if (N <= n && N >= 0)
    {
        char *txt;
        if (XmStringGetLtoR (strlist[N], XmSTRING_DEFAULT_CHARSET, &txt))
        {
            wxString str(txt);
            XtFree (txt);
            return str;
        }
        else
            return wxEmptyString;
    }
    else
        return wxEmptyString;
}

void wxListBox::DoSetSize(int x, int y, int width, int height, int sizeFlags)
{
    wxWindow::DoSetSize(x, y, width, height, sizeFlags);

    // Check resulting size is correct
    int tempW, tempH;
    GetSize (&tempW, &tempH);

    /*
    if (tempW != width || tempH != height)
    {
    cout << "wxListBox::SetSize sizes not set correctly.");
    }
    */
}

void wxListBox::InsertItems(int nItems, const wxString items[], int pos)
{
    int width1, height1;
    int width2, height2;

    Widget listBox = (Widget) m_mainWidget;

    GetSize(&width1, &height1);

    bool managed = XtIsManaged(listBox);

    if (managed)
        XtUnmanageChild(listBox);

    XmString *text = new XmString[nItems];
    int i;
    // Steve Hammes: Motif 1.1 compatibility
    // #if XmVersion > 1100
    // Corrected by Sergey Krasnov from Steve Hammes' code
#if XmVersion > 1001
    for (i = 0; i < nItems; i++)
        text[i] = XmStringCreateSimple((char*) (const char*) items[i]);
    XmListAddItemsUnselected(listBox, text, nItems, pos+1);
#else
    for (i = 0; i < nItems; i++)
    {
        text[i] = XmStringCreateSimple((char*) (const char*) items[i]);
        //    XmListAddItemUnselected(listBox, text[i], i);
        XmListAddItemUnselected(listBox, text[i], pos+i+1); // Another Sergey correction
    }
#endif
    for (i = 0; i < nItems; i++)
        XmStringFree(text[i]);

    delete[] text;

    // It seems that if the list is cleared, we must re-ask for
    // selection policy!!
    Arg args[3];
    XtSetArg(args[0], XmNlistSizePolicy, XmCONSTANT);
    if (m_windowStyle & wxLB_MULTIPLE)
        XtSetArg(args[1], XmNselectionPolicy, XmMULTIPLE_SELECT);
    else if (m_windowStyle & wxLB_EXTENDED)
        XtSetArg(args[1], XmNselectionPolicy, XmEXTENDED_SELECT);
    else XtSetArg(args[1], XmNselectionPolicy, XmBROWSE_SELECT);
    XtSetValues(listBox,args,2) ;

    if (managed)
        XtManageChild(listBox);

    GetSize(&width2, &height2);
    // Correct for randomly resized listbox - bad boy, Motif!
    if (width1 != width2 /*|| height1 != height2*/)
        SetSize(-1, -1, width1, height1);

    m_noItems += nItems;
}

void wxListBox::SetString(int N, const wxString& s)
{
    int width1, height1;
    int width2, height2;

    Widget listBox = (Widget) m_mainWidget;
    GetSize (&width1, &height1);

    XmString text = XmStringCreateSimple ((char*) (const char*) s);

    // delete the item and add it again.
    // FIXME isn't there a way to change it in place?
    XmListDeletePos (listBox, N+1);
    XmListAddItem (listBox, text, N+1);

    XmStringFree(text);

    GetSize (&width2, &height2);
    // Correct for randomly resized listbox - bad boy, Motif!
    if (width1 != width2 || height1 != height2)
        SetSize (-1, -1, width1, height1);
}

int wxListBox::Number () const
{
    return m_noItems;
}

// For single selection items only
wxString wxListBox::GetStringSelection () const
{
    wxString res;
    int sel = GetSelection();
    if (sel > -1)
        res = GetString(sel);

    return res;
}

bool wxListBox::SetStringSelection (const wxString& s, bool flag)
{
    int sel = FindString (s);
    if (sel > -1)
    {
        SetSelection (sel, flag);
        return TRUE;
    }
    else
        return FALSE;
}

void wxListBox::Command (wxCommandEvent & event)
{
    if (event.m_extraLong)
        SetSelection (event.m_commandInt);
    else
    {
        Deselect (event.m_commandInt);
        return;
    }
    ProcessCommand (event);
}

void wxListBoxCallback (Widget WXUNUSED(w), XtPointer clientData,
                        XmListCallbackStruct * cbs)
{
    /*
    if (cbs->reason == XmCR_EXTENDED_SELECT)
        cout << "*** Extend select\n";
    else if (cbs->reason == XmCR_SINGLE_SELECT)
        cout << "*** Single select\n";
    else if (cbs->reason == XmCR_MULTIPLE_SELECT)
        cout << "*** Multiple select\n";
    else if (cbs->reason == XmCR_BROWSE_SELECT)
        cout << "*** Browse select\n";

    if (cbs->selection_type == XmMODIFICATION)
        cout << "*** Modification\n";
    else if (cbs->selection_type == XmINITIAL)
        cout << "*** Initial\n";
    else if (cbs->selection_type == XmADDITION)
        cout << "*** Addition\n";
    */

    wxListBox *item = (wxListBox *) clientData;

    if (item->InSetValue())
        return;

    wxCommandEvent event (wxEVT_COMMAND_LISTBOX_SELECTED, item->GetId());
    switch (cbs->reason)
    {
    case XmCR_MULTIPLE_SELECT:
    case XmCR_BROWSE_SELECT:
        {
            event.m_clientData = item->GetClientData (cbs->item_position - 1);
            event.m_commandInt = cbs->item_position - 1;
            event.m_extraLong = TRUE;
            event.SetEventObject(item);
            item->ProcessCommand (event);
            break;
        }
    case XmCR_EXTENDED_SELECT:
        {
            switch (cbs->selection_type)
            {
            case XmINITIAL:
            case XmADDITION:
            case XmMODIFICATION:
                {
                    event.m_clientData = item->GetClientData (cbs->item_position - 1);
                    event.m_commandInt = cbs->item_position - 1;
                    event.m_extraLong = TRUE;
                    event.SetEventObject(item);
                    item->ProcessCommand (event);
                    break;
                }
            }
            break;
        }
    }
}

/* Respond by getting the
* designated "default button" in the action area and activate it
* as if the user had selected it.
*/
void wxListBoxDefaultActionProc (Widget WXUNUSED(list_w), XtPointer client_data, XmListCallbackStruct * WXUNUSED(cbs))
{
    wxListBox *lbox = (wxListBox *) client_data;

    wxCommandEvent event(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, lbox->GetId());
    event.SetEventObject( lbox );
    lbox->GetEventHandler()->ProcessEvent(event) ;
}

WXWidget wxListBox::GetTopWidget() const
{
    return (WXWidget) XtParent( (Widget) m_mainWidget );
}

void wxListBox::ChangeFont(bool keepOriginalSize)
{
    wxWindow::ChangeFont(keepOriginalSize);
}

void wxListBox::ChangeBackgroundColour()
{
    wxWindow::ChangeBackgroundColour();

    Widget parent = XtParent ((Widget) m_mainWidget);
    Widget hsb, vsb;

    XtVaGetValues (parent,
        XmNhorizontalScrollBar, &hsb,
        XmNverticalScrollBar, &vsb,
        NULL);

   /* TODO: should scrollbars be affected? Should probably have separate
    * function to change them (by default, taken from wxSystemSettings)
    */
    wxColour backgroundColour = wxSystemSettings::GetSystemColour(wxSYS_COLOUR_3DFACE);
    DoChangeBackgroundColour((WXWidget) hsb, backgroundColour, TRUE);
    DoChangeBackgroundColour((WXWidget) vsb, backgroundColour, TRUE);

    XtVaSetValues (hsb,
        XmNtroughColor, backgroundColour.AllocColour(XtDisplay(hsb)),
        NULL);
    XtVaSetValues (vsb,
        XmNtroughColor, backgroundColour.AllocColour(XtDisplay(vsb)),
        NULL);

    DoChangeBackgroundColour((WXWidget) parent, m_backgroundColour, TRUE);
}

void wxListBox::ChangeForegroundColour()
{
    wxWindow::ChangeForegroundColour();

    Widget parent = XtParent ((Widget) m_mainWidget);
    Widget hsb, vsb;

    XtVaGetValues(parent,
                  XmNhorizontalScrollBar, &hsb,
                  XmNverticalScrollBar, &vsb,
                  NULL);

    /* TODO: should scrollbars be affected? Should probably have separate
             function to change them (by default, taken from wxSystemSettings)

        DoChangeForegroundColour((WXWidget) hsb, m_foregroundColour);
        DoChangeForegroundColour((WXWidget) vsb, m_foregroundColour);
        DoChangeForegroundColour((WXWidget) parent, m_foregroundColour);
    */
}

// These implement functions needed by wxControlWithItems.
// Unfortunately, they're not all implemented yet.

int wxListBox::GetCount() const
{
    return Number();
}

int wxListBox::DoAppend(const wxString& item)
{
    Append(item, (void*) NULL);
    return GetCount() - 1;
}

// Just appends, doesn't yet insert
void wxListBox::DoInsertItems(const wxArrayString& items, int WXUNUSED(pos))
{
    size_t nItems = items.GetCount();

    for ( size_t n = 0; n < nItems; n++ )
    {
        Append( items[n], (void*) NULL);
    }
}

void wxListBox::DoSetItems(const wxArrayString& items, void **clientData)
{
    size_t nItems = items.GetCount();
    wxString* strings = new wxString[nItems];

    for ( size_t n = 0; n < nItems; n++ )
    {
        strings[n] = items[n];
    }
    Set(nItems, strings, clientData);

    delete[] strings;
}

void wxListBox::DoSetFirstItem(int WXUNUSED(n))
{
    wxFAIL_MSG( wxT("wxListBox::DoSetFirstItem not implemented") );
}

void wxListBox::DoSetItemClientData(int n, void* clientData)
{
    SetClientData(n, clientData);
}

void* wxListBox::DoGetItemClientData(int n) const
{
    return GetClientData(n);
}

void wxListBox::DoSetItemClientObject(int n, wxClientData* clientData)
{
    DoSetItemClientData(n, (void*) clientData);
}

wxClientData* wxListBox::DoGetItemClientObject(int n) const
{
    return (wxClientData*) DoGetItemClientData(n);
}

void wxListBox::Select(int n)
{
    SetSelection(n, TRUE);
}
