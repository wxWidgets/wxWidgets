TRUE  = 1
FALSE = 0

def RestOfLine(sx, width, data, bool):
    if len(data) == 0 and sx == 0:
        return [('', bool)]
    if sx >= len(data):
        return []
    return [(data[sx:sx+width], bool)]

def Selection(SelectBegin,SelectEnd, sx, width, line, data):
    if SelectEnd is None or SelectBegin is None:
        return RestOfLine(sx, width, data, FALSE)
    (bRow, bCol) = SelectBegin
    (eRow, eCol) = SelectEnd
    if (eRow < bRow):
        (bRow, bCol) = SelectEnd
        (eRow, eCol) = SelectBegin
    if (line < bRow or eRow < line):
        return RestOfLine(sx, width, data, FALSE)
    if (bRow < line and line < eRow):
        return RestOfLine(sx, width, data, TRUE)
    if (bRow == eRow) and (eCol < bCol):
        (bCol, eCol) = (eCol, bCol)
    # selection either starts or ends on this line
    end = min(sx+width, len(data))
    if (bRow < line):
        bCol = 0
    if (line < eRow):
        eCol = end
    pieces = []
    if (sx < bCol):
        if bCol <= end:
            pieces += [(data[sx:bCol], FALSE)]
        else:
            return [(data[sx:end], FALSE)]
    pieces += [(data[max(bCol,sx):min(eCol,end)], TRUE)]
    if (eCol < end):
        pieces += [(data[eCol:end], FALSE)]
    return pieces


