//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

/**
 * GetNextOverflowParent -- Verallgemeinerte Aufwärtstraversierung?
 * GetDOMObject(node) -check if node.nodeType valid, than read attributes nodeObjId and nodeObjType
 * SendAttributeChangesToCEF(DOMAttribute, domObj); instead of InformCEF
 */

// Helper function for console output
function ConsolePrint(msg)
{
	window.cefQuery({ request: (""+msg), persistent : false, onSuccess : (response) => {}, onFailure : (error_code, error_message) => {} });
}

ConsolePrint("Starting to import helpers.js ...");

if(ClientRectList.prototype.map === undefined)
{
    ConsolePrint("JS: Extending JS ClientRectList by own map function.");
    ClientRectList.prototype.map = function(f){
        var output = [];
        var n = this.length;
        for(var i = 0; i < n; i++)
        {
            output.push( f(this[i]) ); 
        }
        return output;
    }
}

if(NodeList.prototype.forEach === undefined)
{
    ConsolePrint("JS: Extending JS NodeList by own forEach function.")

    NodeList.prototype.forEach = function(f){
        var n = this.length;
        for(var i = 0; i < n; i++)
        {
            f(this[n]);
        }
    }
}
/**
 * Adjusts given DOMRects to window properties in order to get screen coordinates
 * 
 * args:    rects : [DOMRect]
 * return:  [[float]] - top, left, bottom, right coordinates of each DOMRect in rects
 */
function AdjustClientRects(rects)
{
    if(rects === undefined || rects === null || rects.length === 0)
        return [[0,0,0,0]];

    // if(typeof(rects.map) !== "function")
    // {
    //     console.log(rects);
    //     return [[0,0,0,0]];
    // }

    return rects.map(
        (rect) => {
            return AdjustRectCoordinatesToWindow(rect);
        }
    );
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
    var n = r1.length;
	for(var i = 0; i < n; i++)
		for(var j = 0; j < 4; j++)
            if(r1[i][j] !== r2[i][j])
                return false;   // Stop iterating asap
	return true;

}

function ForEveryChild(parentNode, applyFunction, abortFunction)
{
    if(parentNode === undefined || typeof(applyFunction) !== "function")
        return;
        
	var childs = parentNode.childNodes;

	// Abort further processing of child nodes if abort conditions are met
	if(abortFunction !== undefined && abortFunction(parentNode))
		return;

	if(childs && applyFunction)
	{
		for(var i = 0; i < childs.length; i++)
		{
			// childs.forEach((child) => {	// TODO: This line needs a Chromium update to work
			var child = childs[i];
			// Execute function of every child node
			applyFunction(child);
			// Recursively call function for every child of given child node
			ForEveryChild(child, applyFunction, abortFunction);
		}
	}
}

// Returns corresponding DOMNode object, if it exists. Returns undefined if not
function GetDOMObject(node)
{
    if(node === undefined || typeof(node.getAttribute) !== "function" || typeof(node.setAttribute) !== "function")
        return undefined;

    var nodeObjId = node.getAttribute("nodeObjId");
    var nodeObjType = node.getAttribute("nodeObjType");

    if(nodeObjId === null || nodeObjType === null || nodeObjId < 0 || nodeObjType < 0)
    {
        console.log("Error: Couldn't fetch corresponding DOMObject. Insufficient information stored in node!");
        console.log("Required integer attributes: nodeObjId="+nodeObjId+", nodeObjType="+nodeObjType);
        return undefined;
    }

    var objList = domNodes[nodeObjType];
    if (objList === undefined)
    {
        console.log("Error: Unknown DOMObjectType with numerical interpretation "+nodeObjType+" when accessing nodeObjId="+nodeObjId);
        return undefined;
    }

    var obj = domNodes[nodeObjType][nodeObjId];
    return obj;
}

function GetFixedElement(node)
{
    if(typeof(node.getAttribute) !== "function" || typeof(node.setAttribute) !== "function")
        return undefined;

    var fixedId = node.getAttribute("fixedId");
    if(fixedId === null || fixedId < 0)
        return undefined;

    return window.domFixedElements[fixedId];
}

function GetFixedElementById(id)
{
    if(id !== null && id >= 0 && id < window.domFixedElements.length)
    {
        return window.domFixedElements[id];
    }
    else
    {
        console.log("Couldn't find FixedElement with id="+id);
        return undefined;
    }
}

function DeleteFixedElement(node)
{
    // TODO
    console.log("TODO DeleteFixedElement");
}


function UpdateDOMRects()
{
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
    var domObj = GetDOMObject(node);
    if(domObj !== undefined)
        domObj.updateRects();
}



function CreateDOMTextInput(node) { CreateDOMObject(node, 0); }
function CreateDOMLink(node){ CreateDOMObject(node, 1); }
function CreateDOMSelectField(node) { CreateDOMObject(node, 2); }
function CreateOverflowElement(node) { CreateDOMObject(node, 3); }

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
        default: {
            console.log("Warning: Unknown DOM node type: "+type);
            return undefined;
        }
    }
}

function GetDOMObject(type, id)
{
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

function GetCorrespondingDOMObject(node)
{
    if(typeof(node.getAttribute) !== "function")
        return undefined;
    
    var type = node.getAttribute("nodeObjType");
    var id = node.getAttribute("nodeObjId");

    if(type === null || id === null)
        return undefined;

    return GetDOMObject(type, id);
}

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
    console.log(header);
    console.log(param);
    var f = header[0], type = header[1], id = header[2];
    var a = param[0], b = param[1], c = param[2], d = param[3];

    ConsolePrint("Executing CefExecute -- "+f+", "+type+", "+id);

    // Determine object, which holds function to execute
    var obj = (id === undefined || type === undefined) ? window : GetDOMObject(type, id);

    if(obj === undefined || obj[f] === undefined)
        return false; 

    // Execute function with obj as context and parameters given
    // Return value may be e.g. a DOMNodeInteractionResponse
    return obj[f](a, b, c, d);
}



ConsolePrint("Successfully imported helpers.js!");