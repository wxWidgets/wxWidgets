from tokenize import *
from keyword import *
from string import *

class Tokenizer:
    """
    Simple class to create a list of token-tuples like:

    	(type, string, first, last)

    Example:
    t = Tokenizer('def hallo(du): # juchee')
    print t.tokens()
    """

    def __init__(self, text):
    	self.text = text
    	self.toks = []
    	try:
    	    tokenize(self.readline, self.get)
	except TokenError:
	    pass

    def tokens(self):
    	return self.toks

    def get(self, type, string, begin, end, l):
    	#print begin,end
    	h1, b = begin
	h2, e = end
	tname = tok_name[type]
   	if iskeyword(string):
    	    tname = "KEY"
    	self.toks.append(tname, string, b, e)

    def readline(self):
    	t = self.text
	self.text = ""
	return t

    def line(self):
    	pre = ""
    	out = ""
	for type, string, begin, end in self.toks:
    	    if (pre in ["NAME","KEY"]) and (not string in [".",",","("]):
	    	out = out + " "

	    if type in ["NAME","KEY"]:
		out = out + string
	    elif type=="OP":
	    	if string in [",",":"]:
		    out = out + string + " "
		else:
       		    out = out + string
	    else:
    	        out = out + string
    	    pre = type
	return out


