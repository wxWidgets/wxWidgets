#----------------------------------------------------------------------
# Name:        rpcMixin
# Version:     0.1
# Purpose:     provides xmlrpc server functionality for wxPython
#              applications via a mixin class
#
# Requires:    (1) Python with threading enabled.
#              (2) xmlrpclib from PythonWare
#                  (http://www.pythonware.com/products/xmlrpc/)
#                  the code was developed and tested using version 0.9.8
#
# Author:      greg Landrum (Landrum@RationalDiscovery.com)
#
# Copyright:   (c) 2000 by Greg Landrum and Rational Discovery LLC
# Licence:     wxWindows license
#----------------------------------------------------------------------

"""
Some Notes:

1)  The xmlrpc server runs in a separate thread from the main GUI
    application, communication between the two threads using a custom
    event (see the Threads demo in the wxPython docs for more info).

2)  Neither the server nor the client are particularly smart about
    checking method names.  So it's easy to shoot yourself in the foot
    by calling improper methods.  It would be pretty easy to add
    either a list of allowed methods or a list of forbidden methods.

3)  Authentication of xmlrpc clients is *not* performed.  I think it
    would be pretty easy to do this in a hacky way, but I haven't done
    it yet.

4)  The default port number is 800, it's a windows thing... at least
    it seems like a windows thing to me.  Since I'm not being smart
    about port numbers, you can probably hork yourself arbitrarily by
    firing up more than one xmlrpc-active frame at the same time, but
    I haven't tried that.

5)  See the bottom of this file for an example of using the class.

Obligatory disclaimer:
  This is my first crack at both using xmlrpc and multi-threaded
  programming, so there could be huge horrible bugs or design
  flaws. If you see one, I'd love to hear about them.

"""

from wxPython.wx import *
import xmlrpcserver
import Threading
import SocketServer

rpcPENDING = 0
rpcDONE = 1
rpcEXCEPT = 2
class RPCRequest:
  """A wrapper to use for handling requests and their responses"""
  status = rpcPENDING
  result = None

# here's the ID for external events
wxEVT_EXTERNAL_EVENT = 25015
class ExternalEvent(wxPyEvent):
  """The custom event class used to pass xmlrpc calls from
     the server thread into the GUI thread
  """
  def __init__(self,method,args):
    wxPyEvent.__init__(self)
    self.SetEventType(wxEVT_EXTERNAL_EVENT)
    self.method = method
    self.args = args
    self.rpcStatus = RPCRequest()
    self.rpcStatusLock = Threading.Lock()
    self.rpcCondVar = Threading.Condition()

def EVT_EXTERNAL_EVENT(win,func):
  win.Connect(-1,-1,wxEVT_EXTERNAL_EVENT,func)

class Handler(xmlrpcserver.RequestHandler):
  """The handler class that the xmlrpcserver actually calls
     when a request comes in.
  """
  def call(self,method,params):
    """When an xmlrpc request comes in, this is the method that
       gets called.
    """
    # construct the event
    evt = ExternalEvent(method,params)

    # update the status variable
    evt.rpcStatusLock.acquire()
    evt.rpcStatus.status = rpcPENDING
    evt.rpcStatusLock.release()

    # acquire the condition lock
    evt.rpcCondVar.acquire()
    # dispatch the event to the GUI
    wxPostEvent(self._app,evt)
    # wait for the GUI to finish
    while evt.rpcStatus.status == rpcPENDING:
      evt.rpcCondVar.wait()
    evt.rpcCondVar.release()
    evt.rpcStatusLock.acquire()
    if evt.rpcStatus.status == rpcEXCEPT:
      # The GUI threw an exception, release the status lock
      #  and re-raise the exception
      evt.rpcStatusLock.release()
      raise evt.rpcStatus.result[0],evt.rpcStatus.result[1]
    else:
      # everything went through without problems
      s = evt.rpcStatus.result
      evt.rpcStatusLock.release()
      return s

class rpcMixin:
  """A mixin class to provide xmlrpc server functionality to wxPython
     frames/windows

     If you want to customize this, probably the best idea is to
     override the OnExternal method, which is what's invoked when an
     RPC is handled.

  """
  def __init__(self,host='',port=800):
    """
    Arguments:
      host: (optional) the hostname for the server
      port: (optional) the port the server will use
    """
    EVT_EXTERNAL_EVENT(self,self.OnExternal)
    if hasattr(self,'OnClose'):
      self._origOnClose = self.OnClose
    else:
      self._origOnClose = None
    EVT_CLOSE(self,self.OnClose)

    exec('class Handler%d(Handler): pass'%(port))
    exec('tClass= Handler%d'%(port))
    tClass._app = self
    self._xmlServ = SocketServer.TCPServer((host,port),tClass)
    self.servThread = Threading.Thread(target=self._xmlServ.serve_forever)
    self.servThread.setDaemon(1)
    self.servThread.start()

  def OnClose(self,event):
    """ be sure to shutdown the server and the server thread before
        leaving
    """
    self._xmlServ = None
    self.servThread = None
    if self._origOnClose is not None:
      self._origOnClose(event)

  def OnExternal(self,event):
    """ this is the callback used to handle RPCs

      Exceptions are caught and returned in the global _rpcStatus
      structure.  This allows the xmlrpc server to report the
      exception to the client without mucking up any of the delicate
      thread stuff.
    """
    event.rpcStatusLock.acquire()
    try:
      res = eval('apply(self.%s,event.args)'%event.method)
    except:
      import sys,traceback
      traceback.print_exc()
      event.rpcStatus.result = sys.exc_info()[:2]
      event.rpcStatus.status = rpcEXCEPT
    else:
      if res is None:
        event.rpcStatus.result = []
      else:
        event.rpcStatus.result = res
      event.rpcStatus.status = rpcDONE
    event.rpcStatusLock.release()
    event.rpcCondVar.acquire()
    event.rpcCondVar.notify()
    event.rpcCondVar.release()

if __name__ == '__main__':
  import sys
  port = 800
  if len(sys.argv)>1:
    port = int(sys.argv[1])

  class rpcFrame(wxFrame,rpcMixin):
    """A simple wxFrame with the rpcMixin functionality added
    """
    def __init__(self,*args,**kwargs):
      """ rpcHost or rpcPort keyword arguments will be passed along to
          the xmlrpc server.
      """
      mixinArgs = {}
      if kwargs.has_key('rpcHost'):
        mixinArgs['host'] = kwargs['rpcHost']
        del kwargs['rpcHost']
      if kwargs.has_key('rpcPort'):
        mixinArgs['port'] = kwargs['rpcPort']
        del kwargs['rpcPort']

      apply(wxFrame.__init__,(self,)+args,kwargs)
      apply(rpcMixin.__init__,(self,),mixinArgs)

      EVT_CHAR(self,self.OnChar)

    def TestFunc(self,args):
      """a demo method"""
      return args

    def OnChar(self,event):
      key = event.GetKeyCode()
      if key == ord('q'):
        self.OnQuit(event)

    def OnQuit(self,event):
      self.OnClose(event)

    def OnClose(self,event):
      self.Destroy()

  class MyApp(wxApp):
    def OnInit(self):
      frame = rpcFrame(NULL, -1, "wxPython RPCDemo", wxDefaultPosition, wxSize(300,300),rpcHost='localhost',rpcPort=port)
      frame.Show(TRUE)
      import time

      #self.SetTopWindow(frame)
      frame2 = rpcFrame(NULL, -1, "wxPython RPCDemo2", wxDefaultPosition, wxSize(300,300),rpcHost='localhost',rpcPort=port+1)
      frame2.Show(TRUE)

      return TRUE
  app = MyApp(0)
  app.MainLoop()

