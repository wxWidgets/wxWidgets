

from wxPython.wx import *

def t():
    r1 = wxRect(0,0, 25, 500)
    r2 = wxRect(10, 10, 50, 100)

    r = wxIntersectRect(r1, r2)
    print r

    r = wxIntersectRect(r1, (50, 10, 50, 100))
    print r





class MyApp(wxApp):
    def OnInit(self):
        t()
        return false

app = MyApp(0)

