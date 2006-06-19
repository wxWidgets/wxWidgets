///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/classic/dnd.cpp
// Purpose:     wxDropTarget, wxDropSource, wxDataObject implementation
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DRAG_AND_DROP

#include "wx/dnd.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/window.h"
    #include "wx/toplevel.h"
    #include "wx/gdicmn.h"
#endif // WX_PRECOMP

#include "wx/mac/private.h"

// ----------------------------------------------------------------------------
// global
// ----------------------------------------------------------------------------

void wxMacEnsureTrackingHandlersInstalled() ;

typedef struct
{
    wxWindow* m_currentTargetWindow ;
    wxDropTarget* m_currentTarget ;
    wxDropSource* m_currentSource ;
} MacTrackingGlobals ;

MacTrackingGlobals gTrackingGlobals ;

//----------------------------------------------------------------------------
// wxDropTarget
//----------------------------------------------------------------------------

wxDropTarget::wxDropTarget( wxDataObject *data )
            : wxDropTargetBase( data )
{
    wxMacEnsureTrackingHandlersInstalled() ;
}

wxDragResult wxDropTarget::OnDragOver( wxCoord WXUNUSED(x),
                                       wxCoord WXUNUSED(y),
                                       wxDragResult def )
{

    return CurrentDragHasSupportedFormat() ? def : wxDragNone;
}

bool wxDropTarget::OnDrop( wxCoord WXUNUSED(x), wxCoord WXUNUSED(y) )
{
    if (!m_dataObject)
        return false;

    return CurrentDragHasSupportedFormat() ;
}

wxDragResult wxDropTarget::OnData( wxCoord WXUNUSED(x), wxCoord WXUNUSED(y),
                                   wxDragResult def )
{
    if (!m_dataObject)
        return wxDragNone;

    if (!CurrentDragHasSupportedFormat())
        return wxDragNone;

    return GetData() ? def : wxDragNone;
}

bool wxDropTarget::CurrentDragHasSupportedFormat()
{
    bool supported = false ;
    if ( gTrackingGlobals.m_currentSource != NULL )
    {
        wxDataObject* data = gTrackingGlobals.m_currentSource->GetDataObject() ;

        if ( data )
        {
            size_t formatcount = data->GetFormatCount() ;
            wxDataFormat *array = new wxDataFormat[ formatcount  ];
            data->GetAllFormats( array );
            for (size_t i = 0; !supported && i < formatcount ; i++)
            {
                wxDataFormat format = array[i] ;
                if ( m_dataObject->IsSupported( format ) )
                {
                    supported = true ;
                    break ;
                }
            }
            delete[] array ;
        }
    }
    if ( !supported )
    {
        UInt16 items ;
        OSErr result;
        CountDragItems((DragReference)m_currentDrag, &items);
        for (UInt16 index = 1; index <= items && supported == false ; ++index)
        {
            ItemReference theItem;
            FlavorType theType ;
            UInt16 flavors = 0 ;
            GetDragItemReferenceNumber((DragReference)m_currentDrag, index, &theItem);
            CountDragItemFlavors( (DragReference)m_currentDrag, theItem , &flavors ) ;
            for ( UInt16 flavor = 1 ; flavor <= flavors ; ++flavor )
            {
                result = GetFlavorType((DragReference)m_currentDrag, theItem, flavor , &theType);
                if ( m_dataObject->IsSupportedFormat( wxDataFormat( theType ) ) )
                {
                    supported = true ;
                    break ;
                }
            }
        }
    }
    return supported ;
}

bool wxDropTarget::GetData()
{
    if (!m_dataObject)
        return false;

    if ( !CurrentDragHasSupportedFormat() )
        return false ;

    bool transferred = false ;
    if ( gTrackingGlobals.m_currentSource != NULL )
    {
        wxDataObject* data = gTrackingGlobals.m_currentSource->GetDataObject() ;

        if ( data )
        {
            size_t formatcount = data->GetFormatCount() ;
            wxDataFormat *array = new wxDataFormat[ formatcount  ];
            data->GetAllFormats( array );
            for (size_t i = 0; !transferred && i < formatcount ; i++)
            {
                wxDataFormat format = array[i] ;
                if ( m_dataObject->IsSupported( format ) )
                {
                    int size = data->GetDataSize( format );
                    transferred = true ;

                    if (size == 0)
                    {
                        m_dataObject->SetData(format , 0 , 0 ) ;
                    }
                    else
                    {
                        char *d = new char[size];
                        data->GetDataHere( format , (void*) d );
                        m_dataObject->SetData( format , size , d ) ;
                        delete[] d ;
                    }
                }
            }
            delete[] array ;
        }
    }
    if ( !transferred )
    {
        UInt16 items ;
        OSErr result;
        bool firstFileAdded = false ;
        CountDragItems((DragReference)m_currentDrag, &items);
        for (UInt16 index = 1; index <= items; ++index)
        {
            ItemReference theItem;
            FlavorType theType ;
            UInt16 flavors = 0 ;
            GetDragItemReferenceNumber((DragReference)m_currentDrag, index, &theItem);
            CountDragItemFlavors( (DragReference)m_currentDrag, theItem , &flavors ) ;
            for ( UInt16 flavor = 1 ; flavor <= flavors ; ++flavor )
            {
                result = GetFlavorType((DragReference)m_currentDrag, theItem, flavor , &theType);
                wxDataFormat format(theType) ;
                if ( m_dataObject->IsSupportedFormat( format ) )
                {
                    FlavorFlags theFlags;
                    result = GetFlavorFlags((DragReference)m_currentDrag, theItem, theType, &theFlags);
                    if (result == noErr)
                    {
                        Size dataSize ;
                        Ptr theData ;
                        GetFlavorDataSize((DragReference)m_currentDrag, theItem, theType, &dataSize);
                        if ( theType == 'TEXT' )
                        {
                            // this increment is only valid for allocating, on the next GetFlavorData
                            // call it is reset again to the original value
                            dataSize++ ;
                        }
                        theData = new char[dataSize];
                        GetFlavorData((DragReference)m_currentDrag, theItem, theType, (void*) theData, &dataSize, 0L);
                        if( theType == 'TEXT' )
                        {
                            theData[dataSize]=0 ;
                            wxString convert( theData , wxConvLocal ) ;
                            m_dataObject->SetData( format, convert.length() * sizeof(wxChar), (const wxChar*) convert );
                        }
                        else if ( theType == kDragFlavorTypeHFS )
                        {
                            HFSFlavor* theFile = (HFSFlavor*) theData ;
                            wxString name = wxMacFSSpec2MacFilename( &theFile->fileSpec ) ;
                            if (  firstFileAdded )
                                ((wxFileDataObject*)m_dataObject)->AddFile( name ) ;
                            else
                            {
                                ((wxFileDataObject*)m_dataObject)->SetData( 0 , name.c_str() ) ;
                                firstFileAdded = true ;
                            }
                        }
                        else
                        {
                            m_dataObject->SetData( format, dataSize, theData );
                        }
                        delete[] theData;
                    }
                    break ;
                }
            }
        }
    }
    return true ;
}

//-------------------------------------------------------------------------
// wxDropSource
//-------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// drag request

wxDropSource::wxDropSource(wxWindow *win,
                           const wxCursor &cursorCopy,
                           const wxCursor &cursorMove,
                           const wxCursor &cursorStop)
            : wxDropSourceBase(cursorCopy, cursorMove, cursorStop)
{
    wxMacEnsureTrackingHandlersInstalled() ;
    m_window = win;
}

wxDropSource::wxDropSource(wxDataObject& data,
                           wxWindow *win,
                           const wxCursor &cursorCopy,
                           const wxCursor &cursorMove,
                           const wxCursor &cursorStop)
            : wxDropSourceBase(cursorCopy, cursorMove, cursorStop)
{
    wxMacEnsureTrackingHandlersInstalled() ;
    SetData( data );
    m_window = win;
}

wxDropSource::~wxDropSource()
{
}


wxDragResult wxDropSource::DoDragDrop(int WXUNUSED(flags))
{
    wxASSERT_MSG( m_data, wxT("Drop source: no data") );

    if (!m_data)
        return (wxDragResult) wxDragNone;

    if (m_data->GetFormatCount() == 0)
        return (wxDragResult) wxDragNone;

    OSErr result;
    DragReference theDrag;
    RgnHandle dragRegion;
    if ((result = NewDrag(&theDrag)))
    {
        return wxDragNone ;
    }
    // add data to drag
    size_t formatCount = m_data->GetFormatCount() ;
    wxDataFormat *formats = new wxDataFormat[formatCount] ;
    m_data->GetAllFormats( formats ) ;
    ItemReference theItem = 1 ;
    for ( size_t i = 0 ; i < formatCount ; ++i )
    {
        size_t dataSize = m_data->GetDataSize( formats[i] ) ;
        Ptr dataPtr = new char[dataSize] ;
        m_data->GetDataHere( formats[i] , dataPtr ) ;
        OSType type = formats[i].GetFormatId() ;
        if ( type == 'TEXT' )
        {
            dataSize-- ;
            dataPtr[ dataSize ] = 0 ;
            wxString st( (wxChar*) dataPtr ) ;
            wxCharBuffer buf = st.mb_str( wxConvLocal) ;
            AddDragItemFlavor(theDrag, theItem, type , buf.data(), strlen(buf), 0);
        }
        else if (type == kDragFlavorTypeHFS )
        {
            HFSFlavor  theFlavor ;
            OSErr err = noErr;
            CInfoPBRec cat;

            wxMacFilename2FSSpec( dataPtr , &theFlavor.fileSpec ) ;

            cat.hFileInfo.ioNamePtr = theFlavor.fileSpec.name;
            cat.hFileInfo.ioVRefNum = theFlavor.fileSpec.vRefNum;
            cat.hFileInfo.ioDirID = theFlavor.fileSpec.parID;
            cat.hFileInfo.ioFDirIndex = 0;
            err = PBGetCatInfoSync(&cat);
            if (err == noErr )
            {
                theFlavor.fdFlags = cat.hFileInfo.ioFlFndrInfo.fdFlags;
                if (theFlavor.fileSpec.parID == fsRtParID) {
                    theFlavor.fileCreator = 'MACS';
                    theFlavor.fileType = 'disk';
                } else if ((cat.hFileInfo.ioFlAttrib & ioDirMask) != 0) {
                    theFlavor.fileCreator = 'MACS';
                    theFlavor.fileType = 'fold';
                } else {
                    theFlavor.fileCreator = cat.hFileInfo.ioFlFndrInfo.fdCreator;
                    theFlavor.fileType = cat.hFileInfo.ioFlFndrInfo.fdType;
                }
                AddDragItemFlavor(theDrag, theItem, type , &theFlavor, sizeof(theFlavor), 0);
            }
        }
        else
        {
            AddDragItemFlavor(theDrag, theItem, type , dataPtr, dataSize, 0);
        }
        delete[] dataPtr ;
    }
    delete[] formats ;

    dragRegion = NewRgn();
    RgnHandle tempRgn = NewRgn() ;

    EventRecord* ev = NULL ;
#if !TARGET_CARBON // TODO
    ev = (EventRecord*) wxTheApp->MacGetCurrentEvent() ;
#else
    EventRecord rec ;
    ev = &rec ;
    wxMacConvertEventToRecord( (EventRef) wxTheApp->MacGetCurrentEvent() , &rec ) ;
#endif
    const short dragRegionOuterBoundary = 10 ;
    const short dragRegionInnerBoundary = 9 ;

    SetRectRgn( dragRegion , ev->where.h - dragRegionOuterBoundary ,
        ev->where.v  - dragRegionOuterBoundary ,
        ev->where.h + dragRegionOuterBoundary ,
        ev->where.v + dragRegionOuterBoundary ) ;

    SetRectRgn( tempRgn , ev->where.h - dragRegionInnerBoundary ,
        ev->where.v  - dragRegionInnerBoundary ,
        ev->where.h + dragRegionInnerBoundary ,
        ev->where.v + dragRegionInnerBoundary ) ;

    DiffRgn( dragRegion , tempRgn , dragRegion ) ;
    DisposeRgn( tempRgn ) ;

    // TODO:work with promises in order to return data only when drag
    // was successfully completed

    gTrackingGlobals.m_currentSource = this ;
    result = TrackDrag(theDrag, ev , dragRegion);
    DisposeRgn(dragRegion);
    DisposeDrag(theDrag);
    gTrackingGlobals.m_currentSource = NULL ;

    KeyMap keymap;
    GetKeys(keymap);
    bool optionDown = keymap[1] & 4;
    wxDragResult dndresult = optionDown ? wxDragCopy : wxDragMove;
    return dndresult;
}

bool wxDropSource::MacInstallDefaultCursor(wxDragResult effect)
{
    const wxCursor& cursor = GetCursor(effect);
    if ( cursor.Ok() )
    {
        cursor.MacInstall() ;

        return true;
    }
    else
    {
        return false;
    }
}

bool gTrackingGlobalsInstalled = false ;

// passing the globals via refcon is not needed by the CFM and later architectures anymore
// but I'll leave it in there, just in case...

pascal OSErr wxMacWindowDragTrackingHandler(DragTrackingMessage theMessage, WindowPtr theWindow,
  void *handlerRefCon, DragReference theDrag) ;
pascal OSErr wxMacWindowDragReceiveHandler(WindowPtr theWindow, void *handlerRefCon,
DragReference theDrag) ;

void wxMacEnsureTrackingHandlersInstalled()
{
    if( !gTrackingGlobalsInstalled )
    {
        OSErr result;

        result = InstallTrackingHandler(NewDragTrackingHandlerUPP(wxMacWindowDragTrackingHandler), 0L,&gTrackingGlobals);
        wxASSERT( result == noErr ) ;
        result = InstallReceiveHandler(NewDragReceiveHandlerUPP(wxMacWindowDragReceiveHandler), 0L, &gTrackingGlobals);
        wxASSERT( result == noErr ) ;

        gTrackingGlobalsInstalled = true ;
    }
}

pascal OSErr wxMacWindowDragTrackingHandler(DragTrackingMessage theMessage, WindowPtr theWindow,
  void *handlerRefCon, DragReference theDrag)
{
    MacTrackingGlobals* trackingGlobals = (MacTrackingGlobals*) handlerRefCon;
    Point mouse, localMouse;
    DragAttributes attributes;
    GetDragAttributes(theDrag, &attributes);
    wxTopLevelWindowMac* toplevel = wxFindWinFromMacWindow( (WXWindow) theWindow ) ;

    KeyMap keymap;
    GetKeys(keymap);
    bool optionDown = keymap[1] & 4;
    wxDragResult result = optionDown ? wxDragCopy : wxDragMove;

    switch(theMessage)
    {
        case kDragTrackingEnterHandler:
            break;
        case kDragTrackingLeaveHandler:
            break;
        case kDragTrackingEnterWindow:
            trackingGlobals->m_currentTargetWindow = NULL ;
            trackingGlobals->m_currentTarget = NULL ;
            break;
        case kDragTrackingInWindow:
            if (toplevel == NULL)
                break;

            GetDragMouse(theDrag, &mouse, 0L);
            localMouse = mouse;
            GlobalToLocal(&localMouse);



//            if (attributes & kDragHasLeftSenderWindow)
            {
                wxPoint point(localMouse.h , localMouse.v) ;
                wxWindow *win = NULL ;
                toplevel->MacGetWindowFromPointSub( point , &win ) ;
                int localx , localy ;
                localx = localMouse.h ;
                localy = localMouse.v ;
                //TODO : should we use client coordinates
                if ( win )
                    win->MacRootWindowToWindow( &localx , &localy ) ;
                if ( win != trackingGlobals->m_currentTargetWindow )
                {
                    if ( trackingGlobals->m_currentTargetWindow )
                    {
                        // this window is left
                        if ( trackingGlobals->m_currentTarget )
                        {
                            HideDragHilite(theDrag);
                            trackingGlobals->m_currentTarget->SetCurrentDrag( theDrag ) ;
                            trackingGlobals->m_currentTarget->OnLeave() ;
                            trackingGlobals->m_currentTarget = NULL;
                            trackingGlobals->m_currentTargetWindow = NULL ;
                        }
                    }
                    if ( win )
                    {
                        // this window is entered
                        trackingGlobals->m_currentTargetWindow = win ;
                        trackingGlobals->m_currentTarget = win->GetDropTarget() ;
                        {

                            if ( trackingGlobals->m_currentTarget )
                            {
                                trackingGlobals->m_currentTarget->SetCurrentDrag( theDrag ) ;
                                result = trackingGlobals->m_currentTarget->OnEnter(
                                    localx , localy , result ) ;
                            }


                            if ( result != wxDragNone )
                            {
                                int x , y ;
                                x = y = 0 ;
                                win->MacWindowToRootWindow( &x , &y ) ;
                                RgnHandle hiliteRgn = NewRgn() ;
                                SetRectRgn( hiliteRgn , x , y , x+win->GetSize().x ,y+win->GetSize().y) ;
                                ShowDragHilite(theDrag, hiliteRgn, true);
                                DisposeRgn( hiliteRgn ) ;
                            }
                        }
                    }
                }
                else
                {
                    if( trackingGlobals->m_currentTarget )
                    {
                        trackingGlobals->m_currentTarget->SetCurrentDrag( theDrag ) ;
                        trackingGlobals->m_currentTarget->OnDragOver(
                            localx , localy , result ) ;
                    }
                }

                // set cursor for OnEnter and OnDragOver
                if ( trackingGlobals->m_currentSource && trackingGlobals->m_currentSource->GiveFeedback( result ) == FALSE )
                {
                  if ( trackingGlobals->m_currentSource->MacInstallDefaultCursor( result ) == FALSE )
                  {
                      switch( result )
                      {
                          case wxDragCopy :
                              {
                                  wxCursor cursor(wxCURSOR_COPY_ARROW) ;
                                  cursor.MacInstall() ;
                              }
                              break ;
                          case wxDragMove :
                              {
                                  wxCursor cursor(wxCURSOR_ARROW) ;
                                  cursor.MacInstall() ;
                              }
                              break ;
                          case wxDragNone :
                              {
                                  wxCursor cursor(wxCURSOR_NO_ENTRY) ;
                                  cursor.MacInstall() ;
                              }
                              break ;

                          case wxDragError:
                          case wxDragLink:
                          case wxDragCancel:
                              // put these here to make gcc happy
                              ;
                      }
                  }
                }

          }
            // MyTrackItemUnderMouse(localMouse, theWindow);
            break;
        case kDragTrackingLeaveWindow:
            if (trackingGlobals->m_currentTarget)
            {
                trackingGlobals->m_currentTarget->SetCurrentDrag( theDrag ) ;
                trackingGlobals->m_currentTarget->OnLeave() ;
                HideDragHilite(theDrag);
                trackingGlobals->m_currentTarget = NULL ;
            }
            trackingGlobals->m_currentTargetWindow = NULL ;
            break;
    }
    return(noErr);
}

pascal OSErr wxMacWindowDragReceiveHandler(WindowPtr theWindow,
                                           void *handlerRefCon,
                                           DragReference theDrag)
{
    MacTrackingGlobals* trackingGlobals = (MacTrackingGlobals*) handlerRefCon;
    if ( trackingGlobals->m_currentTarget )
    {
        Point mouse,localMouse ;
        int localx,localy ;

        trackingGlobals->m_currentTarget->SetCurrentDrag( theDrag ) ;
        GetDragMouse(theDrag, &mouse, 0L);
        localMouse = mouse;
        GlobalToLocal(&localMouse);
        localx = localMouse.h ;
        localy = localMouse.v ;
        //TODO : should we use client coordinates
        if ( trackingGlobals->m_currentTargetWindow )
            trackingGlobals->m_currentTargetWindow->MacRootWindowToWindow( &localx , &localy ) ;
        if ( trackingGlobals->m_currentTarget->OnDrop( localx , localy ) )
        {
            KeyMap keymap;
            GetKeys(keymap);
            bool optionDown = keymap[1] & 4;
            wxDragResult result = optionDown ? wxDragCopy : wxDragMove;
            trackingGlobals->m_currentTarget->OnData( localx , localy , result ) ;
        }
    }
    return(noErr);
}
#endif
