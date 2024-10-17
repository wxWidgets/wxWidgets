/////////////////////////////////////////////////////////////////////////////
// Name:        press.pov
// Purpose:     POV-Ray scene used to generate clip for splash
// Author:      Wlodzimierz ABX Skiba
// Created:     24/11/2004
// Copyright:   (c) Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////
            
#version 3.6;

// Rendering options : +FT +W80 +H60 +AM1 +A0.05 +R5 +J0 +KFF500
// Produced TGA images converted to MPG with good old (1993) CMPEG tool
// Conversion : cmpeg.exe -v1 ipb.ctl frames.lst press.mpg

#include "colors.inc"
#include "rad_def.inc"

global_settings {
    assumed_gamma 1.0
    max_trace_level 100
}

background { colour White }

#declare Texts = array[ 3 ];

#declare Texts[0] = "PRESS";
#declare Texts[1] = "ANY";
#declare Texts[2] = "KEY";

camera{ orthographic look_at .5 location .5-z right 1.05*x up 1.05*y }

#declare Items = dimension_size( Texts , 1 );
 
#declare Objects = array[ Items + 1 ];

#declare f_line = function(x,xa,ya,xb,yb){((yb-ya)/(xb-xa))*(x-xa)+ya};

#declare Counter = 0;
#while ( Counter <= Items )
    #if ( Counter < Items )
        #declare Object = text{ ttf "crystal.ttf" Texts[ Counter ] 1 0 };
    #else
        #declare Object = Objects[ Items ];
    #end

    #declare M = max_extent( Object );
    #declare m = min_extent( Object );
    #declare S = M - m;
    #declare Objects[ Counter ] = object{ Object translate -m + z*Counter scale <1/S.x,1/S.y,1> };
    #declare Objects[ Items ] = 
        #if ( Counter = 0 | Counter = Items )
            object{
        #else
            union{
                object{ Objects[ Items ] translate y*1.1 }
        #end
                object{ Objects[ Counter ] }
            };

    #declare Pause=0.1;    
    #declare X0=(Counter+Pause)/(Items+2);
    #declare Y0=0;
    #declare X1=(Counter+1)/(Items+2);
    #declare Y1=1;
    #declare X2=(Counter+2-Pause)/(Items+2);
    #declare Y2=0;

    #declare C1=f_line(clock,0,0,3/4,1);
    #declare C2=(Items+1)/(Items+2);
    #declare C3=f_line(clock,1/4,0,1,1);
    #declare C=max(min(C1,C2),C3);
    
    #declare increase=f_line(C,X0,Y0,X1,Y1);    
    #declare decrease=f_line(C,X1,Y1,X2,Y2);    
    #declare change=min(increase,decrease);
    #declare level=min(max(change,0),1);

    object{
        Objects[ Counter ]
        pigment{ rgb level transmit 1-level }
    } 
    #declare Counter = Counter + 1;
#end 

