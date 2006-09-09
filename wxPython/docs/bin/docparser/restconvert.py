import re

conversion_table = {
                    "B" : "**", 
                    "I" : "*", 
                    "TT": "``",
                    "P" : "\n",
                    "BR": "\n",
                    }

html_classlink_re = "<A HREF=\".*?\">(.*?)</A>"

def htmlToReST(html):
    # \n is useless in the HTML docs, we'll use P tags to break paragraphs.
    restText = html.replace("\n", "")
    restText = restText.replace("*", "\\*")
    if restText.find("<P>") == 0:
        restText = restText[3:]
        
    link_regex = re.compile(html_classlink_re, re.DOTALL | re.MULTILINE | re.IGNORECASE)
    restText = link_regex.sub("`\g<1>`", restText)
    
    
    for htmltag in conversion_table:
        
        for tagname in [htmltag, htmltag.lower()]:
            restText = restText.replace("<%s>" % tagname, conversion_table[htmltag])
            restText = restText.replace("</%s>" % tagname, conversion_table[htmltag])
    
    # we need to escape any remaining double-quotes
    restText = restText.replace('"', '\\"')
    return restText.strip()