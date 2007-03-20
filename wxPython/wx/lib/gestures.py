#Mouse Gestures

#Version 0.0.1

#By Daniel Pozmanter
#drpython@bluebottle.com

#Released under the terms of the wxWindows License.

"""
This is a class to add Mouse Gestures to a program.
It can be used in two ways:

1.  Automatic:
    Automatically runs mouse gestures.
    You need to set the gestures, and their associated actions,
    as well as the Mouse Button/Modifiers to use.

2.  Manual:
    Same as above, but you do not need to set the mouse button/modifiers.
    You can launch this from events as you wish.

An example is provided in the demo.
The parent window is where the mouse events will be recorded.
(So if you want to record them in a pop up window, use manual mode,
and set the pop up as the parent).

Start() starts recording mouse movement.
End() stops the recording, compiles all the gestures into a list,
and looks through the registered gestures to find a match.
The first matchs associated    action is then run.

The marginoferror is how much to forgive when calculating movement:
If the margin is 25, then movement less than 25 pixels will not be detected.

Recognized:  L, R, U, D, 1, 3, 7, 9

Styles:  Manual (Automatic By Default), DisplayNumbersForDiagonals (Off By Default).
Not Yet Implemented

The criteria for a direction is as follows:
x in a row.  (Where x is the WobbleTolerance).
So if the WobbleTolerance is 9
'URUUUUUUUUUUUUUUURUURUUUU1' is Up.

The higher this number, the less sensitive this class is.
So the more likely something like 1L will translate to 1.

This is good, since the mouse does tend to wobble somewhat,
and a higher number allows for this.

To change this, use SetWobbleTolerance

Also, to help with recognition of a diagonal versus
a vey messy straight line, if the greater absolute value
is not greater than twice the lesser, only the grater value
is counted.

In automatic mode, EVT_MOUSE_EVENTS is used.
This allows the user to change the mouse button/modifiers at runtime.
"""

###########################################

'''
Changelog:
0.0.1:  Treats a mouse leaving event as mouse up.
        (Bug Report, Thanks Peter Damoc).
        
    
0.0.0:  Initial Release.
'''

###########################################
#ToDo:

#Fully Implement Manual Mode

#Add "Ends With":  AddGestureEndsWith(self, gesture, action, args)
#Add "Starts With":  AddGestuteStartsWith(self, gesture, action, args)

#For better control of when the gesture starts and stops,
#use manual mode.
#At the moment, you need to Bind the OnMouseMotion event if you want to use
#manual mode.

import wx

class MouseGestures:
    def __init__(self, parent, Auto=True, MouseButton=wx.MOUSE_BTN_MIDDLE):
        self.parent = parent
                
        self.gestures = []
        self.actions = []
        self.actionarguments = []
        
        self.mousebutton = MouseButton
        self.modifiers = []
        
        self.recording = False
        
        self.lastposition = (-1, -1)
        
        self.pen = wx.Pen(wx.Colour(0, 144, 255), 5)

        self.dc = wx.ScreenDC()
        self.dc.SetPen(self.pen)
        
        self.showgesture = False
        
        self.wobbletolerance = 7
                
        self.rawgesture = ''
        
        self.SetAuto(Auto)
    
    def _check_modifiers(self, event):
        '''Internal:  Returns True if all needed modifiers are down
        for the given event.'''
        if len(self.modifiers) > 0:
            good = True
            if wx.WXK_CONTROL in self.modifiers:
                good = good and event.ControlDown()
            if wx.WXK_SHIFT in self.modifiers:
                good = good and event.ShiftDown()
            if wx.WXK_ALT in self.modifiers:
                good = good and event.AltDown()
            return good
        return True
            
    def AddGesture(self, gesture, action, *args):
        '''Registers a gesture, and an associated function, with any arguments needed.'''
        #Make Sure not a duplicate:
        self.RemoveGesture(gesture)
        
        self.gestures.append(gesture)
        self.actions.append(action)
        self.actionarguments.append(args)

    def DoAction(self, gesture):
        '''If the gesture is in the array of registered gestures, run the associated function.'''
        if gesture in self.gestures:
            i = self.gestures.index(gesture)
            apply(self.actions[i], self.actionarguments[i])

    def End(self):
        '''Stops recording the points to create the mouse gesture from,
        and creates the mouse gesture, returns the result as a string.'''
        self.recording = False
        
        #Figure out the gestures (Look for occurances of 5 in a row or more):
            
        tempstring = '0'
        possiblechange = '0'
        
        directions = ''
        
        for g in self.rawgesture:
            l = len(tempstring)
            if g != tempstring[l - 1]:
                if g == possiblechange:
                    tempstring = g + g
                else:
                    possiblechange = g
            else:
                tempstring += g
            if len(tempstring) >= self.wobbletolerance:
                ld = len(directions)
                if ld > 0:
                    if directions[ld - 1] != g:
                        directions += g
                else:
                    directions += g
                tempstring = '0'
        
        if self.showgesture:
            self.parent.Refresh()
                
        return directions
    
    def GetDirection(self, point1, point2):
        '''Gets the direction between two points.'''
        #point1 is the old point
        #point2 is current
                
        x1, y1 = point1
        x2, y2 = point2
                
        #(Negative = Left, Up)
        #(Positive = Right, Down)
        
        horizontal = x2 - x1
        vertical = y2 - y1
                
        horizontalchange = abs(horizontal) > 0
        verticalchange = abs(vertical) > 0
                
        if horizontalchange and verticalchange:
            ah = abs(horizontal)
            av = abs(vertical)
            if ah > av:
                if (ah / av) > 2:
                    vertical = 0
                    verticalchange = False
            elif av > ah:
                if (av / ah) > 2:
                    horizontal = 0
                    horizontalchange = False
        
        if horizontalchange and verticalchange:
            #Diagonal
            if (horizontal > 0) and (vertical > 0):
                return '3'
            elif (horizontal > 0) and (vertical < 0):
                return '9'
            elif (horizontal < 0) and (vertical > 0):
                return '1'
            else:
                return '7'
        else:
            #Straight Line
            if horizontalchange:
                if horizontal > 0:
                    return 'R'
                else:
                    return 'L'
            else:
                if vertical > 0:
                    return 'D'
                else:
                    return 'U'
    
    def GetRecording(self):
        '''Returns whether or not Gesture Recording has started.'''
        return self.recording
        
    def OnMotion(self, event):
        '''Internal.  Used if Start() has been run'''
        if self.recording:
            currentposition = event.GetPosition()
            if self.lastposition != (-1, -1):
                self.rawgesture += self.GetDirection(self.lastposition, currentposition)
                if self.showgesture:
                    #Draw it!
                    px1, py1 = self.parent.ClientToScreen(self.lastposition)
                    px2, py2 = self.parent.ClientToScreen(currentposition)
                    self.dc.DrawLine(px1, py1, px2, py2)
                
            self.lastposition = currentposition
                        
        event.Skip()
    
    def OnMouseEvent(self, event):
        '''Internal.  Used in Auto Mode.'''
        if event.ButtonDown(self.mousebutton) and self._check_modifiers(event):
            self.Start()
        elif (event.ButtonUp(self.mousebutton) or event.Leaving()) and self.GetRecording():
            result = self.End()
            self.DoAction(result)
        event.Skip()
                    
    def RemoveGesture(self, gesture):
        '''Removes a gesture, and its associated action'''
        if gesture in self.gestures:
            i = self.gestures.index(gesture)
            
            del self.gestures[i]
            del self.actions[i]
            del self.actionarguments[i]
    
    def SetAuto(self, auto):
        '''Warning:  Once auto is set, it stays set, unless you manually use UnBind'''
        if auto:
            self.parent.Bind(wx.EVT_MOUSE_EVENTS, self.OnMouseEvent)
            self.parent.Bind(wx.EVT_MOTION, self.OnMotion)
    
    def SetGesturePen(self, pen):
        '''Sets the wx pen used to visually represent each gesture'''
        self.pen = pen
        self.dc.SetPen(self.pen)
    
    def SetGesturePen(self, colour, width):
        '''Sets the colour and width of the line drawn to visually represent each gesture'''
        self.pen = wx.Pen(colour, width)
        self.dc.SetPen(self.pen)
    
    def SetGesturesVisible(self, vis):
        '''Sets whether a line is drawn to visually represent each gesture'''
        self.showgesture = vis
        
    def SetModifiers(self, modifiers=[]):
        '''Takes an array of wx Key constants (Control, Shift, and/or Alt).
        Leave empty to unset all modifiers.'''
        self.modifiers = modifiers
    
    def SetMouseButton(self, mousebutton):
        '''Takes the wx constant for the target mousebutton'''
        self.mousebutton = mousebutton
    
    def SetWobbleTolerance(self, wobbletolerance):
        '''Sets just how much wobble this class can take!'''
        self.WobbleTolerance = wobbletolerance
        
    def Start(self):
        '''Starts recording the points to create the mouse gesture from'''
        self.recording = True
        self.rawgesture = ''
        self.lastposition = (-1, -1)
        if self.showgesture:
            self.parent.Refresh()
