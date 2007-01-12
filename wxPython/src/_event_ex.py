

#---------------------------------------------------------------------------

class PyEventBinder(object):
    """
    Instances of this class are used to bind specific events to event
    handlers.
    """
    def __init__(self, evtType, expectedIDs=0):
        if expectedIDs not in [0, 1, 2]:
            raise ValueError, "Invalid number of expectedIDs"
        self.expectedIDs = expectedIDs

        if type(evtType) == list or type(evtType) == tuple:
            self.evtType = evtType
        else:
            self.evtType = [evtType]


    def Bind(self, target, id1, id2, function):
        """Bind this set of event types to target."""
        for et in self.evtType:
            target.Connect(id1, id2, et, function)


    def Unbind(self, target, id1, id2):
        """Remove an event binding."""
        success = 0
        for et in self.evtType:
            success += target.Disconnect(id1, id2, et)
        return success != 0

    def _getEvtType(self):
        """
        Make it easy to get to the default wxEventType typeID for this
        event binder.
        """
        return self.evtType[0]
    
    typeId = property(_getEvtType)

    
    def __call__(self, *args):
        """
        For backwards compatibility with the old EVT_* functions.
        Should be called with either (window, func), (window, ID,
        func) or (window, ID1, ID2, func) parameters depending on the
        type of the event.
        """
        assert len(args) == 2 + self.expectedIDs
        id1 = wx.ID_ANY
        id2 = wx.ID_ANY
        target = args[0]
        if self.expectedIDs == 0:
            func = args[1]
        elif self.expectedIDs == 1:
            id1 = args[1]
            func = args[2]
        elif self.expectedIDs == 2:
            id1 = args[1]
            id2 = args[2]
            func = args[3]
        else:
            raise ValueError, "Unexpected number of IDs"

        self.Bind(target, id1, id2, func)


# These two are square pegs that don't fit the PyEventBinder hole...
def EVT_COMMAND(win, id, cmd, func):
    win.Connect(id, -1, cmd, func)
def EVT_COMMAND_RANGE(win, id1, id2, cmd, func):
    win.Connect(id1, id2, cmd, func)

    
#---------------------------------------------------------------------------
