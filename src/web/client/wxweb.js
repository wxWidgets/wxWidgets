//Copyright (c) 2007 John Wilmes


// requires Prototype, JSON, Prototype-Graphic, and parts of Base2
// see http://www.prototypejs.org/download
// and http://www.json.org/json.js
// and http://prototype-graphic.xilinus.com/
// and http://base2.googlecode.com/svn/trunk/src/base2/Base.js

// Event Type Definitions
//TODO - better way to do this than redefining in JS?
var wxEVT_LEFT_DOWN = 100;
var wxEVT_LEFT_UP = 101;
var wxEVT_MIDDLE_DOWN = 102;
var wxEVT_MIDDLE_UP = 103;
var wxEVT_RIGHT_DOWN = 104;
var wxEVT_RIGHT_UP = 105;
var wxEVT_MOTION = 106;
var wxEVT_LEFT_DCLICK = 109;
var wxEVT_MIDDLE_DCLICK = 110;
var wxEVT_RIGHT_DCLICK = 111;
var wxEVT_CHAR = 212;
var wxEVT_KEY_DOWN = 215;
var wxEVT_KEY_UP = 216;

var App = Base.extend({
    constructor: null,
}, {
    initialize: function(url, canvas) {
        if (App.initialized) {
            throw "Already initialized!";
        }
        App.url = url;
        App.canvas = canvas;
        App.dc = new DC(this.canvas);
        App.evtManager = new EventManager(this.dc);
        App.initialized = true;
    },

    getDC: function() {
        return this.dc;
    },

    getEventManager: function() {
        return this.evtManager;
    },

    getUrl: function() {
        return this.url;
    },

    processUpdate: function(response) {
        App.eval(response);
    },

    canvas: "",

    dc: null,

    evtManager: null,

    initialized: false,

    url: ""
});

var EventState = Base.extend({
    /* Constructor */
    constructor: function() {
    },

    /* Key State */
    altDown: false,
    controlDown: false,
    metaDown: false,
    shiftDown: false,

    /* Mouse State */
    leftDown: false,
    middleDown: false,
    rightDown: true,
    x: 0,
    y: 0
});

var EventManager = Base.extend({
    /* Constructor */
    constructor: function(dc) {
        this.timer = setInterval(this.sendEvents.bind(this));
        this.dc = dc;

        Event.observe(document, 'mouseDown',
            this.mouseDown.bindAsEventListener(this), false);
        Event.observe(document, 'mouseUp',
            this.mouseUp.bindAsEventListener(this), false);
        Event.observe(this.dc.getElement(), 'doubleClick',
            this.doubleClick.bindAsEventListener(this), false);
        Event.observe(document, 'mouseMove',
            this.mouseMove.bindAsEventListener(this), false);
        Event.observe(document, 'keyDown',
            this.keyDown.bindAsEventListener(this), false);
        Event.observe(document, 'keyUp',
            this.keyUp.bindAsEventListener(this), false);
        Event.observe(document, 'keyPress',
            this.keyPress.bindAsEventListener(this), false);
    },

    mouseDown: function(event) {
        var type = -1;
        //FIXME: button detection not 100% accurate for some browsers and for
        //  simultaneous clicking of multiple buttons
        if (Event.isLeftClick(event)) {
            type = wxEVT_LEFT_DOWN;
            this.eventState.leftDown = true;
        } else if (event.which == 3 || event.button == 2) {
            type = wxEVT_RIGHT_DOWN;
            this.eventState.rightDown = true;
        } else {
            type = wxEVT_MIDDLE_DOWN;
            this.eventState.middleDown = true;
        }
        this.doMouseEvent(type, event);
    },

    mouseUp: function(event) {
        var type = -1;
        if (Event.isLeftClick(event)) {
            type = wxEVT_LEFT_UP;
            this.eventState.leftDown = true;
        } else if (event.which == 3 || event.button == 2) {
            type = wxEVT_RIGHT_UP;
            this.eventState.rightDown = true;
        } else {
            type = wxEVT_MIDDLE_UP;
            this.eventState.middleDown = true;
        }
        this.doMouseEvent(type, event);
    },

    mouseMove: function(event) {
        this.eventState.x = this.getEventX(event);
        this.eventState.y = this.getEventY(event);
        this.doMouseEvent(wxEVT_MOTION, event);
    },

    doubleClick: function(event) {
        var type = -1;
        if (Event.isLeftClick(event)) {
            type = wxEVT_LEFT_DCLICK;
        } else if (event.which == 3 || event.button == 2) {
            type = wxEVT_RIGHT_DCLICK;
        } else {
            type = wxEVT_MIDDLE_DCLICK;
        }
        this.doMouseEvent(type, event);
    },

    keyDown: function(event) {
        this.updateKeyState(event, true);
        this.doKeyEvent(wxEVT_KEY_DOWN, getWxKeyCode(event.keyCode), event);
    },

    keyUp: function(event) {
        this.updateKeyState(event, false);
        this.doKeyEvent(wxEVT_KEY_UP, getWxKeyCode(event.keyCode), event);
    },

    keyPress: function(event) {
        this.doKeyEvent(wxEVT_CHAR, (event.charCode || event.keyCode), event);
    },

    doMouseEvent: function(eventType_, event) {
        var wxevt = {
            eventType: eventType_,
            eventState: Object.clone(this.eventState)
        };
        this.addEvent(wxevt);
    },

    doKeyEvent: function(eventType_, keyCode_, event) {
        var wxevt = {
            eventType: eventType_,
            eventState: Object.clone(this.eventState),
            keyCode: keyCode_,
        };
        this.addEvent(wxevt);
    },

    getWxKeyCode: function(jsKeyCode) {
        //TODO - translate per
        //  http://www.wxwidgets.org/manuals/stable/wx_keycodes.html#keycodes
        return jsKeyCode;
    },

    updateKeyState: function(event, down) {
        // detect special keys for most browsers
        // this is the best way especially for some mac browsers
        this.eventState.altDown = (event.altKey || false);
        this.eventState.controlDown = (event.ctrlKey || false);
        this.eventState.metaDown = (event.metaKey || false);
        this.eventState.shiftDown = (event.shiftKey || false);
        // override some buggy event.xxxKey implementations
        // opera is probably still broken for some of these
        //TODO get rid of magic numbers
        if (event.keyCode == 18) { //alt key
            this.eventState.altDown = down;
        } else if (event.keyCode == 17) { // ctrl key
            this.eventState.ctrlDown = down;
        } else if (event.keyCode == 91) { //windows key
            this.eventState.metaDown = down;
        } else if (event.keyCode == 16) { //shift key
            this.eventState.shiftDown = down;
        }
    }
     

    getEventX: function(event) {
        var dim = dc.getElement().getDimensions();
        var scroll = getWindowScroll(window);
        var offset = dc.getElement().getOffset();
        var x = Event.pointerX(event) - offset[0] - scroll.left;
        if (x < 0) {
            x = 0;
        }
        if (x > this.dimension.width) {
            x = this.dimension.width;
        }
        return x;
    },

    getEventY: function(event) {
        var dim = dc.getElement().getDimensions();
        var scroll = getWindowScroll(window);
        var offset = dc.getElement().getOffset();
        var y = Event.pointerY(event) - offset[1] - scroll.top;
        if (y < 0) {
            y = 0;
        }
        if (y > this.dimension.height) {
            y = this.dimension.height;
        }
        return y;
    },


    /* Methods */
    addEvent: function(event) {
        queue.push(event);
    },

    clearEvents: function() {
        queue.clear();
    },

    getEvents: function() {
        return queue;
    },

    sendEvents: function() {
        var req = new Ajax.Request(
            App.getUrl(),
            {
                method: 'post',
                postBody: this.getEvents().toJSONString(),
                onSuccess: this.onSendSuccess.bind(this),
                onFailure: this.onSendFailure.bind(this),
                onException: this.onSendException.bind(this)
            });
        activeReqs[req.transport] = this.getEvents();
        this.clearEvents();
    },

    onSendSuccess: function(request) {
        App.processUpdate(request.responseText);
        this.numFailures = 0;
        delete activeReqs[request];
    },

    onSendFailure: function(request) {
        this.numFailures++;
        queue = queue.concat(activeReqs[request]);
        delete activeReqs[request];
    },

    onSendException: function(request) {
        //TODO - take approporiate action based on what stage of the request
        //      caused the exception
        clearInterval(timer);
        timer = null;
    },

    /* Attributes */
    activeReqs: [],

    numFailures: 0,
    
    queue: [],

    interval: 1000,

    timer: null,

    dc: null
});

var DC = Base.extend({
    constructor: function(canvas) {
        if (Graphic.rendererSupported("VML")) {
            this.renderer = new Graphic.VMLRenderer(canvas);
        } else if (Graphic.rendererSupported("SVG")) {
            this.renderer = new Graphic.VMLRenderer(canvas);
        } else if (Graphic.rendererSupported("Canvas")) {
            //TODO warning, may not be fully implemented
            this.renderer = new Graphic.VMLRenderer(canvas);
        } else {
            //TODO error
        }
    },

    getElement: function() {
        return $(this.renderer.element);
    },

    Clear: function() {
        this.renderer.clear();
    },

    DrawPoint: function(x, y) {
        this.DrawLine(x, y, x, y);
    },

    DrawLine: function(x1, y1, x2, y2) {
        var line = new Graphics.Line(this.renderer);
        line.setPoints(x1, y1, x2, y2);
        this.renderer.add(line);
    },

    DrawArc: function(x1, y1, x2, y2, xc, yc) {
    },

    DrawEllipticArc: function(x, y, w, h, sa, ea) {
    },

    DrawRectangle: function(x, y, w, h) {
        var rect = new Graphics.Rectangle(this.renderer);
        line.setBounds(x, y, w, h);
        this.renderer.add(rect);
    },

    DrawRoundedRectangle: function(x, y, w, h, r) {
    },

    DrawEllipse: function(x, y, w, h) {
        var ellipse = new Graphics.Ellipse(this.renderer);
        ellipse.setBounds(x, y, w, h);
        this.renderer.add(ellipse);
    },

    CrossHair: function(x, y) {
    },

    DrawIcon: function(url, x, y) {
        this.DrawBitmap(url, x, y);
    },

    DrawBitmap: function(url, x, y) {
        var image = new Graphics.Image(this.renderer);
        image.setSource(url, true);
        image.setLocation(x, y);
        this.renderer.add(image);
    },

    DrawText: function(textValue, x, y) {
        DrawRotatedText(textValue, x, y, 0);
    },

    DrawRotatedText: function(textValue, x, y, angle) {
        var text = new Graphics.Text(this.renderer);
        text.setTextValue(textValue);
        text.setLocation(x, y);
        text.rotate(angle);
        this.renderer.add(text);
    },

    DrawLines: function(n, points, xoffset, yoffset) {
        var lines = new Graphics.Polyline(this.renderer);
        lines.addPoints(points);
        lines.translate(x, y);
        this.renderer.add(lines);
    },

    DrawPolygon: function(n, points, xoffset, yoffset, fillstyle) {
        var poly = new Graphics.Polygon(this.renderer);
        poly.addPoints(points);
        poly.translate(xoffset, yoffset);
        this.renderer.add(poly);
    },

    renderer: null
});

var Colour = Base.extend({
    constructor: function(arg, green, blue, alpha = 255) {
        if (arg.red) {
            this.red = red.red;
            this.green = red.green;
            this.blue = red.blue;
            this.alpha = (red.alpha || 255)
        } else {
            this.red = arg;
            this.green = green;
            this.blue = blue;
            this.alpha = alpha;
        }
    },

    red: 255,
    green: 255,
    blue: 255,
    alpha: 255
});

var Brush = Base.extend({
    constructor: function(arg, colour) {
        if (arg.style) {
            this.style = arg.style;
            this.colour = new Colour(arg.colour);
        } else {
            this.style = arg;
            this.colour = new Colour(colour);
        }
    }
});

var Pen = Base.extend({
    constructor: function(arg, style, join, cap, colour) {
        if (arg.width) {
            this.width = arg.width;
            this.style = arg.style;
            this.join = arg.join;
            this.cap = arg.cap;
            this.colour = new Colour(arg.colour);
        } else {
            this.width = arg;
            this.style = style;
            this.join = join;
            this.cap = cap;
            this.colour = new Colour(colour);
        }
    }
});
