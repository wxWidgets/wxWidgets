/////////////////////////////////////////////////////////////////////////////
// Name:        splash.pov
// Purpose:     POV-Ray scene used to generate splash image for wxWidgets
// Author:      Wlodzimierz ABX Skiba
// Modified by:
// Created:     04/08/2004
// Copyright:   (c) Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
            
#version 3.6;

// Rendering options : +FN +W638 +H478 +AM1 +A0.05 +R5

#include "colors.inc"
#include "rad_def.inc"
#include "screen.inc"
#include "shapes.inc"

global_settings {
    assumed_gamma 1.0
    radiosity {}
}

#local Location = <0,5,-100> ;
Set_Camera_Location(Location)
Set_Camera_Look_At(<0,0,0>)

background { rgb White }

light_source { 1000*y color White }
light_source { Location color White }

union{
  Center_Object( text { 
    ttf
    "crystal.ttf",
    ".     wxWidgets      ."
    .01, 0
    scale 20 translate 22*y
    pigment { color Black }
  } , x )
  Center_Object( text { 
    ttf
    "crystal.ttf",
    ".     Cross-Platform GUI Library   ."
    .01, 0
    scale 10 translate 10*y
    pigment { color Black }
  } , x )
  Center_Object( text { 
    ttf
    "crystal.ttf",
    ".      wxSplashScreen sample   ."
    .01, 0
    scale 2 translate 3 * y translate -z*84
    pigment { color Gray }
  } , x )
  plane { y 0 pigment { checker Black White } }
  rotate z*25 
}

#local Square = mesh { 
    triangle { <0,0,0> <0,1,0> <1,0,0> }
    triangle { <1,1,0> <0,1,0> <1,0,0> }
}

#macro Round_Cone3(PtA, RadiusA, PtB, RadiusB, UseMerge)
   #local Axis = vnormalize(PtB - PtA);
   #local Len = VDist(PtA, PtB);
   #local SA = atan2(RadiusB - RadiusA, Len);

   #local Pt_A = PtA + Axis*RadiusA;
   #local Pt_B = PtB - Axis*RadiusB;

   #if(UseMerge)
      merge {
   #else
      union {
   #end
      cone {Pt_A, RadiusA, Pt_B, RadiusB}
      sphere {Pt_A + Axis*tan(SA)*RadiusA, RadiusA/cos(SA)}
      sphere {Pt_B + Axis*tan(SA)*RadiusB, RadiusB/cos(SA)}
   }
#end

#local Line = object { 
    Round_Cone3_Union( <.15,.15,0>, .05, <.15,.9,0>, .05)
    pigment { color White }
    finish { ambient 1 diffuse 0 }
    scale <1,1,.01>
}

#macro Put_Square ( With_Pigment , At_Location , Order )
    #local Next_Square = union{ 
        object{ Square pigment { With_Pigment } }
        object{ Line }
        scale .15
    };
    Screen_Object (Next_Square, At_Location, 0, false, .1 + Order / 100 )
#end

Put_Square( pigment {color Red} , <0.65,0.1> , 3 )
Put_Square( pigment {color Blue} , <0.72,0.2> , 2 )
Put_Square( pigment {color Yellow} , <0.81,0.13> , 1 )


