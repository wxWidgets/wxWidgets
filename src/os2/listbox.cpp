///////////////////////////////////////////////////////////////////////////////
// Name:        listbox.cpp
// Purpose:     wxListBox
// Author:      David Webster
// Modified by:
// Created:     10/09/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/window.h"
#include "wx/os2/private.h"

#ifndef WX_PRECOMP
#include "wx/listbox.h"
#include "wx/settings.h"
#include "wx/brush.h"
#include "wx/font.h"
#include "wx/dc.h"
#include "wx/dcscreen.h"
#include "wx/utils.h"
#include "wx/scrolwin.h"
#endif

#define INCL_M
#include <os2.h>

#include "wx/dynarray.h"
#include "wx/log.h"

#if wxUSE_LISTBOX

#if wxUSE_OWNER_DRAWN
    #include  "wx/ownerdrw.h"
#endif

  IMPLEMENT_DYNAMIC_CLASS(wxListBox, wxControl)

// ============================================================================
// list box item declaration and implementation
// ============================================================================

#if wxUSE_OWNER_DRAWN

class wxListBoxItem : public wxOwnerDrawn
{
public:
    wxListBoxItem(const wxString& rsStr = "");
};

wxListBoxItem::wxListBoxItem(
  const wxString& rsStr
)
: wxOwnerDrawn( rsStr
               ,false
              )
{
    //
    // No bitmaps/checkmarks
    //
    SetMarginWidth(0);
} // end of wxListBoxItem::wxListBoxItem

wxOwnerDrawn* wxListBox::CreateItem(
  size_t                            n
)
{
    return new wxListBoxItem();
} // end of wxListBox::CreateItem

#endif  //USE_OWNER_DRAWN

// ============================================================================
// list box control implementation
// ============================================================================

// Listbox item
wxListBox::wxListBox()
{
    m_nNumItems = 0;
    m_nSelected = 0;
} // end of wxListBox::wxListBox

bool wxListBox::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxPoint&                    rPos
, const wxSize&                     rSize
, const wxArrayString&              asChoices
, long                              lStyle
, const wxValidator&                rValidator
, const wxString&                   rsName
)
{
    wxCArrayString chs(asChoices);

    return Create(pParent, vId, rPos, rSize, chs.GetCount(), chs.GetStrings(),
                  lStyle, rValidator, rsName);
}

bool wxListBox::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxPoint&                    rPos
, const wxSize&                     rSize
, int                               n
, const wxString                    asChoices[]
, long                              lStyle
, const wxValidator&                rValidator
, const wxString&                   rsName
)
{
    m_nNumItems = 0;
    m_hWnd      = 0;
    m_nSelected = 0;

    SetName(rsName);
#if wxUSE_VALIDATORS
    SetValidator(rValidator);
#endif

    if (pParent)
        pParent->AddChild(this);

    wxSystemSettings                vSettings;

    SetBackgroundColour(vSettings.GetColour(wxSYS_COLOUR_WINDOW));
    SetForegroundColour(pParent->GetForegroundColour());

    m_windowId = (vId == -1) ? (int)NewControlId() : vId;

    int                             nX      = rPos.x;
    int                             nY      = rPos.y;
    int                             nWidth  = rSize.x;
    int                             nHeight = rSize.y;

    m_windowStyle = lStyle;

    lStyle = WS_VISIBLE;

    if (m_windowStyle & wxCLIP_SIBLINGS )
        lStyle |= WS_CLIPSIBLINGS;
    if (m_windowStyle & wxLB_MULTIPLE)
        lStyle |= LS_MULTIPLESEL;
    else if (m_windowStyle & wxLB_EXTENDED)
        lStyle |= LS_EXTENDEDSEL;
    if (m_windowStyle & wxLB_HSCROLL)
        lStyle |= LS_HORZSCROLL;
    if (m_windowStyle & wxLB_OWNERDRAW)
        lStyle |= LS_OWNERDRAW;

    //
    // Without this style, you get unexpected heights, so e.g. constraint layout
    // doesn't work properly
    //
    lStyle |= LS_NOADJUSTPOS;

    m_hWnd = (WXHWND)::WinCreateWindow( GetWinHwnd(pParent) // Parent
                                       ,WC_LISTBOX          // Default Listbox class
                                       ,"LISTBOX"           // Control's name
                                       ,lStyle              // Initial Style
                                       ,0, 0, 0, 0          // Position and size
                                       ,GetWinHwnd(pParent) // Owner
                                       ,HWND_TOP            // Z-Order
                                       ,(HMENU)m_windowId   // Id
                                       ,NULL                // Control Data
                                       ,NULL                // Presentation Parameters
                                      );
    if (m_hWnd == 0)
    {
        return false;
    }

    //
    // Subclass again for purposes of dialog editing mode
    //
    SubclassWin(m_hWnd);

    LONG                            lUi;

    for (lUi = 0; lUi < (LONG)n; lUi++)
    {
        Append(asChoices[lUi]);
    }
    wxFont*                          pTextFont = new wxFont( 10
                                                            ,wxMODERN
                                                            ,wxNORMAL
                                                            ,wxNORMAL
                                                           );
    SetFont(*pTextFont);

    //
    // Set standard wxWidgets colors for Listbox items and highlighting
    //
    wxColour                        vColour;

    vColour.Set(wxString("WHITE"));

    LONG                            lColor = (LONG)vColour.GetPixel();

    ::WinSetPresParam( m_hWnd
                      ,PP_HILITEFOREGROUNDCOLOR
                      ,sizeof(LONG)
                      ,(PVOID)&lColor
                     );
    vColour.Set(wxString("NAVY"));
    lColor = (LONG)vColour.GetPixel();
    ::WinSetPresParam( m_hWnd
                      ,PP_HILITEBACKGROUNDCOLOR
                      ,sizeof(LONG)
                      ,(PVOID)&lColor
                     );

    SetSize( nX
            ,nY
            ,nWidth
            ,nHeight
           );
    delete pTextFont;
    return true;
} // end of wxListBox::Create

wxListBox::~wxListBox()
{
#if wxUSE_OWNER_DRAWN
    size_t                          lUiCount = m_aItems.Count();

    while (lUiCount-- != 0)
    {
        delete m_aItems[lUiCount];
    }
#endif // wxUSE_OWNER_DRAWN
} // end of wxListBox::~wxListBox

void wxListBox::SetupColours()
{
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    SetForegroundColour(GetParent()->GetForegroundColour());
} // end of wxListBox::SetupColours

// ----------------------------------------------------------------------------
// implementation of wxListBoxBase methods
// ----------------------------------------------------------------------------

void wxListBox::DoSetFirstItem(
  int                               N
)
{
    wxCHECK_RET( N >= 0 && N < m_nNumItems,
                 wxT("invalid index in wxListBox::SetFirstItem") );

    ::WinSendMsg(GetHwnd(), LM_SETTOPINDEX, MPFROMLONG(N), (MPARAM)0);
} // end of wxListBox::DoSetFirstItem

void wxListBox::Delete(
  int                               N
)
{
    wxCHECK_RET( N >= 0 && N < m_nNumItems,
                 wxT("invalid index in wxListBox::Delete") );

#if wxUSE_OWNER_DRAWN
    delete m_aItems[N];
    m_aItems.RemoveAt(N);
#else // !wxUSE_OWNER_DRAWN
    if (HasClientObjectData())
    {
        delete GetClientObject(N);
    }
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN

    ::WinSendMsg(GetHwnd(), LM_DELETEITEM, (MPARAM)N, (MPARAM)0);
    m_nNumItems--;
} // end of wxListBox::DoSetFirstItem

int wxListBox::DoAppend(
  const wxString&                   rsItem
)
{
    long                            lIndex = 0;
    LONG                            lIndexType = 0;

    if (m_windowStyle & wxLB_SORT)
        lIndexType = LIT_SORTASCENDING;
    else
        lIndexType = LIT_END;
    lIndex = (long)::WinSendMsg(GetHwnd(), LM_INSERTITEM, (MPARAM)lIndexType, (MPARAM)rsItem.c_str());
    m_nNumItems++;

#if wxUSE_OWNER_DRAWN
    if (m_windowStyle & wxLB_OWNERDRAW)
    {
        wxOwnerDrawn*               pNewItem = CreateItem(lIndex); // dummy argument
        wxScreenDC                  vDc;


        pNewItem->SetName(rsItem);
        m_aItems.Insert(pNewItem, lIndex);
        ::WinSendMsg(GetHwnd(), LM_SETITEMHANDLE, (MPARAM)lIndex, MPFROMP(pNewItem));
        pNewItem->SetFont(GetFont());
    }
#endif
    return (int)lIndex;
} // end of wxListBox::DoAppend

void wxListBox::DoSetItems(
  const wxArrayString&              raChoices
, void**                            ppClientData
)
{
    BOOL                            bHideAndShow = IsShown();
    int                             i;
    LONG                            lIndexType = 0;

    if (bHideAndShow)
    {
        ::WinShowWindow(GetHwnd(), FALSE);
    }
    ::WinSendMsg(GetHwnd(), LM_DELETEALL, (MPARAM)0, (MPARAM)0);
    m_nNumItems = raChoices.GetCount();
    for (i = 0; i < m_nNumItems; i++)
    {

        if (m_windowStyle & wxLB_SORT)
            lIndexType = LIT_SORTASCENDING;
        else
            lIndexType = LIT_END;
        ::WinSendMsg(GetHwnd(), LM_INSERTITEM, (MPARAM)lIndexType, (MPARAM)raChoices[i].c_str());

        if (ppClientData)
        {
#if wxUSE_OWNER_DRAWN
            wxASSERT_MSG(ppClientData[i] == NULL,
                         wxT("Can't use client data with owner-drawn listboxes"));
#else // !wxUSE_OWNER_DRAWN
            ::WinSendMsg(WinUtil_GetHwnd(), LM_SETITEMHANDLE, MPFROMLONG(lCount), MPFROMP(ppClientData[i]));
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN
        }
    }

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        //
        // First delete old items
        //
        WX_CLEAR_ARRAY(m_aItems);

        //
        // Then create new ones
        //
        for (size_t ui = 0; ui < (size_t)m_nNumItems; ui++)
        {
            wxOwnerDrawn*           pNewItem = CreateItem(ui);

            pNewItem->SetName(raChoices[ui]);
            m_aItems.Add(pNewItem);
            ::WinSendMsg(GetHwnd(), LM_SETITEMHANDLE, MPFROMLONG(ui), MPFROMP(pNewItem));
        }
    }
#endif // wxUSE_OWNER_DRAWN
    ::WinShowWindow(GetHwnd(), TRUE);
} // end of wxListBox::DoSetItems

int wxListBox::FindString(
  const wxString&                   rsString
) const
{
    int                             nPos;
    LONG                            lTextLength;
    PSZ                             zStr;


    for (nPos = 0; nPos < m_nNumItems; nPos++)
    {
        lTextLength = LONGFROMMR(::WinSendMsg(GetHwnd(), LM_QUERYITEMTEXTLENGTH, (MPARAM)nPos, (MPARAM)0));
        zStr = new char[lTextLength + 1];
        ::WinSendMsg(GetHwnd(), LM_QUERYITEMTEXT, MPFROM2SHORT(nPos, (SHORT)lTextLength), (MPARAM)zStr);
        if (rsString == (char*)zStr)
        {
            delete [] zStr;
            break;
        }
        delete [] zStr;
    }
    return nPos;
} // end of wxListBox::FindString

void wxListBox::Clear()
{
#if wxUSE_OWNER_DRAWN
    size_t                          lUiCount = m_aItems.Count();

    while (lUiCount-- != 0)
    {
        delete m_aItems[lUiCount];
    }

    m_aItems.Clear();
#else // !wxUSE_OWNER_DRAWN
    if (HasClientObjectData())
    {
        for (size_t n = 0; n < (size_t)m_lNumItems; n++)
        {
            delete GetClientObject(n);
        }
    }
#endif // wxUSE_OWNER_DRAWN/!wxUSE_OWNER_DRAWN
    ::WinSendMsg(GetHwnd(), LM_DELETEALL, (MPARAM)0, (MPARAM)0);

    m_nNumItems = 0;
} // end of wxListBox::Clear

void wxListBox::SetSelection(
  int                               N
, bool                              bSelect
)
{
    wxCHECK_RET( N >= 0 && N < m_nNumItems,
                 wxT("invalid index in wxListBox::SetSelection") );
    ::WinSendMsg( GetHwnd()
                 ,LM_SELECTITEM
                 ,MPFROMLONG(N)
                 ,(MPARAM)bSelect
                );
    if(m_windowStyle & wxLB_OWNERDRAW)
        Refresh();
} // end of wxListBox::SetSelection

bool wxListBox::IsSelected(
  int                               N
) const
{
    wxCHECK_MSG( N >= 0 && N < m_nNumItems, false,
                 wxT("invalid index in wxListBox::Selected") );

    LONG                            lItem;

    if (GetWindowStyleFlag() & wxLB_EXTENDED)
    {
        if (N == 0)
            lItem = LONGFROMMR(::WinSendMsg(GetHwnd(), LM_QUERYSELECTION, (MPARAM)LIT_FIRST, (MPARAM)0));
        else
            lItem = LONGFROMMR(::WinSendMsg(GetHwnd(), LM_QUERYSELECTION, (MPARAM)(N - 1), (MPARAM)0));
    }
    else
    {
        lItem = LONGFROMMR(::WinSendMsg(GetHwnd(), LM_QUERYSELECTION, (MPARAM)LIT_FIRST, (MPARAM)0));
    }
    return (lItem == (LONG)N && lItem != LIT_NONE);
} // end of wxListBox::IsSelected

wxClientData* wxListBox::DoGetItemClientObject(
  int                               n
) const
{
    return (wxClientData *)DoGetItemClientData(n);
}

void* wxListBox::DoGetItemClientData(
  int                               n
) const
{
    wxCHECK_MSG( n >= 0 && n < m_nNumItems, NULL,
                 wxT("invalid index in wxListBox::GetClientData") );

    return((void *)::WinSendMsg(GetHwnd(), LM_QUERYITEMHANDLE, MPFROMLONG(n), (MPARAM)0));
} // end of wxListBox::DoGetItemClientData

void wxListBox::DoSetItemClientObject(
  int                               n
, wxClientData*                     pClientData
)
{
    DoSetItemClientData( n
                        ,pClientData
                       );
} // end of wxListBox::DoSetItemClientObject

void wxListBox::DoSetItemClientData(
  int                               n
, void*                             pClientData
)
{
    wxCHECK_RET( n >= 0 && n < m_nNumItems,
                 wxT("invalid index in wxListBox::SetClientData") );

#if wxUSE_OWNER_DRAWN
    if ( m_windowStyle & wxLB_OWNERDRAW )
    {
        //
        // Client data must be pointer to wxOwnerDrawn, otherwise we would crash
        // in OnMeasure/OnDraw.
        //
        wxFAIL_MSG(wxT("Can't use client data with owner-drawn listboxes"));
    }
#endif // wxUSE_OWNER_DRAWN

    ::WinSendMsg(GetHwnd(), LM_SETITEMHANDLE, MPFROMLONG(n), MPFROMP(pClientData));
} // end of wxListBox::DoSetItemClientData

bool wxListBox::HasMultipleSelection() const
{
    return (m_windowStyle & wxLB_MULTIPLE) || (m_windowStyle & wxLB_EXTENDED);
} // end of wxListBox::HasMultipleSelection

int wxListBox::GetSelections(
  wxArrayInt&                       raSelections
) const
{
    int                             nCount = 0;
    LONG                            lItem;


    raSelections.Empty();
    if (HasMultipleSelection())
    {
        lItem = LONGFROMMR(::WinSendMsg( GetHwnd()
                                        ,LM_QUERYSELECTION
                                        ,(MPARAM)LIT_FIRST
                                        ,(MPARAM)0
                                       )
                          );
        if (lItem != LIT_NONE)
        {
            nCount++;
            while ((lItem = LONGFROMMR(::WinSendMsg( GetHwnd()
                                                    ,LM_QUERYSELECTION
                                                    ,(MPARAM)lItem
                                                    ,(MPARAM)0
                                                   )
                                      )) != LIT_NONE)
            {
                nCount++;
            }
            raSelections.Alloc(nCount);
            lItem = LONGFROMMR(::WinSendMsg( GetHwnd()
                                            ,LM_QUERYSELECTION
                                            ,(MPARAM)LIT_FIRST
                                            ,(MPARAM)0
                                           )
                              );

            raSelections.Add((int)lItem);
            while ((lItem = LONGFROMMR(::WinSendMsg( GetHwnd()
                                                    ,LM_QUERYSELECTION
                                                    ,(MPARAM)lItem
                                                    ,(MPARAM)0
                                                   )
                                      )) != LIT_NONE)
            {
                raSelections.Add((int)lItem);
            }
            return nCount;
        }
        return 0;
    }
    else  // single-selection listbox
    {
        lItem = LONGFROMMR(::WinSendMsg( GetHwnd()
                                        ,LM_QUERYSELECTION
                                        ,(MPARAM)LIT_FIRST
                                        ,(MPARAM)0
                                       )
                          );
        raSelections.Add((int)lItem);
        return 1;
    }
    return 0;
} // end of wxListBox::GetSelections

int wxListBox::GetSelection() const
{
    wxCHECK_MSG( !HasMultipleSelection(),
                 -1,
                 wxT("GetSelection() can't be used with multiple-selection "
                    "listboxes, use GetSelections() instead.") );

    return(LONGFROMMR(::WinSendMsg( GetHwnd()
                                   ,LM_QUERYSELECTION
                                   ,(MPARAM)LIT_FIRST
                                   ,(MPARAM)0
                                  )
                     ));
} // end of wxListBox::GetSelection

wxString wxListBox::GetString(
  int                               N
) const
{
    LONG                            lLen = 0;
    char*                           zBuf;
    wxString                        sResult;

    wxCHECK_MSG( N >= 0 && N < m_nNumItems, "",
                 wxT("invalid index in wxListBox::GetClientData") );

    lLen = LONGFROMMR(::WinSendMsg(GetHwnd(), LM_QUERYITEMTEXTLENGTH, (MPARAM)N, (MPARAM)0));
    zBuf = new char[lLen + 1];
    ::WinSendMsg(GetHwnd(), LM_QUERYITEMTEXT, MPFROM2SHORT((SHORT)N, (SHORT)lLen), (MPARAM)zBuf);
    zBuf[lLen] = '\0';
    sResult = zBuf;
    delete [] zBuf;
    return sResult;
} // end of wxListBox::GetString

void wxListBox::DoInsertItems(
  const wxArrayString&              asItems
, int                               nPos
)
{
    wxCHECK_RET( nPos >= 0 && nPos <= m_nNumItems,
                 wxT("invalid index in wxListBox::InsertItems") );

    int                             nItems = asItems.GetCount();

    for (int i = 0; i < nItems; i++)
    {
        int                         nIndex = (int)::WinSendMsg( GetHwnd()
                                                               ,LM_INSERTITEM
                                                               ,MPFROMLONG((LONG)(i + nPos))
                                                               ,(MPARAM)asItems[i].c_str()
                                                              );

        wxOwnerDrawn*               pNewItem = CreateItem(nIndex);

        pNewItem->SetName(asItems[i]);
        pNewItem->SetFont(GetFont());
        m_aItems.Insert(pNewItem, nIndex);
        ::WinSendMsg( GetHwnd()
                     ,LM_SETITEMHANDLE
                     ,(MPARAM)((LONG)nIndex)
                     ,MPFROMP(pNewItem)
                    );
        m_nNumItems += nItems;
    }
} // end of wxListBox::DoInsertItems

void wxListBox::SetString(
  int                               N
, const wxString&                   rsString
)
{
    wxCHECK_RET( N >= 0 && N < m_nNumItems,
                 wxT("invalid index in wxListBox::SetString") );

    //
    // Remember the state of the item
    //
    bool                            bWasSelected = IsSelected(N);
    void*                           pOldData = NULL;
    wxClientData*                   pOldObjData = NULL;

    if (m_clientDataItemsType == wxClientData_Void)
        pOldData = GetClientData(N);
    else if (m_clientDataItemsType == wxClientData_Object)
        pOldObjData = GetClientObject(N);

    //
    // Delete and recreate it
    //
    ::WinSendMsg( GetHwnd()
                 ,LM_DELETEITEM
                 ,(MPARAM)N
                 ,(MPARAM)0
                );

    int                             nNewN = N;

    if (N == m_nNumItems - 1)
        nNewN = -1;

    ::WinSendMsg( GetHwnd()
                 ,LM_INSERTITEM
                 ,(MPARAM)nNewN
                 ,(MPARAM)rsString.c_str()
                );

    //
    // Restore the client data
    //
    if (pOldData)
        SetClientData( N
                      ,pOldData
                     );
    else if (pOldObjData)
        SetClientObject( N
                        ,pOldObjData
                       );

    //
    // We may have lost the selection
    //
    if (bWasSelected)
        Select(N);

#if wxUSE_OWNER_DRAWN
    if (m_windowStyle & wxLB_OWNERDRAW)
        //
        // Update item's text
        //
        m_aItems[N]->SetName(rsString);
#endif  //USE_OWNER_DRAWN
} // end of wxListBox::SetString

int wxListBox::GetCount() const
{
    return m_nNumItems;
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

wxSize wxListBox::DoGetBestSize() const
{
    //
    // Find the widest string
    //
    int                             nLine;
    int                             nListbox = 0;
    int                             nCx;
    int                             nCy;

    for (int i = 0; i < m_nNumItems; i++)
    {
        wxString                    vStr(GetString(i));

        GetTextExtent( vStr
                      ,&nLine
                      ,NULL
                     );
        if (nLine > nListbox)
            nListbox = nLine;
    }

    //
    // Give it some reasonable default value if there are no strings in the
    // list.
    //
    if (nListbox == 0)
        nListbox = 100;

    //
    // The listbox should be slightly larger than the widest string
    //
    wxGetCharSize( GetHWND()
                  ,&nCx
                  ,&nCy
                  ,(wxFont*)&GetFont()
                 );
    nListbox += 3 * nCx;

    int                             hListbox = EDIT_HEIGHT_FROM_CHAR_HEIGHT(nCy) * (wxMax(m_nNumItems, 7));

    return wxSize( nListbox
                  ,hListbox
                 );
} // end of wxListBox::DoGetBestSize

// ----------------------------------------------------------------------------
// callbacks
// ----------------------------------------------------------------------------

bool wxListBox::OS2Command(
  WXUINT                            uParam
, WXWORD                            WXUNUSED(wId))
{
    wxEventType                     eEvtType;

    if (uParam == LN_SELECT)
    {
        eEvtType = wxEVT_COMMAND_LISTBOX_SELECTED;
    }
    if (uParam == LN_ENTER)
    {
        eEvtType = wxEVT_COMMAND_LISTBOX_DOUBLECLICKED;
    }
    else
    {
        //
        // Some event we're not interested in
        //
        return false;
    }
    wxCommandEvent                  vEvent( eEvtType
                                           ,m_windowId
                                          );

    vEvent.SetEventObject(this);

    wxArrayInt                      aSelections;
    int                             n;
    int                             nCount = GetSelections(aSelections);

    if (nCount > 0)
    {
        n = aSelections[0];
        if (HasClientObjectData())
            vEvent.SetClientObject(GetClientObject(n));
        else if ( HasClientUntypedData() )
            vEvent.SetClientData(GetClientData(n));
        vEvent.SetString(GetString(n));
    }
    else
    {
        n = -1;
    }
    vEvent.SetInt(n);
    return GetEventHandler()->ProcessEvent(vEvent);
} // end of wxListBox::OS2Command

// ----------------------------------------------------------------------------
// wxCheckListBox support
// ----------------------------------------------------------------------------

#if wxUSE_OWNER_DRAWN

//
// Drawing
// -------
//
#define OWNER_DRAWN_LISTBOX_EXTRA_SPACE    (1)

long wxListBox::OS2OnMeasure(
  WXMEASUREITEMSTRUCT*              pItem
)
{
    if (!pItem)
        pItem = (WXMEASUREITEMSTRUCT*)new OWNERITEM;

    POWNERITEM                      pMeasureStruct = (POWNERITEM)pItem;
    wxScreenDC                      vDc;

    //
    // Only owner-drawn control should receive this message
    //
    wxCHECK( ((m_windowStyle & wxLB_OWNERDRAW) == wxLB_OWNERDRAW), FALSE );

    vDc.SetFont(GetFont());

    wxCoord                         vHeight;
    wxCoord                         vWidth;

    GetSize( &vWidth
            ,NULL
           );

    pMeasureStruct->rclItem.xRight = (USHORT)vWidth;
    pMeasureStruct->rclItem.xLeft  = 0;
    pMeasureStruct->rclItem.yTop   = 0;
    pMeasureStruct->rclItem.yBottom = 0;

    vHeight = (wxCoord)(vDc.GetCharHeight() * 2.5);
    pMeasureStruct->rclItem.yTop  = (USHORT)vHeight;

    return long(MRFROM2SHORT((USHORT)vHeight, (USHORT)vWidth));
} // end of wxListBox::OS2OnMeasure

bool wxListBox::OS2OnDraw (
  WXDRAWITEMSTRUCT*                 pItem
)
{
    POWNERITEM                      pDrawStruct = (POWNERITEM)pItem;
    LONG                            lItemID = pDrawStruct->idItem;
    int                             eAction = 0;
    int                             eStatus = 0;

    //
    // Only owner-drawn control should receive this message
    //
    wxCHECK(((m_windowStyle & wxLB_OWNERDRAW) == wxLB_OWNERDRAW), false);


    //
    // The item may be -1 for an empty listbox
    //
    if (lItemID == -1L)
        return false;

    wxListBoxItem*                   pData = (wxListBoxItem*)PVOIDFROMMR( ::WinSendMsg( GetHwnd()
                                                                                       ,LM_QUERYITEMHANDLE
                                                                                       ,MPFROMLONG(pDrawStruct->idItem)
                                                                                       ,(MPARAM)0
                                                                                      )
                                                                        );

    wxCHECK(pData, false );

    wxDC                              vDc;
    wxRect                            vRect( wxPoint( pDrawStruct->rclItem.xLeft
                                                     ,pDrawStruct->rclItem.yTop
                                                    )
                                            ,wxPoint( pDrawStruct->rclItem.xRight
                                                     ,pDrawStruct->rclItem.yBottom
                                                    )
                                           );

    vDc.SetHPS(pDrawStruct->hps);

    if (pDrawStruct->fsAttribute == pDrawStruct->fsAttributeOld)
    {
        //
        // Entire Item needs to be redrawn (either it has reappeared from
        // behind another window or is being displayed for the first time
        //
        eAction = wxOwnerDrawn::wxODDrawAll;

        if (pDrawStruct->fsAttribute & MIA_HILITED)
        {
            //
            // If it is currently selected we let the system handle it
            //
            eStatus |= wxOwnerDrawn::wxODSelected;
        }
        if (pDrawStruct->fsAttribute & MIA_CHECKED)
        {
            //
            // If it is currently checked we draw our own
            //
            eStatus |= wxOwnerDrawn::wxODChecked;
            pDrawStruct->fsAttributeOld = pDrawStruct->fsAttribute &= ~MIA_CHECKED;
        }
        if (pDrawStruct->fsAttribute & MIA_DISABLED)
        {
            //
            // If it is currently disabled we let the system handle it
            //
            eStatus |= wxOwnerDrawn::wxODDisabled;
        }
        //
        // Don't really care about framed (indicationg focus) or NoDismiss
        //
    }
    else
    {
        if (pDrawStruct->fsAttribute & MIA_HILITED)
        {
            eAction = wxOwnerDrawn::wxODDrawAll;
            eStatus |= wxOwnerDrawn::wxODSelected;
            //
            // Keep the system from trying to highlight with its bogus colors
            //
            pDrawStruct->fsAttributeOld = pDrawStruct->fsAttribute &= ~MIA_HILITED;
        }
        else if (!(pDrawStruct->fsAttribute & MIA_HILITED))
        {
            eAction = wxOwnerDrawn::wxODDrawAll;
            eStatus = 0;
            //
            // Keep the system from trying to highlight with its bogus colors
            //
            pDrawStruct->fsAttribute = pDrawStruct->fsAttributeOld &= ~MIA_HILITED;
        }
        else
        {
            //
            // For now we don't care about anything else
            // just ignore the entire message!
            //
            return true;
        }
    }
    return pData->OnDrawItem( vDc
                             ,vRect
                             ,(wxOwnerDrawn::wxODAction)eAction
                             ,(wxOwnerDrawn::wxODStatus)eStatus
                            );
} // end of wxListBox::OS2OnDraw

#endif // ndef for wxUSE_OWNER_DRAWN

#endif // ndef for wxUSE_LISTBOX

