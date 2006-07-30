# This file was created automatically by SWIG.
import streamsc
import wx
import string
class wxInputStreamPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def close(self, *_args, **_kwargs):
        val = apply(streamsc.wxInputStream_close,(self,) + _args, _kwargs)
        return val
    def flush(self, *_args, **_kwargs):
        val = apply(streamsc.wxInputStream_flush,(self,) + _args, _kwargs)
        return val
    def eof(self, *_args, **_kwargs):
        val = apply(streamsc.wxInputStream_eof,(self,) + _args, _kwargs)
        return val
    def read(self, *_args, **_kwargs):
        val = apply(streamsc.wxInputStream_read,(self,) + _args, _kwargs)
        return val
    def readline(self, *_args, **_kwargs):
        val = apply(streamsc.wxInputStream_readline,(self,) + _args, _kwargs)
        return val
    def readlines(self, *_args, **_kwargs):
        val = apply(streamsc.wxInputStream_readlines,(self,) + _args, _kwargs)
        return val
    def seek(self, *_args, **_kwargs):
        val = apply(streamsc.wxInputStream_seek,(self,) + _args, _kwargs)
        return val
    def tell(self, *_args, **_kwargs):
        val = apply(streamsc.wxInputStream_tell,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxInputStream instance at %s>" % (self.this,)
class wxInputStream(wxInputStreamPtr):
    def __init__(self,*_args,**_kwargs):
        self.this = apply(streamsc.new_wxInputStream,_args,_kwargs)
        self.thisown = 1




class wxOutputStreamPtr :
    def __init__(self,this):
        self.this = this
        self.thisown = 0
    def write(self, *_args, **_kwargs):
        val = apply(streamsc.wxOutputStream_write,(self,) + _args, _kwargs)
        return val
    def __repr__(self):
        return "<C wxOutputStream instance at %s>" % (self.this,)
class wxOutputStream(wxOutputStreamPtr):
    def __init__(self,this):
        self.this = this






#-------------- FUNCTION WRAPPERS ------------------



#-------------- VARIABLE WRAPPERS ------------------

