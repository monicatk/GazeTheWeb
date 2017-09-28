//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================


// Helper function for console output
function ConsolePrint(msg)
{
	window.cefQuery({ request: (""+msg), persistent : false, onSuccess : (response) => {}, onFailure : (error_code, error_message) => {} });
}

ConsolePrint("Starting to import helpers.js ...");

function CefPoll(tpf)
{
    UpdateDOMRects();
}

if(ClientRectList.prototype.map === undefined)
{
    ConsolePrint("JS: Extending JS ClientRectList by own map function.");
    ClientRectList.prototype.map = function(f){
        var output = [];
        for(var i = 0, n = this.length; i < n; i++)
        {
            output.push( f(this[i]) ); 
        }
        return output;
    }
}

if(NodeList.prototype.forEach === undefined)
{
    ConsolePrint("JS: Extending JS NodeList by own forEach function.");

    NodeList.prototype.forEach = function(f){
        var n = this.length;
        for(var i = 0; i < n; i++)
        {
            f(this[n]);
        }
    }
}


/**
	Adjust bounding client rectangle coordinates to window, using scrolling offset and zoom factor.

	@param: DOMRect, as returned by Element.getBoundingClientRect()
	@return: Double array with length = 4, containing coordinates as follows: top, left, bottom, right
*/
function AdjustRectCoordinatesToWindow(rect)
{
	if(rect === undefined)
		ConsolePrint("WARNING: rect == undefined in AdjustRectCoordinatesToWindow!");

	var doc = document.documentElement;
	var zoomFactor = 1;
	var offsetX = (window.pageXOffset || doc.scrollLeft) - (doc.clientLeft || 0); 
	var offsetY = (window.pageYOffset || doc.scrollTop) - (doc.clientTop || 0); 

	// var docRect = document.body.getBoundingClientRect(); // not used

	if(document.body.style.zoom)
	{
		zoomFactor = document.body.style.zoom;
	}

	var output = [];
	output.push(rect.top*zoomFactor + offsetY);
	output.push(rect.left*zoomFactor + offsetX);
	output.push(rect.bottom*zoomFactor + offsetY);
	output.push(rect.right*zoomFactor + offsetX);
	return output;
}

// Formerly known as 'CompareClientRectsData'
function EqualClientRectsData(r1, r2)
{
    if (r1 === undefined || r2 === undefined || r1.length !== r2.length)
        return false;
    
    // Check if width and height of each Rect are identical
	for(var i = 0, n = r1.length; i < n; i++)
    {
        if((r1[i] === undefined && r2[i] !== undefined) || (r1[i] !== undefined && r2[i] === undefined))
            return false;
        if(r1[i] === undefined)
            continue;
		for(var j = 0; j < 4; j++)
            if(r1[i][j] !== r2[i][j])
                return false;   // Stop iterating asap
    }
	return true;

}

function ForEveryChild(parentNode, applyFunction, abortFunction)
{
    if(parentNode === null || parentNode === undefined || typeof(applyFunction) !== "function")
        return;
        
	var childs = parentNode.childNodes;

	// Abort further processing of child nodes if abort conditions are met
	if(abortFunction !== undefined && abortFunction(parentNode))
		return;

	if(childs && applyFunction)
	{
		// childs.forEach((child) => {	// TODO: This line needs a Chromium update to work
		for(var i = 0; i < childs.length; i++)
		{
			var child = childs[i];
			// Execute function of every child node
			applyFunction(child);
			// Recursively call function for every child of given child node
			ForEveryChild(child, applyFunction, abortFunction);
		}
	}
}

function GetFixedElementByNode(node)
{
    if(typeof(node.getAttribute) !== "function" || typeof(node.setAttribute) !== "function")
        return undefined;

    var fixedId = node.getAttribute("fixedId");
    return GetFixedElementById(fixedId);
}

function GetFixedElementById(id)
{
    if(id !== null && id !== undefined && id >= 0 && id < window.domFixedElements.length)
        return window.domFixedElements[id];
    return undefined;
}

function DeleteFixedElement(fixId)
{
    var fixObj = domFixedElements[fixId];
    if(fixObj === undefined)
        return;
    
    delete fixObj;
    ConsolePrint("#fixElem#rem#"+fixId+"#");
    console.log("Removed fixed element with id: "+fixId);
}

var debug_updateDOMRects_count = 0;
function UpdateDOMRects(why)
{

    console.log("UpdateDOMRects called because "+why+", "+(++debug_updateDOMRects_count)+" times until now");

    
    window.domNodes.forEach(domList => {
        domList.forEach(obj => {
            if(typeof(obj.updateRects) === "function")
                obj.updateRects();
        });
    });

    window.domFixedElements.forEach(fixObj => {
        if(typeof(fixObj.updateRects) === "function")
            fixObj.updateRects();
    });
}

function UpdateNodesRect(node)
{
    var domObj = GetCorrespondingDOMObject(node);
    if(domObj !== undefined)
        domObj.updateRects();

    var fixObj = GetFixedElementByNode(node);
    if(fixObj !== undefined)
        fixObj.updateRects();
}

function UpdateChildNodesRects(node)
{
    if(node === null || node === undefined)
        return;

    for(var i = 0, n = node.childNodes.length; i < n; i++)
        UpdateNodesRect(node.childNodes[i]);
    for(var i = 0, n = node.childNodes.length; i < n; i++)
        UpdateChildNodesRects(node.childNodes[i]);
}



function CreateDOMTextInput(node) { CreateDOMObject(node, 0); }
function CreateDOMLink(node){ CreateDOMObject(node, 1); }
function CreateDOMSelectField(node) { CreateDOMObject(node, 2); }
function CreateDOMOverflowElement(node) { CreateDOMObject(node, 3); }
function CreateDOMVideo(node){ CreateDOMObject(node, 4); }

function CreateDOMObject(node, type)
{
    var obj = GetCorrespondingDOMObject(node);
    if(obj !== undefined && obj.getType() === type)
        return undefined;

    switch(type){
        case 0: return new DOMTextInput(node);
        case 1: return new DOMLink(node);
        case 2: return new DOMSelectField(node);
        case 3: return new DOMOverflowElement(node);
        case 4: return new DOMVideo(node);
        default: {
            console.log("Warning: Unknown DOM node type: "+type);
            return undefined;
        }
    }
}

function GetDOMObject(type, id)
{
    if(id === undefined && typeof(type.getAttribute) === "function")
        console.log("You should use GetCorrespondingDOMObject, when you only use single nodes! ;)");

    if(type < 0 || id < 0)
        return undefined;
    if(window.domNodes[type] === undefined)
        return undefined;

    return window.domNodes[type][id];
}

function GetDOMTextInput(id){ return GetDOMObject(0, id);}
function GetDOMLink(id){ return GetDOMObject(1, id);}
function GetDOMSelectField(id){ return GetDOMObject(2, id); }
function GetDOMOverflowElement(id){ return GetDOMObject(3, id); }
function GetDOMVideo(id){ return GetDOMObject(4, id); }

function GetCorrespondingDOMObject(node, expected_type)
{
    if(typeof(node.getAttribute) !== "function")
        return undefined;
    
    var type = node.getAttribute("nodeObjType");
    if(expected_type !== undefined && type !== expected_type)
        return undefined;

    var id = node.getAttribute("nodeObjId");

    if(type === null || id === null)
        return undefined;

    return GetDOMObject(type, id);
}
function GetCorrespondingDOMOverflow(node){ return GetCorrespondingDOMObject(node, 3); }

function RemoveDOMObject(type, id)
{
    console.log("RemoveDOMObject called with parameters type="+type+" and id="+id+"! Doesn't do anything at the moment. TODO!");
}
function RemoveDOMTextInput(id){ return RemoveDOMObject(0, id); }
function RemoveDOMLink(id){ return RemoveDOMObject(1, id); }
function RemoveDOMSelectField(id){ return RemoveDOMObject(2, id); }
function RemoveDOMOverflowElement(id){ return RemoveDOMObject(3, id); }


/**
 * Usage example: Determine what parts of a node's rect are visible when inside an overflowing element
 */
function CutRectOnRectWindow(innerRect, outerRect)
{
    if(!(innerRect.length > 0) || !(outerRect.length > 0))
        return [0,0,0,0];

    var t = Math.max(innerRect[0], outerRect[0]);
    var l = Math.max(innerRect[1], outerRect[1]);
    var b = Math.min(innerRect[2], outerRect[2]);
    var r = Math.min(innerRect[3], outerRect[3]);
    
    // return size zero rect if edges flipped sides
    if(t >= b || l >= r) 
        return [0,0,0,0]

    return [t, l, b, r]
}

function CefExecute(header, param)
{
    var f = header[0], type = header[1], id = header[2];
    var a = param[0], b = param[1], c = param[2], d = param[3];

    // Determine object, which holds function to execute
    var obj = (id === undefined || type === undefined) ? window : GetDOMObject(type, id);

    if(obj === undefined || obj[f] === undefined)
    {
        if(!obj)
            console.log("CefExecute: Invalid object for id: "+id+" and type: "+type+" for function named "+f);
        else
            console.log("CefExecute: Could not find function called "+f+" in DOM object with id: "+id+
                " and type: "+type);
        return false; 
    }
    // Execute function with obj as context and parameters given
    // Return value may be e.g. a DOMNodeInteractionResponse
    return obj[f](a, b, c, d); // TODO: Use variadic approach? But sufficient right now
}



function DrawRect(rect, color)
{
	//Position parameters used for drawing the rectangle
	var x = rect[1];
	var y = rect[0];
	var width = rect[3] - rect[1];
	var height = rect[2] - rect[0];

	var canvas = document.createElement('canvas'); //Create a canvas element
	//Set canvas width/height
	canvas.style.width='100%';
	canvas.style.height='100%';
	//Set canvas drawing area width/height
	canvas.width = window.innerWidth;
	canvas.height = window.innerHeight;
	//Position canvas
	canvas.style.position='absolute';
	canvas.style.left=0;
	canvas.style.top=0;
	canvas.style.zIndex=100000;
	canvas.style.pointerEvents='none'; //Make sure you can click 'through' the canvas
	document.body.appendChild(canvas); //Append canvas to body element
	var context = canvas.getContext('2d');
	//Draw rectangle
	context.rect(x, y, width, height);
	context.fillStyle = color;
	context.fill();
}

function DrawObject(obj)
{
	var colors = ["#FF0000", "#00FF00", "#0000FF", "#FFFF00", "#FF00FF", "#FFFFFF"];
	for(var i = 0; i < obj.rects.length; i++)
	{
		DrawRect(obj.rects[i], colors[i % 6]);
	}
}

function SendLSLMessage(msg) {
    window.cefQuery({ request: ("lsl:" + msg), persistent: false, onSuccess: (response) => { }, onFailure: (error_code, error_message) => { } });
}

function SendDataMessage(msg) {
    window.cefQuery({ request: ("data:" + msg), persistent: false, onSuccess: (response) => { }, onFailure: (error_code, error_message) => { } });
}

function LoggingMediator()
{
	/* This function is indirectly called via this.log */
    this.logFunction = null;

	/* Register your own log function with this function */
    this.registerFunction = function(f)
    {
        this.logFunction = f;
    }

    /* Unregister the log function with this function */
    this.unregisterFunction = function() {
        this.logFunction = null;
    }

	/* This function is called by CEF's renderer process */
    this.log = function(logText)
    {
        try
        {
            if(this.logFunction !== null)
                this.logFunction(logText);
        }
        catch(e)
        {
            console.log("LoggingMediator: Something went wrong while redirecting logging data.");
            console.log(e);
        }
    }

    /* Code, executed on object construction */
    ConsolePrint("LoggingMediator instance was successfully created!");

}

window.loggingMediator = new LoggingMediator();



function GetTextSelection()
{
	// Pipe message to C++ MsgRouter
	ConsolePrint("#select#"+document.getSelection().toString()+"#");
}

// parent & child are 1D arrays of length 4
function ComputeBoundingRect(parent, child)
{
	// top, left, bottom, right

	var bbs = [];
	for(var i = 0, n=child.length/4; i < n; i++)
	{
		var _child = [child[i], child[i+1], child[i+2], child[i+3]];

		// // no intersection possible
		// if(parent[3] <= _child[0] || parent[3] <= _child[1])
		// 	bbs.push(_child);

		if(_child[2] - _child[0] > 0 && _child[3] - _child[1])
			bbs.push(_child);
	}

	return bbs;
}

var time_spent_rects_updating = 0.0;
var time_spent_creating_bitmask = 0.0;
function UpdateRectUpdateTimer(t0)
{
    time_spent_rects_updating += (performance.now() - t0);
}
function UpdateBitmaskTimer(t0)
{
    time_spent_creating_bitmask += (performance.now() - t0);
}
function PrintPerformanceInformation()
{
    window.page_load_time_ = window.performance.now() - starting_time_;
    var rect_update_time = time_spent_rects_updating;
    ConsolePrint('### Page load took \t'+Math.round(window.page_load_time_ / 1000)+'s / '
        +Math.round((window.page_load_time_)*1000)/1000+'ms');
    ConsolePrint('### Rect updates: \t'+Math.round(rect_update_time / 1000)+'s / '
        +Math.round(rect_update_time * 1000)/1000+'ms -- '+100*Math.round(rect_update_time/window.page_load_time_
        *1000)/1000 +'% of page load time');
    ConsolePrint('### Bitmask creation: \t'+Math.round(time_spent_creating_bitmask / 1000)+'s / '+
        Math.round(time_spent_creating_bitmask * 1000) / 1000 +'ms -- '+
            100*Math.round(time_spent_creating_bitmask/window.page_load_time_*1000)/1000 +'% of page load time');
}

function SendFaviconURLtoCEF(url)
{
    console.log("Found favicon url: "+url);
    ConsolePrint("#FaviconURL#"+url+"#");
}

function AttrRequest(domObj, attrStr, compare_with_js=true)
{
    if (domObj === undefined)
        return;
    var attrCode = GetAttributeCode(attrStr);
    if (attrCode === undefined)
    {
        ConsolePrint("AttrRequest: Couldn't find integer represantion for DOMAttribute with name "+attrStr);
        return;
    }
    
    if(compare_with_js)
    {
        var data, getter = domObj["get"+attrStr];
        if (getter !== undefined)
            data = domObj["get"+attrStr](false); 
        else
            ConsolePrint("AttrRequest: Didn't find Javascript getter for DOMAttribute "+attrStr);

    }
    ConsolePrint("DOMAttrReq#"+domObj.getType()+"#"+domObj.getId()+"#"+attrCode+"#");

    if (compare_with_js)
        ConsolePrint(attrStr+" data:\n\t"+data);
}

ConsolePrint("Successfully imported helpers.js!");