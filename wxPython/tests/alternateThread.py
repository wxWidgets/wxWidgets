from time import sleep
from threading import Thread

def test_a_window():
    print "starting window thread"

    from wxPython.wx import *    # <-- the wxWin DLL is not loaded until here

    app = wxPySimpleApp(1)
    frame = wxFrame(None, -1, "Hello", size=(400,200))
    frame.Show(true)
    EVT_SIZE(frame, OnFrameSize)
    app.MainLoop()
    print "finishing window thread"

def OnFrameSize(evt):
    print evt.GetSize()



keep_going = 1
def counter():
    print "starting counter thread"
    count = 0
    while keep_going:
        sleep(1)
        count += 1
        print count
    print "finishing counter thread"

def main():
    print "main startup"

    ct = Thread(target=counter)
    wt = Thread(target=test_a_window)

    ct.start()
    wt.start()
    wt.join()

    global keep_going
    keep_going = 0

    ct.join()

    print "main finished"

main()




