#Mouse Gestures

#Version 0.0.0 ALPHA

#By Daniel Pozmanter
#drpython@bluebottle.com

#Released under the terms of the wxWindows License.

#This is a class to add Mouse Gestures to a program.
#It can be used in two ways:
#	
#1.  Automatic:
#	Automatically runs mouse gestures.
#	You need to set the gestures, and their associated actions,
#	as well as the Mouse Button/Modifiers to use.
#
#	(Mouse Buttons are set in init)
#
#2.  Manual:
#	Same as above, but you do not need to set the mouse button/modifiers.
#	You can launch this from events as you wish.
#
#An example is provided in the demo.
#The parent window is where the mouse events will be recorded.
#(So if you want to record them in a pop up window, use manual mode,
#and set the pop up as the parent).
#
#Start() starts recording mouse movement.
#End() stops the recording, compiles all the gestures into a list,
#and looks through the registered gestures to find a match.
#The first matchs associated	action is then run.

#The marginoferror is how much to forgive when calculating movement:
#If the margin is 25, then movement less than 25 pixels will not be detected.

#Recognized:  L, R, U, D, 1, 3, 7, 9

#Styles:  Manual (Automatic By Default), DisplayNumbersForDiagonals (Off By Default).
#Not Yet Implemented

#The criteria for a direction is as follows:
#x in a row.  (Where x is the WobbleTolerance).
#So if the WobbleTolerance is 9
# 'URUUUUUUUUUUUUUUURUURUUUU1' is Up.

#The higher this number, the less sensitive this class is.
#So the more likely something like 1L will translate to 1.

#This is good, since the mouse does tend to wobble somewhat,
#and a higher number allows for this.

#To change this, use SetWobbleTolerance

#Also, to help with recognition of a diagonal versus
#a vey messy straight line, if the greater absolute value
#is not greater than twice the lesser, only the grater value
#is counted.

#ToDo:

#Add in modifier code (Ctrl, Alt, etc).

#SetGestureLine(wx.Colour(), int width)

#Add "Ends With":  AddGestureEndsWith(self, gesture, action, args)
#Add "Starts With":  AddGestuteStartsWith(self, gesture, action, args)

#At the moment, the mouse button can only be set at startup.
#I could use UnBind, but this may limit the wxPython version being used,
#and, what if the user has other events bound?
#So I think, if the user wants to change the mouse button at runtime,
#the best solution is to use manual mode.


import wx

class MouseGestures:
    def __init__(self, parent, Auto=True, MouseButton=wx.MOUSE_BTN_MIDDLE):
        self.parent = parent
        
        self.gestures = []
        self.actions = []
        self.actionarguments = []
        
        self.mousebutton = MouseButton
        self.modifiers = 0
        
        self.recording = False
        
        self.lastposition = (-1, -1)
        
        self.pen = wx.Pen(wx.Colour(0, 144, 255), 5)

        self.dc = wx.ScreenDC()
        self.dc.SetPen(self.pen)
        
        self.showgesture = False
        
        self.wobbletolerance = 7
                
        self.rawgesture = ''
        
        self.SetAuto(Auto)
    
    def Action(self, gesture):
        if gesture in self.gestures:
            i = self.gestures.index(gesture)
            apply(self.actions[i], self.actionarguments[i])
    
    def AddGesture(self, gesture, action, *args):
        #Make Sure not a duplicate:
        self.RemoveGesture(gesture)
        
        self.gestures.append(gesture)
        self.actions.append(action)
        self.actionarguments.append(args)

    def End(self):
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
                
        return directions
    
    def GetDirection(self, point1, point2):
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
                
    
    def OnEnd(self, event):
        result = self.End()  
                        
        self.Action(result)
                
        event.Skip()
    
    def OnMotion(self, event):
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
    
    def OnStart(self, event):
        self.Start()
        event.Skip()
                    
    def RemoveGesture(self, gesture):
        if gesture in self.gestures:
            i = self.gestures.index(gesture)
            del self.gestures[i]
            del self.actions[i]
            del self.actionarguments[i]
    
    def SetAuto(self, auto):
        #I was not sure about making this part of init, so I left it as its own method for now.
        if auto:
            if self.mousebutton == wx.MOUSE_BTN_LEFT:
                self.parent.Bind(wx.EVT_LEFT_DOWN, self.OnStart)
                self.parent.Bind(wx.EVT_LEFT_UP, self.OnEnd)
            elif self.mousebutton == wx.MOUSE_BTN_MIDDLE:
                self.parent.Bind(wx.EVT_MIDDLE_DOWN, self.OnStart)
                self.parent.Bind(wx.EVT_MIDDLE_UP, self.OnEnd)
            elif self.mousebutton == wx.MOUSE_BTN_RIGHT:
                self.parent.Bind(wx.EVT_RIGHT_DOWN, self.OnStart)
                self.parent.Bind(wx.EVT_RIGHT_UP, self.OnEnd)
            self.parent.Bind(wx.EVT_MOTION, self.OnMotion)
    
    def SetGesturesVisible(self, vis):
        self.showgesture = vis
        
    def SetModifiers(self, modifers):
        self.modifiers = modifiers
    
    def SetWobbleTolerance(self, wobbletolerance):
        self.WobbleTolerance = wobbletolerance
        
    def Start(self):
        self.recording = True
        self.rawgesture = '' 
        self.lastposition = (-1, -1)
        if self.showgesture:     
            self.parent.Refresh()