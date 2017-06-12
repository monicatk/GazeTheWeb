//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

/**
 * Basic definition of each DOMNode Object's attributes, update functions and
 * getters. Attribute setting will be handled by the MutationObserver.
 * Methods for user interaction with node objects can be found in dom_nodes_interaction.js
 */
ConsolePrint("Starting to import dom_nodes.js ...");

/*
   ___  ____  __  ____  __        __      
  / _ \/ __ \/  |/  / |/ /__  ___/ /__ ___
 / // / /_/ / /|_/ /    / _ \/ _  / -_|_-<
/____/\____/_/  /_/_/|_/\___/\_,_/\__/___/
*/
// Contains lists to all kinds of DOMNode derivates, position defined by numeric node type
window.domNodes = [];

function DOMNode(node, id, type)
{    
    // TODO: Check if node already refers to an DOMObject?

    if(typeof(node.setAttribute) !== "function")
    {
        console.log("Error: Given node does not suffice DOMNode object criteria! id="+id+", type="+type);
    }
    else
    {
        // Store information in node attributes
        node.setAttribute("nodeObjId", id);
        node.setAttribute("nodeObjType", type);
    }
    this.node = node;
    this.id = id;
    this.type = type;

    this.rects = []
    // MutationObserver will handle setting up the following references, if necessary
    this.fixObj = undefined;
    this.overflow = undefined;

    if(typeof(node.getAttribute) === "function")
    {
        // Initial setup of fixObj, if already set in node
        this.setFixObj(  GetFixedElementById( node.getAttribute("fixedId") )  );
        if(this.fixObj === null)
            this.setFixObj(  GetFixedElementById( node.getAttribute("childFixedId") )  );

        // Initial setup of overflow, if already set in node
        var overflowId = node.getAttribute("overflowid");
        if(overflowId !== null)
            this.setOverflow(GetDOMOverflowElement(overflowId));
    }

    // Inform C++ about added DOMNode
    ConsolePrint("DOM#add#"+type+"#"+id+"#");
}
DOMNode.prototype.Class = "DOMNode";

// Set up methods, which will be inherited
DOMNode.prototype.getId = function(){
    return this.id;
}
DOMNode.prototype.getType = function(){
    return this.type;
}

// DOMAttribute Rects
DOMNode.prototype.getRects = function(){
    this.updateRects();

    // TODO: Set each DOMNode's overflow attribute to undefined, if corresponding Overflow gets deleted/removed
    // IDEA: Sign up each DOMNode in OverflowElements list if assigned to overflow attribute?
    if(this.overflow !== undefined)
    {
        return this.overflow.cutRectsWith(this);
    }
    // FUTURE TODO: Cut with overlying fixed areas, if any
    return this.rects;
}
DOMNode.prototype.getUnalteredRects = function(){
    this.updateRects();
    return this.rects;
}

DOMNode.prototype.updateRects = function(){
    if(typeof(this.node.getClientRects) !== "function")
    {
        console.log("Error: Could not update rects because of missing 'getClientRects' function in node!");
        return;
    }
    var adjustedRects = AdjustClientRects(this.node.getClientRects());
    if(!EqualClientRectsData(this.rects, adjustedRects))
    {
        this.rects = adjustedRects
        SendAttributeChangesToCEF("Rects", this);
        return true; // Rects changed and were updated
    }
    return false; // No update needed, no changes
}

// DOMAttribute FixedId
DOMNode.prototype.getFixedId = function(){
    if(this.fixObj === undefined)
        return -1;
    return this.fixObj.getId();
}

DOMNode.prototype.setFixObj = function(fixObj){
    // Reset of fixObj
    if(fixObj === undefined)
    {
        this.fixObj = undefined;
        SendAttributeChangesToCEF("FixedId", this);
        return true;
    }
    // Update of fixObj
    if(typeof(fixObj.getId) === "function")
    {
        this.fixObj = fixObj;
        var fixId = fixObj.getId();
        SendAttributeChangesToCEF("FixedId", this);
        return true;
    }
    // else: Invalid object, do nothing.
    console.log("Error: DOMNode.setFixObj: Invalid object given! ("+this.Class+" id: "+this.getId()+")");
    return false;
}

// DOMAttribute OverflowId
DOMNode.prototype.getOverflowId = function(){
    if (this.overflow === undefined)
        return -1;
    return this.overflow.getId();
}
DOMNode.prototype.setOverflow = function(obj){
    if(this.overflow !== obj)
    {
        this.overflow = obj;
        SendAttributeChangesToCEF("OverflowId", this);
        // Automatically trigger rect update
        this.updateRects();
    }
}
DOMNode.prototype.setOverflowViaId = function(id){
    // Reset overflow
    if(id === null || id === -1)
    {
        this.setOverflow(undefined);
    }
    // Invalid input
    if(id === undefined || id < 0)
        return;
    // Fetch overflow object and set attribute with it
    var obj = GetDOMOverflowElement(id);
    this.setOverflow(obj);
}

// TODO: What was this good for? oO
DOMNode.prototype.setDOMAttribute = function(str){
    switch(str){
        case "fixed":
            return this.setFixed;
        case "overflow":
            return this.setOverflow;
    }
    return undefined;
}


/*
   ___  ____  __  _________        __  ____               __    
  / _ \/ __ \/  |/  /_  __/____ __/ /_/  _/__  ___  __ __/ /____
 / // / /_/ / /|_/ / / / / -_) \ / __// // _ \/ _ \/ // / __(_-<
/____/\____/_/  /_/ /_/  \__/_\_\\__/___/_//_/ .__/\_,_/\__/___/
                                            /_/ 
*/
window.domTextInputs = [];
if(window.domNodes[0] !== undefined)
    console.log("Warning! DOMNode list slot 0 already in use!");
window.domNodes[0] = window.domTextInputs;

function DOMTextInput(node)
{
    window.domTextInputs.push(this);
    var id = window.domTextInputs.indexOf(this);
    DOMNode.call(this, node, id, 0);

    this.text = "";
}
DOMTextInput.prototype = Object.create(DOMNode.prototype)
DOMTextInput.prototype.constructor = DOMTextInput;
DOMTextInput.prototype.Class = "DOMTextInput";  // Override base class identifier for this derivated class

// DOMAttribute Text
DOMTextInput.prototype.getText = function(){
    return this.text;
}
DOMTextInput.prototype.setText = function(text){
    var informCEF = (this.text !== text);
    this.text = text;
    if(informCEF)
        SendAttributeChangesToCEF("Text", this);
}

// DOMAttribute IsPassword
DOMTextInput.prototype.getIsPassword = function(){
    return (this.node.type === "password");
}


/*
   ___  ____  __  _____   _      __      
  / _ \/ __ \/  |/  / /  (_)__  / /__ ___
 / // / /_/ / /|_/ / /__/ / _ \/  '_/(_-<
/____/\____/_/  /_/____/_/_//_/_/\_\/___/
*/
window.domLinks = [];
if(window.domNodes[1] !== undefined)
    console.log("Warning! DOMNode list slot 1 already in use!");
window.domNodes[1] = window.domLinks;

function DOMLink(node)
{
    window.domLinks.push(this);
    var id = window.domLinks.indexOf(this);
    DOMNode.call(this, node, id, 1);

    this.text = "";
    this.url = "";


}
DOMLink.prototype = Object.create(DOMNode.prototype)
DOMLink.prototype.constructor = DOMLink;
DOMLink.prototype.Class = "DOMLink";  // Override base class identifier for this derivated class

// DOMAttribute Text
DOMLink.prototype.getText = function(){
    return this.text;
}
DOMLink.prototype.setText = function(text){
    var informCEF = (this.text !== text);
    this.text = text;
    if(informCEF)
        SendAttributeChangesToCEF("Text", this);
}

// DOMAttribute Url
DOMLink.prototype.getUrl = function(){
    return this.url;
}
DOMLink.prototype.setUrl = function(url){
    var informCEF = (this.url !== url);
    this.url = url;
    if(informCEF)
        SendAttributeChangesToCEF("Url", this);

}


/*
   __________   _______________  ______________   ___  ____
  / __/ __/ /  / __/ ___/_  __/ / __/  _/ __/ /  / _ \/ __/
 _\ \/ _// /__/ _// /__  / /   / _/_/ // _// /__/ // /\ \  
/___/___/____/___/\___/ /_/   /_/ /___/___/____/____/___/
*/
window.domSelectFields = []
if(window.domNodes[2] !== undefined)
    console.log("Warning! DOMNode list slot 2 already in use!");
window.domNodes[2] = window.domSelectFields;

function DOMSelectField(node)
{
    window.domSelectFields.push(this);
    var id = window.domSelectFields.indexOf(this);

    DOMNode.call(this, node, id, 2);
}
DOMSelectField.prototype = Object.create(DOMNode.prototype);
DOMSelectField.prototype.constructor = DOMSelectField;
DOMSelectField.prototype.Class = "DOMSelectField";  // Override base class identifier for this derivated class

// DOMAttribute Options
DOMSelectField.prototype.getOptions = function(){
    var options = [];
    for(var i = 0, n = this.node.childNodes.length; i < n; i++)
    {
        var child = this.node.childNodes[i];
        if(child === undefined || child.tagName !== "OPTION")
            continue;
        options.push(child.text); // Use text instead of value, text is visibile for user, value isn't
    }
    return options;
}
DOMSelectField.prototype.setSelectionIdx = function(idx){
    this.node.selectedIndex = idx;
}
DOMSelectField.prototype.getSelectionIdx = function(){
    return this.node.selectedIndex;
}


/*
  ____               _____           ______                   __    
 / __ \_  _____ ____/ _/ /__ _    __/ __/ /__ __ _  ___ ___  / /____
/ /_/ / |/ / -_) __/ _/ / _ \ |/|/ / _// / -_)  ' \/ -_) _ \/ __(_-<
\____/|___/\__/_/ /_//_/\___/__,__/___/_/\__/_/_/_/\__/_//_/\__/___/
*/
window.domOverflowElements = []
if(window.domNodes[3] !== undefined)
    console.log("Warning! DOMNode list slot 3 already in use!");
window.domNodes[3] = window.domOverflowElements;

function DOMOverflowElement(node)
{
    window.domOverflowElements.push(this);
    var id = window.domOverflowElements.indexOf(this);

    DOMNode.call(this, node, id, 3);

    this.scrollLeftMax = -1
    this.scrollTopMax = -1
    this.scrollLeft = -1
    this.scrollTop = -1

    // Extend node's scroll callback
    this.node.last_scroll_x = -1;
    this.node.last_scroll_y = -1;
    this.node.onscroll = function(e){
        if(e.target.scrollLeft !== e.target.last_scroll_x || e.target.scrollTop !== e.target.last_scroll_y)
        {
            ForEveryChild(e.target, function(child){
                var obj = GetCorrespondingDOMObject(child);
                if(obj !== undefined)
                {
                    obj.updateRects();
                }
            });
            e.target.last_scroll_x = e.target.scrollLeft;
            e.target.last_scroll_y = e.target.scrollTop;
        }
    };


    // Set first generation of childs overflowId, MutationObserver will handle the rest
    for(var i = 0, n = node.childNodes.length; i < n; i++)
    {
        var child = node.childNodes[i];
        if(child === undefined || typeof(child.setAttribute) !== "function")
            continue;
        child.setAttribute("overflowId", id);
    }

}
DOMOverflowElement.prototype = Object.create(DOMNode.prototype);
DOMOverflowElement.prototype.constructor = DOMOverflowElement;
DOMOverflowElement.prototype.Class = "DOMOverflowElement";  // Override base class identifier for this derivated class

// DOMAttribute MaxScrolling
DOMOverflowElement.prototype.getMaxScrolling = function(){
    return [this.scrollLeftMax, this.scrollTopMax]
}
DOMOverflowElement.prototype.setMaxScrolling = function(left, top){
    this.scrollLeftMax = left;
    this.scrollTopMax = top;
}

// DOMAttribute CurrentScrolling
DOMOverflowElement.prototype.getCurrentScrolling = function(){
    return [this.scrollLeft, this.scrollTop]
}
DOMOverflowElement.prototype.setCurrentScrolling = function(left, top){
    this.scrollLeft = left;
    this.scrollTop = top;
}

// DOMOverflow.prototype.updateRects = function(){
//     // TODO: Override DOMNode's updateRects method
//     // IDEA: Only trigger updateRects for every child on width/height changes?
// }

DOMOverflowElement.prototype.cutRectsWith = function(domObj){
    if(typeof(domObj.getRects) !== "function")
    {
        console.log("Error: Attempted to cut object rects on DOMOverflowElement without a 'getRects' function inside object!");
        return [[]];
    }

    var oeRects = this.getRects();
    var objRects = domObj.getUnalteredRects();

    return objRects.map(
        (objRect) => {
            return oeRects.reduce(
                (acc, oeRect) => {
                    return CutRectOnRectWindow(acc, oeRect);
                }, objRect
            )
        }
    )
}

ConsolePrint("Successfully imported dom_nodes.js!");