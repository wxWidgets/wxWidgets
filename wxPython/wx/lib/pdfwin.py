#----------------------------------------------------------------------
# Name:        wx.lib.pdfwin
# Purpose:     A class that allows the use of the Acrobat PSF reader
#              ActiveX control
#
# Author:      Robin Dunn
#
# Created:     22-March-2004
# RCS-ID:      $Id$
# Copyright:   (c) 2004 by Total Control Software
# Licence:     wxWindows license
#----------------------------------------------------------------------

import wx

#----------------------------------------------------------------------

_acroversion = None

def get_acroversion():
    " Return version of Adobe Acrobat executable or None"
    global _acroversion
    if _acroversion == None:
        import _winreg
        acrosoft = [r'SOFTWARE\Adobe\Acrobat Reader\%version%\InstallPath',
                    r'SOFTWARE\Adobe\Adobe Acrobat\%version%\InstallPath',]
            
        for regkey in acrosoft:
            for version in ('7.0', '6.0', '5.0', '4.0'):
                try:
                    path = _winreg.QueryValue(_winreg.HKEY_LOCAL_MACHINE,
                                              regkey.replace('%version%', version))
                    _acroversion = version
                    break
                except:
                    continue
    return _acroversion

#----------------------------------------------------------------------

# The ActiveX module from Acrobat 7.0 has changed and it seems to now
# require much more from the container than what our wx.activex module
# provides.  If we try to use it via wx.activex then Acrobat crashes.
# So instead we will use Internet Explorer (via the win32com modules
# so we can use better dynamic dispatch) and embed the Acrobat control
# within that.  Acrobat provides the IAcroAXDocShim COM class that is
# accessible via the IE window's Docuemnt property after a PDF file
# has been loaded.

if get_acroversion() >= '7.0':

    from wx.lib.activexwrapper import MakeActiveXClass
    import win32com.client.gencache
    _browserModule = win32com.client.gencache.EnsureModule(
        "{EAB22AC0-30C1-11CF-A7EB-0000C05BAE0B}", 0, 1, 1)

    class PDFWindowError(RuntimeError):
        def __init__(self):
            RuntimeError.__init__(self, "A PDF must be loaded before calling this method.")
            

    class PDFWindow(wx.Panel):
        def __init__(self, *args, **kw):
            wx.Panel.__init__(self, *args, **kw)
            
            # Make a new class that derives from the WebBrowser class
            # in the COM module imported above.  This class also
            # derives from wxWindow and implements the machinery
            # needed to integrate the two worlds.
            _WebBrowserClass = MakeActiveXClass(_browserModule.WebBrowser)
            self.ie = _WebBrowserClass(self, -1)
            sizer = wx.BoxSizer()
            sizer.Add(self.ie, 1, wx.EXPAND)
            self.SetSizer(sizer)

            
        def LoadFile(self, fileName):
            if self.ie.Document:
                return self.ie.Document.LoadFile(fileName)
            else:
                self.ie.Navigate2(fileName)
                return True  # can we sense failure at this point?
            
        def GetVersions(self):
            if self.ie.Document:
                return self.ie.Document.GetVersions()
            else:
                raise PDFWindowError()
            
        def Print(self):
            if self.ie.Document:
                return self.ie.Document.Print()
            else:
                raise PDFWindowError()
            
        def goBackwardStack(self):
            if self.ie.Document:
                return self.ie.Document.goBackwardStack()
            else:
                raise PDFWindowError()

        def goForwardStack(self):
            if self.ie.Document:
                return self.ie.Document.goForwardStack()
            else:
                raise PDFWindowError()

        def gotoFirstPage(self):
            if self.ie.Document:
                return self.ie.Document.gotoFirstPage()
            else:
                raise PDFWindowError()

        def gotoLastPage(self):
            if self.ie.Document:
                return self.ie.Document.gotoLastPage()
            else:
                raise PDFWindowError()

        def gotoNextPage(self):
            if self.ie.Document:
                return self.ie.Document.gotoNextPage()
            else:
                raise PDFWindowError()

        def gotoPreviousPage(self):
            if self.ie.Document:
                return self.ie.Document.gotoPreviousPage()
            else:
                raise PDFWindowError()

        def printAll(self):
            if self.ie.Document:
                return self.ie.Document.printAll()
            else:
                raise PDFWindowError()

        def printAllFit(self, shrinkToFit):
            if self.ie.Document:
                return self.ie.Document.printAllFit()
            else:
                raise PDFWindowError()

        def printPages(self, from_, to):
            if self.ie.Document:
                return self.ie.Document.printPages()
            else:
                raise PDFWindowError()

        def printPagesFit(self, from_, to, shrinkToFit):
            if self.ie.Document:
                return self.ie.Document.printPagesFit()
            else:
                raise PDFWindowError()

        def printWithDialog(self):
            if self.ie.Document:
                return self.ie.Document.printWithDialog()
            else:
                raise PDFWindowError()

        def setCurrentHighlight(self, a, b, c, d):
            if self.ie.Document:
                return self.ie.Document.setCurrentHighlight()
            else:
                raise PDFWindowError()

        def setCurrentHightlight(self, a, b, c, d):
            if self.ie.Document:
                return self.ie.Document.setCurrentHightlight()
            else:
                raise PDFWindowError()

        def setCurrentPage(self, n):
            if self.ie.Document:
                return self.ie.Document.setCurrentPage()
            else:
                raise PDFWindowError()

        def setLayoutMode(self, layoutMode):
            if self.ie.Document:
                return self.ie.Document.setLayoutMode()
            else:
                raise PDFWindowError()

        def setNamedDest(self, namedDest):
            if self.ie.Document:
                return self.ie.Document.setNamedDest()
            else:
                raise PDFWindowError()

        def setPageMode(self, pageMode):
            if self.ie.Document:
                return self.ie.Document.setPageMode()
            else:
                raise PDFWindowError()

        def setShowScrollbars(self, On):
            if self.ie.Document:
                return self.ie.Document.setShowScrollbars()
            else:
                raise PDFWindowError()

        def setShowToolbar(self, On):
            if self.ie.Document:
                return self.ie.Document.setShowToolbar()
            else:
                raise PDFWindowError()

        def setView(self, viewMode):
            if self.ie.Document:
                return self.ie.Document.setView()
            else:
                raise PDFWindowError()

        def setViewRect(self, left, top, width, height):
            if self.ie.Document:
                return self.ie.Document.setViewRect()
            else:
                raise PDFWindowError()

        def setViewScroll(self, viewMode, offset):
            if self.ie.Document:
                return self.ie.Document.setViewScroll()
            else:
                raise PDFWindowError()

        def setZoom(self, percent):
            if self.ie.Document:
                return self.ie.Document.setZoom()
            else:
                raise PDFWindowError()

        def setZoomScroll(self, percent, left, top):
            if self.ie.Document:
                return self.ie.Document.setZoomScroll()
            else:
                raise PDFWindowError()

        

else:
    import wx.activex

    clsID = '{CA8A9780-280D-11CF-A24D-444553540000}'
    progID = 'AcroPDF.PDF.1'


    # Create eventTypes and event binders
    wxEVT_Error = wx.activex.RegisterActiveXEvent('OnError')
    wxEVT_Message = wx.activex.RegisterActiveXEvent('OnMessage')

    EVT_Error = wx.PyEventBinder(wxEVT_Error, 1)
    EVT_Message = wx.PyEventBinder(wxEVT_Message, 1)


    # Derive a new class from ActiveXWindow
    class PDFWindow(wx.activex.ActiveXWindow):
        def __init__(self, parent, ID=-1, pos=wx.DefaultPosition,
                     size=wx.DefaultSize, style=0, name='PDFWindow'):
            wx.activex.ActiveXWindow.__init__(self, parent,
                     wx.activex.CLSID('{CA8A9780-280D-11CF-A24D-444553540000}'),
                     ID, pos, size, style, name)

        # Methods exported by the ActiveX object
        def QueryInterface(self, riid):
            return self.CallAXMethod('QueryInterface', riid)

        def AddRef(self):
            return self.CallAXMethod('AddRef')

        def Release(self):
            return self.CallAXMethod('Release')

        def GetTypeInfoCount(self):
            return self.CallAXMethod('GetTypeInfoCount')

        def GetTypeInfo(self, itinfo, lcid):
            return self.CallAXMethod('GetTypeInfo', itinfo, lcid)

        def GetIDsOfNames(self, riid, rgszNames, cNames, lcid):
            return self.CallAXMethod('GetIDsOfNames', riid, rgszNames, cNames, lcid)

        def Invoke(self, dispidMember, riid, lcid, wFlags, pdispparams):
            return self.CallAXMethod('Invoke', dispidMember, riid, lcid, wFlags, pdispparams)

        def LoadFile(self, fileName):
            return self.CallAXMethod('LoadFile', fileName)

        def setShowToolbar(self, On):
            return self.CallAXMethod('setShowToolbar', On)

        def gotoFirstPage(self):
            return self.CallAXMethod('gotoFirstPage')

        def gotoLastPage(self):
            return self.CallAXMethod('gotoLastPage')

        def gotoNextPage(self):
            return self.CallAXMethod('gotoNextPage')

        def gotoPreviousPage(self):
            return self.CallAXMethod('gotoPreviousPage')

        def setCurrentPage(self, n):
            return self.CallAXMethod('setCurrentPage', n)

        def goForwardStack(self):
            return self.CallAXMethod('goForwardStack')

        def goBackwardStack(self):
            return self.CallAXMethod('goBackwardStack')

        def setPageMode(self, pageMode):
            return self.CallAXMethod('setPageMode', pageMode)

        def setLayoutMode(self, layoutMode):
            return self.CallAXMethod('setLayoutMode', layoutMode)

        def setNamedDest(self, namedDest):
            return self.CallAXMethod('setNamedDest', namedDest)

        def Print(self):
            return self.CallAXMethod('Print')

        def printWithDialog(self):
            return self.CallAXMethod('printWithDialog')

        def setZoom(self, percent):
            return self.CallAXMethod('setZoom', percent)

        def setZoomScroll(self, percent, left, top):
            return self.CallAXMethod('setZoomScroll', percent, left, top)

        def setView(self, viewMode):
            return self.CallAXMethod('setView', viewMode)

        def setViewScroll(self, viewMode, offset):
            return self.CallAXMethod('setViewScroll', viewMode, offset)

        def setViewRect(self, left, top, width, height):
            return self.CallAXMethod('setViewRect', left, top, width, height)

        def printPages(self, from_, to):
            return self.CallAXMethod('printPages', from_, to)

        def printPagesFit(self, from_, to, shrinkToFit):
            return self.CallAXMethod('printPagesFit', from_, to, shrinkToFit)

        def printAll(self):
            return self.CallAXMethod('printAll')

        def printAllFit(self, shrinkToFit):
            return self.CallAXMethod('printAllFit', shrinkToFit)

        def setShowScrollbars(self, On):
            return self.CallAXMethod('setShowScrollbars', On)

        def GetVersions(self):
            return self.CallAXMethod('GetVersions')

        def setCurrentHightlight(self, a, b, c, d):
            return self.CallAXMethod('setCurrentHightlight', a, b, c, d)

        def setCurrentHighlight(self, a, b, c, d):
            return self.CallAXMethod('setCurrentHighlight', a, b, c, d)

        def postMessage(self, strArray):
            return self.CallAXMethod('postMessage', strArray)

        # Getters, Setters and properties
        def _get_src(self):
            return self.GetAXProp('src')
        def _set_src(self, src):
            self.SetAXProp('src', src)
        src = property(_get_src, _set_src)

        def _get_messageHandler(self):
            return self.GetAXProp('messageHandler')
        def _set_messageHandler(self, messageHandler):
            self.SetAXProp('messageHandler', messageHandler)
        messagehandler = property(_get_messageHandler, _set_messageHandler)


#  PROPERTIES
#  --------------------
#  src
#      type:string  arg:string  canGet:True  canSet:True
#  
#  messagehandler
#      type:VT_VARIANT  arg:VT_VARIANT  canGet:True  canSet:True
#  
#  
#  
#  
#  METHODS
#  --------------------
#  QueryInterface
#      retType:  VT_VOID
#      params:
#          riid
#              in:True  out:False  optional:False  type:unsupported type 29
#          ppvObj
#              in:False  out:True  optional:False  type:unsupported type 26
#  
#  AddRef
#      retType:  int
#  
#  Release
#      retType:  int
#  
#  GetTypeInfoCount
#      retType:  VT_VOID
#      params:
#          pctinfo
#              in:False  out:True  optional:False  type:int
#  
#  GetTypeInfo
#      retType:  VT_VOID
#      params:
#          itinfo
#              in:True  out:False  optional:False  type:int
#          lcid
#              in:True  out:False  optional:False  type:int
#          pptinfo
#              in:False  out:True  optional:False  type:unsupported type 26
#  
#  GetIDsOfNames
#      retType:  VT_VOID
#      params:
#          riid
#              in:True  out:False  optional:False  type:unsupported type 29
#          rgszNames
#              in:True  out:False  optional:False  type:unsupported type 26
#          cNames
#              in:True  out:False  optional:False  type:int
#          lcid
#              in:True  out:False  optional:False  type:int
#          rgdispid
#              in:False  out:True  optional:False  type:int
#  
#  Invoke
#      retType:  VT_VOID
#      params:
#          dispidMember
#              in:True  out:False  optional:False  type:int
#          riid
#              in:True  out:False  optional:False  type:unsupported type 29
#          lcid
#              in:True  out:False  optional:False  type:int
#          wFlags
#              in:True  out:False  optional:False  type:int
#          pdispparams
#              in:True  out:False  optional:False  type:unsupported type 29
#          pvarResult
#              in:False  out:True  optional:False  type:VT_VARIANT
#          pexcepinfo
#              in:False  out:True  optional:False  type:unsupported type 29
#          puArgErr
#              in:False  out:True  optional:False  type:int
#  
#  LoadFile
#      retType:  bool
#      params:
#          fileName
#              in:True  out:False  optional:False  type:string
#  
#  setShowToolbar
#      retType:  VT_VOID
#      params:
#          On
#              in:True  out:False  optional:False  type:bool
#  
#  gotoFirstPage
#      retType:  VT_VOID
#  
#  gotoLastPage
#      retType:  VT_VOID
#  
#  gotoNextPage
#      retType:  VT_VOID
#  
#  gotoPreviousPage
#      retType:  VT_VOID
#  
#  setCurrentPage
#      retType:  VT_VOID
#      params:
#          n
#              in:True  out:False  optional:False  type:int
#  
#  goForwardStack
#      retType:  VT_VOID
#  
#  goBackwardStack
#      retType:  VT_VOID
#  
#  setPageMode
#      retType:  VT_VOID
#      params:
#          pageMode
#              in:True  out:False  optional:False  type:string
#  
#  setLayoutMode
#      retType:  VT_VOID
#      params:
#          layoutMode
#              in:True  out:False  optional:False  type:string
#  
#  setNamedDest
#      retType:  VT_VOID
#      params:
#          namedDest
#              in:True  out:False  optional:False  type:string
#  
#  Print
#      retType:  VT_VOID
#  
#  printWithDialog
#      retType:  VT_VOID
#  
#  setZoom
#      retType:  VT_VOID
#      params:
#          percent
#              in:True  out:False  optional:False  type:double
#  
#  setZoomScroll
#      retType:  VT_VOID
#      params:
#          percent
#              in:True  out:False  optional:False  type:double
#          left
#              in:True  out:False  optional:False  type:double
#          top
#              in:True  out:False  optional:False  type:double
#  
#  setView
#      retType:  VT_VOID
#      params:
#          viewMode
#              in:True  out:False  optional:False  type:string
#  
#  setViewScroll
#      retType:  VT_VOID
#      params:
#          viewMode
#              in:True  out:False  optional:False  type:string
#          offset
#              in:True  out:False  optional:False  type:double
#  
#  setViewRect
#      retType:  VT_VOID
#      params:
#          left
#              in:True  out:False  optional:False  type:double
#          top
#              in:True  out:False  optional:False  type:double
#          width
#              in:True  out:False  optional:False  type:double
#          height
#              in:True  out:False  optional:False  type:double
#  
#  printPages
#      retType:  VT_VOID
#      params:
#          from
#              in:True  out:False  optional:False  type:int
#          to
#              in:True  out:False  optional:False  type:int
#  
#  printPagesFit
#      retType:  VT_VOID
#      params:
#          from
#              in:True  out:False  optional:False  type:int
#          to
#              in:True  out:False  optional:False  type:int
#          shrinkToFit
#              in:True  out:False  optional:False  type:bool
#  
#  printAll
#      retType:  VT_VOID
#  
#  printAllFit
#      retType:  VT_VOID
#      params:
#          shrinkToFit
#              in:True  out:False  optional:False  type:bool
#  
#  setShowScrollbars
#      retType:  VT_VOID
#      params:
#          On
#              in:True  out:False  optional:False  type:bool
#  
#  GetVersions
#      retType:  VT_VARIANT
#  
#  setCurrentHightlight
#      retType:  VT_VOID
#      params:
#          a
#              in:True  out:False  optional:False  type:int
#          b
#              in:True  out:False  optional:False  type:int
#          c
#              in:True  out:False  optional:False  type:int
#          d
#              in:True  out:False  optional:False  type:int
#  
#  setCurrentHighlight
#      retType:  VT_VOID
#      params:
#          a
#              in:True  out:False  optional:False  type:int
#          b
#              in:True  out:False  optional:False  type:int
#          c
#              in:True  out:False  optional:False  type:int
#          d
#              in:True  out:False  optional:False  type:int
#  
#  postMessage
#      retType:  VT_VOID
#      params:
#          strArray
#              in:True  out:False  optional:False  type:VT_VARIANT
#  
#  
#  
#  
#  EVENTS
#  --------------------
#  Error
#      retType:  VT_VOID
#  
#  Message
#      retType:  VT_VOID
#  
#  
#  
#  
