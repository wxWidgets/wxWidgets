# Name:         core.py
# Purpose:      Core componenets
# Author:       Roman Rolinsky <rolinsky@femagsoft.com>
# Created:      31.05.2007
# RCS-ID:       $Id$

import component
import images

# Test

print 'core components'

c = component.ContainerComponent('wxFrame', 'windows', ['pos', 'size', 'title'])
component.manager.register(c)
c.image = images.getTreeFrameImage()
component.manager.setMenu(c, None, 'Frame', 'Frame window', 10)

c = component.Component('wxStaticText', 'controls', ['pos', 'size', 'label'])
component.manager.register(c)
component.manager.setMenu(c, 'control', 'Label', 'Label', 10)
