#----------------------------------------------------------------------------
# Name:         dependencymgr.py
# Purpose:      Dependency Manager
#
# Author:       Jeff Norton
#
# Created:      01/28/05
# CVS-ID:       $Id$
# Copyright:    (c) 2004-2005 ActiveGrid, Inc.
#----------------------------------------------------------------------------

DM_NO_ID = 0
DM_ID_ATTR = "_DependencyMgr__ID"

##class ManageableObject(object):
##    
##    def __init__(self):
##        self.__id = DM_NO_ID
##        
##    def __repr__(self):
##        return "<ManageableObject id = %s>" % self.__id
##        
##    def __getID(self):
##        return self.__id
##        
##    def __setID(self, value):
##        if (self.__id != DM_NO_ID):
##            raise DependencyMgrException("Cannot set the dependency ID on object %s to \"%s\" because it already has one (\"%s\")." % (repr(self), value, self.__id))
##        self.__id = value
##        
##    _DependencyMgr__ID = property(__getID, __setID)
        
class DependencyMgr(object):
    
    def __init__(self):
        self.clear()
        
    def clear(self):
        self.__dependencies = {}
        self.__lastID = DM_NO_ID
        
    def addDependency(self, parent, child):
        pid = self._initObjectID(parent)
        try:
            parentCollection = self.__dependencies[pid]
        except KeyError:
            parentCollection = self._newDependencyCollection()
            self.__dependencies[pid] = parentCollection
        if (child not in parentCollection):
            parentCollection.append(child)
            
    def removeDependency(self, parent, child):
        pid = self._getObjectID(parent)
        if (pid != DM_NO_ID):
            try:
                parentCollection = self.__dependencies[pid]
                parentCollection.remove(child)
                if (len(parentCollection) == 0):
                    del self.__dependencies[pid]
            except KeyError, ValueError:
                pass

    def clearDependencies(self, parent):
        "Returns a list of objects or an empty list if no dependencies exist as for getDependencies, and then removes the dependency list."
        pid = self._getObjectID(parent)
        try:
            deps = self.__dependencies[pid]
            del self.__dependencies[pid]
            return deps
        except KeyError:
            return []

    def hasDependency(self, parent):
        "Returns a boolean"
        return (self._getObjectID(parent) in self.__dependencies)
        
    def getDependencies(self, parent):
        "Returns a list of objects or an empty list if no dependencies exist."
        try:
            return self.__dependencies[self._getObjectID(parent)]
        except KeyError:
            return []
            
    def dumpState(self, out):
        "Writes the state of the dependency manager (as reported by getState) to out"
        for line in self.getState():
            print >> out, line
        
    def getState(self):
        "Returns the state of the dependency manager including all managed objects as a list of strings"
        out = []
        out.append("DependencyMgr %s has %i parent objects, last id assigned is %i" % (repr(self), len(self.__dependencies), self.__lastID))
        for key, val in self.__dependencies.iteritems():
            out.append("Object %s has dependents: %s " % (repr(key), ", ".join([repr(d) for d in val])))
        return out

    def _initObjectID(self, obj):
        try:
            id = getattr(obj, DM_ID_ATTR)
        except AttributeError:
            id = DM_NO_ID
        if (id == DM_NO_ID):
            id = self._newID()
            setattr(obj, DM_ID_ATTR, id)
        return id
        
    def _getObjectID(self, obj):
        try:
            id = getattr(obj, DM_ID_ATTR)
        except AttributeError:
            id = DM_NO_ID
        return id
    
    def _newID(self):
        self.__lastID += 1
        return self.__lastID
        
    def _newDependencyCollection(self):
        return []
        
globalDM = DependencyMgr()

def addDependency(parent, child):
    getGlobalDM().addDependency(parent, child)

def removeDependency(parent, child):
    getGlobalDM().removeDependency(parent, child)

def clearDependencies(parent):
    return getGlobalDM().clearDependencies(parent)
    
def hasDependency(parent):
    return getGlobalDM().hasDependency(parent)
    
def getDependencies(parent):
    return getGlobalDM().getDependencies(parent)
    
def getState():
    return getGlobalDM().getState()
    
def dumpState(out):
    getGlobalDM().dumpState(out)

def getGlobalDM():
    return globalDM
