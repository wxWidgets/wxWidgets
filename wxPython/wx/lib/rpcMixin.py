#
#  This was modified from rpcMixin.py distributed with wxPython
#
#----------------------------------------------------------------------
# Name:        rpcMixin
# Version:     0.2.0
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
# Copyright:   (c) 2000, 2001 by Greg Landrum and Rational Discovery LLC
# Licence:     wxWindows license
#----------------------------------------------------------------------
# 12/11/2003 - Jeff Grimmett (grimmtooth@softhome.net)
#
# o 2.5 compatability update.
# o xmlrpcserver not available.
#

"""provides xmlrpc server functionality for wxPython applications via a mixin class

**Some Notes:**

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

  4)  See the bottom of this file for an example of using the class.

**Obligatory disclaimer:**
  This is my first crack at both using xmlrpc and multi-threaded
  programming, so there could be huge horrible bugs or design
  flaws. If you see one, I'd love to hear about them.

"""


""" ChangeLog
23 May 2001:  Version bumped to 0.2.0
  Numerous code and design changes

21 Mar. 2001:  Version bumped to 0.1.4
  Updated rpcMixin.OnExternal to support methods with further references
   (i.e. now you can do rpcClient.foo.bar() and have it work)
  This probably ain't super legal in xmlrpc land, but it works just fine here
   and we need it.

6  Mar. 2001:  Version bumped to 0.1.3
  Documentation changes to make this compatible with happydoc

21 Jan. 2001:  Version bumped to 0.1.2
  OnExternal() method in the mixin class now uses getattr() to check if
    a desired method is present.  It should have been done this way in
    the first place.
14 Dec. 2000:  Version bumped to 0.1.1
  rearranged locking code and made other changes so that multiple
    servers in one application are possible.

"""

import  new
import  SocketServer
import  sys
import  threading
import  xmlrpclib
import  xmlrpcserver

import  wx

rpcPENDING = 0
rpcDONE = 1
rpcEXCEPT = 2

class RPCRequest:
  """A wrapper to use for handling requests and their responses"""
  status = rpcPENDING
  result = None

# here's the ID for external events
wxEVT_EXTERNAL_EVENT = wx.NewEventType()
EVT_EXTERNAL_EVENT = wx.PyEventBinder(wxEVT_EXTERNAL_EVENT, 0)

class ExternalEvent(wx.PyEvent):
  """The custom event class used to pass xmlrpc calls from
     the server thread into the GUI thread

  """
  def __init__(self,method,args):
    wx.PyEvent.__init__(self)
    self.SetEventType(wxEVT_EXTERNAL_EVENT)
    self.method = method
    self.args = args
    self.rpcStatus = RPCRequest()
    self.rpcStatusLock = threading.Lock()
    self.rpcCondVar = threading.Condition()

  def Destroy(self):
    self.method=None
    self.args=None
    self.rpcStatus = None
    self.rpcStatusLock = None
    self.rpcondVar = None

class Handler(xmlrpcserver.RequestHandler):
  """The handler class that the xmlrpcserver actually calls
     when a request comes in.

  """
  def log_message(self,*args):
    """ causes the server to stop spewing messages every time a request comes in

    """
    pass
  def call(self,method,params):
    """When an xmlrpc request comes in, this is the method that
       gets called.

       **Arguments**

         - method: name of the method to be called

         - params: arguments to that method

    """
    if method == '_rpcPing':
      # we just acknowledge these without processing them
      return 'ack'

    # construct the event
    evt = ExternalEvent(method,params)

    # update the status variable
    evt.rpcStatusLock.acquire()
    evt.rpcStatus.status = rpcPENDING
    evt.rpcStatusLock.release()

    evt.rpcCondVar.acquire()
    # dispatch the event to the GUI
    wx.PostEvent(self._app,evt)

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
      evt.Destroy()
      self._app = None
      return s

# this global Event is used to let the server thread
#  know when it should quit
stopEvent = threading.Event()
stopEvent.clear()

class _ServerThread(threading.Thread):
  """ this is the Thread class which actually runs the server

  """
  def __init__(self,server,verbose=0):
    self._xmlServ = server
    threading.Thread.__init__(self,verbose=verbose)

  def stop(self):
    stopEvent.set()

  def shouldStop(self):
    return stopEvent.isSet()

  def run(self):
    while not self.shouldStop():
      self._xmlServ.handle_request()
    self._xmlServ = None

class rpcMixin:
  """A mixin class to provide xmlrpc server functionality to wxPython
     frames/windows

     If you want to customize this, probably the best idea is to
     override the OnExternal method, which is what's invoked when an
     RPC is handled.

  """

  # we'll try a range of ports for the server, this is the size of the
  #  range to be scanned
  nPortsToTry=20
  if sys.platform == 'win32':
    defPort = 800
  else:
    defPort = 8023

  def __init__(self,host='',port=-1,verbose=0,portScan=1):
    """Constructor

      **Arguments**

        - host: (optional) the hostname for the server

        - port: (optional) the port the server will use

        - verbose: (optional) if set, the server thread will be launched
          in verbose mode

        - portScan: (optional) if set, we'll scan across a number of ports
          to find one which is avaiable

    """
    if port == -1:
      port = self.defPort
    self.verbose=verbose
    self.Bind(EVT_EXTERNAL_EVENT,self.OnExternal)
    if hasattr(self,'OnClose'):
      self._origOnClose = self.OnClose
      self.Disconnect(-1,-1,wx.EVT_CLOSE_WINDOW)
    else:
      self._origOnClose = None
    self.OnClose = self.RPCOnClose
    self.Bind(wx.EVT_CLOSE,self.RPCOnClose)

    tClass = new.classobj('Handler%d'%(port),(Handler,),{})
    tClass._app = self
    if portScan:
      self.rpcPort = -1
      for i in xrange(self.nPortsToTry):
        try:
          xmlServ = SocketServer.TCPServer((host,port+i),tClass)
        except:
          pass
        else:
          self.rpcPort = port+i
    else:
      self.rpcPort = port
      try:
        xmlServ = SocketServer.TCPServer((host,port),tClass)
      except:
        self.rpcPort = -1

    if self.rpcPort == -1:
      raise 'RPCMixinError','Cannot initialize server'
    self.servThread = _ServerThread(xmlServ,verbose=self.verbose)
    self.servThread.setName('XML-RPC Server')
    self.servThread.start()

  def RPCOnClose(self,event):
    """ callback for when the application is closed

       be sure to shutdown the server and the server thread before
       leaving

    """
    # by setting the global stopEvent we inform the server thread
    # that it's time to shut down.
    stopEvent.set()
    if event is not None:
      # if we came in here from a user event (as opposed to an RPC event),
      #  then we'll need to kick the server one last time in order
      #  to get that thread to terminate.  do so now
      s1 = xmlrpclib.Server('http://localhost:%d'%(self.rpcPort))
      try:
        s1._rpcPing()
      except:
        pass

    if self._origOnClose is not None:
      self._origOnClose(event)

  def RPCQuit(self):
    """ shuts down everything, including the rpc server

    """
    self.RPCOnClose(None)
  def OnExternal(self,event):
    """ this is the callback used to handle RPCs

      **Arguments**

        - event: an _ExternalEvent_ sent by the rpc server

      Exceptions are caught and returned in the global _rpcStatus
      structure.  This allows the xmlrpc server to report the
      exception to the client without mucking up any of the delicate
      thread stuff.

    """
    event.rpcStatusLock.acquire()
    doQuit = 0
    try:
      methsplit = event.method.split('.')
      meth = self
      for piece in methsplit:
        meth = getattr(meth,piece)
    except AttributeError,msg:
      event.rpcStatus.result = 'No Such Method',msg
      event.rpcStatus.status = rpcEXCEPT
    else:
      try:
        res = apply(meth,event.args)
      except:
        import traceback
        if self.verbose: traceback.print_exc()
        event.rpcStatus.result = sys.exc_info()[:2]
        event.rpcStatus.status = rpcEXCEPT
      else:
        if res is None:
          # returning None across the xmlrpc interface is problematic
          event.rpcStatus.result = []
        else:
          event.rpcStatus.result = res
        event.rpcStatus.status = rpcDONE

    event.rpcStatusLock.release()

    # broadcast (using the condition var) that we're done with the event
    event.rpcCondVar.acquire()
    event.rpcCondVar.notify()
    event.rpcCondVar.release()


if __name__ == '__main__':
  import time
  if sys.platform == 'win32':
    port = 800
  else:
    port = 8023

  class rpcFrame(wx.Frame,rpcMixin):
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
      if kwargs.has_key('rpcPortScan'):
        mixinArgs['portScan'] = kwargs['rpcPortScan']
        del kwargs['rpcPortScan']

      apply(wx.Frame.__init__,(self,)+args,kwargs)
      apply(rpcMixin.__init__,(self,),mixinArgs)

      self.Bind(wx.EVT_CHAR,self.OnChar)

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



  class MyApp(wx.App):
    def OnInit(self):
      self.frame = rpcFrame(None, -1, "wxPython RPCDemo", wx.DefaultPosition,
                            (300,300), rpcHost='localhost',rpcPort=port)
      self.frame.Show(True)
      return True


  def testcon(port):
    s1 = xmlrpclib.Server('http://localhost:%d'%(port))
    s1.SetTitle('Munged')
    s1._rpcPing()
    if doQuit:
      s1.RPCQuit()

  doQuit = 1
  if len(sys.argv)>1 and sys.argv[1] == '-q':
    doQuit = 0
  nT = threading.activeCount()
  app = MyApp(0)
  activePort = app.frame.rpcPort
  t = threading.Thread(target=lambda x=activePort:testcon(x),verbose=0)
  t.start()

  app.MainLoop()
  # give the threads time to shut down
  if threading.activeCount() > nT:
    print 'waiting for all threads to terminate'
    while threading.activeCount() > nT:
      time.sleep(0.5)


