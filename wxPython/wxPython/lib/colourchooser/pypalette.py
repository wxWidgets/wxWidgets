"""
wxPyColourChooser
Copyright (C) 2002 Michael Gilfix

This file is part of wxPyColourChooser.

You should have received a file COPYING containing license terms
along with this program; if not, write to Michael Gilfix
(mgilfix@eecs.tufts.edu) for a copy.

This version of wxPyColourChooser is open source; you can redistribute it and/or
modify it under the terms listed in the file COPYING.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
"""

import canvas
from wxPython.wx import *

def getPaletteXpm():
    return [
        "   175   192      256            2",
        ".. c #0808f8",
        ".# c #0882f8",
        ".a c #878778",
        ".b c #f80708",
        ".c c #38c93e",
        ".d c #b9484e",
        ".e c #08c5f8",
        ".f c #8d07f8",
        ".g c #1446ec",
        ".h c #88c938",
        ".i c #5c9e64",
        ".j c #c74839",
        ".k c #58aa5b",
        ".l c #f8078c",
        ".m c #986871",
        ".n c #48c838",
        ".o c #4848b9",
        ".p c #c88f38",
        ".q c #c8c837",
        ".r c #18e923",
        ".s c #5407f8",
        ".t c #58a891",
        ".u c #8748b7",
        ".v c #18e994",
        ".w c #db258f",
        ".x c #d92836",
        ".y c #4868b8",
        ".z c #89a859",
        ".A c #8f28d7",
        ".B c #68a859",
        ".C c #b84891",
        ".D c #a86859",
        ".E c #87e819",
        ".F c #698996",
        ".G c #18e94e",
        ".H c #38c89f",
        ".I c #4929d7",
        ".J c #d208f7",
        ".K c #c9e918",
        ".L c #4de918",
        ".M c #e82818",
        ".N c #0829f8",
        ".O c #38c96a",
        ".P c #08e7f8",
        ".Q c #689970",
        ".R c #28d99e",
        ".S c #7048b8",
        ".T c #d227d3",
        ".U c #c43c96",
        ".V c #48b94e",
        ".W c #877880",
        ".X c #08a3f8",
        ".Y c #48b890",
        ".Z c #a8579a",
        ".0 c #a8a857",
        ".1 c #c9383b",
        ".2 c #696899",
        ".3 c #a95957",
        ".4 c #a78959",
        ".5 c #1061f0",
        ".6 c #f80753",
        ".7 c #ce6732",
        ".8 c #2fe918",
        ".9 c #b0c938",
        "#. c #c7ab39",
        "## c #7127d8",
        "#a c #6dc839",
        "#b c #f906ce",
        "#c c #689891",
        "#d c #886897",
        "#e c #e8c818",
        "#f c #48b870",
        "#g c #a847b8",
        "#h c #b3e918",
        "#i c #b928d7",
        "#j c #3107f8",
        "#k c #ba07f8",
        "#l c #08f997",
        "#m c #ce3299",
        "#n c #e8e818",
        "#o c #2d27d9",
        "#p c #28d936",
        "#q c #08f90d",
        "#r c #7407f8",
        "#s c #08f954",
        "#t c #27c8d9",
        "#u c #28d96b",
        "#v c #697897",
        "#w c #29a7d7",
        "#x c #e9168f",
        "#y c #6858a7",
        "#z c #919868",
        "#A c #4858b8",
        "#B c #48a8b8",
        "#C c #71b849",
        "#D c #c83766",
        "#E c #2848d8",
        "#F c #9158a8",
        "#G c #91b848",
        "#H c #09f8da",
        "#I c #28d8ce",
        "#J c #a85873",
        "#K c #e94717",
        "#L c #6ce918",
        "#M c #47b8b2",
        "#N c #b2b847",
        "#O c #e8174d",
        "#P c #2888d8",
        "#Q c #b74871",
        "#R c #d82865",
        "#S c #ce32c8",
        "#T c #efae11",
        "#U c #8ed928",
        "#V c #9738c7",
        "#W c #a87958",
        "#X c #e91817",
        "#Y c #51d829",
        "#Z c #79887f",
        "#0 c #a89858",
        "#1 c #2965d7",
        "#2 c #e86918",
        "#3 c #7238c7",
        "#4 c #ee12e0",
        "#5 c #b037c9",
        "#6 c #4979b7",
        "#7 c #789869",
        "#8 c #afd829",
        "#9 c #58a875",
        "a. c #f08e10",
        "a# c #4989b7",
        "aa c #cf16e9",
        "ab c #0719f9",
        "ac c #0892f8",
        "ad c #08d6f8",
        "ae c #9417e8",
        "af c #c95837",
        "ag c #d2d927",
        "ah c #4e17e8",
        "ai c #8cf908",
        "aj c #5037c8",
        "ak c #cef908",
        "al c #53f908",
        "am c #e83718",
        "an c #083af8",
        "ao c #08b4f8",
        "ap c #c87938",
        "aq c #32f908",
        "ar c #e7d919",
        "as c #acf908",
        "at c #2f17e8",
        "au c #b618e7",
        "av c #f3f808",
        "aw c #2738d9",
        "ax c #7118e8",
        "ay c #29b7d7",
        "az c #e85718",
        "aA c #2899d8",
        "aB c #e77c19",
        "aC c #4898b8",
        "aD c #6dd928",
        "aE c #6df809",
        "aF c #cf9e31",
        "aG c #cfba31",
        "aH c #3156cf",
        "aI c #3274ce",
        "aJ c #08f931",
        "aK c #e81737",
        "aL c #1152ef",
        "aM c #1071f0",
        "aN c #ecba14",
        "aO c #f09e10",
        "aP c #17e9df",
        "aQ c #de22d2",
        "aR c #977869",
        "aS c #d93927",
        "aT c #39d927",
        "aU c #f706f4",
        "aV c #f90736",
        "aW c #19e8b6",
        "aX c #19e871",
        "aY c #08f9ba",
        "aZ c #da25ad",
        "a0 c #37c8c6",
        "a1 c #e218e0",
        "a2 c #1987e7",
        "a3 c #978969",
        "a4 c #18cae8",
        "a5 c #d74929",
        "a6 c #d78f29",
        "a7 c #d9c827",
        "a8 c #5968a7",
        "a9 c #b86948",
        "b. c #f82908",
        "b# c #1828e8",
        "ba c #58b948",
        "bb c #18abe8",
        "bc c #b8a848",
        "bd c #b85848",
        "be c #b78949",
        "bf c #d9ac27",
        "bg c #986791",
        "bh c #f8c508",
        "bi c #b847b2",
        "bj c #f8e708",
        "bk c #19fa07",
        "bl c #797988",
        "bm c #38a8c8",
        "bn c #5858a9",
        "bo c #57a8a8",
        "bp c #3848c8",
        "bq c #f84b08",
        "br c #3886c8",
        "bs c #b87948",
        "bt c #f81708",
        "bu c #b89848",
        "bv c #3764c9",
        "bw c #f86e08",
        "bx c #c33ac2",
        "by c #5879a8",
        "bz c #5989a7",
        "bA c #1818e8",
        "bB c #179be9",
        "bC c #18d9e8",
        "bD c #d85928",
        "bE c #f83908",
        "bF c #1836e8",
        "bG c #17bbe9",
        "bH c #d87828",
        "bI c #f8d708",
        "bJ c #3838c9",
        "bK c #38b8c8",
        "bL c #f85c08",
        "bM c #3998c7",
        "bN c #f87e08",
        "bO c #5898a8",
        "bP c #e916b3",
        "bQ c #f807ac",
        "bR c #786897",
        "bS c #f7086d",
        "bT c #08f974",
        "bU c #7958a8",
        "bV c #07f9f5",
        "bW c #e8176c",
        "bX c #1807f8",
        "bY c #a007f8",
        "bZ c #97c839",
        "b0 c #58c938",
        "b1 c #5847b8",
        "b2 c #9847b8",
        "b3 c #98a858",
        "b4 c #a226d9",
        "b5 c #79a858",
        "b6 c #9ae917",
        "b7 c #5927d8",
        "b8 c #e108f7",
        "b9 c #d9e918",
        ".b.b.bbtb.b.bEbEbqbqbLbLbwbwbNbNa.a.aOaO#TbhbhbhbIbIbjbjavavavavavavakakakakasasasasaiaiaiaEaEaEaEalalalaqaqaqbkbkbk#q#q#q#q#q#qaJ#qaJaJ#s#s#s#sbTbTbTbT#l#l#l#laYaYaYaY#H#HbV#HbVbVbVbVbV.Padadad.eaoao.X.Xacac.#.#aMaM.5.5aL.ganan.N.Nabab..........bXbX#j#j#j.s.s.s.s#r#r#r.f.f.fbYbY#k#k#k.J.Jb8b8aUaUaUaUaUaUaU#b#b#b#bbQ#b.lbQ.l.l.l.lbSbS.6bSaV.6aVaVaV",
        ".b.bbtbtb.btbEb.bqbqbLbLbwbwbNbNa.a.aO#T#T#TbhbhbIbIbjavavavavavavavavakakakakasasasaiaiaiaiaEaEalalalaqalaqbkaqbk#q#q#q#q#q#q#q#qaJ#saJaJ#s#s#sbTbTbTbT#l#laY#laYaY#HaY#H#HbVbVbVbVbVbV.P.P.Pad.e.eao.e.X.Xacac.#.#aMaMaL.5.gaLanan.N.Nab..........bXbX#jbX#j#j#j.s.s.s#r.s.f#r.f.fbYbYbY#k.J#k.Jb8b8aUaUaUaUaUaUaUaU#b#b#b#bbQbQbQ.l.lbS.lbSbSbS.6.6.6aVaVaV",
        ".b.b.bbtbtb.bEbEbqbEbLbLbwbwbNbNa.a.#TaO#TbhbhbhbIbIbjbjavavavavavavakakakakasasasasaiaiaiaEaEaEaEalalalaqaqaqbkbkbk#q#q#q#q#q#qaJaJaJaJ#s#s#s#sbT#s#lbT#lbT#laYaYaYaYaY#H#H#HbVbVbVbVbVbV.Pad.P.eadaoao.X.Xacac.#.#aMaM.5.5.g.ganan.N.Nabab............bX#j#j.s.s#j.s.s#r#r#r.f.f.fbYbY#k#k#k.J.Jb8b8aUaUaUaUaUaUaU#b#b#b#bbQbQbQbQ.lbQ.l.lbSbS.6.6.6aVaVaVaV",
        ".b.bbt.bb.b.bEbEbqbqbLbqbwbwbNbNa.a.aOaO#T#TbhbhbIbIavbjavavavavavavavakakakasasasasaiaiaiaiaEaEaEalalalaqalbkaqbk#q#q#q#q#q#q#q#qaJaJaJaJ#s#s#sbTbTbTbT#l#laY#l#laY#HaY#H#H#HbVbVbVbVbV.PbVadad.e.eaoao.X.Xacac.#.#aMaMaL.5.gaLananab.Nab..........bXbXbX#j#jbX.s.s.s.s#r.s.f#r.fbYbYbY#k#k.J#kb8.Jb8aUaUaUaUaUaU#baU#b#b#b#bbQbQbQ.l.l.lbSbSbSbS.6.6.6aVaVaV",
        ".b.b.bbtb.btbEbEbqbqbLbLbwbwbNbNa.a.#TaObh#TbhbhbIbIbjbjavavavavavakakavakakasasasasaiaiaiaEaEaEalaEalaqaqaqaqbkbkbk#q#q#q#q#q#qaJaJaJaJ#s#s#s#sbTbTbTbT#l#l#l#laYaYaYaY#H#HbV#HbVbVbVbV.P.Pad.P.e.eaoao.X.Xacac.#.#aMaM.5.5.gaLanan.N.Nabab..........bX#jbX#j.s#j.s.s.s#r#r#r.f.f.fbYbY#kbY#k.J.J.JaUb8aUaUaUaUaUaU#b#b#b#bbQbQbQ.l.l.l.l.lbSbS.6bS.6aVaVaVaV",
        ".b.bbtbtb.b.bEbEbEbqbLbLbwbwbNbNa.a.aOaO#T#TbhbhbIbIbjbjavavavavavavakakakasakasasasaiaiaiaiaEaEalalalalaqalbkaqbkbk#q#q#q#q#qaJ#qaJaJaJ#s#s#s#sbT#s#lbT#l#l#l#laYaYaYaY#H#H#HbVbVbVbVbV.PbVadad.e.eao.e.X.Xacac.#.#.5aM.5aLaL.ganan.N.Nabab........bXbXbX#j#j#j.s.s.s.s#r#r.f#r.f.fbYbY#k#k.J#k.J.Jb8aUaUaUaUaUaUaUaU#b#b#bbQbQbQbQ.l.lbS.lbSbS.6.6.6.6aVaVaV",
        ".b.b.bbtbtb.bEbEbqbqbqbLbwbwbNbNa.a.aOaObh#TbhbhbIbIbjbjavavavavavavakavasakasasasaiaiaiaiaEaEaEaEalalalaqaqaqbkbkbk#q#q#q#q#q#qaJaJaJaJ#saJ#s#sbTbTbTbT#l#l#laYaYaYaY#H#H#HbV#HbVbVbVbV.P.Padadad.eaoao.X.Xacac.#.#aMaMaL.5aL.ganan.N.N..ab..........bXbX#j#j#j.s.s.s.s#r.s.f#r.f.fbYbY#kbY#k.J.Jb8b8b8aUaUaUaUaU#b#b#b#b#bbQ#b.lbQ.l.l.l.lbSbSbS.6.6.6aVaVaV",
        ".b.bbtbtb.b.bEb.bqbqbLbLbwbwbNbNa.a.aOaO#T#TbhbhbIbIbjbjavavavavavavakakakakakasasasaiaiaiaiaEaEaEalalalaqalbkaqbkbk#q#q#q#q#q#q#qaJaJaJ#s#s#s#sbTbTbTbT#l#l#l#laYaYaYaY#H#H#HbVbVbVbVbV.PbVadad.e.eaoao.X.Xacac.#.#aMaM.5.5.gaLanan.N.Nabab........bXbXbX#j#j#j#j.s.s.s#r#r#r.f.f.fbYbY#k#k.J#kb8.Jb8aUaUaUaUaUaUaUaU#b#b#bbQbQbQbQbQ.lbS.lbSbS.6bSaV.6aVaVaV",
        ".b.b.bbtb.b.bEbEbqbqbLbLbwbwbNbNa.a.#TaO#T#TbhbhbIbIbjbjavavavavavavakakakasasasasasaiaiaiaEaEaEalaEalalaqaqaqbkbkbk#q#q#q#q#q#qaJaJaJaJ#s#s#s#sbTbTbT#lbT#l#l#laYaYaYaY#H#H#HbVbVbVbVbV.P.Padad.e.eaoao.X.Xacac.#.#aMaMaL.5aL.ganan.N.Nabab........bXbX#jbX.s#j.s.s.s.s#r.s.f#r.f.fbYbYbY#k#k#k.J.JaUb8aUaUaUaUaU#b#b#b#b#bbQbQbQ.l.l.l.l.lbSbS.6bS.6.6aVaVaV",
        ".b.bbtbtb.b.bEbEbqbqbLbLbLbwbNbNa.a.aOaO#T#TbhbhbhbIbjbjavavavavakavakakakakakasasasaiaiaiaiaEaEalalalalaqaqaqaqbkbk#q#q#q#q#q#qaJaJaJaJ#s#s#s#sbTbTbTbT#l#l#l#laYaYaYaY#HaYbV#HbVbVbVbV.P.Padad.e.eaoao.X.Xacac.#.#.5aM.5.5.gaLanan.N.Nabab........bXbXbX#j#j#j.s.s.s.s#r#r#r.f.f.fbYbY#k#k#k.J.J.Jb8b8aUaUaUaUaUaU#b#b#bbQ#bbQbQbQ.l.lbS.lbSbS.6bSaV.6aVaVaV",
        ".b.bbtbtb.btbEbEbqbqbqbLbwbwbNbNa.a.aO#T#T#TbhbhbIbIbjbjavavavavavavakavasakasasasaiaiaiaiaEaEaEaEalalalaqalbkaqbkbk#q#q#q#q#q#qaJ#qaJaJ#saJ#s#sbTbTbTbT#l#l#l#laYaYaYaY#H#HbV#HbVbVbVbV.P.Padad.e.eaoao.X.Xacac.#.#aMaM.5.5aL.ganan.N.Nabab..........bX#jbX#j#j.s.s.s.s#r#r#r.f.f.fbYbYbY#k#k#kb8.Jb8aUaUaUaUaUaUaU#b#b#b#bbQbQbQbQ.l.l.l.lbSbSbS.6.6.6aVaVaV",
        ".b.bbtbtb.b.bEbEbqbqbLbLbwbwbNbNa.a.aOaO#T#TbhbhbIbhbjbjavavavavavavakakakakasasasasaiaiaiaiaEaEaEalalalaqaqaqbkbkbk#q#q#q#q#q#qaJaJ#saJ#s#sbT#sbT#s#lbT#lbTaY#laYaYaYaY#H#H#HbVbVbVbVbV.P.Padad.e.eaoao.X.Xacac.#.#aMaMaL.5.gaLanan.N.Nabab........bXbXbX#j#j.s#j.s.s.s#r.s.f#r.f.fbYbY#kbY.J#k.J.Jb8b8aUaUaUaU#baU#b#b#b#bbQ#b.lbQ.l.lbS.lbSbS.6.6.6.6aVaVaV",
        ".b.bbtbtb.b.bEbEbqbqbLbLbwbwbNbNa.a.aOaO#T#TbhbhbIbIbjbIavavavavavakakakakasasakaiasaiaiaiaEaEaEaEalalalaqalbkaqbkbk#q#q#q#q#qaJ#qaJaJaJ#s#s#s#sbTbTbTbT#l#l#l#laYaYaY#HaY#H#H#HbVbVbVbV.P.Padad.e.eaoao.X.Xacac.#.#.5aM.5.5aLaLanan.N.Nabab........bXbX#j#j#j#j.s.s.s.s#r#r#r.f.f.fbYbY#k#k#k#kb8.Jb8b8aUaUaUaUaUaU#b#b#b#bbQbQbQbQ.l.l.l.lbSbSbS.6.6.6aVaVaV",
        ".b.bbtbtb.b.bEbEbqbqbLbLbwbwbNbNa.a.aOaO#T#TbhbhbhbIbIbjavavavavavavavakakakasasasasaiaiaiaiaEaEaEalalalaqaqaqaqbkbk#q#q#q#q#q#qaJaJaJaJ#s#s#s#s#sbT#lbT#lbTaY#l#laYaYaY#H#H#HbVbVbVbVbV.P.Padad.e.eaoao.X.Xac.#.#.#aMaM.5.5.gaLananan.Nabab........bXbXbX#j#j#j.s.s.s.s#r#r.f#r.f.fbYbY#kbY.J#k.J.Jb8b8aUaUaUaU#baU#b#bbQ#bbQbQ.lbQ.l.lbS.lbSbSbS.6.6.6aVaVaV",
        ".b.bbtbtb.b.bEbEbqbqbLbLbwbwbNbNa.a.aO#T#T#TbhbhbIbhbjbjbjavavavavakakakakakasasasasaiaiaiaEaEaEalaEalalaqalbkaqbkbk#q#q#q#q#qaJaJ#q#saJ#saJ#sbTbTbTbTbT#l#l#l#laYaYaYaY#H#H#H#HbVbVbVbV.P.Padad.e.eaoao.X.Xacac.#.#aMaM.5.5aLaLanan.N.Nabab........bXbX#j#j#j.s#j.s.s.s#r.s.f#r.f.fbYbYbY#k#k#kb8.Jb8b8aUaUaUaUaUaU#b#b#b#bbQbQbQbQ.l.l.l.lbSbS.6bS.6.6aV.6.b",
        ".b.bbtbtb.b.bEbEbqbqbLbLbwbwbNbNa.a.aOaO#T#TbhbhbhbIbjbjavavavavavavakakasak#hasasaib6aiaEaiaEaEaEalalalalaqaqaqbk.8#q#q#q#q#q#qaJaJaJaJ#s#s#s#sbTbTbTbT#l#l#l#laYaYaYaY#H#H#HbVbVbVbVbV.P.Padad.e.eaoao.X.Xacac.#.#aMaMaL.5.gaLanan.N.N.Nab........bXbXbX#j#j#j.s.s.s.s#r#r#r.f.f.fbYbY#k#k#k#k.J.Jb8b8aUaUaUaU#baU#b#4#b#bbQbQbQbQ.l.lbS.lbSbS.6bSaV.6aVaVaV",
        ".b.bbtbtb.b.bEbEbqbqbLbLbwbwbNbNa.a.aOaO#T#TbhbhbhbIbIbjbjavbjavb9avakb9akakasasasasaib6aiaiaEaEal#Lal.8aq.Laq.8bkbk#q.r#q#q.r#qaJ.raJ#s.G#s#saX#sbT.vbT#l.v#l#laWaYaYaY#H#H#H.PbVbV.PbV.P.Padad.e.eaoao.X.Xacac.#aMaMaM.5.5aLaLananan.Nababab......bXbX#j#j#j.s.s.s.s.s#r#r#r#r.f.fbYbYbY#k#k#kb8.Jb8b8aUb8aU#4aU#4#b#bbQ#bbQbQbQ.l.lbQbS.lbSbSbS.6.6.6aV#OaV",
        ".b.bb.btb..MbEbEbqbqbLazbwbwbNbNa.a.aOaO#T#TaNbhbIbhbjbjbjavavavakavakak#hak#hasasb6aiaiaiaE#LaE#Lalalalalaqbkaqbk.r#q#q#q#q#q.raJaJaJaJ#s.G#s#sbTbTbT.vbT#l#laY#laWaY#HaW#H#H#HbVbVbV.P.Padadadao.eaoaoac.Xac.#.#.#aMaM.5.5aLaLanan.N.N.Nab..ab....bXbX#j#j#j#j#j.sax.s#r#r#r.f.faebYbYbY#k#k#k.J.Jb8b8aU#4aUaU#baU#4#bbP#bbPbQbPbQ.l#x.lbSbSbS.6bS.6#OaVaVaV",
        ".b.b#XbtbEb.bEbE#KbqbLbLbw#2bNbNa.a.aOaO#T#TbhbhbhbI#nbIavavbjavavb9akb9akasas#hasaib6aiai.EaEaEalaE.Lal.8al.8aq.rbk.r#q.r#q#q#qaJ.r#s.r#s#s#saX#saXbTbT#l.v#l.vaYaYaYaY#H#H#HaPbV.PbVbV.P.Pa4ad.ebGaoao.X.Xacac.#.#aMaMaL.5aLaLananan.NabbAab..bX..bAbXat#jat.sah.s.s.sax#r#rae.f.fbYae#kau#k#kaa.Jb8b8#4aU#4aU#4aU#b#4#bbQbQbQbQ.l.l.l.l#xbSbSbW.6.6.6.6aKaV",
        ".bbtbtbt.Mb.bE#KbqbqbLbLbwbwbNbNa.a.aOaO#T#TbhaNbIbIbIbjbjavavavb9avakakakak#hasaiasb6aiaEaiaEaE#Lalalalaqaqaqaqbkbk#q#q#q#q.raJ.raJaJaJ#s.G#s#sbTbTbT.v#lbTaY#l#laYaYaWaYaP#H#HbVbVaPbVad.Padad.e.eaoao.X.Xacac.#.#aMaM.5.5aLaLan.g.NbFab.N........bXbX#j#j#j#j.s.s.s.s#r#r#r#r.f.fbYbY#kbY.J#k.J.Jb8b8aU#4aUaUaU#4#b#b#b#bbPbQ.lbP.l#x.lbSbSbSbS.6.6#OaVaVaV",
        ".b#X#Xb.b.ambEbEazbqbLazbwbwaBbNa.a.aOaO#T#TbhaNbh#ebjaravbjavavavb9akb9#hakas#hasb6aiai.Eai#LaEaEal.L.Lal.8.8aq.8.8#q.r#q.r#q#q.raJ.GaJ.G#s.G#saXbTaXbT.v#l.v.vaYaWaYaW#H#H.P#HaPbVbVaP.PbCad.e.ebGaobb.XbBaca2.#.#aMaM.5.5.gaLananbF.N.NbAbAbA..bAbXatbXatah#jahah.saxaxax#rae.faebYae#kau#kaa.Jaaa1b8#4aU#4#4#4aU#4bP#bbPbQbPbQbQ#x.l.l#xbSbW.6bW.6.6aK.6#X",
        "#X.bbtbt.Mb.#KbEbqbqbL#2#2bwbNbNa.a.aOaO#T#TaNbharbhbjbIavbjavbjak#nakak#hak#hasb6asaib6aE.EaE#L.LaE.Lal.8alaq.8bk.r#q.r#q#q#q.raJ.raJ.G#s.G#saXbTaXbTaX#l.v#l#laWaYaWaY#HaW#HaPbVaPbVaP.P.Pa4ad.ebGaobb.XbBacac.#.#.5aM.5.5aLaLan.g.NbFab.N......bAbXbXat#j#j#j.s.sax.s.s#r#r#r.faebYaebYbYau#kaa.Jb8b8b8#4aU#4#b#4#b#4bP#bbQbP.lbP.l.l#xbSbSbWbS.6#O.6.6aKaV",
        ".b#Xbt.Mb.b.bE#K#KazazbLbwbwaBbNa.a.aOaO#T#TbhaNbhbIarbj#nav#nav#nakb9akak#has#hasb6aib6aiaE#LaEaEal.Lal.Laq.8aqbk.8#q#q.r#q.r#q.raJ#s.r#s#s.G#sbTaXbT#l.vbTaW#laW#l#HaW#H#H#HaPbV.PbV.P.PbCada4.ebGaoaobB.Xaca2.#.#aMaM.5.5aLaLan.gan.N.NbAbA..bA..bXat#j#jatah.s#j.sax#rax#rae.f.faebYau#k#kau.J#ka1b8#4aU#4aU#4#4#b#4bQ#4bQbPbQ.l#x.l#xbS#xbS.6bW.6#OaVaVaK",
        ".b.b#Xb..MambEbEbqbqbLbLbw#2bNbNa.a.aOaO#T#TbhaNbIbh#nbIavbjavavb9avakak#hak#hasb6asai.Eai.EaEaE#L#Lal.Laqal.8aq.8bk.r#q.r#q.raJ.raJ.GaJ.G#saX#saXbTbTaX#l#l.v#laYaWaYaW#HaW#HaPbVbVaPbVbC.Pada4.e.ebbao.X.XacacaM.#aMaM.5.5aLaLananbF.Nab.N..bA....bAbXbXat.s#jahah.s.sax#r#rax.faebYbY#kbYaa#k.Jaab8b8#4aU#4#4aU#4#4#b#4bQbPbQbPbQ#x.lbS#xbSbSbW.6#O.6.6aKaV",
        "#X#Xbt.Mb.ambE#K#KazbLazbw#2a.aBa.a.aOaO#T#TbhaNbh#ebIar#n#nav#navb9b9b9#hak#hasb6b6b6ai.EaE.E#Lal#Lal.L.L.8aq.8.r.8#q.r#q.r#q.raJ.r.GaJ.G.G#saX#saX.vbT.v.v#l.vaW#laWaYaPaY.PaPaP.PbVaP.PbCa4adbGbGaobb.XbBbBaca2.#aMaM.5.5aLaLan.gbFbF.NbAbA..abbAbXatat#jatah.sahax.sax#rax.fae.faeaeauau#kauaa.Ja1aa#4#4#4#4#4#4#bbPbP#bbPbQ#xbP.l#x#xbSbWbWbS#O.6#OaK.6aK",
        ".b#Xbt.Mb..M#KbEaz#KbL#2#2bwaBbNa.a.aOaO#T#TaNaNbI#e#nbIavbj#navb9b9ak.Kak#has#hasb6aib6aE.E#LaE#Lal.Lal.8al.8aq.8bk.r.r.r#q.r.r.r.raJ.G#s.G#s.GaXbTaX.vbT.v.v#laWaYaWaW#HaWaP#HaPbVaP.PaPadada4.ebGaobb.XbB.#bBaM.#aMaM.5.5aLaL.gan.NbFbA.Nab..bA..atbXat#jah#j.sah.sax.saxaeax.faeaebYaubY#kaa#kaab8b8#4#4#4aU#4#4#4#4#bbPbQbPbP.l#x#xbS#xbSbW.6bW.6#OaVaKaV",
        "#X.b.M#XbE.MbqambqazbLazbwbwbNaBa.a.aOaO#T#TaNaNbh#ebIar#nav#navb9av.Kak#h.Kas#hb6asb6.Eai.EaE#L#L#Lal.L.Laq.8.8bk.8#q.r#q.r#q.raJ.G.GaJ.G.GaX#saXbTaXbT.v.v#l.vaWaYaWaYaPaY.P#HaPbVaPbVbC.Pa4a4.ebGaoaobB.XbBa2.#a2.5aM.5.5aLaLan.gbFanabb#bA..bA..bAbXat#jatahah.sah.sax#r#rax.faebYaebYauau#kaaaaaab8#4#4#4#4#4#4#bbPbPbPbQbP.lbP.l#x.l#xbSbWbSbW.6#O#O.6#X",
        ".b#X#Xbtamb.am#Kbq#K#2bLaB#2aBbNa.a.aOaO#T#TaN#ebharbI#nbj#nbjavb9b9b9ak#h.K#hasb6as.Eai.EaE#LaE#Lal.L.Laq.L.8aq.8.r.r#q.r#q.r.raJ.raJ.G.G#saX#saXaX#laX.v#l.vaY.vaYaWaWaYaP#HaPaP.PaPbVadbCada4.ebGaobb.XbBaca2a2.#aMaM.5.5aLaL.g.g.NbFabb#..bA..bAbXatat#jah#jahah.sax#rax#rae.faebYaebYau#kau.Jaab8a1b8#4#4aU#4#4#4#4bQ#4bQbPbPbQ#x#xbS#xbWbSbW.6#O.6aKaVaK",
        "#X.b.M.M.Mam#KbEazbqaz#2#2bwbNaBa.a.aOaOaO#TaNaN#ebhararbjav#n#nb9akb9.Kak#h#h#hb6b6b6.E.E.E#L#LaE.Lal.L.L.Laq.8.r.8#q.r.r.r#q.r.r.r.GaJ.G.G.GaXbTaXaXbT.v.v.v.vaWaWaYaW.PaWaP#HaPbVaPaPbC.Pa4a4bG.ebbbb.XbBacac.#aMaMaM.5.5aLaLan.gbFbF.Nb#bAbAbA..atbXatat#jahah.sahaxaxaxax#raeaeaeaeaubYaaauaa#ka1aa#4#4#4#4#4#4bP#4bPbPbPbP.l#x#x.l#x#xbSbWbW#O#O#O.6#OaK",
        ".b#Xb.#XbE.MbE#K#Kbq#2bLbw#2aBaBa.a.aOaO#T#TaNbh#earbIar#n#n#nav#nb9ak.K.K#has#hasb6aib6aE.EaE.E.L#L#L.Laq.L.8.8.8.r.r.r#q.r.r.r.raJ.G.G.G#s.G#saXaXbTaX.v.v#l.v#laWaWaYaWaYaPaPbVaPaP.PaPada4a4.ebGbbaobBbBbBa2a2a2aMaM.5.5aLaL.g.g.NbFbAb#..bA..bAatbA#jatah#jahahax.s.sax.faxaeaebYaubYauau#kaaaaaab8#4a1b8#4#4#4bP#4#bbPbQbPbPbP#x#xbS#xbWbWbSbW.6#O#OaVaK",
        "#X.b.M#Xam.M#KbEaz#K#2az#2aBaBbNa.a.aOaO#T#TaNaN#ebhar#nbj#nav#nb9b9.K.K#hak#hb6b6b6b6b6.EaE.E#LaE.L.Lal.L.Laq.8bk.8.r.r.r#q.r.raJ.r.GaJ.G.GaX.GaXbT.vaX#l.v.vaWaWaYaWaW.PaWaP#HaP.PaPbVbCbCada4bGbGaobb.XbBaca2.#a2aMaM.5.5aLaL.g.gbFanb#.NbAbAbA..bAatat#jatahah.sahaxax#rax#rae.faeae#kau#kauaa.Ja1aa#4#4#4#4#4#4#4bPbPbPbPbQ#x.l#x#x#xbSbWbS#ObW.6#O#OaKaV",
        "#X#X#X.Mb.ambE#Kbqazaz#2bwbwaBaBa.a.aOaO#T#TaNaNbI#earbI#n#n#navb9b9ak.Kak#h#h#hasb6aib6aE.E#LaE#L#L.Lal.8.L.8.8.8.r.r#q.r.r#q.r.r.G.r.G.G#saX#saXaXbTaX.v.v.v#l.vaWaWaYaW#HaPaPaPbVaP.PbCada4a4.ebGaobbbB.Xa2aca2aMaMaM.5.5aLaL.ganbFb#.NbAabbA..bAbXbA#jatah#j.sahax.saxaxaeaxaeaebYaeaubYauaa#kaaaaa1b8#4#4#4#4#4#4#4bP#bbPbPbPbP.l#xbS#xbWbWbWbS#O#OaV#OaK",
        "#X#X.M.Mamam#Kamazbq#2bL#2#2aBbNa6a.a6aO#T#TaNaN#e#earar#n#n#n#nb9b9.K.K#h#h#h#hb6b6b6.E.E.E#L#L.L#L.L.L.L.L.8.8.8.8.r.r.r.r.r.r.r.r.G.G.G.G.GaXaXaXaX.v.v.v.v.vaWaWaWaWaPaWaPaPaPaPaPaPbC.Pa4a4bGbGbbbbbBbBbBa2a2a2aMaM#1.5aLaL.g.gbFbFb#b#bAbAbAbAatatatatatahahah.sahax#rax#raeaeaeaeauaeaaauaaaaaab8a1a1#4a1#4a1bP#4bPbPbPbP#x#x#x#x#xbW#xbS#ObW#O#O#O#OaK",
        "#X#X#X.M.Mamam#Kazazaz#2#2aBaBaBa.a.aOa.bf#TaNaN#e#eararbj#n#n#nb9#n.K.K.K#h#h#hb6b6.E.E.E.E#L#L#L#L.L.L.L.L.8.8bk.r.r.r.r.r.r.r.r#s.r.G.G.GaX.GaXaXaXaX.v#l.v.vaW.vaWaW#HaWaPaP.PaPaP.PbCbCa4a4bGbGbbaobBbBaca2a2.##1aM.5.5aLaL.g.gb#anb#b#bAbAbA..atbAat#jahahahahahaxaxaxaeaxaeaeaeaeauauauauaaaaaaa1a1a1#4a1#4#4bP#4bPbPbPbP#xbP#x#x#x#xbWbWbWbW#O#O.6aKaK",
        "#X#X.Mb.amam#KbE#Kazaz#2#2bwaBaBa.a6aOaO#T#TaNaN#e#ebIar#n#n#navb9b9.K.K#h.K#hb6b6b6b6.Eai.E#L#L#Lal.L.L.L.8.L.8.8.8.r#q.r.r.r.r.r.G.G.r.G.G.GaXbTaX.vaX.v.v.v.vaYaWaWaWaWaPaPaPaPaPaPaPbCbCa4a4bGbGaobbbBbBbBa2.#a2aMaM#1.5aLaL#E.gbFbFb#abbAbAbAbAbAatatatahat.sahaxahaxax#raxaeae.faeaubYauau#kaaaaa1a1#4a1#4a1#4#4bPbPbPbPbP#xbP#x#xbW#xbWbWbW.6#O#O#O#OaK",
        "#X#X#X.M.Mamam#Kazazaz#2#2aBaBaBa.a.aOaO#TbfaNaN#e#earar#n#n#n#nb9b9.K.K.Kas#h#hb6as.Eb6#L.E#L#L#L#L.L.L.L.L.8.8.8.r.r.r.r#q.r.r.r.r.G.G.G.GaX.GaXaXbTaX.v.v.v.v.vaWaYaWaPaWaP#HaP.PaPaP.Pada4a4.ebGbbbbbB.Xa2bBaMa2aMaM.5#1aLaLan.gbFbFb#b#bAbAbAbXbAat#jatatahahahax.saxaxaeaxaeaeaeaebYauauauaaaaaab8a1#4#4a1#4a1#4bP#4bPbPbPbQ#x#x#xbS#xbWbSbWbW#O#O#OaKaK",
        "#X#X.M.Mamam#K#K#Kaz#2az#2#2aBaBa6a.a6aObfaOaNaN#e#earbI#n#n#n#nb9b9.K.K#h.Kb6#hb6b6b6.E.E.E#L#L#L#L.L.L.L.L.8.8.8.8.r.r.r.r.r.r.G.r.G.G.G.GaX.GaXaXaX.v.v.v.v.vaWaWaWaWaWaPaWaPaPaPaPaPbCbCa4a4bGbGbbbbbB#wa2bBa2a2#1aM.5.5aLaH.g.gbFbFb#b#bAbAbAbAatbAatatahahahahaxahaxaxaxaxaeaeaeaeauauauauaaaaaaaaa1a1a1a1a1a1bPa1bPbPbPbP#x#x#x#x#xbWbWbWbW#ObW#O#O#OaK",
        "#X#X.M.Mamam#Kamazazaz#2#2aBaBaBa.a6aOa6#TbfaNaN#e#eararar#n#n#nb9b9.Kb9#h#h#h#hb6b6.E.E.E.E#L.E.L#L.L.L.L.L.8.8.8.8.r.r.r.r.r.r.r.G.r.G.G.G.GaXaXaX.vaXaX.v.v.vaWaWaWaWaPaWaPaPaPaPaPaPbCbCa4a4bGbGbbbbbBbBbBa2aMa2aMaM.5#1aLaL.g.gbFbFb#b#bAbAbAbAatatatahatahahahahaxaxaxaeaxaeaeaeaeauaeauauauaaaaa1a1a1#4a1#4a1bPaQbPbPbPbP#xbP#x#xbW#xbWbWbWbW#O#OaK#OaK",
        "#X#X.M.M.Mam#K#Ka5az#2az#2#2aBaBa.a.aOaObf#TbfaN#e#e#ear#nar#n#nb9b9.K.K#h.K#hb6b6b6b6.E.E#L#L#L#L#L.L.L.L.L.8.8.8.8.r.r.r.r.r.r.r.G.G.G.G.GaX.GaXaXaX.v.v.v.vaW.vaWaWaWaWaPaPaPaPbCaPaPbCbCa4a4bGbGbbbbbBbBa2bBa2a2#1aM#1.5#1aL#E.gbFbFb#b#bAbAbAbAbAatatatahahahahaxahaxaxaxaeaeaeaeaeauauauauaaaaaa.Ta1a1a1a1a1#4aQbPbPaQbP#xbP#x#x#x#xbWbWbWbWbW#O#O#O#OaK",
        "#X#X.M.Mamamam#Kaz#K#2#2#2aBaBaBa.a6aOa6#TbfaNaN#e#earar#n#n#n#nb9b9.Kag#h#h#8#hb6b6b6.E.E.E#L.E.L#L#Y.L.L.L.8aT.8.r.r#p.r.r.r.r.G.r.r.G.G.G.GaXaXaXaXaXaX.v.v.v.vaWaWaWaWaW#IaPaPaPaPbCbCbCa4a4bGbGbbbbbBbBbBa2aMa2aMaM#1.5aLaL.g.gbFbFb#b#bAbAbAbAatatatatatahahahahaxaxaxaxaxaeaeaeaeaeauauauaaaaaaa1a1a1a1#4aQ#4bPa1bPbPbPbPbP.w#x#x.w#x#xbW#ObW#O#O#O#OaK",
        "#X#X.M.Mamam#K#Ka5azaz#2#2bHaBaBa6a.a6aO#Tbf#TaNa7aNar#eb9ar#nb9agb9.K.K#h#h#h#8b6#8.E#U.E.EaD#LaD#L.L.L.LaT.L.8.8aT.r.r.r#p.r.r#p.r#u.G.G#u.GaXaX#u.vaX.R.v.R.vaWaWaWaWbCaWaPaPbCaPaPaPbCbCbGa4bGbGbbbb#wbBa2aAa2a2#1aM#1.5aLaH.g#EanbFb#b#b#bAbAbAbA#oat.Iahatb7ahaxahaxaxaxaeaxaeb4aeauauauau#iaaaaaaa1a1a1a1a1aQbPa1bPaZbP.w#xbP#x.wbWbW#RbWbW#R#ObWaK#OaK",
        "#X#X.M.Mamam#K#Kazaz#2azbH#2aBaBa.a6aOa6aObfaNbf#e#ea7ar#nar#n#nb9ag.Kag#hagb6#hb6b6.Eb6aD.E#L#L#L#L.L#Y.L.L.8aT.8.8.r#p.r.r.r.r.G.r.G#p.G.G.G#uaXaX#u.vaX.v.v.vaW.RaWaWaWbCaWaPaPbCaPbCbC#Ia4a4aybGbbbbbBbBbBa2aMa2aMaM#1.5#1aL.g.gbFawb#b#bAb#bAbA#oatatatahahahahaxb7axax.Aaxaeaeaeb4aeb4auauaa#iaaaQa1a1aQ#4aQ#4aQaQbPbPbPbPbP#x.w#xbW#x#xbWbWbW#O#O#O.xaK",
        "#X#X.x.MaSamaS#Ka5azbD#2#2#2aBaBa6a.aOaObf#TbfaN#e#earararb9#narb9b9.K#h#h#h#8#hb6b6#U.E.E.EaD#LaD.LaD.L.LaT.8.8.8#p.r.r.r.r#p.r#p.r.G.G.G#u.GaXaXaXaXaX.R.v.R.v.R.vaWaWaWaWaP#IaPaPaP#IbCbCbGa4bGaybbbbbBbBa2aAa2a2#1aM.5#1aLaL#E.gawanbFb#bAbAbAbAatatatat.Iatb7ahaxahaxaxaxaxae.Aaeaeauauauauaaaaaa.Ta1a1a1a1a1#4bPaQbPaQ#xaZ#x.w#x#x.wbW#RbWbW#R#O#O#O#OaK",
        ".x#X.M.Mamam#K#Kazaz#2azbHaBaBaBa.a6aOa6#TbfaNaN#e#eara7#nar#nb9b9ag.Kag#h#h#h#h#Ub6b6.E.E.E#L#L#L#L.L#Y.L.L.LaT.8.8.r#p.r.r.r.r.G.r.G#p.G.G#u.GaXaX#u.vaX.v.v.vaWaWaWbCaW#IaPaPaPaPbCaPbCbC#ta4bGbGbbbbbBbBaAa2aMa2#1a2.5#1aLaL#EaLbFawb#b#bAb#bAbAat#oatatahahahahb7axaxax.Aaxaeaeaeaeauae#iauaa#iaaa1a1a1a1a1aQa1aQbPbPaZbPbP#xbP.w#x#x#xbWbWbWbW#ObW.x#OaK",
        "#X#X.x.MaSama5#Ka5azbD#2#2bHaBbHa6a.a6aObfbfbfbf#eaG#eararb9arb9agb9.K.K#8#h#8b6b6#8.E#U.EaD.EaDaD.LaD.L.LaTaT.8aT.8#p.r#p.r#p.r#p#p.G#p.G#u.G#uaX#u.v#u.v.R.R.v.RaW.RaWaWaW#IaWbC#IaPbCbC#ta4bGayaybbbbaAbBa2aAa2a2#1aM#1.5aH#1.g#EbF#Eb#bF#obAbAb#at#oat.Iahatb7ahaxb7axaxax.A.Aae.Ab4aeauaub4aaau.TaaaQaQaQa1aQaQbPaQbPaQbPaZ.w#x#x.w.wbW.w#RbW#R#O#R#O#O.x",
        "#X.x.M.Mamama5#Kazaz#2#2#2bHaBaBa6a6aOaFaObfaNaNaN#eararararb9ar.Kagag#hag#h#8#h#Ub6.E#UaD.E#LaD#L#L#Y.L#Y.L.8aT.8#p.r#p.r#p.r#p.r.G#p.G.G#u.GaXaX#uaXaX.RaX.v.RaW.RaWaW#IaWbCaPaP#IaPbC#IbC#ta4bGbG#wbbbB#wa2a2a2a2aIaM.5#1aLaL#E.gawbFbFb#bAbA#obAat#oatat.Iahahb7axb7ax##axax.Aaeaeaeaub4#iau#iaa.TaaaQa1a1aQ#4aQbPaQaQbP.wbP.wbP.w#xbW.wbWbW#RbW#R#O#O.xaK",
        "#X#XaS.MaSam#KaSaza5#2bDbH#2aBaBa6a.a6aObfbfbfaNa7aG#eagarb9arb9b9ag#hag#h#h#8#8b6#8#Ub6.EaD.EaD.LaD.L#Y.L.LaT.L.8#p.r.r#p.r.r#p.r#p.G#p.G#u.G#uaXaX.RaX.R.v.R.vaW.RaW.RbCaW#IaPbCaP#IaPbC#ta4bGaybGbbbbaAbBa2aAaMa2#1aM#1#1.5aL#EaLawbF#obFb##obAbA#oatat.Iah.Iahahb7axaxax.Aaxae.Aaeb4b4auauauaa#iaa.Ta1aQa1aQa1aQaQbPbPaZbPaZ#x.w#x.w#x#RbW.w#ObW#O#R#O#O.x",
        ".x#X.M.MamaS#Ka5azazbD#2#2bHbHaBa.a6aOa6bf#TaNbf#e#ear#eb9arb9arb9ag.K.K#8#hb6#8b6b6.E#U.E.EaD#LaD#L.L#Y.L#Y.8aT.8aT.r#p.r.r#p.r#p.G#p.G#u.GaX#uaX#uaX#u.vaX.R.v.RaWaWaWaWaWaP#IaP#IaPbC#tbC#ta4bGay#wbbbB#wa2a2#Pa2aMaI.5#1aLaH.g#EbFawbF#obAbAbAbA#oat.Iatatahb7ahaxb7axb7aeax.Aaeaeaeauae#iau#iaaaa.Ta1a1aQa1aQaQbPaQbPaZbPaZ#xaZ#x.wbW.wbWbW#O#R#R#O#O.xaK",
        "#X.x.xaSaSaS#Ka5azbD#2bDbH#2aBbHa6a6a6aObfaFbfaNaGaNa7aragarag#nagag.Kag#8#h#8#8b6#U#U#U.EaDaD.E#YaD#Y#LaT.LaTaT.8#p.r#p#p#p.r#p#p.r#p.G#p#u#u.G#uaX.RaX.R.R.v.R.R.R.RbC.R#I#IaP#IaP#IaPbC#tbG#tbbbGbb#wbBaAa2aAa2aIaMaIaM#1aLaHaL#E.gawbFb##ob##obA#oat#oatb7.Iahb7b7axax##ax.Aae.A.Ab4b4b4au#iau#i.TaaaQaQaQaQaQaQaQaQaZaQaZbP.waZ#x.w.wbW.w#RbW#R#O#R.x#O.x",
        "#X.x.M.MamaS#Ka5bDazbD#2#2bHbHaBa.a6aOa6aObfaNbfa7a7ag#eagar#nag.Kag#8.K#8#h#8#h#U#8.E#U.E#U#LaD#LaD.L#Y.L#Y.LaT#paT#p.r#p.r#p.r#p.G#u#p.G#u.G#u#u#uaX#u.RaX.R.RaWaW.RaW#IaW#IaP#IaP#IbC#t#I#ta4ayaybb#wbB#wa2#Pa2a2aIaM#1#1aHaL#EaLawawawb#b##obAb#at#o.Iat.Iahahb7axb7##ax.A##ax.Aaeaeaub4au#iaa#iaa.TaQaQaQa1aQaQaQbPaZbPaZaZ#x.w.w#x.w.w#RbW#RbW#R#O#R.x#O",
        ".M.x.MaSaSaSa5#Ka5azbD#2bH#2a6aBa6a6a6aF#TaF#TaNaNaNa7aragarag#nagb9.Kag#8#h#8b6#8b6#U#U.EaD.EaDaD#L#Y#Y.LaTaT.8aT.8#p#p.r#p#p.r#p#p.G#p#u.G#uaX#uaX.RaX.R#uaW.R.v.RaW.RbCaW#IaWbC#IaP#IbC#ta4aybGaybb#wbBaAa2aAaM#P#1aM#1.5#1aL#E#EbF#Eb#awbA#obA#o#oatat.I.I.Ib7ahb7axax##ax.Aae.Aaeb4b4b4aub4aa#iaa.Ta1aQa1aQaQaQaQaQaZbPaZbP.waZ#x.wbW.wbW.w#R#O#R#O#O#O.x",
        "#X.x.M.xamaS#Ka5azbD#2bDbH#2aBbHa6a.aFa.bfbfbfaGaGa7ara7ar#nag#nagag#8.K#8#h#8#8b6#U.E#UaD.EaDaD#L#Y#L#Y#Y.LaTaT.8#p.r#p.r#p.r#p.G#p#p.G#p.G#u#uaX#u#uaX.RaX.R.RaW.R#IaWaW#I#IaP#IaP#IaPbC#ta4#tbbaybb#wbBaAa2#Pa2a2aIaM#1#1aL#1#E#E.gawb#awbA#ob#bAat#oat#oahahb7ahb7b7ax##axax.Aae.Ab4aeb4au#iaa#i.TaaaQaQaQa1aQaQbPaQbPaZaZaZ.w#x.w.w.w#x#R#RbW#R#O#R.x#O.x",
        ".x#XaS.MaSaSa5a5bDbDbD#2bHbHbHaBa6a6a.aFaObfbfaNaNa7a7a7agaragarag.Kagag#8#8b6#8#U#8#U#U.E#UaDaDaDaD#Y.L#Y#YaT.LaTaT#p#p#p#p#p#p#p#p.G#u#u#p#u.G#u#u.v#u.R.R.v.R.R.R.RaW#IaW#I#IbC#I#I#I#tbCaya4aybb#wbbaAaAaAa2#PaIaMaI.5#1#1aL#EaLaw#Eb#awb##obA#o#o#o.Iat.I.Iahb7axb7##ax.A##.A.Aaeb4b4aub4#i#iaa.T#iaQaQaQaQaQaQaQaZaZaQ.wbP.waZ.w.wbW.w#RbW#R#R#R#O#R.x#O",
        "#X.x.xaSaSaS#Ka5azaz.7bD#2bHaBbHaBa6a6a6bfbfbfaNaGa7a7aragaragb9agagag#h#8#h#8#8#Ub6.E#UaD.EaD.E.LaD#L#Y#Y.LaTaT#paT.r#p.r#p.r#p.G#p#p#p#u.G#u#u#u#u.R#u.v#u.R.R.RaW.R#I.R#I#IaP#IaP#IbCbC#t#tbGayay#w#wbBaAa2#Pa2a2aIaM#1#1aL#1#E#Eawawawb##ob##obA#oat.Iat.Iahb7b7##b7ax##.Aax.Aae.Aaeb4b4au#iau#i.TaaaQaQaQaQaQaQaZaQaZaZaZaZ.w.w.w.w.wbW.w#RbW#R#O#R.x#O.x",
        ".x#XaS.MaSa5a5a5azbD#2.7bH#2bHa6a6a.a6aObfbfbfaNaGaNa7a7arag#nagag.K#8ag#8#8#8b6#8#U#U#U.EaDaDaDaDaD#Y#Y.L#YaT.LaT#p#p#p#p#p#p#p#p#p.G#p#u#p#uaX#uaX#u#u.R.R.v.R.v.RaW.RbCaW#I#IaP#I#I#I#I#t#t#tbbaybb#waAbB#PaAaIa2#1aIaM#1aHaL#EaL#EawbF#ob##obA#o#o#o#o.Iah.Ib7ahb7ax####ax.A.A.Aaeb4b4b4#iau#i#iaa.T.TaQaQaQaQaQaQaQaZbPaZbP.w.w#x.w#R.w#RbW#R#R#R#O#R.x#O",
        ".x.M.xaSaSaS#Ka5bDbDbD#2#2bHbHaBa6a6bfa6aF#Tbfbfa7a7a7aragaragaragag#hag#h#8b6#8#Ub6#U#UaD#U.EaDaD#L#Y.L#Y#YaTaT.8#p#p.r#p#p.r#p.G#p#p.G#u.G#u#u#u#u.v#u.v#u.R.R.R.RbC.RaW#IaW#I#IbC#IbCbCa4#tbGaybb#wbbaAaAa2#Pa2a2aIaM#1#1aL#1#E#EawbFaw#ob##obA#o#oat.Iat.Iahb7b7axb7##ax.A##axae.A.Aaub4#ib4aa#i.T.TaQaQaQaQaQaQaQbPaZaZaZ.waZ.w.w.w.w#x#R.wbW#R#O#R.x#O.x",
        ".x#XaSaSaSaSa5a5bDbD#2.7bHbHbHaBa6aBa6a6bfbfbfbfaNaGa7a7agagagagagagag#8#8#8#8#8#U#8#U#U#UaDaDaDaDaD#YaD#YaTaTaTaTaT#p#p#p#p#p#p#p#p#u#p#u#u#u#u#u#u#u.R.R#u.R.R.R.R.R.R#I#I#I#IaP#I#I#I#t#t#tayayay#w#waAaAaA#Pa2aIaMaI#1#1#1aHaL#E#E#Eawawb##ob##o#o#o.I.I.I.Ib7b7b7b7######.A.A.A.Ab4b4b4au#i#i#i.T#i.T.TaQ.T.TaQaZ.TaZaQ.waZ.waZ.w.w#R.w#R#R#R#R#R#R#R.x.x",
        ".x.x.xaSaS#Ka5a5bDbDbD.7#2bHbHbHa6a6aFa6bfaFaNbfa7a7a7a7aragaragagag#8ag#8#8#8#8#U#U#U#UaD#UaDaDaDaD#Y#Y#Y.L#YaTaTaT#p#p#p#p#p#p#p#p#p#p#u#p#u#u#u#u.R#u#u.R.R.R.R.R#IaW#I.R#I#I#I#I#IbC#t#Iay#tbbaybb#waAbB#P#P#Pa2aIaM#1#1aL#1#E#E#Eawawb##ob##o#o#o#o#o#o.Iahb7b7b7##ax##.Aax.A.Aae.Ab4b4b4#i#i#i.TaaaQaQ.TaQ.TaQ.TaZaZaZaZaZ.w.w.w.w.w.w#R.w#R#R#R#R#O#R.x",
        ".x.MaS.xaSaSa5a5bDbD#2bDbHbHaBbHa6a6a.aFaObfbfbfaGaNa7agagaragagagag#hag#8#8#8#8b6#U#U#U#U.EaDaDaDaD#Y#Y#Y#YaTaTaT#p#p.r#p#p#p#p#p#p#u#p#u#u#u.G#u#u#u.R.RaX.R.R.R.R.R.R#I#I#I#IbC#I#I#I#I#t#tayayay#w#waAaA#P#Pa2aIaIaIaM#1aH#1.g#E#Eawawaw#o#obA#o#oat.Iah.I.Ib7b7b7ax######.A.A.A.Ab4b4b4#iau#i#i.T#iaQ.TaQ.TaQaQaZaQaZaZaZaZ.w.w.w.w#R.w#RbW#RbW.x#R.x#R.x",
        ".x.x.xaSaS#Ka5a5bDbD#2.7#2bHapaBa6a6aFa6aFbfbfbfa7a7a7#eagaragagagagag#8#8#8b6#8#U#8#U#UaD#UaDaDaD#YaD.L#Y#YaTaT.8aT#p#p#p#p#p#p#p#p#u#p#u#p#u#u#u#u.R#u#u.R.R.R.R.R.RbC.R#IaW#I#I#I#IbC#t#t#tbGaybb#w#wbBaA#P#P#Pa2aIaM#1#1aL#1#E#E#Eawawb##ob##o#o#o#o.I#oah.Iahb7##b7########.Aae.A.Aaub4b4#i#iau.T.T.T.T.TaQaQ.TaZ.TaZaZ.waZaZ.w.w.w.w#R.w#R#R#R#R#R.x#O.x",
        ".x.xaSaSaSa5a5a5bDbD.7.7bH.7aBapa6a6a6a6bfbfbfbfaGaN.qa7aga7agagag#8ag#8#8#8#8#8#U#U#U#U#UaDaDaDaDaD#YaD#Y#YaT#YaTaT#p#p#p#p#p#p#p#p#p#u#p#u#u#u#u#u#u.R.R#u.R.R.R.R#I.R#I.R#I#IbC#I#I#I#t#tay#t#way#w#waAaA#P#Pa2aIaIaI#1#1#1aH#E#E#E#Eawaw#o#ob##o#o#o.I.I.I.Ib7b7b7######.A##.A.A.Ab4b4b4#ib4#i#i.T#i.T.TaQ#SaQ.TaZ#SaZaZaZaZ.w.w.w.w.w.w#R#R#R#R#R#R.x#R.x",
        ".x.xaSaSaSaSbDa5bDbD#2bDbHbHa6aBa6aBaFa6bfaFbfbfaGaGa7a7agagagagagag#8ag#8#8#8#U#8#U#U#UaD#U#UaDaDaD#Y#Y#Y#YaTaTaTaT#p#p#p#p#p#p#p#p#u#p#u#u#u#u#u#u.R#u#u.R.R.R.R.R.R.R.R#I#I#I#I#I#I#I#t#t#tayay#w#w#waAaA#P#P#P#PaMaI#1#1aH.5aH#E#Eawawawb##o#o#o.I#o.I.I.Ib7b7b7##b7####.A##.A.A.A.Ab4b4b4#i#i#i.T.TaQ#SaQ.TaQ#SaQaZaZ#SaZaZ.w#m.w.w#R.w#R.w#R#R#R#R.x#R.x",
        ".x.xaS.xa5aSa5a5bDbD.7.7#2.7bHbHa6a6aFa6bfbfbfbfa7aNa7a7a7agaragagag#8ag#8#8#8#8#U#U#U#U#UaDaDaDaDaD#YaD#Y#YaT#Y#p.c#p#p#p#p#p#p#p#p#u#p#u#p#u#u#u#u#u.R.R#u.R.R.R.R#t.R#I#I#I#I#I#I#I#I#t#tay#t#way#w#waAaA#PaAaI#P#1aI#1#1#1aH#E#E#E#Eawaw#o#ob##o#o#o.I#o.I.Ib7b7b7########.A.A.A.Ab4b4b4#i#i#i#i.T#i.T.T.T.T#SaQaZ.TaZaZ.w#maZ.w.w#m.w#R#R#R#R#R#R#R.x#R.x",
        ".x.xaS.1aSa5a5bDbDaf#2.7bHbHbHbHa6a6a6a6bfaFbfbfaGaGa7a7aga7agag#8ag.q#8#8#8bZ#8#U#8.h#U#U.haDaD#aaD#Y#Y#Y#Y.naTaTaT#p#p#p#p#p#p#p#p#p#u#p#u#u#u.O#u.R#u#u.R.R.R.R.R.R.R#I.R#I#I#I#I#I#I#t#t#tayay#w#w#waAaA#P#Pa2aIa2aI#1#1aH.5aH#E#Eawawawb##o#o#o#o#o.I.Ib7.Ib7b7##b7####.A####.Ab4.Ab4b4b4#i#i#i#i.T.T.TaQ#SaQaQ#SaZaZ#SaZaZ.w#m.w.w#R.w#m#R#R#R#R#R.x#R.x",
        ".x.x.1aSaSa5a5a5bDbD.7.7.7bHbHa6a6a6a6aFaFbfbfbfaGaGa7a7aga7aga7agag#8ag.9#8#8#U#8bZ#U#U.haDaD#UaDaDb0aD#Y#YaT.naTaT#p.c#p#p.c#p.c#p.O#p.O#u.c#u#u#u.O.R.H#u.H.R.R#t.R#t.R#t#I#I#I#I#I#I#ta0ayayayay#waAaAaA#P#Pbra2aIaM#1#1#1aH#EaH#E#Eawaw#o#o#o#obJ#o.I#o.Ib7ajb7b7########.A.A.A.A#Vb4#5b4#i#i#i.T#S.T#SaQ#SaQ#SaZ#SaZ#maZ#m#m.w#m.w#m#R#R#R#R#R#R#R#R.1.x",
        ".x.xaSaSaSa5a5afbDbD.7bDbHbHapbHbHa6aFa.aFaFbfaGaNaGa7a7a7agagagagag.9#8#8#8#8.9#U#U#U#UaD#U#aaD#aaD#Y#Y.n#Y#YaTaT.c#p#p#p.c#p#p#u#p#p#u#p#u#u#u#u.O#u#u.R#u.R.R.R.R.R.R#I.R#Ia0#I#I#t#I#t#t#tayay#w#w#waAaAaA#P#PaIaIaI#1#1aH#1#EaHaw#Eawaw#oaw#o#o#o#o.I.Ib7.Ib7b7##b7#3##.A#3.A.Ab4.Ab4b4b4#5#i#i.T#i.T.T#SaQ#S.T#SaZ#SaZ#maZ.w.w.w#m#R#m#R#m#R#R#D#R.x#R.x",
        ".x.x.1aS.1a5a5a5bDbD.7#2ap.7a6bHa6a6a6aFaFbfbf#.aGaGa7a7aga7aga7ag#8ag#8.9#8#U#8bZ#U.h#U#UaDaDaDaDaDb0aD#Y#YaT.naTaT#p.c#p#p.c#p.c#p.O#p.O#u.O#u#u#u.R.O.R.H.R.O.R.H.R#t.R#I#I#I#I#I#I#I#ta0ayayay#w#w#waAaA#P#P#P#PaIaI#1#1#1#1#E#E#E#Eawaw#o#o#oaw#o.IbJ.I.I.Ib7b7#3b7########.A.A#V.Ab4#5b4#i#i#i#S#i#S.TaQ#SaQ#SaZ.TaZ#maZaZ#m#m.w.w#R.w#R#R#R#R#R#R#R.x#R",
        ".x.xaSaSa5a5a5afbDbD.7.7.7bHbHbHa6a6a6a6bfaFbfbfbfaGa7aGaga7agag#8ag.q#8#8.9#8#8#U#U#U#U.h#U.haD#aaD#Yb0#Y#Y.naTaTaT.c#p#p#p#p#p#p#p#u#p#u#p#u#u.O#u.O#u.R#u.R.R.R.R.R.Ra0.R#Ia0#I#t#I#t#t#t#tay#way#waAaAaA#P#P#PaIaMaI#1#1aH#1#EaH#E#Eawaw#oaw#o#o#o#o.I.I.I.I#3b7b7######.A#3.A.A.A.Ab4b4b4#i#5#i#S.T.T.T#SaQ#S.TaZaZ#SaZaZ#m.w.w.w#m#m#R#m#R#R#D#R#R.x#D.x",
        ".x.1.1aS.1a5a5a5bDbD.7.7bHbHapapa6.pa6aFaFaFbfaGaGaGa7a7a7.qaga7.q.q#8ag.9#8bZ#8bZ#UbZ#U#aaDaD.haD#a#YaD#Y.naT.naT.c#p.c.c#p.c.c#p.c.c.O#u.c.O#u.O#u.R.O.R.H.R.H.R.H#t.R#Ia0.R#I#Ia0#Ia0#ta0aybKay#wbm#wbMaAbr#Pbr#PaIaI#1aI#1aHaH#E#EbpawbJaw#obJ#obJ.IbJ.I.I.Ib7aj###3#3###3.A.A.A#V.A#5b4#5b4#i#i#S#i#S#S.T#S#SaQ#S#SaZ#m#m#S#m#m#m.w#R#m#R#m#R#R#D#R.1#R.x",
        ".1.xaSaS.1a5afa5.7bD.7.7.7bHbHa6apa6aFa6aFaF#.bfaGaGaG.qagag.qagagag#8.9#8#8bZ#8#UbZ#U.h#U#U#aaD#aaD#Yb0.n#Yb0aT.caT.c#p#p.c#p#p.c#p#u.c#u.O#u#u.O#u.O.R.O.R.O.R.H.R.R.H.Ra0#Ia0#I#Ia0#I#ta0ayaybK#w#waAaAaA#P#PaI#PaIaI#1#1aH#1#EaH#E#Eawaw#oaw#o#obJ#o.IbJb7ajajb7#3b7####.A###3.A.A#Vb4#5b4#5#i#5#i.T#S.T#S.T#S#SaZ#S#m#SaZ#m.w#m#R#m#R#m#R#R#D#R.x#D.x#R.1",
        ".x.x.1.1a5a5.ja5bDaf.7#2apbHapbHa6a6a6aFaFbfbfbfaGaGa7aGa7agaga7.9.q#8ag.9#8#8bZ#UbZ#U.haD.haDaD#aaDb0aD#Yb0aT#Y.caT.c#p.c#p.c#p.c#p.c#u#p.O#u.O#u.O.R.O.R.O.R.R.R.H#t.R#I.H#Ia0#I#t#Ia0#ta0ayay#waybm#wbMaAbraAaI#PaIaI#1#1#1#1aH#Ebp#EbJawbJ#o#oaw#o.IbJ.I.Ib7b7b7###3###3###3b4.A#V.Ab4#V#ib4#i#i#S#S.T#S.T#SaQ#S#SaZ#maZ#m#maZ#m#m#m#R#m#R#m#R#D#R#R#D.x.x",
        ".x.1aSaS.1a5a5afbD.7.7.7.7apbHbH.pa6.pa6aFaF#.bfaGbfa7a7.qa7ag.qagag#8.9#8.9#U.9#UbZ#U#U.haD.haD#aaD#Yb0#Yb0aT.naT.c#p#p.c#p#p.c#p#u.c#u.c#u.O#u.O#u#u.H#u.R.H.R.H.R.R.Ra0.R#I#Ia0#I#t#Ia0#tbK#tbm#w#wbMaAaA#P#P#PbraMaIaIbv#1aH#EaH#Ebpawaw#o#obJ#o#obJ.I.I.Iajb7aj#3b7#3##.A#3.A.A#V.Ab4#5b4#5#i#5#i#S.T#S#S.T#S#SaZ#S#SaZ#maZ#m.w#m#R#m.w#D#R#D#R#R.1#R#R.1",
        ".1.x.1.1a5.jafa5afbD.7.7bHapbHapa6apaFaFaFaFbf#.aGaG.qaGag.qag.q.q#8.q#8.9.9#8.9bZ#U.h.h#U.h#aaD#a#a#a#Yb0#Y.n#Y.caT.c.c#p.c.c#p.c.c#u.c#u.c#u.O#u.H.O#u.H.O.R.H.R.Ha0.H#Ia0a0#Ia0#Ia0#Iaya0aybK#wbKaAbmaAbMbrbraIbraIaI#1#1bvaHaHaHbp#EbJbpbJaw#obJbJ.IbJ.Iajajb7#3b7#3###3#3.A#3#V.A#V#Vb4#5#5#i#5#S#i#S#S#S#S#S#S#S#m#m#S#m#m#m#m#m#m#D#R#m#R#D#R#D#R.1#D.x",
        ".x.1.1aS.ja5a5af.7af.7.7apbHapapa6.pa6.paFaFbf#.aGaGaGa7.qa7.qag.q.q#8.9.9#8bZ#8#UbZ#U.h.haD.haD#aaD#a#Yb0b0#Y.n.c.c#p.c.c#p.c.c#p.c.c#p.O.O.O#u.O.O.R.O.H#u.H.R.H.H.R.R.Ha0.Ra0#Ia0a0#ta0a0aybK#wbm#wbMaAbM#P#Pbr#PaIaIaIbv#1aHaHaH#EbpawbJ#o#obJ#o.IbJ.I.Iajb7ajb7#3b7#3##.A#3.A.A#V.A#Vb4#5b4#5#i#ibx#S#S#S#SaZ#S#SaZ#S#m#maZ#m#m#D.w#m#D#m#D#R#D#D#R#D#R.1",
        ".1.x.1aS.j.ja5afafbD.7.7bH.7.pbH.pa6.paFaFaF#.bfaG#.a7aG.qag.qag.qag.9#8.9.9bZ#8bZbZ#U.h#U.h#aaD#aaDb0b0#Yb0.naT.naT.c#p.c.c#p.c.c#u.c.O#u.c#u.O.O#u.H#u.H.H.R.H.R.H#t.H#I.H#Ia0#Ia0#Ia0#tbKaybKaybm#wbmaAbM#Pbr#PaIaIaI#1bv#1bvaH#Ebp#EbJ#EbJaw#obJ#obJ.Iaj.Iajb7#3b7#3###3#3.A#3#V.A#Vb4#5#5b4bx#5#S#i#S.T#S#S#S#S#S#m#S#m#m#m#m#m.w#m#R#m#R#D#R#D#R#D.x#D.x",
        ".x.1a5.1a5.ja5afbDaf.7.7apapbHapa6apaFa6aFaFbf#.aGaGaG.qa7ag.qag.q#8.q.9#8.9#8bZ#UbZ.h#U#a.haD#a#a#Y#a#Yb0#Y#Y.naT.c#p.c#p.c#p.c#p.c.c#u.c#u.O#u.O#u.O.O.R.O.R.O.H.R.H.Ra0.Ra0a0#Ia0#I#ta0a0aybK#waybM#wbraAbM#PaI#PaIaI#1bv#1aHaHaHbp#EbJawbJ#obJ#obJbJaj.Iajajb7ajb7#3###3.A#3.A.A.A#Vb4#Vb4#5#i#5#i#S#S#S#S.T#S#SaZ#S#m#SaZ#m#m#m#m#m#D#m#R#m#D#R#D#R.1#R.1",
        ".1.1.1.1a5.jafa5.7af.7.7bH.7apap.pa6.paFaFaF#.#.bfaGaG.q.q.qa7.q.q.q#8.9.9#8bZ.9bZ#U.h.h.haD.h#aaD#ab0#a.nb0.n.n.c.n.c.c.c.c.c.c.c#u.c.c.O.O.O.O.O.O.H#u.H.O.H.H.R.H.H.Ra0a0a0#Ia0a0a0a0a0aybKaybmbm#wbMaAbMbr#PbrbraIaIaIbvbvbvaHaHbpbp#EbJbJaw#obJbJ.IbJ.Iajajb7#3#3#3###3#3.A#3#V#V#V#Vb4#5#5#5#ibx#i#S#S#S#S#S#S#S#m#S#m#m#m#m#m#m#D#m#D#D#R#D#D#D#D#D.1#R",
        ".1.x.1aS.j.ja5afaf.7.7.7apapapa6ap.paF.paFaFbf#.#.aGaG.qa7.q.q.qag.9.q#8.9.9.9#UbZbZ.h#U.h.haD#a#a#a#Y#a#Y.n#Y.naT.c#p.c.c#p.c#p.c.c.c#u.c#u.O#u.O#u.O.H.O.H.R.H.H.H.R.Ha0.Ra0a0#Ia0#I#ta0a0aybKbK#wbM#wbMaAbr#PaIbraIaIbv#1aHbv#EaHbp#EbJbp#obJbJbJ#obJ.Iajaj.I#3aj#3b7#3#3.A#3#V#3.A#Vb4#V#5#5#i#5#ibx#S#S#S#S#S#S#m#S#m#S#m#m#m#m#m#m#R#m#D#m#R#D#R#R.1#R.1",
        ".1.x.j.1a5.jafafafaf.7.7bH.7apbH.pa6.p.paFaF#.#.aGaGaG.q.qag.qag.9.q#8.9.9#8bZ.9bZ#U.h.h.h.h#aaD#aaD#ab0b0b0.n.n.n.c.c.c.c.c.c.c.c#u.c.O.O.c.O.O.O.O.H#u.H#u.H.H.R.H#t.H.Ra0a0.Ra0a0a0a0a0aybKaybmbm#wbMbMbM#PbMbr#PaIaIaIbv#1bvaHaHbp#EbJ#EbJ#obJ#obJbJ.Iaj.Iajb7aj###3#3###3.A.A#V#V.A#V#5b4#5#5#5bx#S#Sbx#S#S#S#S#S#mbx#m#m#m#m#m#D#m#m#D#R#D#D#D#D#D#R.1#D",
        ".x.1.1aS.j.jafa5.7bD.7.7apapbH.pap.p.paFaFaF#.bf#.aG.qaGa7.q.qa7.9ag.9.9.9bZ.9#UbZbZ.h#U#a#U#a.h#a#a#a#Yb0#Y.n#Y.caT.c#p.c#p.c#p.c.c.O.c#u.O#u.O#u.O#u.H.O.H.O.H.H.R.H.Ha0.Ra0a0a0a0#I#tbKa0aybKbm#wbmaAbMaAbr#PbraIaIaIbv#1bvaHaHaHbpaHawbJbJawbJbJ.IbJ.Iajaj.I#3aj#3b7#3###3#3#V#3.A#Vb4#V#5b4#5#i#i#5#S#S#S#S#S#S#m#S#m#S#m#m#m#m#m#m#m#m#D#m#R#D#R#D.1#R.1",
        ".1.1.1.j.j.jafafaf.7.7.7apapapapa6.p.p.paFaF#.#.aG#.aGaG.q.q.q.q.q.9.9.9.9.9bZ.9bZbZ.h.h.h.h#a#aaD#ab0#ab0b0.n.n.n.c.c.c.c.c.c.c.c.c.O.c.O.O.O.O.H.O.H.O.H.O.R.H.H.H.H.Ra0a0a0a0#Ia0a0a0a0bKbKbK#wbmbMbmbMbMbrbrbrbraIaIaIbvbvbvaHaHbpbpbpbJbJbJ#obJbJbJajajajajajb7#3#3#3#3.A#3#V.A#V#V#V#V#5#5#5#5bx#i#Sbx#Sbx#Sbxbx#m#S#m.U#m#m#m#D#m#D#D#m#D#D#D#D#D#D.1#D",
        ".1.1.j.1a5.ja5afaf.7.7.7.7apapap.papaF.paFaF#.aFaG#.aG.q.q.q.qag.9.q.9.9#8.9bZbZ#UbZ.h.h.h#a.h#a#a#ab0b0b0b0.n.n.n.c.c.c.c.c.c.c.c.c.O.c.O.c.O.O.O.O.O.O.O.H.H.H.H.H.H.Ha0.Ha0a0a0a0a0a0bKa0#wbKbmbmbmbM#PbM#PbraIbraIaIaIbvbvbvaHaHbpbpbpbJawbJbJbJbJbJaj.Iajaj#3aj#3#3#3#3#3#3#V#3#V.A#5#V#5#5#5#5#5bxbx#Sbx#Sbx#S#m#S#mbx#m#m.U#m#m#m#D#m#D#D#D#D#D#D.1#D.1",
        ".1.1.1.j.j.jafafafaf.7a9apapapap.p.p.p.paFaF#.#.#.aGaGaG.q.q.q.q.q.q.9.9.9.9bZ.9bZbZ.h.h.h.h#a#a#a#ab0#a.nb0.n.n.n.c.c.c.c.c.c.c.c.O.c.O.O.O.O.O.O.O.H.O.H.O.H.O.R.H#t.H.Ha0.Ha0#ta0a0a0a0bKbKbK#wbm#wbMbMbMbr#PbrbraIaIbvaIaHbvaHaHbpaHbJbpbJbJ#obJbJ.Iajajajajb7#3#3b7#3#3#V#3#V.A#V#V#V#Vb4#5#5#5#S#5#Sbx#S#S#Sbxbx#mbx#m.U#m#m.U#m#D#m#D#m#D#R#D#D#R#D#D.1",
        ".1.x.j.1.ja5.jaf.7.7.7.7apapapbH.p.p.paFbuaF#.#.aG#..qaG.q.q.q.q.9.q.9.9.9.9bZbZbZbZ.h.h.haD.h#aaD#a#a#Yb0b0#Y.n.c.n#p.c.c.c.c.c.c.c.O.c.c.O.O.O.O.O.O.H.O.R.H.H.H.H.H.Ha0.Ra0a0#Ia0a0#ta0bKaybKbmbmbM#wbMbMbrbrbraIaIaIaIbvbvbvaHaHbpbpbpawbJbJbJbJbJbJajajajajajaj#3#3#3#3#3###V#3#V#V#V#V#5#5#5#5#S#5#S#Sbx#Sbx#Sbx#S#m#m#i.U#m#m#m#m#D#m#D#m#D#D#D#D.1#R.1",
        ".1.1.j.j.jafafafafafa9.7apapapap.papaF.paFaFbcaF#.aG#NaG.q.q.q.q.q.9.9.9.9.9bZ.9.hbZ.h.h.h.h#a#a#a#ab0#ab0b0.nb0.n.n.c.c.c.c.c.c.c.O.c.O.O.O.O.O.O.H.O.O.H.H.H.O.H.H.H.Ha0.Ha0.Ha0a0a0a0bKbKbKbmbmbmbmbMbMbMbrbrbrbraIaI.ybvbvbvaH#AaHbpbpbpbJbJbJbJajbJajajajaj#3#3aj#3#3#3.A#3#V#3#V#V#V#V#5#V#5#5bx#5bxbxbxbx#Sbx#mbx.Ubx.U#m.U.U#D.U.U#D#D#D#D#D#D#D#D.1#D",
        ".1.1.1.j.j.jafaf.7a9.7.7apapapap.p.p.p.paF.p#.#.#N#.aGaG.q.q.q.q.9.q.9.9bZ.9bZbZbZbZ.h.h.h#a.h#a#a#a#ab0b0b0.n.n.n.c.c.c.c.c.c.c.O.c.c.O.c.O.O.O.O.O.H.O.H.O.H.H.H.H.H.Ha0.Ha0a0a0a0a0a0bKbKbKbKbmbmbMbMbMbMbrbrbr#6aIaIaIbvbvbvaHaHbpbpbpbJbJbJbJbJbJbJajajajaj#3aj#3#3#3#3.u#3#3#V#V#V#V#V#5#5#5#5bx#5bx#Sbx#Sbxbxbxbx#m.U#m.U.U#m.U#m#D.U#D.U#D#D#D#D#D#D.1",
        ".1.1.j.1.j.jafafaf.7a9.7apapapap.pap.pbuaFbu#.aFaG#.#NaG.q.q.q.q.q.q.9.9.9.9bZbZbZbZ.h.h.h.h#a#a#a#ab0#ab0b0.n.n.n.n.c.c.c.c.c.c.c.c.O.O.O.O.O.O.O.O.H.O.H.O.H.H.H.H.H.Ha0.Ha0a0a0a0a0a0bKa0bmbKbmbmbmbMbMbMbrbrbrbraIaI.ybvbvbv#AaHbpbpbpbpbJbJbJbJajbJajajajaj#3aj#3#3#3#3.A#3#V#3#V#V#V#V#5#V#5#5bx#5#Sbxbx#Sbx#mbx.Ubxbx.U#m.U.U#D.U#m#D#D#D#D#D#D#D.1#D.1",
        ".1.1.1.jaf.jafafafa9.7.7apapapap.p.p.p.paFaF#.#.#.aGaG#N.q.q.q.q.q.9.9.9.9bZ.9bZbZbZ.h.h.h#C.h#a#a#a#abab0b0ba.n.n.c.c.c.c.c.c.c.O.c.O.c.O.c.O.O.H.O.O.O.H.O.H.H.H.Ha0.H.Ha0.Ha0a0a0a0a0bKbKbmbKbmbmbMbMbMbMbrbrbr#6aIaIaIbvbvbvaHaHbpaHbJbpbJbJbJbJajbJajajajaj#3aj#3#3#3#3#3#V#3#V#V#V#V#V#5#5#5#5bx#5bxbx#Sbxbxbx#mbx#m.U#m.U#m.U#m.U#D.U#D.U#D#D#D#D#D.1#D",
        ".1.1.j.j.j.dafbd.7afa9a9ap.7beap.pbe.p.pbuaFbcaF#.#.aGaG.q#N.q.q#N.q.9#N.9.9bZbZbZ#G.h.h#C.h#a#C#a#aba#ab0ba.nb0.n.V.c.c.V.c.V.c.V.c.V.O.V.O.O#f.O.O.Y.H.O.H.H.O.H.H.Y.HbK.Ha0bKa0#Ma0a0bKbKbKbmbmbmbmbMbMbrbrbrbrbraI#6.yaIbv#AaHbvbp#AbpbpbJbJbJbJbJbJajajajaj#3aj#3#3#3#3#3.u#V#3#V#V#V#V#5#V#5#5bxbxbxbxbxbxbxbxbxbx.Ubx.U.U.U.U#D.U.U#D.U#D#D#D#D#D.d#D.1",
        ".1.d.1.daf.jafafafa9.7.7apapapapap.p.pbuaFbuaFbc#.#NaG#N.qaG.q.q.9.9#N.9bZbZ#NbZ.hbZ.h.h.h.h.h#a#a#a#ab0b0b0.n.n.n.c.V.c.c.c.c.c.c.c.O.c.O#f.O.O.O.O.O.O.Y.O.Y.H.Y.H.H.H.Ha0#M.Ha0a0a0a0#MbKbmbKbmbmbMbMbMbMbrbrbr#6aIaIaIbvbvbv#AaH#AbpbpbpbJbJbJbJajbJajb1ajb1aj#3b1#3#3#3#V#3#3#V#V#V#V#V#5#g#5#5#5#5bxbxbxbx#S.Ubx.Ubx#m.U.U.U.U.U.U#D.U#D#D.U#D#D#D.1#D#D",
        ".1.1.j.1.j.dafafa9afapa9apapapbe.p.p.p.paFbu#.#.#.#.aG#N.q.q.q#N.q.q.9.9#N.9bZbZbZ#G.h.h#C.h#a#C#a#aba#abab0ba.n.n.V.c.c.V.c.V.c.V.O.c#f.O.c.O#f.O.Y.H.O.H.H.H.H.H.H.H.HbK.Ha0a0#Ma0a0a0bKbK#MbmbmbmbMbmbMbMbra#brbraI#6bv.ybvbvaH#AbpbpbpbpbJbJbpbJ.oajajbJ#3aj#3aj#3#3#3#3#3.u#V.u#V#V#V#V#g#5#5#5bx#5bxbxbxbxbxbx.Ubx.Ubx.U.U#m.U.U#D.U#D.U#D#D#Q#D#D#D.d.1",
        ".1.1.j.j.jafafafaf.7a9.7a9apapap.papbu.pbuaFaF#.#.bcaGaG.q#N.q.q#N.9.9#N.9bZbZbZbZbZ#G.h.h.h#C#a#a#a#ab0b0b0b0.n.n.n.c.c.c.c.c.c.c.V.O.c.O#f.O.O.O.O.O.H#f.H.O.H.Y.HbK.H.Ha0.Ha0a0a0#Ma0bKbKbmbKbmbmbMbMa#bMbrbrbraI#6aI.yaIbvbv#AaHaHbpbpbpbJbJbJbJbJbJajajajaj#3b1#3#3#3#3#3#3#V#3#V#Vb2#V#5#V#5#g#5bxbxbxbxbxbxbx#S.Ubx#m.U.U.U.U.U.U#D.U#D.U#D#D#D#Q.1#D#D",
        ".1.d.d.j.d.dafbda9a9.7a9apapbsbeap.p.p.pbubu#.bc#.#.#NaG#N.qaG.q.9#N.9.9bZ#N#GbZ#GbZ.h#G#C.h#a#C#a#Cba#ababa.nba.V.n.V.c.V.c.V.c.V.c.V.O.V.O#f.O.Y#f.O#f.H#f.H.Y.H.H.Y.H#M.H#Ma0#Ma0a0#MbK#MbK#Bbm#BbMbmbMa#bra#br#6aI#6aI.ybvbv#Abv.o#Abp.obJ.obJ.oaj.oaj.ob1ajb1#3b1#3.S#3.u#3#V.u#V.u#V#V#g#Vbi#5bi#5bxbxbxbxbx.Ubx.U.U.Ubx.U.U.U#D.U.U#Q#D#Q#D#Q#D#D.d#D.d",
        ".1.1.j.d.jafbdafafa9.7a9apbsapap.pbe.pbuaFbuaFbc#.#.#N#..q#N.q#N.q#N.9#N#NbZbZ#NbZ#G.h.h#C.h#C.h#C#aba#ab0b0ba.n.n.V.c.V.c.c.V.c.V.c.O.V.O.O#f.O#f.O.H.O.Y.O.H.H.Y.H#M.H.Ha0#Ma0#Ma0bKa0bKbK#MbmbmbmaCbMbMbMbrbrbrbr#6aIaI.ybvbv#AaH#AbpbpbpbJbpbJbJaj.oajb1ajaj#3b1#3#3#3#3#3.u#V#3#V#V#Vb2#5#V#5#g#5bxbxbibxbibxbxbi.Ubxbi.U.U.U.U.U.U#D.U.U#D#Q#D.d#D.d#D.1",
        ".1.d.1.jbd.jafafa9afa9.7apapbsbebe.p.p.pbuaFbcaF#.#NaGaG#N.q.q.q#N.9#N.9bZ#NbZbZ#G.h#G.h.h#C.h#a#a#a#ababab0ba.n.V.n.c.V.c.V.c.V.c#f.c#f.O.V.O.O#f.O.Y.O.H.Y.H#f.H.H.H#M#M.Ha0#Ma0a0#Ma0#MbKbm#Bbm#BbMbMbMa#bra#br#6aI#6.yaIbv.yaH#AaH#Abp.obJ.obJ.obJbJb1ajb1ajb1#3#3b1#3.u#3.u#3.u#V.u#V#V#g#Vbi#5bi#5bxbxbxbxbxbx.Ubx.U.U.U.U.U.U.U.U#Q.U#D.U#D#Q#D#D#D.d#D",
        ".d.1.j.d.jbdafbd.7a9.7a9apbsapapapbebu.paFbuaFbc#.bcaG#N.q#N.q#N.9.q#NbZ.9#NbZ#GbZbZ.h#G#C.h#a#C#C#aba#abab0ba.n.n.V.V.c.V.c.c.V.O.c#f.c.O#f.O#f.H.O.Y.O.Y.O.Y.H.H.Y.H.H.H#M.Ha0a0#Ma0a0#MbK#MbmbmbmbMaCbMbMbra#aIbr#6aIbv.ybvbv#AaH#Abpbpbp.obJbJbJ.oajajajb1aj#3b1#3#3.u#3#3#3.u#V#V#Vb2#V#5#g#5#5bxbibxbibxbxbibxbxbi.U#5.U.U.U.U#Q.U#D.U#D#Q#D#D#D#D.d#D.d",
        ".1.d.d.jbd.jbdafa9a9a9a9apbsapbebe.pbe.pbububcbc#.bc#.#NaG#NaG.q#N#N.9#N#NbZ#GbZ#G#G#G.h.h#C#C.h#a#C#abab0bababa.n.V.c.V.c.V.V.c.V.V.O.V#f.c#f.O#f#f.O.Y.H.Y.H#f.Y.Y.H#M#M.H#M#M#Ma0#MbKbK#M#Bbm#B#BbMaCbMa#a#brbr#6aI#6aI.ybv.y#Abv.o#A.obp.obJ.o.obJ.ob1ajb1#3b1aj.Sb1#3.u.u#3#V.u.u#Vb2#Vb2#V#g#g#5#5bibxbibxbibx.Cbx.Ubi.U.C.U.C.U.U#Q.U#Q#Q#Q#Q#Q#D.d#D.d",
        ".1.d.j.d.jbdafbdafa9.7a9apbsapbeapbe.p.pbu.pbcaFbc#.#NaG#N.q#N.q#N.9#N.9#NbZ#NbZ#GbZ.h#C#G.h#C#C#C#aba#abab0ba.n.V.V.c.V.V.c.V.V.c.V.V.c#f.O#f#f.O#f.H#f.Y.O.Y.H.Y.H#M.H.H#Ma0#Ma0#Ma0bK#MbK#Mbm#BbmaCbMaCbMbra#brbr#6#6.y.y.ybv#Abv.oaH.obp.obJ.obJaj.oajb1ajb1#3b1#3#3.S#3#3.u.u.u#V.u#Vb2#g#5#g#5bibibxbxbibxbxbibx.Ubi.U.C.U.C.U.U#Q.U#Q#D.U#D#D.d#D.d#D.d",
        ".d.1.d.daf.dbda9a9afbsa9apa9bsapbe.pbe.pbubuaFbcbc#.#N#.#NaG#N.q#N#N.9#N#GbZ#N#GbZ#G#G#G.h#C.h#C#a#Cba#ababab0ba.n.V.V.c.V.c.V.c.V.V.O#f.V.O#f.O#f#f.O.Y.O.Y.H.Y.H.YbK.Y#M.H#Ma0#Ma0#M#MbK#Mbm#Bbm#BbMaCbMa#a#bra##6aI#6aI.ybvbv#A#A#Abp.obp.obJ.obJ.o.oaj.ob1ajb1#3.S.S#3.u#3.u#3#V.u#Vb2#V#g#V#g#g#5bibibibxbibxbi.Ubi.Ubx.Ubi.U.U#Q.U#Q.U#Q#D#Q#Q#D#Q#D.d#D",
        ".1.d.d.j.dafbdafafa9.7a9bsapbeapbebe.pbuaFbubcaFbc#.#NaG#N.q#N.q#N.9#N#N.9#NbZ#GbZ#G.h.h#C.h#C#C#a#C#a#Cb0baba.n.V.V.c.V.V.c.V.V.V.O.V.V.O#f.O#f#f.O.Y.O.Y#f.H.Y.Y.H.Y.H.H#Ma0#M#Ma0#Ma0#MbK#Bbm#BbmaCbMaCbMa#br#6br#6aI.y.ybv.yaH#AaH#Abp.obJ.obJ.o.obJb1aj#3b1b1#3b1#3.S#3.u#3#V.u.u#Vb2#V#g#V#g#5bi#5bxbxbxbibxbi.Ubxbi.U.C.U.U.C.U.U.U#D#Q#Q#D#D#Q#D.d#D.d",
        ".d.1.d.dbdbdbdbda9a9a9a9apbsbsbs.pbebe.pbububcbu#.bc#.#NaG#NaG#N.9#N#NbZ#G#N#GbZ#G#G#G#G#C.h#C#C#a#Cbababa#ababa.n.V.V.c.V.V.V.c.V.V.O.V#f.V#f.O#f.Y#f.Y.Y.H.Y.Y.H.YbK.Y#M.H#M#M#Ma0#M#MbK#M#B#BbmaCbMaCbMa#bra#br#6#6#6aI.y.y.y#Abv#A#A.o.o.o.obJ.oaj.ob1ajb1b1#3b1#3.S#3.S.u#3.u.u#V.ub2#V#g#g#g#gbi#gbibibibxbibibxbi.C.Ubi.C.U.C#Q#Q.U#Q.U#Q#Q#Q#D#Q.d.d#D",
        ".j.d.d.jbdafbdafa9a9.7a9bsapbsapbebebu.pbubuaFbcbcbc#N#.#N.q#N#N#N.9#N#NbZ#NbZ#G#G#G.h#C#G#C.h#C#C#C#a#Cbaba.nba.V.V.V.V.V.c.V.V.O.V.V.V#f.O#f#f#f.O.Y.O.Y#f.H.Y.Y.Y.H#M.H#M#Ma0#M#Ma0#M#BbK#Bbm#BbmaCbMa#aCa#brbr#6aI#6.y#6bv.ybv#A#Abp#Abp.obJ.o.o.obJb1b1b1b1#3b1.Sb1.u#3.u#3.u.u#Vb2#Vb2#g#V#g#5bi#5bibxbibibxbi.Ubi.Ubi.U.C.U.C.U.U#Q.U#Q#Q#D#Q#Q#D.d#D.d",
        ".1.d.d.daf.da9bda9a9a9bsapbsbsbeapbebe.pbububcbu#.bcaG#N#N#N.qaG#N#NbZ#N#N#GbZ#GbZ#G#G.h#C.h#C#C#a#Cba#ababababa.n.V.c.V.V.V.V.V.V.V.O.V.O#f#f.O#f#f.Y#f.Y.H#f.Y.H.Y.Y.H#M.H#M#M#Ma0#M#MbK#M#B#B#BaCbMaCbMa#bra##6br#6#6aI.y.ybv#A#AaH#A.o.obp.obJ.oaj.ob1ajb1aj.Sb1#3.S#3.S.u.u.u#3b2.u#Vb2#g#V#g#gbi#5bibxbibxbibibxbi.C.U.C.U.C.U#Q.C#Q#D.U#D#Q#D#Q.d#Q.d#D",
        ".d.d.j.dbdbdafbdafa9.7a9a9bsapbs.pbebu.pbubuaFbcbc#.#N#.#NaG#N.9#N.9#N#N#GbZ#N#G#GbZ#C#G#G.h#C#C#C#a#Cba#abab0ba.V.V.V.V.c.V.c.V.c.V#f.V#f.V#f#f#f.Y.O#f.H#f.Y.H.Y.Y.H#M.Y#Ma0#Ma0#M#MbK#MbK#Bbmbm#BbMaCa#bMa#a#br#6#6aI.y.ybv.ybv#A#A#Abp#AbJ.obJ.o.o.oajb1b1b1b1#3.S#3.S.u#3#3.u#V.u#Vb2b2#V#g#g#5#gbibxbibxbibibxbi.Ubi.U.C.U.C.U#Q.U.U.C#Q#Q#D#Q#Q#D.d#D.d",
        ".d.d.d.dbdbdbda9a9a9a9bsbsbsbsbsbebebe.pbubububcbcbcbc#N#N#N#N#N#N#N#N#NbZ#N#G#G#G#G#G.h#C#C#C.h#C#Cba#Cbabababa.Vba.V.V.V.V.V.V.V.V.V#f#f.O#f#f.O#f.Y#f.Y.Y.Y.Y.Y.Y#M.Y#M#M#M#M#M#M#M#M#M#B#B#B#BaCaCaCaCa#a#br#6br#6#6#6.y.y.y#A#A#A#A.o.o.o.o.o.o.o.ob1b1#3b1.Sb1.S.S.S#3.u.u.u.u.ub2b2b2#gb2#g#gbi#gbibibibibibibibibibi.C.C.C.C.C#Q#Q#Q.U#Q#Q#Q#Q#Q.d#Q.d",
        ".d.d.d.jbdbdbdbda9a9a9a9bsapbebsbebebubebu.pbcbubc#.#N#.#NaG#NaG#N#N#N#N#G#N#GbZ#G#G#G#G#C#G#C#C#C#C#C#Cbabababa.V.V.V.V.V.V.V.V.V.V#f.V.V#f#f#f#f#f.Y#f.Y#f.Y.Y.Y.H#M.Y#M.H#M#M#M#M#MbK#B#M#B#BbM#BaCbMa#aCbra#a##6#6#6.y#6.y.ybv#A#AaH#A.o.o.o.o.o.oajb1b1b1b1b1.S.S#3.S.u.S.u.u.ub2.u#Vb2#gb2#5#g#gbibibibibxbibi.Ubi.C.U#g.U.C.U.C.C.U#Q#Q#Q#Q#D#Q.d#D#Q.d",
        ".d.j.dbdbdbdbdbda9a9.7bsa9bsbsbs.pbs.pbebubububcbcbcbc#.#N#N#N#N#N.9#N#G#N#G#G#G#G#G.h#G#C#G#C#C#C#Cba#Cbababababa.n.V.V.V.V.V.V.V.V#f.O#f.V#f#f#f#f#f.H#f.Y.H.Y.Y.Y.Y#M.Y#M#M#M#Ma0#M#M#MbK#B#B#BaCaCaCaCa#a#br#6#6#6aI.yaI.y.y#Abv#A#A.o.obp.obJ.o.o.ob1b1b1b1#3b1.S.S.S.S#3.u.u.u#V.ub2b2b2#g#g#gbi#5bibibibibibibi.Ubi.C.C.C.C.C#Q.U#Q#Q.U#Q#Q#Q#Q#Q.d.d.d",
        ".d.d.d.dbd.ja9afa9a9a9a9bsbsbsbsbebebebubububcbubc#.#N#.#N#N.q#N#N#N#G#N#N#G#NbZ#G#G#G#G#C.h#C#C#C#C#a#Cbabababa.V.V.V.V.V.c.V.V#f.V.V.V#f#f#f#f.O#f.Y#f.Y.Y.Y.Y.Y.Y.H.Y#M.H#M#M#M#M#M#MbK#B#B#BaCbmaCaCbraCa#a##6br#6#6#6.y.y.y#A#A#A#A#A.o.o.o.o.ob1.ob1ajb1b1b1#3.S.S#3.S.u.u#3.u.u.ub2b2#g#V#g#g#g#gbibibxbibi.Ubibibi.U.C.U.C.U.C#Q.C#Q#Q#Q#D#Q.d#Q#D#Q.d",
        ".d.d.dbdbdbdbda9a9a9a9bsbsbsbsbebebebebebububububcbcbcbc#N#N#.#N#N#N#N#N#G#G#G#G#G#G#C#G#C#G#C#C#C#Cba#Cba#Cbababa.V.V.V.V.V.V.V.V.V#f.V#f#f#f#f#f.Y#f#f.Y#f.Y.Y.Y#M.Y#M#M.Y#M#M#M#M#M#M#B#M#B#B#BaCaCaCaCa#bra#a##6#6#6.y#6.y.ybv.y#A#A.o#A.o.o.o.o.ob1b1b1b1.S.S.S.S.S.u.S.u.u.u.ub2.ub2b2b2b2#g#g#gbibibibibibibibi.C.Cbi.C.C.C.C.C.C#Q.C#Q#Q#Q#Q#Q#Q.d#Q.d",
        ".d.dbd.dbdbdbdbda9a9a9a9bsbsbebsbsbebubebububcbubcbc#Nbc#N#N#N#N#N#N#G#N#G#N#G#G#G#G#G#G#C#G#C#C#C#C#C#Cbababababa.V.V.V.V.V.V.V.V#f.V#f.V#f#f#f#f#f.Y#f.Y.Y.Y.Y.Y.Y#M.Y.Y#M#M#M#M#M#M#M#M#B#B#B#BaCaCaCa#a#a#a##6#6#6#6#6.y.y.y#A#A#A#A#A.o.o.o.o.ob1.ob1b1.Sb1b1b1.S.S.S.S.u.S.u.ub2.ub2b2b2#g#g#g#g#gbibibibibibibibi.C#g.C.C.C.C#Q.C#Q.C#Q#Q#Q#Q#Q#Q.d#Q.d",
        ".d.d.dbdbdbda9.3a9a9a9bsbsa9bsbebebebebebububububcbcbcbc#N#N#N#N#N#N#N#N#G#N#G#G#G#G#G#C#G#C#C#G#C#Cba#Cba#Cbaba.Vba.V.V.V.V.V.V.V.V#f.V#f#f#f#f#f#f.Y#f.Y#f.Y.Y.Y.Y.Y#M#M#M#M#M#M#M#M#M#B#M#B#BaC#BaCaCaCa#a#a##6a##6#6.y#6.y.y.y#A#A#A.o#A.o.o.o.o.o.ob1b1b1b1.S.S.S.S.S.u.S.u.u.u.u.ub2b2b2b2#g#g#gbi#gbibibibibibi.Cbi.C.C#g.C.C.C.C#Q#Q#Q.C#Q#Q.d#Q.d#Q.d",
        ".d.dbd.dbdbdbdbda9a9bsa9bsbsbsbsbebebebebububcbubcbc#Nbc#N#.#N#N#N#N#G#N#G#N#G#G#G#G#G#G#C.z#C#C#C#C#C#Cbaba.kba.V.V.k.V.V.V.V.V#f.V.V#f.V#f#f#f#f#f.Y#f.Y.Y.Y.Y.Y.Y#B.Y#M.Y#M#M#M#M#M#M#B#B#B#BaCaCaCaCaCa#a#a#a##6.y#6#6.y.y.y#A.y#A#A#A.o.o.o.o.o.ob1b1b1.Sb1.S.S.S.S.u.S.S.u.u.u.ub2b2b2#gb2#g#g#g#gbibibibibibibibi.Cbi.C.C.C.C#Q.C#Q.C#Q#Q#Q#Q#Q#Q.d#Q.d",
        ".d.d.3.dbdbda9.3a9a9a9a9bsbsbsbebsbebebububebububcbubcbc#N#N#.#N#N#N.0#N#Gb3#Gb3#G.z#C#G#C#G#Cb5#C#C.Bba#C.Bbaba.k.V.V.V.k.V.V.k.V#f.k#f#f#f#f#f#f#f.Y#f.Y#f#B#f#B.Y.Y.Y#B.Y#Mbo#M#M#M#M#Bbo#B#BaC#BaCa#a#aCa#a##6a##6#6.y#6.y.y.y#A#A#A.o#A.o.o.o.ob1.ob1b1b1#yb1.S.S.S.S.S.u.u.u.u.u.ub2b2b2b2#g#g#g#gbi#g.Zbibi.Zbi.Cbi.Z.C.C.C.C.C.C#Q.C#Q#Q#Q#Q#Q#Q.d#Q.d",
        ".d.d.dbd.3bdbda9a9a9bsa9bsbsbsbsbebebebebubu.0bu.0bcbcbc#N#N#N#N.0#N#G#N#G#N#G#G#G#G.z#G#C.z#C#C#C#C#C#Cbaba.kbaba.V.k.V.V.V.V.V.k.V#f.V#f.V#9#f#f#9.Y#f.t#f.Y.Y.Y.Y.Y#M.Y#M#M#M#M#B#M#B#M#B#B#BaCaCaCaCa#a#a#a##6#6#6#6#6.y.y.y#A.y#A#A#A#A.o.o.o.o.ob1b1b1.Sb1.Sb1.S.SbU.S.u.S.u.ub2.u#Fb2.Zb2#g.Z#gbi.Zbibibibibi.C#g.C.C.C.C.C.C#Q.C#Q.C#Q#Q#Q#Q#Q#Q#Q#Q.d",
        ".d.d.3.dbd.3a9bda9a9.Dbsbs#Wbsbebebebebebububububcbcbc#Nbc#N#N#N#N#N.0#N#Gb3#G#G#G#G#G#G#C#G#Cb5#C#C.Bba#Cbaba.kba.V.k.V.V.k.V.k.V.V.k#f.k#f#f#f#f#f#f.Y.Y.Y.Y.Y.Ybo.Y.Y#B.Ybo#M#M#Mbo#M#Bbo#B#BaC#BaCaCa#aCa#a##6a##6#6.y#6.y.y.y.y#A#A#A.o.o.o.o.ob1#Ab1bnb1#yb1.S.S.S.S.u.SbU.u.u.u.ub2b2b2b2#g#g#g#gbibi.Zbibibi.Zbi.C.Z.C.C.Z.C.C.C#Q#Q#Q.C#Q#Q#Q.3.d#Q.d",
        ".d.dbdbdbdbda9.3.Da9a9a9bsbsbs#Wbebebebe#0bu#0bu.0bcbcbc#Nbc#N#N.0#N#G#N#G#Nb3#G#G#Gb5#G#C.z#C#C#C#C#C#C.k#Cbaba.V.k.V.V.V.V.V.V#f.V#f.V#f#f#f#f#9#f#B#f.Y#9.Y.t.Y.Y#Mbo.Y#M#M#M#M#M#M#M#B#B#B#BaCbOaCaCa#a#a#a##6#6#6#6#6.y.y.y.y#A#A#A.o#A.o#A.o#Ab1b1b1b1b1b1.S#y.S.S.S.S.u.u.u.u#F.ub2#Fb2b2#g.Z#g.Zbibibibibi.Zbibi.Cbi.C.C.C.C#Q.C#J.C#Q#Q#J#Q#Q#Q.d#J.d",
        ".d.3.3.d.3.3a9.3a9a9#Wa9#Wbsbsbe.4be.4bebubu#0bubc#0bcbc#N.0#Nbc#N.0.0#Gb3#G#Gb3#G#G.z#Gb5#C#C.z#C.B.Bba#C.k.B.kba.k.V.k.k.V.k.k.k.V#9.k#f.k#9#f#f#9#f#9.Y#f.t.Y.Y.t.Y.Ybo#Mbobo#Mbo#Mbo#Mbo#BbO#BaCbOaCbza#a#a##6a##6#6.yby.ya8.ya8#Aa8#Abn.obn.ob1.obnb1#y.S#yb1.S.S#ybU.u.SbU.u.u.u#Fb2b2b2#F#gb2.Z#g.Zbi.Zbi.Zbi.Zbi.Z.C.Z.C.Z.C#J.C#Q#J.C#J#Q#Q.3#Q#Q#Q.3",
        ".3.dbdbd.3bd.3a9a9.Da9bs#Wbs#Wbsbe.4bebebu#0bubu.0bcbc.0bc#Nbc#N#N#N.0#N#G#Nb3#G.z#G#C.z#C#Gb5#C#C#C#Cb5ba.Bbaba.k.V.k.V.V.V.k.V.V.k.V#f.k#f#f#9#f#f#9.Y#f.t.Y.Y.t.Y#B.Ybo.Y#M#Mbo#M#B#Mbo#Bbo#BaCbOa#aCaCa#bza#by#6by#6a8#6.y.ybn.y#Abn#A#Ab1.o.obnb1bnb1.o.Sb1#y.SbU.S.S.SbU.ubU.u#F.ub2#F#g#F#g.Z#g#g#gbi.Zbibi.Zbi.Cbi.Z.C.C#J.C.C.C#J.C#Q#Q#J#Q#Q.3#Q.3.d",
        ".d.d.3bdbd.3a9a9.Da9bs.Dbsbs#Wbsbebe.4be#0bu#0bu.0bu.0bc#N.0#N.0#N#G.0#G.0#G#Gb3#G#G.z#Gb5#G#Cb5#C.B#C.Bba#C.k.kba.k.V.k.V.k.V.k.V.k#f.k#f.k#f#f#9#f.Y#9.Y.Y.t.Y.Y.t.Y.Y#Mbobo#M#Mbo#Mbo#Bbo#BaCboaCaCbOa#a#a#a##6a##6#6.y#6a8.y.y.ybn#A#Abn.obn.o.ob1.o#yb1#yb1.S#y.S.SbU.u.u.u.u.u.u#Fb2#Fb2b2.Z#g.Z#g.Zbibi.Zbibi.Zbi.Z.C.Z.C#g.C#J.C#Q#J#Q#Q#Q#J#Q#Q#Q#Q.d",
        ".3.d.3bd.3bda9.3a9a9.Da9#Wbsbe#Wbe.4bebubebu#0bubc.0bcbc#Nbc#N#N#N.0#Nb3#N#Gb3#G.z#G.z#G#Cb5#Cb5#C#C.B#Cba.Bbaba.k.V.k.V.k.V.k.V.k.V.k#f#f#9#f#9.Y#f#9#f.Y#9.Y.t.Y.Ybo.Y.t#M.Y#Mbo#M#M#Mbo#B#BboaCaCbOaCaCbza#bz#6by#6by.yby.y.ybn.y#Abn#A#A.o.obn.obnb1b1bnb1.Sb1#y.S.S.S.SbU.ubU.u#F.ub2#F#g#Fb2#g#g.Zbibi.Zbibi.Zbi.Z.C#g.C.Z.C#J.C#J.C.C#J#Q#J#Q#Q#Q.3.d.3",
        ".d.3.d.3.3.3a9.Da9.Dbs#Wbs#W#Wbs.4be.4.4#0bu#0#0bu.0.0.0bc.0.0#N.0.0#G.0#Gb3#Gb3#G.z#Gb5.z#Gb5#Cb5b5bab5.B.B.k.Bba.k.k.V.k.V.k.k#f.k.k.k.k#f#9#f#9#9.Y#9.t#f.t.Y.t.t.Ybo#M.tbobo#Mbobo#Bbo#BbO#BbOaCbOa#bza#bza#bya#by#6by.ya8#6a8.ybn.ybnbn.obn.obnb1bnbn.S#y#y.S#y.SbUbU.SbUbU.ubUb2#F.u#Fb2#F#g#F.Z#g.Z.Zbi.Z.Zbi.Zbi.Z.Z.C.Z.C.Z.C#J.C#J#Q#J#Q#J#J#Q.3#Q#J",
        ".3.d.3bd.3.3a9.Da9.D.Da9#Wbs#Wbs.4be.4bebe#0bu#0bc#0bcbc.0#N.0#N.0#N.0b3#Nb3b3#G.z#Gb5#G#Cb5b5#Cb5#C.B#C.Bbaba.k.k.V.k.k.V.k.k.V.k.k#f.k#f#9#f#9#f#9#f.t#f.t.Y.t.Ybo.Y.t.Ybo#Mbo#Mbo#M#MboboaCboaCbOaCbOaCbza#a#bz#6by#6by#6a8.ybn.ybn#Abn#Abn.obn.obnb1bnb1#yb1#y.S#y.SbU.ubU.u#F.u#F.u#Fb2#Fb2.Z.Z#g.Zbi.Zbi.Zbi.Zbi.Z.C.C.Z.C#J.C#J.C#J.C#J#Q#J#Q#Q.3#Q#J.d",
        ".d.3.3bd.3a9.3a9.Da9#W#Wbs#Wbs.4bs.4be.4#0bu#0#0bu.0.0.0bc#N.0#N.0.0#G.0#Gb3#Gb3#G.z#G.zb5#G#Cb5#Cb5#C.B.Bba.B.kba.k.V.k.k.V.k.k.V.k.k#f#9.k#9#f#9#f.t#f.t#9.Y.t.Y.tbo.Ybo.Ybobobo#Mbobo#Bbo#BbOaCbOaCbza#bza#bz#6by#6by.yby.ya8.ya8#Abn#Abn.obn.obn.obnb1bn.S#y.S#y.SbU.SbU.u.SbU.u#F.u#Fb2#F#g#F#g.Z#g.Z#g.Zbi.Zbi.Z.Zbi.Z.Z.C.Z.C.Z.C#J.C#J.C#J#Q#J#Q.3#Q.3",
        ".3.d.3bd.3.3a9.D.Da9#Wa9#Wbs#W#Wbe.4be#0be#0bu#0bc#0bcbc.0bc.0#N.0#N.0#G.0#Gb3#G.z#Gb5#Gb5b5#Cb5.B#C.B#Cba.B.Bba.k.k.k.k.V.k.k.V.k.k#f.k#f.k#f#9#f#9#9.Y#9.Y.t.t.Y.t.Ybo.Ybo#Mbo#M#Mbo#Mbobo#BboaCbOaCbOaCbza#bya#by#6by.yby.ya8.ya8.ybn#Abnbn.obn.obnb1bnb1#yb1#y.S#y.SbU.SbUbU.ubU.u#Fb2#F#g#F#g#F#g.Zbi.Zbi.Z.Zbi.Z.C.Z.Z.C.C.Z.C#J.C#J#Q#J#Q#J#Q.3#Q#J.d.3",
        ".3.d.3.3.3.D.3.Da9.D#W.D#W#Wbe#W.4be.4be#0be#0#0#0bc.0.0.0#N.0.0.0#G.0.0b3b3b3b3.z.z.z.z#C.zb5b5#Cb5.B.B.B.Bba.B.k.k.V.k.k.k.k.k.k#f.k#9.k#9#9#9#9#9.Y#9.t#9.Y.t.t.Y.tbobobo.Ybobobobobo#BbobOaCbObOaCbza#bza#bz#6bzby#6bya8.ybya8#Abna8bn#Abnbn.obnbnbn#yb1#y#y.S#y.SbU.SbU.ubU.u#F#F.u#F#F#Fb2.Z.Z.Z#F.Z.Z.Zbi.Z.Zbi.Z.Z.C.Z.Z#J.Z#J.C#J.Z#Q#J#J#J#Q#J.3#Q#J",
        ".d.3.3bd.3.3a9.D.D.Da9#Wbs#W#W#Wbe.4.4.4be#0#0#0bc#0.0.0bc.0.0#N.0.0#Nb3#Gb3#G.z#G.z#G.zb5.z#Cb5b5#C.B#C.B#C.k.k.k.k.k.k.k.V.k.V.k.k.k.k#f#9#f#9#f#9#9#9#9.Y#9.t.Y.t.t.Ybo.Ybobo#Mbo#Mbobo#BboboaCbOa#bObza#bza#byby#6byby#6a8a8.ya8a8#Abnbn.obn.obnbnb1bnb1#y#y#y.S#ybUbUbU.ubU#F.ubU#Fb2#F#g#F#Fb2.Z#g.Zbi.Z.Zbi.Z.Zbi.Z.Z.C.Z.C.Z.C#J.C#J#J#Q#J#Q#J.3#Q#J.3",
        ".3.3.3.3.Dbd.D.Da9.D#Wa9#W#Wbs.4#Wbe.4be#0#0bu#0#0bu.0bc.0.0#N.0.0#Gb3.0#Gb3b3b3.z#Gb5.z#Gb5b5b5b5.B.B.B.B.B.kba.k.k.V.k.k.k.k.k.k#f.k.k#9.k#9#9#9.Y#9.Y.t.t.t.Y.t.t.Ybo.t#B.Ybobo#Mbobo#BbOaCbobObOaCbOa#bzbza#bya#by#6a8by.ya8a8.ybna8#Abnbnbnbn.obnbn#yb1#y.S#y#y.SbU.SbUbUbU.ubUb2#F#F#F#Fb2.Z.Z#g.Z.Z#g.Z.Z.Zbi.Z.Z.Z.Z.C#J.Z#Q.Z#J#J.C#J#J#Q#J#Q#J#Q.3.d",
        ".3.d.3bd.3.3a9.D.Da9#W#W#W#W#W#W.4be.4be#0be#0#0bc#0.0.0bc.0.0#N.0.0b3#Nb3b3#G.z#G.z.z#Gb5.z#Cb5#Cb5#Cb5ba.Bba.B.k.k.k.k.V.k.k.V.k.k#9.k#f#9#f#9#f#9.t#9#f.t#f.t.t.Ybo.tbo.Ybobo#Mbobo#BboboboaCbOaCbzbOa#bza#bza#by#6bya8#6a8a8.ya8a8#Abnbn.obn.obnbnb1bnb1#ybn.S#y.SbUbU.S.ubU#F.ubU#F.u#F#g#F#F#g#F.Zbi.Z.Zbi.Z.Zbi.Z.C.Z#J#g.Z#J.C.Z#Q.Z#J.C#J#J#J.3#Q.3#J",
        ".3.3.3.3.D.3.D.Da9.D#Wa9#W#Wbe#Wbe.4.4.4#0.4#0#0#0#0.0.0.0.0.0.0.0.0b3b3b3b3b3b3.z.z.zb5b5b5b5b5b5.B.B.B.B.B.B.B.k.k.k.k.k.k.k.i.k.k.k#9.k#9#9#9#9#9#9.t.t#9.t.t.t.t.t.YbobobobobobobobobobObObobObObObzbzbzbzbybybzbybybybya8#6a8a8#Aa8bnbnbnbnbnbnbnbn#y#y#y#y#y#ybU#ybUbUbU.ubU#F#F.u#F#F#Fb2.Z#F.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z#J.C.Z#J#J#J#J#J#J#J#Q#J#J.3#J.3",
        ".d.3.3.3.3.D.D.D#W.D#W#W#W#W#W#W.4.4.4.4be#0#0#0#0.0bu.0.0.0.0#Nb3.0#Gb3b3b3b3.z.z.z.z.z#C.zb5#Cb5b5.Bb5.B.B.B.k.k.k.k.k.k.k.k.k.k.k#9.k#9.k#9#9#9#9.t#f#9.t.t.t.t.t.tbo.t.tboboboboboboboboaCbObOaCbObOa#bza#bza#bybybya8#6a8a8a8a8a8bna8#Abnbn.obnb1bn#yb1#y#y.S#ybU.SbUbUbUbU#FbU#F#F#F#F#F#F.Z#F.Z.Z.Z.Z.Z.Zbi.Z.Z.Z.Z.Z.Z.Z.Z#J.Z#Jb2#J.C#J#J#J#J#J.3#Q#J",
        ".3.3.3.3.D.3.D.D.D.D#Wa9#W#Wbe#W.4#Wbe.4#0.4#0#0bu#0.0.0.0.0.0.0.0.0b3.0#Gb3.z#G.z.z.z.z.zb5b5b5b5.B#C.B.B.B.k.B.k.k.k.k.k.k.k.k.k.k#9.k#9#9#f#9#9#f#9.t.t#9.Y#9#B.t.Y.tbobo.Ybobobobo#BbobobObobObObzbObzbzbzbzbybyby#6bybya8.ya8.ybna8#Abnbnbnbnbnbnbn#ybn#y#y#ybU#ybUbU.ubU.ubU#F.ubU#Fb2#F#F#g#F.Z#g.Z.Z#g.Z.Z.Z.Zbi.Z.Z.C.Z#J.Z#J.C#J#J#J#J#J#Q#J#J#J.3.3",
        ".3.3.3.3.3.D.D.Da9.D#W#W#W#W#W#Wbe.4#0.4be#0#0#0#0.0#0.0.0.0.0.0.0b3.0b3b3b3b3b3.z.zb5#Gb5b5b5b5b5b5.B.B.B#C.k.B.k.k.k.k.k.k.k.k.k.k#f#9.k#9#9#9#9#9.t#9#9.t.t#9.t.t.t.tbo.tbobobo#MbobobObobObOaCbOa#bObza#bya#bya#bybyby.ybya8a8a8#Aa8bnbnbn.obnbnbnb1bn#y#y.S#y#y.SbUbUbUbUbU#FbU#F#F#F#F.Z#F.Z#F.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.C.Z#J.Z#J#J.Z#J#Q#J#J#J#J.3#Q#J",
        ".3.3.3.D.D.3.D.D#W.D#W.D#W#W.4#W.4#W.4.4#0.4#0#0#0#0.0#0.0.0.0.0b3.0b3b3b3b3.z.z.z.z.z.zb5.zb5b5.Bb5.Bb5.B.B.B.k.k.k.k.k.k.i.k.k#9.k.i.k#9#9#9#9#9#9#9.t#9.t.t.t.t.t.t.t.tbo.tbobobobobobobobObObObObzbObzbzbzbzbybybybybybya8a8a8a8a8a8bna8bnbnbnbnbn#y#y#y#y#y#ybUbU#ybUbUbUbU#FbU#FbU#F#F#F#F#F.Z#F.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z#J.Z#J.Z#J.Z#J#J#J#F#J#J#J.3#J.3.3",
        ".3.3.3.3.D.D.D.D.D#W.D#W#W#W.4#W.4.4.4.4.4.4#0#0#0#0#0.0.0.0.0.0.0.0b3b3b3b3b3b3.z.z.z.zb5b5b5b5b5b5.B.B.B.B.B.B.k.i.k.i.k.k.k.k.k.i.k#9#9.k#9#9#9#9.t#9.t#9.t#9.t.t.t.tbo.tbobo.tbobobobObobObObObObObzbzbzbzbzbybzbybya8bya8bya8a8a8a8bnbnbnbnbnbnbnbn#ybn#y#y#y#ybUbUbUbUbU#FbUbU#F#F#F#F#F#F.Z#F.Z#F.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z#J.Z.Z#J.Z#J#J#J#J#J#J#J#J#J#J",
        ".3.3.3.3.D.3.D.D.D.D#W#W#W#W#W#W.4.4.4.4#0.4#0#0#0#0.0#0.0.0.0.0b3.0b3b3b3b3.z.z.z.z.z.zb5.zb5b5b5.Bb5.B.B.B.B.B.k.k.k.k.k.i.k.i.k#9.i.k#9#9#9#9#9#9#9.t#9.t.t.t.t.t.t.t.tbo.tbobobobobobobObObObObObzbObzbzbzbzbybybybybybya8a8a8a8a8bna8bnbnbnbnbnbn#ybn#y#y#ybU#ybU#ybUbUbUbU#FbU#F#F#F#F#F#F#F.Z.Z#F.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z#J.Z.Z#J#J.Z#J#J#J#J#J#J#J.3#J.3.3",
        ".3.3.D.3.D.D.D.D#W.D#W.D#W#W#W.4#W.4.4.4#0.4#0#0#0#0.0#0.0.0.0.0b3.0b3b3b3b3.zb3.z.z.z.zb5b5b5b5b5b5.Bb5.B.B.i.B.k.i.k.k.i.k.k.k.i.k.k#9.i#9#9#9#9#9.t#9.t#9.t#9.t.t.t.tbo.tbo.tbobobobobObobObObObObObzbzbzbybzbybzbybybybya8bya8a8a8a8bnbnbnbnbnbnbnbn#y#y#y#y#y#ybUbUbUbUbUbU#FbU#FbU#F#F#F#F.Z#F#F.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z.Z#J.Z.Z#J.Z#J.Z#J#J#J#J#J#J.3#J#J",
        ".3.3.3.3.D.D.D.D.D#W#W#W#W#W#W.4.4#W.4.4.4.4#0#0#0#0#0.0.0#0b3.0.0b3b3b3b3b3.zb3.z.z.zb5.zb5b5b5b5.Bb5.B.B.B.B.B.i.B.k.i.k.i.k.i.k.i#9.i#9#9.i#9#9#9#9#9.t#9.t.t.t.tbO.t.tbobO.tbobobO.tbobObObObObzbzbObzbzbzbzbybybybybya8bya8a8a8a8a8bna8bnbnbnbn#ybn#y#y#y#ybU#ybUbUbUbUbU#FbU#F#F#F#F#F#F#F#F.Z.Z#F.Z.Z.Z.Z.Z.Z.Zbg.Zbg.Z.Z#J.Z#J.Z#J#F#J#J#J#J#J#J#J#J.3",
        ".3.3.3.m.3.D.m.D#W.m.D#WaR#WaR#W.4a3.4.4#0.4#0#0#0#0#0#0b3.0.0#0b3b3#0b3#z.zb3.z.z.z.z#7b5.zb5#7b5b5.B#7.B.B.i.B.k.i.k.i.k.k.i.k.k.i.k#9.i#9#9.i.t.i.t.i.t#9bO.i.t.t.t.tbO.t.tbo#cbobobo#c.tbObObObObObzbzbzbzbzbybzbybybybya8bya8a8a8a8bna8bnbnbnbnbn#ybn#y#y#y#ybU#ybUbUbU#FbUbUbU#FbU#F#F#F#Fbg#F.Zbg.Z.Zbg.Z.Zbg.Z.Z.Z.Z.Z#J.Zbg#J#F#J#J#J#J#J#J#J#J#J.3#J",
        ".3.3.m.3.D.D.D.D.D#W#W.D#W#W.4#W.4#W.4a3.4.4#z.4#0#0b3#0.0.0b3.0.0b3b3b3b3b3#z.z#z.z.z.zb5#7b5b5b5#7.Bb5.B.B.B.B.i.B.k.i.k.i.k.k.i#9.k#9#9#9.i#9#9.t#9.tbO#9.t.t.t.t.t#c.t.tbo.tbobo#cbobObobObObObzbzbObzbzbzbybzbybybybybya8a8a8a8a8a8bna8bnbnbnbn#ybn#y#y#y#ybU#ybUbUbUbUbU#d#FbU#F#F#F#F#F#F.Z#F#F.Z.Z#F.Z.Z.Z.Z.Zbg.Z.Zbg.Z#J.Z#J#J#Jbg#J#F#J#J#J.3#J#J.3",
        ".3.3.D.3.m.D.D.m.D.m#W#WaR#W#W#Wa3.4.4.4#0.4#0#0#0#0#0.0#z.0#0.0b3#0b3#zb3.zb3.z.z.z.z#7b5.zb5b5b5b5.Bb5.ib5.i.B.B.i.i.k.i.k.i.k.i.k#9.i.i#9#9.i.t.i.t.i#9.t.i.t#9bO.t.t.t.tbo#cbobobobobobObObO#cbObObzbzbzbzbzbybzbybya8bya8bya8a8a8a8bna8bnbna8bn#ya8#y#y.2#y#y.2bU#ybRbUbUbU#FbU#d#FbUbg#F#Fbg#F.Zbg.Z.Zbg.Z.Zbg.Z.Z.Z.Z#J.Zbg#J.Zbg#J#J#J#J#J.m#J#J.3.m#J",
        ".3.m.3.m.D.3.m.D#W#W.m#W#WaR.4aR.4#Wa3.4a3.4#z.4#z#0#z#0.0#0b3.0#zb3b3#0.z#z.z#z.z#z.z.z#7.z#7b5#7.B#7.B#7.B.Q.B.i.B.k.i.k.i.i.i.i.i.i#9.i#9.i.t.i.t.i.t#c#9bO.t#c#9bO.t#c.t#cbo#cbo#cbO#cbO#cbObO.Fbz.Fbzbz#vbzby#vby#vbyby#va8a8bya8a8a8#ya8#ybn.2bn#y.2bn#y.2bUbUbRbUbUbRbU#dbU#d#F#d#F#F#F#Fbg#F.Z#Fbg.Z.Zbg.Z.Zbg.Zbgbg.Zbg#J.Z#Jbg#Jbg#J#J.m#J#J.m#J#J.m",
        ".3.3.m.3.D.m.D.m#W.m#WaR#WaR#W#W.4a3.4.4.4.4#0#0#0#0#0b3#0b3.0#z.0#zb3b3#zb3#zb3.z#7.z#7.z#7b5#7b5#7.Bb5.Q.B.B.i.B.i.i.i.i.i.k.i.k.i#9.i.i.t.i.i#9.ibO.i.i.t.i.t.i.t#c.tbo#cbobobo#cbobobobObO#cbObObzbO.Fbzbzbz#vbzbybybybya8#va8a8.2a8#ya8bna8bnbn.2bn#y.2#y#ybU.2#ybUbU#dbUbU#dbU#FbU#d#Fbg#F.Z#Fbg.Zbg.Zbg.Zbg.Z.Zbg.Z.Zbg#J#F#Jbg#J.Z#J#J.m#J.m#J#J.m#J.3",
        ".3.m.3.m.D.D.m.D#W.m#W#W#W#Wa3#Wa3#W.4a3.4#z.4#z#0#z#0#0.0#0b3.0#z.0#zb3b3#z.z.z#z.z.z.z#7.zb5b5#7b5.Qb5.B.Q.B.Q.B.i.i.k.i.i.i.k.i.i#9.i#9.i.t.i#c#9#9.t#c.t#c.tbO.t.t#c.t.t#cbo#cbobO#cbO#cbObO.FbO.Fbzbzbz.Fbybzby#vby#vbya8bya8a8a8a8a8#ybn#ybn.2bn#y.2#y#y.2#ybUbRbUbRbUbU#dbU#F#d#F#FbUbg#Fbg#F.Zbg.Z.Zbg.Z.Zbg.Z.Zbg.Z.Z.Zbg#Jbg#Jbg#Jbg#J#J#J.m#J.3.m#J",
        ".3.3.m.3.D.m.D.m#W.DaR#WaR#Wa3#W.4a3.4a3.4.4#0a3#0#0#0#z.0#z.0#0.0b3b3b3#zb3#zb3#7.z#7.z.z#7b5#7.B#7b5.B#7.B.Q.B.i.B.B.i.k.i.k.i.i.k.i#9.Q#9.Q#9#9.Q.t.i.t.i.t.t.Q.t#c.t.t#cbobo#cbobobobo#cbO#cbObObzbO.Fbzbzbzby.Fbybyby#va8#va8.2a8.2a8a8#ya8bnbn.2bn#y#y.2bU#ybR#ybUbU#dbUbU#dbU#FbU#Fbg#F#Fbg#Fbg#F.Zbg.Z.Zbg.Z.Zbg.Z.Zbg#Jbg.Z#Jbg#J#F#Jbg#J.m#J#J.m#J.3",
        ".m.3.m.m.m.D.m#W.maR#WaR#WaR#WaR.4aR.4a3.4a3#0a3#0#z#0#z#0.0#zb3#z#z#z#0.z#z.z#z.z#z.z#7#7.z#7b5#7b5.Q#7.B.Q.B.Q.i.i.i.i.i.i.i.B.Q.Q.i.i.i.Q#9.Q.i#c#9#c#c#9#c#c.t#c.t#cbO.t#c#cbo#c#cbO#cbO#cbO.F#cbz.Fbz.Fby.F#vby#v#vby#va8#va8#va8.2bn.2a8#y.2bn.2#y.2.2#y.2bU.2bUbRbUbU#d#dbU#d#d#d#F#d#F#d#Fbg.Zbgbg.Zbgbg.Zbgbg.Zbgbg#Jbg.Z.mbg#J.m#J.m#J.m#J.m#J.m#J.m",
        ".3.m.3.D.D.m.D.m.DaR.DaR#WaR.4aR.4a3.4a3.4a3#0.4#z#0#z#0.0#z.0#0b3#0b3#z.z#z#zb3#7.z#z#7.z#7b5#7b5#7.B#7.B#7.B.Q.B.Q.i.B.i.B.i.i.i.i#9.Q.Q#9.Q.t.Q.t.Q#9#c#9#c#9#c.t#c.t#c.t#cbo#cbo#cbo#cbObO#cbzbO.Fbz.Fbz.Fbz#vbzby#vby#vby#va8.2a8a8.2a8#ya8bn.2bn.2#y#y.2#ybR#ybRbUbRbRbU#dbU#d#FbU#d#Fbg#Fbg#Fbg#Fbg.Zbg.Zbg.Zbg.Zbg.Zbgbg#Jbg#Jbg#Jbg#Jbg#J.m#J.m#J.m.3",
        ".m.3.m.m.m.DaR.DaR.DaR#WaR#WaR.4aR.4a3.4a3.4#za3#0#z#0#z#0#z.0#z.0#zb3#zb3#z.z#z.z#7.z#7.z#7b5#7#7b5#7.B.Q.Q.B.Q.i.B.i.i.i.Q.i.i.i.i.Q#9.i.Q#9.Q.i#c#9#c#9#c.t.Q#c.t#c#c.t#cbo#cbo#cbo#cbO#cbO#cbO.FbO.Fbzbz#vbzby.Fby#vbya8#va8a8#va8.2a8bn.2bn.2bn.2#y.2.2#ybR#ybRbUbRbUbUbRbU#dbU#d#F#d#F#d#Fbg#Fbg.Zbg#Fbg.Zbg.Zbgbg.Zbg.Zbg.Zbg#Jbgbg#J.m#J.m#J.m#J.m.m.m",
        ".3.m.3.m.D.m.DaR.m#WaR#WaR#Wa3#Wa3#Wa3.4a3.4#0a3#0#0#z#0.0#z.0#0#zb3#zb3#z#z.z#z.z#z#z.z#7#7b5#7b5#7.B#7.Qb5.Q.Q.B.Q.i.Q.B.i.i.B.Q#9.i.Q#9.Q#9#c.i#9#c#9#c.i#c.t.i.t#c.tbO#cbo#c#cbo#cbo#cbO#cbO.FbO.Fbz.Fbz.Fbz.Fby#vby#v#va8#v.2a8a8.2a8.2bn.2bn.2bn.2#y#y.2#ybR#ybRbUbRbU#dbU#d#F#dbU#d#F#dbg#Fbgbgbgbg.Zbgbg.Zbg.Zbg.Zbg#Jbgbg#Jbg#Jbg#J.m#F.m#J.m#J.m#J.3",
        ".m.3.m.m.m.m.m.DaRaRaRaRaRaR#WaRa3.4a3a3.4a3a3#0a3#z#0#z#0#z#0b3#z#0#z#zb3#z#7#z#7.z#7#7.z#7#7#7#7#7#7b5.Q.Q.B.Q.Q.Q.i.i.Q.i.Q.i.Q.i.Q.i.Q.Q.i#c#9.Q#c.i#c.t#c#c#c#c.t#c#c.t#c#cbo#cbO#cbO#cbO#c.FbO.Fbz.Fbz#vbz#vby#vby#vby#vbya8#v.2a8.2a8#ya8.2#y.2.2.2.2bU.2bR#ybRbRbUbRbU#dbU#d#d#F#d#d#F#d#Fbg#Fbg.Zbgbg.Zbgbgbgbgbgbgbg.Z.mbgbg#Jbg#J.m#J.m.m.m#J.m.m.m",
        ".3.m.m.m.D.m#W.maR.D#WaR#WaRa3aR.4aRa3.4a3a3#0.4#z#0#z#0#z#0b3#0#zb3#zb3#z#z#z.z#z#7.z#z#7.z#7b5#7b5.Q#7.B#7.Q#7.i.B.Q.i.Q.B.i.Q.i.Q.i.Q#9.Q#9.Q#c.i#c#9#c.i#c#9#c.t#c#c.t#c#cbo#cbO#cbo#c#cbO.FbO.Fbz.F.Fbz.F.Fby.F#vby#v#va8#v.2.2a8.2bn.2.2a8#ya8.2#y#y.2.2bR#ybRbRbUbRbR#dbU#d#dbU#d#d#F#dbg#Fbgbgbgbgbg.Zbgbg.Zbg.Zbg.Zbgbg#Jbg#Jbg.mbg#Jbg.m#J.m.m#J.m.3",
        ".m.3.m.D.m.m.m#W.maRaRaRaR#W#Wa3.4a3.4a3.4a3a3#0a3#0a3#0b3#z#0#zb3#z#z#z.z#z.z#z.z#z#7.z#7#7.z#7#7#7b5#7.Q.B.Q.B.Q.Q.i.Q.i.Q.i.B.Q.i.Q.i#c.Q.Q#9.Q.i#c.Q.i#c.i#c.i#c#c.t#c#c.t#cbO#c#c#cbO#cbO#c.F#cbz.Fbz.Fby.F#vbz#vby#vby#v#va8#va8.2.2a8#y.2bn.2#ya8.2.2#y.2bR#ybRbRbUbRbU#dbR#d#F#d#F#d#F#d#Fbg#Fbg.Zbgbg.Zbgbgbg.Zbgbg#Jbgbgbg#Jbg#J.m.m#J.mbg#J.m.m#J.m",
        ".m.m.D.m.m.D.m.m#WaR.D#WaRaRa3#WaRaRa3.4a3a3#0a3#z#0#z#0#z.0#z.0#zb3#zb3#z#z#z#z#7.z#z#7.z#7#7#7b5#7.Qb5.Q#7.Q.Q.B.Q.i.Q.i.i.Q.i.Q#9.Q.i.i.i#c.i#c.i.Q.t#c.t#c.t#c.t#c#c.t#c#c#cbo#cbobO#c#cbO.FbO.Fbz.Fbz.Fbz#vbz#vby#vby#v#va8#va8.2a8.2a8.2bn.2a8#y.2.2#y.2bR#ybRbUbRbUbR#dbU#dbU#dbU#d#d#dbgbg#Fbgbgbgbg.Zbgbg.Zbgbg.Zbgbg#Jbg#Jbgbg.m.Z.mbg#J.m#J.m#J.m.m",
        ".m.m.m.m.maRaR.maR.maRaRaR#WaRa3a3.4a3a3a3a3a3a3#za3#z#0#z#0#z#z#z#0#z#z#z.z#7#z#z#7#7#z#7#z#7#7#7#7#7#7.Q#7.Q#7.Q.Q.Q.Q.Q.Q.Q.i.Q.Q.Q.Q.Q.Q.Q#c.Q#c#c.Q#c.Q#c#c#c#c#c.i#c#c#c#c#c#c#c#c#c#c.FbO.F.F.F.F.F.F#v.F#v.F#v#v#v#vby#v.2#v.2.2.2.2.2.2.2.2.2.2.2bR.2bUbR.2bRbRbRbRbR#d#d#d#d#d#d#Fbg#dbgbgbg#Fbgbgbgbgbgbgbgbgbgbgbgbgbgbg.mbg#J#d.m.m.m.m.m.m.m.m.m",
        ".m.3.m.m.D.maR.DaRaRaRaR#WaRa3aRaRa3aRa3a3a3#0a3#0a3#0#z#0#z#z#0#z#z#z#z#z#z#z#7.z#z#7#z#7#7.z#7#7#7#7#7.Q#7.Q.Q.Q.Q.Q.i.Q.i.Q.Q.Q.i.Q.Q.t.Q.Q.Q#9#c.i.Q#c.i#c.Q#c.i#c#c#c#c#c#cbo#c#c#c#cbO#c#c.F#c.Fbz.Fbz.Fbz#vbz#vby#v#v.2#va8#va8#va8.2bn.2a8.2#y.2.2#y.2bR.2bRbRbRbRbU#dbUbR#dbU#d#d#d#d#d#d#Fbgbgbgbgbgbg.Zbgbgbgbgbgbgbgbg#Jbg.mbg.m#J.mbg.m.m.m#J.m.m",
        ".m.m.m.m.m.m.maRaR.DaRaRaRaR.4aRa3#Wa3.4a3a3a3a3#za3#za3#z#0#zb3#z#zb3#z#z#z#z#z#7#z.z#7#7#7#7#7#7#7#7#7.B#7.Q.Q.Q.Q.i.Q.Q.Q.Q.i.Q.Q.Q.Q.Q.Q.i#c.Q.Q#c#c.i#c#c.i#c#c#cbO.t#c#c#c#c#cbO#c#c#c.FbO.F.F.F.F.F#v.F#v.F#v#vby#v#v#v#va8#v.2.2.2.2.2.2#y.2.2#y.2bR.2bRbR#ybRbUbRbR#dbR#d#d#d#d#F#d#Fbgbg#dbgbgbgbg.Zbgbgbgbg.Zbgbg#Jbg.mbg.mbg.mbg.mbg#J.m#J.m.m.m.m",
        ".D.m.m.maR.maR.DaRaRaRaR.4aRaRa3aRa3a3a3a3a3#0a3#0a3#0#z#0#z#z#0#z#z#z#z#z#z.z#z#z#7#z#7#z#7.z#7#7#7.B#7.Q#7.Q#7.B.Q.Q.Q.Q.i.Q.Q.Q.i.Q.Q.Q.Q#c.Q.Q#c.i.Q#c#c#c#c#c.t#c#c#c#c#c#cbo#c#c#c#cbO.F#c.FbO.Fbz.Fbz.Fbz#vby.F#v#vby#v#v.2#v.2a8.2a8.2a8.2a8.2.2.2.2bR#ybRbRbRbRbRbRbU#dbR#dbU#d#d#dbg#d#dbgbg#Fbgbgbgbgbgbgbgbgbgbgbgbg#Jbgbgbg#Jbg.m.m.m.m.m.m#J.m.m",
        ".m.m.m.m.m.maR.maRaRaRaRaRaRa3aRa3aRa3aRa3a3a3a3#za3#z#z#z#0#z#z#z#z#z#z#z#7#z#7#z#7#7#z#7#7#7#7#7#7#7#7#7.Q.Q.Q.Q#7.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q#c.Q#c#c.Q.Q#c.Q#c#c#c#c#c#c#c#c#c#c#c#c#c.F#c.F.F.F.F.F.F.F#v.F#v.F#v#v#v#v#v#va8#v.2#v.2.2.2.2.2.2.2.2.2bR.2bR.2bRbRbRbR#dbR#dbR#d#d#d#d#d#d#dbg#dbgbgbgbgbgbgbgbgbgbgbgbg.mbgbg.m.mbg.m.mbg.m#d.m.m.m#d.m.m",
        ".m.m.m.maR.maR.maRaRaRaRaRaRaRa3aRa3a3a3a3a3a3a3a3#za3#0#z#z#z#0#z#z#z#z#z#z#z#z#7#z#z#7#7#z#7#7#7#7#7#7.Q#7#7.Q#7.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q#c.Q.Q#c.Q#c.Q#c#c#c.Q#c#c#c#c#c#c#c#c#c#c.F#c#c.F#c.F.F.F#v.F.F#v.F#v#vby#v#v.2#v#v#v.2#v.2.2.2.2.2.2.2.2.2.2bR.2bRbRbRbRbRbR#dbR#dbR#d#d#d#d#d#dbg#dbg#dbgbgbgbgbgbgbgbgbgbgbgbgbgbg.mbg.mbg.m.m.m.m.m.m.m.m.m",
        ".m.m.m.m.maR.maRaR.maRaRaRaRa3aRa3aRa3a3a3a3a3#za3#za3#za3#0#z#z#z#z#z#z#7#z#7#z#7#z#7#z#7#7#7#7#7#7#7#7.Q#7.Q#7.Q.Q.Q.Q#7.Q#7.Q.Q.Q.Q.Q.Q#c.Q.Q.Q#c.Q#c.Q#c.Q#c#c#c#c#c#c#c#c#c#c#c#c#c#c#c.F#c.F.F.F.F.F.F#v.F#v.F#v#v#v#v#v#v.2.2#v.2.2.2.2.2.2.2.2bR.2bR.2bRbRbRbRbRbRbRbR#d#d#d#d#d#d#dbg#d#dbgbgbgbgbgbgbgbgbgbgbgbg.mbg.mbg.m#d.mbg.mbg.mbg.m.m.m.m.m.m",
        ".m.m.m.maR.maR.maRaRaRaRa3aRaRaRa3aRa3a3a3a3a3a3a3a3#0#z#z#z#0#z#z#z#z#z#z#z#z#7#z#z#7#7#7#z#7#7#7#7#7.Q#7#7.Q.Q#Z.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.Q.F.Q.Q#c.Q.Q#c.Q#c.Q#c#c.Q#c.Q#c#c#c#c#c#c#c#c#c#c.F#cbz.F.F.F.F.F.F.F.F#v.F#v#v#v#v#v#v#v#v.2#v.2.2.2.2.2.2.2.2bR.2.2bR.2bRbRbRbRbR#dbRbR#dbR#d#d#d#d#dbg#dbg#dbgbgbgbgbgbgbgbgbgbgbgbgbgbg.mbg.m#d.m.m.m.m.m.m.m.m.m",
        ".m.maR.maR.maRaRaRaRaRaRaRaRa3a3aRa3a3aRa3a3a3#za3#za3a3#za3#z#za3#z#z#za3#7#za3#7.a#7#z.a#7.a#7#7#7#7#Z#7#7#Z.Q.Q#7#Z.Q.Q#Z.Q.Q#Z.Q.Q.F.Q.Q.Q.F.Q.F.Q.F.Q.F.Q.F#c#c.F#c#c#c#c.F#c#c.F#c.F#c.F.F#c.F.F.F.F#v.F#v.F#v#v.F#v#v#v#v.2#v.2#v.2#v.2.2.2.2.2.2.2bRbRbRbRbRbRbRbR#dbR#d#dbR#d#d#d#d#d#d#d#dbg#dbg#dbgbgbgbgbg.Wbgbg.W.m#d.mbg.mbg.m.m#d.m#d.m.m.m.m.m",
        ".m.m.m.maR.maR.maRaRaRaRa3aRaRaRa3aRa3a3a3a3a3a3a3a3#za3#z#z#z#z#z#z.a#z#7#z#z#7#z#7#z#7#7#7#7#7#7#Z#7#7.Q#7.i#Z.Q.Q.Q.Q.Q.i#Z.Q.Q.Q.Q.Q.Q.Q.Q#Z.Q.Q.Q.Q#c.Q#c.Q.F.Q#c.Q#c.F#c#c#c#c#c#c#c.F#c.F.F.F.F.F.F.F#v.F#v.F#v#v#v#v#v#v#v#v.2#v.2.2.2.2.2.2bR.2bR.2bR.2bRbRbRbRbRbR#dbRbR#d#d#d#d#dbg#dbg#dbg.Wbgbg.Wbgbg.Wbgbgbgbgbgbg.mbg.Wbg.m.Wbg.m.m.m.m.m.m.m.m",
        ".m.maR.maR.W.maR.WaRaRaRaRaRa3aRa3a3a3aRa3a3a3a3#za3#za3#z#0#za3#z#z#z#z.a#z.a#z#7a3#7#z.a#7#7.a#7#7#7#Z#7.Q#Z.Q#7.Q#Z.Q.Q#Z.Q.Q#Z.Q.Q#Z.Q.F.Q.Q#c.F.Q.F.Q#c.Q#c#c#c#Z#c#c#c#c.F#c.F#c.F#c.F.F#c.F.F.F.F#v.F.F#v.F#v#v.F#v#v#v.2#v.2#v.2#v.2.2.2.2.2.2.2bR.2bRbRbRbRbRbRblbR#dbR.W#dbR#d.W#d#d#dbg#dbgbgbgbgbgbgbgbgbg.Wbg.mbgbg.Wbg.mbg.mbg.m.W.m#d.m.W.m.m.m",
        ".W.maR.m.m.maR.maRaRaRaR.aaRaR.aa3aR.aa3a3.aa3a3#za3a3#za3#za3#z#z#z#z#z#z#z#z#7#z#7#z#7#7#z#7#7#7#7#7#7#Z#7.Q#7.Q#Z.Q.Q.Q.Q.Q.Q.i#Z.Q.Q.Q.Q.Q#Z.Q.Q#c.Q#c.Q.F#7.F.Q#c#c#c#Z#c#c#c#c#c#c#c.F#c.F.F.F.F.F.F.F#v.F#v.F#v#v#v#v#v#v#v#v.2#v.2#v.2.2.2.2bR.2.2bR.2bR.2blbRbR#dbR#dbR#dbR#d#d#d#d#d.W#dbg.W#dbg.Wbgbg.Wbgbgbgbgbg.W.mbg.mbg.m#d.m.mbg.m.m.m.m.m.m.m",
        ".m.m.W.maR.WaR.WaR.WaR.WaRaR.aaRaR.aa3a3.aa3.aa3a3#z#za3#z#z#za3#za3#z.a#7a3#7a3.a#7.a#7.a#7.a#7#Z#7#Z#7.Q#Z#7#Z.Q.Q#Z.Q#Z.Q#Z.F.Q.Q#Z.Q#Z.Q.F.Q.F#7.F#Z.Q#Z#c#c.F#7.F#Z#c#c.F#Z#c#Z#c#Z.F#c#Z.F.F.F.F#Z.Fbl.F#v.Fbl#v#v#v#v#v#v.2#vbl.2.2#vbR#v.2#v.2blbR#vbRblbRbRblbRblbRbl#dbl#d.W#d#d#d.W#d#d.Wbg#d.Wbg.Wbgbg.Wbg.Wbg.Wbgbg.Wbg.W.maR#daR.m#d.W.m.W.m.W.m",
        ".W.m.m.W.m.WaR.m.WaRaRaRaR.Wa3aRa3aR.aaR.aa3a3a3.aa3a3a3#za3#z.a#z#z.a#z.a#z.a#7#7#z#7.a#7.a#7.a#7#Z#7#7#Z#7.Q#Z.Q#Z.Q.Q.F.Q.Q#Z.Q#Z.Q.F.Q#Z.Q#Z.Q#Z.Q#c#7.F.Q.F#7.F#c#c#c#Z#c#c#c#Z#c.F#c#Z.F.F#Z.F.F#Z.F.F#v#Z#v.F#v.Fbl#vbl#vbl.2#v#vbR#v.2.2bR.2bl.2.2bl.2bRblbRbRbRbRbl#dbR#dbR#dbl#d.W#d#d.Wbg#dbg.Wbgbgbg.Wbgbg.Wbgbg.W.mbg.W.m#daR#d.m#daR.m.m.W.m.m.m",
        ".m.W.maR.W.maR.WaRaR.WaR.aaRaR.aaRa3a3a3.aa3.aa3#za3#z.a#za3#z#za3#z#z#7#z.a#za3#7a3#7#7#7#7.a#7#7#7#Z#7#Z#7#Z.Q.Q#Z.Q#Z.Q#Z.Q.Q#Z.Q.Q#Z.Q.Q#Z.Q.F.Q.F#7.F.Q#Z#c#c#Z#c#Z#c#Z#c#Z#c#c#Z#c#Z#c.F#c#Z.F.F.Fbl.F.F#v#Z#vbl#v#v#v#vbl#v#vbR#v.2#vbR#v.2#v.2bRbl.2bRblbRbRblbR#dbRbl#dbl#d.W#d#d#d#d.Wbg#d.Wbgbg.Wbg.Wbgbg.Wbg.Wbgbg.Wbg.m.Wbg.m.W.maR#d.m.W.m.m.W.m",
        ".m.m.W.maR.m.WaRaR.WaRaRaR.Wa3aR.aaR.aaRa3a3a3a3a3.aa3#za3#za3#z.a#z.a#z.a#7#z#7.a#7.aa3#7.a#7#7#Z#7#Z#7.Q#Z.Q#Z.Q#Z.Q.F.Q.Q#Z.Q.F.Q#Z.Q.Q#Z.Q.Q.Q#Z.Q.F.Q.F#c#7.F#c#c#Z#c#c.F#c#c.F#c.F#c.F.F#Z.F.F.F#Z.F.Fbl.F#v.F#v#Z#vbl#v#v.2#v#v.2bl.2.2.2bR.2bR#v.2bRbR.2blbRbRblbRbl#dbR#d.WbR#d.W#d.W#d#d.Wbg#d.Wbg.Wbgbg.Wbgbgbg.Wbgbg.Wbgbg.Wbg.mbg.W.m.W.m.m.m.W.m",
        "aR.W.m.W.W.WaR.WaR.WaR.W.aaR.aaR.aa3.Wa3.a.a.a.aa3.aa3.a#z.a#z.a#z.a#z.a#z.a.a#z.a#7.a#7.a#7.a#7#Z#7#Z#Z#7#Z#7#Z.Q#Z.Q#Z#Z#Z.Q#Z.Q#Z#Z.Q#Z.F#Z#Z#Z.Q#Z#7#Z.Q#Z.F#7#Z.F#Z#c#Z#c#Z#Z#c#Z#c#Z#Z.F#Z.F#Z.Fbl.Fbl.Fbl#v#Z#v#vbl#vblbl#vblbl#vbR#vbl.2bl#vbRblbRblblbRbRblbRbl#dblbR.WbR.W#dbl#d.W#d.W#d.W#d.Wbg.Wbg.W.Wbg.W.Wbg.Wbg.W.m.WaR#daR.W.W.m.W.m.W.W.m.W.m",
        ".m.W.m.WaR.m.W.m.WaR.WaR.aaR.aaR.aaR.aaRa3.aa3a3.aa3#z.aa3.aa3#z.a#z.a.a#z.a#7.a#7a3#7.a#7.a#7.a#Z#7#Z#7#Z#Z.Q#Z.Q#Z#Z.Q#Z.Q#Z.Q#Z#Z.Q.F.Q#Z.Q.Q.F.Q.F#c#Z.F#Z#c#Z.Q#c#Z#c#Z#c#Z#c#Z#c#Z#Z#c.F#Z.Fbl.F.F#v#Z#v#Z.Fbl#vbl#vbl#v#vbl#v#vbR#vbR#vbR#v.2bl.2#vbR#vbRblbRblbRbl#dbl#dbl#dbl#d.W#d.W#d.Wbg.W.Wbg.Wbg.Wbg.W#dbg.Wbg.Wbg.W#daR#daR#d.m#d.m.W.m.m.W.m.W",
        ".m.W.m.W.WaR.WaR.WaR.WaR.WaR.aaR.aa3.a.a.aa3.a.aa3.aa3#z.a#z.a#z.a#z.a#z.a#z.a#7.a#7.a#7.a#7#Z#7#7.a#7#Z.Q#Z#Z.Q#Z.Q.F#Z.Q#Z.F#Z.Q#Z.Q#Z.Q#Z.F#7#Z#Z.Q#Z.Q#Z#c#Z.F#Z#c#Z#c.F#Z#c#Z#c#Z#c.F#Z#Z.F.F#Z.F#Z.F#Z#v#Z#v#Z#v#Z#v#vbl#vbl.2bl#vbR#vbR#vbR#vbRblbRblbRblbRblbRblbRbl#dbl#dbl#d#d.W#dblbg.W#d.Wbg.Wbg.Wbg.Wbg.W.Wbg.W.Wbg.Wbg.W.m.W.m.WaR.W.m.W.m.W.maR",
        ".m.W.m.W.m.WaR.WaR.WaR.Wa3.WaR.aaR.aaR.aaR.aa3a3.a#z.a.aa3#za3#z.a#za3#7.a#z.a#7a3.a#7.a.a#7.a#Z#Z#7#Z#Z#7#Z.Q#Z#Z.Q#Z.Q#Z.Q#Z.Q#Z.Q#Z#Z.Q#Z.Q#Z.Q.F#7.F#Z.Q.F#7#c#Z.F#Z#c#Z#c#Z.F#Z#c#Z#Z#c.F.F#Z.F#Z.F#v#Z#v.Fbl.F#vbl#vbl#vbl#vbl.2bl.2#vbR.2bl.2bl.2bl.2blbRblbRblbRbR.WbRbl#dbl#dbl#d.W#d.W#d.Wbg.W#d.Wbg.Wbg.Wbgbg.Wbg.Wbg.W.m.Wbg.W.m.W#d.m.W.m.W.m.W.m",
        "aR.W.W.m.W.WaR.W.WaR.W.aaR.W.aaR.aaR.a.aa3.a.a.aa3.aa3.a#z.a.a.a.a.a.a#z.a.a.a.a#7.a.a#7.a#7#Z#7#Z#Z#7#Z#Z#7#Z#Z.Q#Z#Z#Z#Z#Z.Q#Z#Z#Z.Q#Z#Z.Q#Z#Z#Z.Q#Z.F#7#Z#Z.F#Z#c#Z#Z#c#Z.F#Z#c#Z.F#Z.F#Z#Z#Z.F#Z.Fbl.F#Zbl#Z#vblbl.Fbl#vbl#vbl#vbl#vblbl.2bl.2bl.2blbRblbRblbRblbR.WblbR.WbR.W.Wbl#d.Wbl#d.W.W#d.W.W.Wbg.W.W.W.W.W.Wbg.W.W.Wbg.W.W#d.W.W.m.W.W.W.m.W.W.W.m",
        ".W.W.m.W.W.m.W.WaR.W.WaR.W.aaR.aaR.a.a.aaRa3.aa3.a.aa3.aa3.a#z.a#za3#7.a.a#7.a#7.a.a#7.a#Z.a#7.a#Z#7#Z.a.Q#Z#Z#Z#Z.Q#Z.Q#Z.Q#Z#Z.Q#Z#Z#Z.Q#Z.F#Z.Q#Z#Z#Z.F.F#Z.Q#Z#Z#c#Z#Z#c#Z#c#Z#Z#c#Z.F#Z.F#Z.Fbl.F#Z.Fbl.Fbl.Fbl.Fbl#vblblbl#vbl.2bl#vbR#vbl#vbRbl#vbR#vblbRblblblbRblbl#dbl.WbR.W#d.W#d.W.W#d.W.Wbg.W.Wbg.Wbgbg.Wbg.W.WbgaR.Wbg.WaR.m.W.W.m.W.m.W.m.W.m.W",
        ".m.W.WaR.W.WaR.W.WaR.aaR.WaR.a.W.aaR.aaR.a.a.aa3.aa3.a.a#z.aa3#z.a.a#z.a#7a3.a#7.a#7.a.a#7.a#Z#7#Z#Z#7#Z#Z.Q#Z.Q#Z#Z#Z#Z.Q#Z#Z.Q#Z#Z.Q#Z#Z.Q#Z.Q#Z.F.Q#Z.Q#Z#c#Z.F#7#Z.F#c#Z.F#Z#c#Z#Z#c#Z.F#Z.F#Z#Z.Fbl.Fbl.Fblbl.Fbl#v#Z#v#vblbl#vblbl.2blbR#vbRbl.2blblbRblblbRbRblbl#dblbl#dbl#dbl#d.Wbl#d.W#d.W#d.Wbg.W.Wbg.W.Wbg.Wbg.W.W#d.WaR#d.W.Wbg.W.W.m.W.WaR.W.W.m",
        "aR.W.m.W.m.WaR.WaR.W.WaR.W.aaR.aaR.a.a.a.aa3.Wa3.a.a#z.aa3.a#z.a.a#z.a.a.a#7.a#z.a.a.a#7.a#7#Z.a#7#Z#Z#7#Z#Z#Z#Z.Q#Z.Q#Z#Z.Q#Z#Z#Z.Q#Z#Z.Q#Z.F#Z#Z.Q#Z.F#Z#Z#c#Z#Z.F#Z#c#Z#Z#c#Z#Z#c#Z.F#Z#Z.F#Zbl.F#Z.Fbl.F#Z#v#Z#v#Z#vblbl#vbl#vbl#vbl.2bl#vblbR#vbl.2bl.2blbRblblbRblbR.WbR.Wbl#d.Wbl#d.W#d.W.W#d.W.W.Wbg.W.Wbg.W.Wbg.W.W#daR#d.Wbg.W.m.W.m.W.W.m.W.m.W.m.W",
        ".W.W.W.W.W.W.W.W.W.W.a.W.aaRbl.aaR.aaR.aaR.a.a.a.aa3.a.a.a.a.a.a#z.a.a#z.a.a.a#Z.a#7.a#Z.a#Z.a#Z#Z.a#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z.F#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#c#Z#Z#Z.F#Z#Z#Z.F#Z#Z#Z#Z.F#Z#Z.F#Zblbl#Zbl.Fbl#Zblblbl#Z#vblblblblblblblblbRblblblbRblblblblblblbR.Wblblbl.Wbl#dbl.W.W.W.W.W.Wbl.W.W#d.W.W.W.W.W.W.W.W.Wbg.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W",
        ".m.W.WaR.W.W.WaR.a.WaR.W.WaR.a.W.a.W.aaR.a.a.a.a.a.a.a.a.a#z.a.a.a.a.a.a.a.a.a.a.a.a#7.a#Z.a#7.a#Z#7#Z#Z#Z#Z.Q#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z.Q#Zbl.Q#Z#Z.Q#Z#Z#Z.Q#Z.F#Z#Z#Z.F#Z#Z#Z#c#Z#Z#Z#Z.F#Z#Z#Z#Z#Zbl#Z#Z.Fbl#Zblbl.Fbl.Fblblblblbl#vbl#vbl#vblbl.2bl#vblblbRblbRblblblbl#dblbR.Wblbl.WbR.Wbl#d.W.W#d.W.W.W.W.W.Wbg.W.Wbg.W.W.W.W.W.W.Wbg.W.W.W.m.W.m.W.m.W.WaR",
        ".W.W.W.W.WaR.W.W.W.W.W.a.W.aaR.aaR.a.a.a.W.aaR.a.aa3.a.aa3.a.a.a#z.a.a#Z#7.a.a#7.a#Z.a#Z.a#7#Z#Z#Z#Z#Z#Z#7#Z#Z#Z.Q#Z#Z.Q#Z#Z.Q#Z#Z#Z#Z#Z.Qbl#Z#Z#Z#Z.Q#Z#Z#Z#Z#Z.F#Z#Z#c#Z#Z#Z#Z#c#Z#Z#Z.F#Z.F#Z.F#Z.Fblbl.Fbl#Zblblblbl#v#Z#vbl#vblblbRblbl.2blblblbRblblblblblblbRblblbl.Wbl.WbR.W.Wbl.W#d.W.W.W.W.Wbg.Wbg.W.W.W.W.W.W.W.Wbg.W.Wbg.W.W.W.m.W.W.W.W.W.W.W.mbl",
        ".m.W.W.m.W.W.W.W.WaR.a.WaR.W.a.W.a.W.aaR.a.a.a.a.a.a.a.a.a.a.a.a.a.a#7.a.a.a#Z.a.a.a#7.a#Z#Z.a#Z.a#Z#Z.a#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Zbl.Q#Z#Z#Z#Z#Z.Q#Z.F#Z#Z#Z.F#Z.Q#Z#Z#c#Z#Z#Z#Z.F#Z#Z#Z.F#Z#Z#Z#Z#Zbl#Zbl#Z#Z.Fbl#Z#v#Z#v#Zblblblblblblblbl#vblbl.2bl#vblbl#vbRblbRblbl.WbR.WbRbl.Wbl#dbl#d.Wbl.W.W#d.W.W.W.W.W.W.W.Wbg.W.Wbg.W.W.WaR.W.W.W.W.W.W.W.W.m.W.W.W.WaR",
        ".W.W.W.W.W.W.W.W.W.W.W.a.W.a.W.a.W.a.W.a.aaR.a.W.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a#Z.a#Z.a#Z.a#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z.F#Z#Z#Z#Z#Z#Z#Z#Z#Zbl#Z#Z#Zbl#Zbl#Zbl#Zbl#Zblblbl#Zblblbl#vblblblblblblblblblblblblblblblblblbl.Wbl.Wbl.Wbl.Wbl.W.W.Wbl.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W",
        ".W.W.W.W.W.W.W.W.W.a.W.W.a.W.a.W.a.W.a.a.W.a.a.a.a.W.a.a.a.a.a.a.a.a.a.a.a#Z.a.a#Z.a#Z.a#Z.a.a#Z.a#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Zbl#Z.F#Zbl#Z.F#Zblbl#Zbl#Zblbl#Zbl#vblblblblblblblblblblblblblblblblblblbl.Wbl.Wblbl.Wblbl.Wbl.Wbl.Wbl.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W",
        ".W.W.W.W.W.W.a.W.a.W.W.a.W.W.a.W.a.W.a.W.a.W.aaR.a.a.a.a.a.a.a.a.a.a#Z.a.a.a#Z.a.a#Z.a#Z.a#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Zbl#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Zbl#Z#Zbl#Zbl#Zbl.Fblbl#Zblblbl#Zblblblblblblblblblblblblblblblblblbl.Wblblbl.Wblbl.W.Wbl.Wbl.W.W.Wbl.Wbl.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.WaRbl.W.W.W.W.W.W",
        ".W.W.W.W.W.W.W.W.W.a.W.W.a.W.a.W.a.W.a.W.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a#Z.a.a#Z.a#Z.a#Z.a#Z.a#Z.a#Z#Z#Z#Z#Z#Z#Z#v#Z#Z#Z#v#Z#Z#Z#Z#Z#Zbl#Z#Zbl#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z.F#Z#Z#Z.F#Z#Z#Z#Zbl#Zbl#Zbl#Zbl#Z#Zblbl#ZblblblblblblblblblblblbRblblblblblblblblblbl.Wblblbl.Wbl.Wbl.W.Wbl.Wbl.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W",
        ".W.W.Wbl.a.W.W.a.Wbl.W.a.Wbl.a.W.a.W#Z.W.abl.a.W.a.W.a.W.a.a.a.a#Z.a#Z.a#Z.a#Z.a#Z.abl.a#Z.a#Z#Zbl#Z.abl#Zbl#Zbl#Z#Z#Zbl#Z#Z#Z#Z#Zbl#Z#Z#Z#Z#Z#Z#Zbl#Zbl#Zbl#Z#Zbl#Z#Zbl#Z#Z#Z.abl#Zbl.abl#Zbl#Z#Zbl#Zbl#Z#Zblblbl#Zblbl#Zblbl#Zblblblblblblblblblblblblblbl.Wblblblbl.Wbl.Wblbl.Wbl.Wbl.Wbl.W.Wbl.W.Wbl.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W.W",
        ".W.a.W.W.W.Wbl.W.a.W.a.W.W.a.W.abl.a.W.a.a.W.abl.a.a.a.a.a.W.a.a.a.a#Z.a.a#Z.a.abl.a#Z.a#Z#Z.W#Z.a#Z#Z#Z.a#Z#Z#Z#Z#Z#Z#Z#Z#Z#Zbl#Z#Z#Z#Zbl#Z#Zbl#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Zbl#Z#Z#Z#Z#Z#Zbl#Zbl#Zbl#Z#Zbl#Zbl#Zbl#Zblbl#Zblbl#Zblblblblblblblblblblblblblbl.Wbl.Wblbl.Wbl.Wbl.Wbl.Wbl.W.Wbl.Wbl.Wbl.Wblbl.Wbl.W.W.W.W.a.W.W.W.W.W.W.W.W.W.W.W.Wbl.W.Wbl.W.W",
        ".Wbl.W.W.a.W.a.Wbl.W.W#Z.W.a.W.a.W.a.W.a.W.a.a.a.W.a.a.a.a#Z.abl.a.a.W#Z.a.W#Z.a#Z.a#Z.W#Z.a#Z.a#Zbl.a#Z#Zbl#Z#Zbl#Zbl#Z#Zbl#Z#Z#Zbl#Z#Z#Z#Z#Z#Z#Zbl#Z#Zbl#Zbl#Z#Zbl#Zbl#Zbl#Z#Z#Zbl#Zbl#Zbl#Z#Zbl#Z#Zblbl#Zbl#Zbl#Zblbl#Zblblblblblblblblblblblblblblblblblblblblbl.Wblblbl.Wbl.W#Z.Wbl.abl.W.a.Wbl.W.a.W.W.a.W.W.a.W.W.W.W.a.W.W.W.W.Wbl.W.Wbl.W.W.W.W.W.W.W",
        ".W.W.W.W.Wbl.W.W.W.a.W.a.Wbl.a.W.abl.a.W#Z.a.W.a#Z.W.abl.a.W.a.a#Z.a#Z.a.a#Z.a#Z.abl.a#Z#Z.a#Zbl.a#Z#Zbl#Z.a#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Zbl#Zbl#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Z#Zbl#Z#Z#Z#Zbl#Z.a#Zbl.abl#Zbl.ablbl#Zblbl#Zblblblblbl#Zblblblblblblblblblbl.Wblbl.Wbl.Wblblbl.Wbl.Wbl.Wbl.Wbl.Wbl.Wbl.W.W.Wbl.W.Wbl.W.W.W.Wbl.W.Wbl.W.Wbl.a.W.W.a.W.W.a.W.W#Z.W.W",
        ".W#Z.W#Z.W.abl.abl.abl.W.a.W#Z.W#Z.W.abl.abl.abl.a.abl.a.abl.abl.abl.abl.abl.abl.a#Z.W#Z.W#Zbl.a#Zbl.a#Zbl#Z.Wbl#Zbl#Zblbl#Zblbl#Zblbl#Zbl#Z#Zbl#Zbl#Zbl#Zbl#Z.Wbl#Z.W#Z.W#Z.a#Z.abl.a#Z.abl#Zbl.a#Zbl.abl#Z.W#Z.W#Zblbl.abl#Zbl.ablbl#Z.Wblbl.Wblbl.Wblbl.W#Zbl.W#Zbl.a.W#Zbl.abl.abl.W#Z.W.abl.a.W.a.W.a.W.a.W.abl.abl.a.W#Z.W.abl.a.Wbl.abl.W#Z.Wbl.a.Wbl.a",
        "bl.W.abl.Wbl.a.W.abl.W.abl.abl.a.Wbl.a.W#Z.W.a.W.abl.a.W#Z.W.a#Z.W.a#Z.W#Z.W#Z.W#Z.W#Z#Z.W#Z.abl#Z.W#Zbl.abl#Z#Z.W#Zbl#Z#Zbl#Z.abl#Z#Zbl#Zbl#Zbl.a#Zbl.abl.abl#Z.W#Z#Z.W#Z#Zbl.abl#Z.W#Zbl#Zbl.ablbl#Zbl.ablbl#Zblbl.ablbl#Z.Wblbl#Z.Wblbl.ablbl.abl#Z.W#Zblbl.W#Zbl.Wblbl.W#Z.Wblbl.W#Z.Wbl.Wbl.W#Z.Wblbl.abl.Wbl.W#Z.W.W#Z.Wbl.a.Wbl.abl.W#Z.W.W#Z.W.W#Z.W.W",
        ".W#Z.W.W#Z.W.W#Z.W.abl.W.abl.W.abl.a.W#Z.W.abl.abl.a.abl.a#Z.W#Z.abl.a#Z.W#Z.a#Z.W#Z.W#Z.abl#Z.abl#Z.abl#Z.abl.a#Zbl.a#Z.W#Z.abl#Z.abl.a#Z.W#Z.abl#Z.abl#Z#Z.a#Z#Zbl.a#Zbl.abl#Z#Z.W#Zbl.abl.abl#Z.abl.abl#Z.W#Z.W#Zbl.ablbl#Z.Wbl.W#Zblblblbl.Wblblblbl.W#Z.W#Z.Wbl#Z.W#Z.Wbl.W#Z.W#Z.Wbl.abl.abl.Wbl.a.Wbl.a.W#Z.W.W#Z.W.W.W#Z.Wbl.a.W.W.a.Wbl.W.W#Z.W.W#Z.W",
        ".a.Wbl.a.Wbl.abl.Wbl.a.W#Z.W.abl.a.W#Z.W.abl.abl.a.W#Z.W.a.W#Z.abl.abl.a#Z.W#Z.W#Z#Z.abl.abl.abl#Z.abl#Z.abl#Z#Zbl#Z#Zbl#Z#Zbl#Z#Zbl.a#Zbl.a#Zbl#Zbl#Z#Z.W#Zblbl.abl#Zbl.abl#Z.abl#Z#Z.abl#Zbl#Z.Wbl#Zbl#Z.W#Zblbl.ablbl.abl.W#Zbl#Zbl.W#Z.W#Zbl#Z.W#Z.W#Zblblblbl#Z.Wblbl.abl#Z.Wbl.Wbl.W#Z.Wbl.a.W#Z.Wbl.abl.W.W#Z.W.Wbl.abl.W.a.Wbl.Wbl.W#Z.W#Z.W.W#Z.Wbl.W"
    ]

class PyPalette(canvas.Canvas):
    """The Pure-Python Palette

    The PyPalette is a pure python implementation of a colour palette. The
    palette implementation here imitates the palette layout used by MS
    Windows and Adobe Photoshop.

    The actual palette image has been embedded as an XPM for speed. The
    actual reverse-engineered drawing algorithm is provided in the
    GeneratePaletteBMP() method. The algorithm is tweakable by supplying
    the granularity factor to improve speed at the cost of display
    beauty. Since the generator isn't used in real time, no one will
    likely care :) But if you need it for some sort of unforeseen realtime
    application, it's there.
    """

    TRANSITION_STEP = 18
    HORIZONTAL_STEP = 2
    VERTICAL_STEP   = 4

    def __init__(self, parent, id):
        """Creates a palette object."""
        # Load the pre-generated palette XPM
        self.palette = wxBitmapFromXPMData(getPaletteXpm())
        canvas.Canvas.__init__(self, parent, id, size=wxSize(175, 192))

    def GetValue(self, x, y):
        """Returns a colour value at a specific x, y coordinate pair. This
        is useful for determining the colour found a specific mouse click
        in an external event handler."""
        return self.buffer.GetPixel(x, y)

    def DrawBuffer(self):
        """Draws the palette XPM into the memory buffer."""
        self.buffer.DrawBitmap(self.palette, 0, 0, 0)

    def GeneratePaletteBMP(self, file_name, granularity=1):
        """The actual palette drawing algorith, This was 100% reverse
        engineered by looking at the values on the MS map. Speed is tweakable
        by changing the granularity factor, but that affects how nice the
        output looks (makes the vertical blocks bigger. This method was used
        to generate the embedded XPM data."""
        # Start of MS layout cloning
        self.vertical_step = self.VERTICAL_STEP * granularity

        width, height = self.GetSize()
        x_pos = 0

        # Draw the red-yellow transtiion
        for x in range(0, 256, self.TRANSITION_STEP):
            for y_pos in range(0, height, self.vertical_step):
                offset = int(float(y_pos) / 1.5)
                r, b = (255 - offset, offset)
                g = int(x * float(1 - ((2 * offset) / 255.0))) + offset
                colour = wxColour(r, g, b)
                self.buffer.SetPen(wxPen(colour, 1, wxSOLID))
                self.buffer.DrawRectangle(x_pos, y_pos, self.HORIZONTAL_STEP, self.vertical_step)
            x_pos = x_pos + self.HORIZONTAL_STEP

        # Draw the yellow-green transition
        for x in range(0, 256, self.TRANSITION_STEP):
            r = 255 - x
            r_step = float(r - 127) / (height / self.vertical_step)
            for y_pos in range(0, height, self.vertical_step):
                offset = int(float(y_pos) / 1.5)
                b = offset
                g = 255 - offset
                colour = wxColour(int(r), g, b)
                self.buffer.SetPen(wxPen(colour, 1, wxSOLID))
                self.buffer.DrawRectangle(x_pos, y_pos, self.HORIZONTAL_STEP, self.vertical_step)
                r = r - r_step
            x_pos = x_pos + self.HORIZONTAL_STEP

        # Draw the green-turquoise transition
        for x in range(0, 256, self.TRANSITION_STEP):
            for y_pos in range(0, height, self.vertical_step):
                offset = int(float(y_pos) / 1.5)
                r = offset
                g = 255 - offset
                b = int(x * float(1 - ((2 * offset) / 255.0))) + offset
                colour = wxColour(r, g, b)
                self.buffer.SetPen(wxPen(colour, 1, wxSOLID))
                self.buffer.DrawRectangle(x_pos, y_pos, self.HORIZONTAL_STEP, self.vertical_step)
            x_pos = x_pos + self.HORIZONTAL_STEP

        # Draw the turquoise-blue transition
        for x in range(0, 256, self.TRANSITION_STEP):
            g = 255 - x
            g_step = float(g - 127) / (height / self.vertical_step)
            for y_pos in range(0, height, self.vertical_step):
                offset = int(float(y_pos) / 1.5)
                r = offset
                b = 255 - offset
                colour = wxColour(r, int(g), b)
                self.buffer.SetPen(wxPen(colour, 1, wxSOLID))
                self.buffer.DrawRectangle(x_pos, y_pos, self.HORIZONTAL_STEP, self.vertical_step)
                g = g - g_step
            x_pos = x_pos + self.HORIZONTAL_STEP

        # Draw the blue-pink transition
        for x in range(0, 256, self.TRANSITION_STEP):
            for y_pos in range(0, height, self.vertical_step):
                offset = int(float(y_pos) / 1.5)
                g = offset
                b = 255 - offset
                r = int(x * float(1 - ((2 * offset) / 255.0))) + offset
                colour = wxColour(r, g, b)
                self.buffer.SetPen(wxPen(colour, 1, wxSOLID))
                self.buffer.DrawRectangle(x_pos, y_pos, self.HORIZONTAL_STEP, self.vertical_step)
            x_pos = x_pos + self.HORIZONTAL_STEP

        # Draw the pink-red transition
        for x in range(0, 256, self.TRANSITION_STEP):
            b = 255 - x
            b_step = float(b - 127) / (height / self.vertical_step)
            for y_pos in range(0, height, self.vertical_step):
                offset = int(float(y_pos) / 1.5)
                r = 255 - offset
                g = offset
                colour = wxColour(r, g, int(b))
                self.buffer.SetPen(wxPen(colour, 1, wxSOLID))
                self.buffer.DrawRectangle(x_pos, y_pos, self.HORIZONTAL_STEP, self.vertical_step)
                b = b - b_step
            x_pos = x_pos + self.HORIZONTAL_STEP

        # Now save the palette to the file
        dc = wxClientDC(self)
        bitmap = wxEmptyBitmap(width, height, -1)
        self.buffer.SelectObject(bitmap)
        self.buffer.Blit(0, 0, width, height, dc, 0, 0)
        self.buffer.SelectObject(wxNullBitmap)
        image = wxImageFromBitmap(bitmap)
        image.SaveFile(file_name, wxBITMAP_TYPE_BMP)
