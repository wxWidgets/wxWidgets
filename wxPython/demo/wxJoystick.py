
#----------------------------------------------------------------------------

from wxPython.wx import *
from joystick_wdr import *


class JoystickTestPanel(wxPanel):
    def __init__(self, parent, id,
        pos = wxPyDefaultPosition, size = wxPyDefaultSize,
        style = wxTAB_TRAVERSAL ):
        wxPanel.__init__(self, parent, id, pos, size, style)

        MakeJoystickTestPanel( self, True )

        try:
            self.stick = wxJoystick()
            self.stick.SetCapture(self)
            EVT_JOYSTICK_EVENTS(self, self.OnJoystick)
            self.UpdateFields()
        except NotImplementedError, v:
            wxMessageBox(str(v), "Exception Message")


    def UpdateFields(self):
        s = self.stick
        self.GetXPositionCtrl().SetValue(str(s.GetPosition().x))
        self.GetYPositionCtrl().SetValue(str(s.GetPosition().y))
        self.GetZPositionCtrl().SetValue(str(s.GetZPosition()))
        self.GetPovCtsPosCtrl().SetValue(str(s.GetPOVPosition()))
        self.GetRudderPosCtrl().SetValue(str(s.GetRudderPosition()))
        self.GetHasRudderCtrl().SetValue(str(s.HasRudder()))
        self.GetHasZCtrl().SetValue(str(s.HasZ()))
        self.GetHasPovCtrl().SetValue(str(s.HasPOV()))
        self.GetHasPov4dirCtrl().SetValue(str(s.HasPOV4Dir()))
        self.GetMfgIdCtrl().SetValue(str(s.GetManufacturerId()))
        self.GetProdNameCtrl().SetValue(str(s.GetProductName()))
        self.GetZMinCtrl().SetValue(str(s.GetZMin()))
        self.GetXMaxCtrl().SetValue(str(s.GetXMax()))
        self.GetNumButtonsCtrl().SetValue(str(s.GetNumberButtons()))
        self.GetNumAxesCtrl().SetValue(str(s.GetNumberAxes()))
        self.GetPollingMinCtrl().SetValue(str(s.GetPollingMin()))
        self.GetPollingMaxCtrl().SetValue(str(s.GetPollingMax()))
        self.GetUMinCtrl().SetValue(str(s.GetUMin()))
        self.GetUMaxCtrl().SetValue(str(s.GetUMax()))
        self.GetButtonStateCtrl().SetValue(str(s.GetButtonState()))
        self.GetPovPositionCtrl().SetValue(str(s.GetPOVPosition()))
        self.GetUPositionCtrl().SetValue(str(s.GetUPosition()))
        self.GetVPositionCtrl().SetValue(str(s.GetVPosition()))
        self.GetHasUCtrl().SetValue(str(s.HasU()))
        self.GetHasVCtrl().SetValue(str(s.HasV()))
        self.GetHasPovCtsCtrl().SetValue(str(s.HasPOVCTS()))
        self.GetNumSticksCtrl().SetValue(str(s.GetNumberJoysticks()))
        self.GetXMinCtrl().SetValue(str(s.GetXMin()))
        self.GetYMinCtrl().SetValue(str(s.GetYMin()))
        self.GetYMaxCtrl().SetValue(str(s.GetYMax()))
        self.GetZMaxCtrl().SetValue(str(s.GetZMax()))
        self.GetMaxButtonsCtrl().SetValue(str(s.GetMaxButtons()))
        self.GetMaxAxesCtrl().SetValue(str(s.GetMaxAxes()))
        self.GetRudderMinCtrl().SetValue(str(s.GetRudderMin()))
        self.GetRudderMaxCtrl().SetValue(str(s.GetRudderMax()))
        self.GetVMinCtrl().SetValue(str(s.GetVMin()))
        self.GetVMaxCtrl().SetValue(str(s.GetVMax()))


    def OnJoystick(self, evt):
        self.UpdateFields()


    # WDR: methods for JoystickTestPanel

    def GetYPositionCtrl(self):
        return self.FindWindowById(ID_Y_Position_Ctrl)

    def GetXPositionCtrl(self):
        return self.FindWindowById(ID_X_Position_Ctrl)


    def GetVMaxCtrl(self):
        return self.FindWindowById(ID_V_Max_Ctrl)

    def GetVMinCtrl(self):
        return self.FindWindowById(ID_V_Min_Ctrl)

    def GetRudderMaxCtrl(self):
        return self.FindWindowById(ID_Rudder_Max_Ctrl)

    def GetRudderMinCtrl(self):
        return self.FindWindowById(ID_Rudder_Min_Ctrl)

    def GetMaxAxesCtrl(self):
        return self.FindWindowById(ID_Max_Axes_Ctrl)

    def GetMaxButtonsCtrl(self):
        return self.FindWindowById(ID_Max_Buttons_Ctrl)

    def GetZMaxCtrl(self):
        return self.FindWindowById(ID_Z_Max_Ctrl)

    def GetYMaxCtrl(self):
        return self.FindWindowById(ID_Y_Max_Ctrl)

    def GetYMinCtrl(self):
        return self.FindWindowById(ID_Y_Min_Ctrl)

    def GetXMinCtrl(self):
        return self.FindWindowById(ID_X_Min_Ctrl)

    def GetNumSticksCtrl(self):
        return self.FindWindowById(ID_Num_Sticks_Ctrl)

    def GetHasPovCtsCtrl(self):
        return self.FindWindowById(ID_Has_POV_CTS_Ctrl)

    def GetHasVCtrl(self):
        return self.FindWindowById(ID_Has_V_Ctrl)

    def GetHasUCtrl(self):
        return self.FindWindowById(ID_Has_U_Ctrl)

    def GetVPositionCtrl(self):
        return self.FindWindowById(ID_V_Position_Ctrl)

    def GetUPositionCtrl(self):
        return self.FindWindowById(ID_U_Position_Ctrl)

    def GetPovPositionCtrl(self):
        return self.FindWindowById(ID_POV_Position_Ctrl)

    def GetButtonStateCtrl(self):
        return self.FindWindowById(ID_Button_State_Ctrl)

    def GetUMaxCtrl(self):
        return self.FindWindowById(ID_U_Max_Ctrl)

    def GetUMinCtrl(self):
        return self.FindWindowById(ID_U_Min_Ctrl)

    def GetPollingMaxCtrl(self):
        return self.FindWindowById(ID_Polling_Max_Ctrl)

    def GetPollingMinCtrl(self):
        return self.FindWindowById(ID_Polling_Min_Ctrl)

    def GetNumAxesCtrl(self):
        return self.FindWindowById(ID_Num_Axes_Ctrl)

    def GetNumButtonsCtrl(self):
        return self.FindWindowById(ID_Num_Buttons_Ctrl)

    def GetXMaxCtrl(self):
        return self.FindWindowById(ID_X_Max_Ctrl)

    def GetZMinCtrl(self):
        return self.FindWindowById(ID_Z_Min_Ctrl)

    def GetProdNameCtrl(self):
        return self.FindWindowById(ID_Prod_Name_Ctrl)

    def GetMfgIdCtrl(self):
        return self.FindWindowById(ID_Mfg_ID_Ctrl)

    def GetHasPov4dirCtrl(self):
        return self.FindWindowById(ID_Has_POV_4DIR_Ctrl)

    def GetHasPovCtrl(self):
        return self.FindWindowById(ID_Has_POV_Ctrl)

    def GetHasZCtrl(self):
        return self.FindWindowById(ID_Has_Z_Ctrl)

    def GetHasRudderCtrl(self):
        return self.FindWindowById(ID_Has_Rudder_Ctrl)

    def GetRudderPosCtrl(self):
        return self.FindWindowById(ID_Rudder_Pos_Ctrl)

    def GetPovCtsPosCtrl(self):
        return self.FindWindowById(ID_POV_CTS_Pos_Ctrl)

    def GetZPositionCtrl(self):
        return self.FindWindowById(ID_Z_Position_Ctrl)

    # WDR: handler implementations for JoysticktestPanel


#----------------------------------------------------------------------

def runTest(frame, nb, log):
    win = JoystickTestPanel(nb, -1)
    return win

#----------------------------------------------------------------------


overview = """\
"""
