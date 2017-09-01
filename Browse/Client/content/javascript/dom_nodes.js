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

function DOMNode(node, id, type, cef_hidden=false)
{    
    // TODO: Check if node already refers to an DOMObject?

    if(typeof(node.setAttribute) !== "function")
    {
        console.log("Error: Given node does not suffice DOMNode object criteria! id="+id+", type="+type);
        return;
    }

    // Store information in node attributes
    node.setAttribute("nodeObjId", id);
    node.setAttribute("nodeObjType", type);

    this.node = node;
    this.id = id;
    this.type = type;
    this.cef_hidden = cef_hidden;

    this.rects = []
    this.bitmask = [0];
    // MutationObserver will handle setting up the following references, if necessary
    this.fixObj = undefined;
    this.overflow = undefined;

    // Initial setup of fixObj
    this.setFixObj(  GetFixedElementById( this.node.getAttribute("fixedId") )  );
    if(this.fixObj === undefined)
        this.setFixObj(  GetFixedElementById( this.node.getAttribute("childFixedId") )  );


    // Initial setup of overflow, if already set in node
    var overflowId = node.getAttribute("overflowid");
    if(overflowId !== null)
        this.setOverflow(GetDOMOverflowElement(overflowId));

    this.cppReady = false; // TODO: Queueing calls, when node isn't ready yet?

    if(!this.cef_hidden)
    {
        // Inform C++ about added DOMNode
        ConsolePrint("DOM#add#"+type+"#"+id+"#");
    }
}
DOMNode.prototype.Class = "DOMNode";

// Set up methods, which will be inherited
DOMNode.prototype.getId = function(){
    return this.id;
}
DOMNode.prototype.getType = function(){
    return this.type;
}
DOMNode.prototype.setCefHidden = function(hidden){
    if(hidden === this.cef_hidden)
        return;

    this.cef_hidden = hidden;
    if(hidden)
        ConsolePrint("DOM#rem#"+this.getType()+"#"+this.getId()+"#");
    else
        ConsolePrint("DOM#add#"+this.getType()+"#"+this.getId()+"#");
}
DOMNode.prototype.getCefHidden = function(){
    return this.cef_hidden;
}

// DOMAttribute Rects
DOMNode.prototype.getRects = function(update=true){
    // Prevent SendAttributeChangesToCEF (called in updateRects) from calling updateRects again
    if(update)
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
DOMNode.prototype.getUnalteredRects = function(update=true){
    if(update)
        this.updateRects();
    return this.rects;
}
/**
 * param: altNode - Use alternative node for retrieval of client rects (e.g. DOMLinks refering to images: use image node for rects)
 */
DOMNode.prototype.updateRects = function(altNode){
    var t0 = performance.now();
    if(typeof(this.node.getClientRects) !== "function")
    {
        console.log("Error: Could not update rects because of missing 'getClientRects' function in node!");
        UpdateRectUpdateTimer(t0);
        return "Error!";
    }
    if(window.getComputedStyle(this.node, null).getPropertyValue("opacity") === 0)
    {
        UpdateRectUpdateTimer(t0);  
        return this.setRectsToZero();
    }

    if(this.node.hidden_by !== undefined && this.node.hidden_by.size > 0)
    {
        for(var hiding_parent of this.node.hidden_by.keys())
        {
            var hiding_reason = this.node.hidden_by.get(hiding_parent);
            if (hiding_reason === "opacity" && 
                window.getComputedStyle(hiding_parent, null).getPropertyValue(hiding_reason) === "0")
                {
                    UpdateRectUpdateTimer(t0);
                    return this.setRectsToZero();
                }
        }
    }

    var adjustedRects = this.getAdjustedClientRects(altNode);

    this.updateOccBitmask(altNode);

    if(!EqualClientRectsData(this.rects, adjustedRects))
    {
        this.rects = adjustedRects;

        SendAttributeChangesToCEF("Rects", this);
        UpdateRectUpdateTimer(t0);
        return true; // Rects changed and were updated
    }
    UpdateRectUpdateTimer(t0);
    return false; // No update needed, no changes
}

DOMNode.prototype.updateOccBitmask = function(altNode){
    var t1 = performance.now();
    var bm = [];
    if(this.rects.length > 0)
    {
        var r = this.rects[0];
        // rect corners
        var pts = [[r[1],r[0]], [r[3],r[0]], [r[3],r[2]-0.0001], [r[1],r[2]-0.0001]]; // Note: Seems infinitely small amount to high
        pts.forEach( (pt) => { // pt[0] == x, pt[1] == y
            // Rect point in currently shown part of website?
            if(pt[0] < window.scrollX || pt[0] > window.scrollX + window.innerWidth ||
                pt[1] < window.scrollY || pt[1] > window.scrollY + window.innerHeight)
                    bm.push(0);
            else
            {
                var topNode = document.elementFromPoint(pt[0],pt[1]);
                // TODO: Quick fix
                if (topNode === null)
                    bm.push(1);
                else
                    bm.push(Number(topNode === (altNode || this.node)));
            }
        });

    }
    else
        bm = [0];
    
    var changed = false;
    for(var i = 0, n = bm.length; i < n && !changed; i++)
        changed = (bm[i] != this.bitmask[i]);

    if (changed)
    {
        this.bitmask = bm;
        SendAttributeChangesToCEF("OccBitmask", this);
    }

    UpdateBitmaskTimer(t1);
}

DOMNode.prototype.getOccBitmask = function(){
    return this.bitmask
}

// DOMAttribute FixedId
DOMNode.prototype.getFixedId = function(){
    if(this.fixObj === undefined)
        return -1;
    return this.fixObj.getId();
}

DOMNode.prototype.setFixObj = function(fixObj){
    // Only accept undefined or a valid fixed element
    if(fixObj !== undefined && typeof(fixObj.getId) !== "function")
        return false;

    if(this.fixObj !== fixObj)
    {
        this.fixObj = fixObj;
        SendAttributeChangesToCEF("FixedId", this);
        this.updateRects();
    }
    return true;
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
    console.log("DOMNode.setDOMAttribute called. Why?");
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

function DOMTextInput(node, cef_hidden=false)
{
    if(typeof(node.getAttribute) === "function" && node.getAttribute("aria-hidden") === "true")
    {
        console.log("Skipping creation of new DOMTextInput due to aria-hidden set to 'true'.");
        return;
    }

    window.domTextInputs.push(this);
    var id = window.domTextInputs.indexOf(this);
    DOMNode.call(this, node, id, 0, cef_hidden);

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

function DOMLink(node, cef_hidden=false)
{
    window.domLinks.push(this);
    var id = window.domLinks.indexOf(this);
    DOMNode.call(this, node, id, 1, cef_hidden);

    this.text = "";
    this.url = "";

    // Search image which might be displayed instead of link text
    var imgs = node.getElementsByTagName("IMG");
    if(imgs.length > 0)
        this.imgNode = imgs[0];

}
DOMLink.prototype = Object.create(DOMNode.prototype)
DOMLink.prototype.constructor = DOMLink;
DOMLink.prototype.Class = "DOMLink";  // Override base class identifier for this derivated class

// Use underlying image node instead of link node for rect data
DOMLink.prototype.origUpdateRects = DOMNode.prototype.updateRects;
DOMLink.prototype.updateRects = function(){
    this.origUpdateRects(this.imgNode);
}

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

function DOMSelectField(node, cef_hidden=false)
{
    window.domSelectFields.push(this);
    var id = window.domSelectFields.indexOf(this);

    DOMNode.call(this, node, id, 2, cef_hidden);
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

function DOMOverflowElement(node, cef_hidden=false)
{
    window.domOverflowElements.push(this);
    var id = window.domOverflowElements.indexOf(this);

    var cs_overflow = window.getComputedStyle(node, null).getPropertyValue("overflow");
    var visible_overflows = ["scroll", "auto"];
    cef_hidden = cef_hidden || (visible_overflows.indexOf(cs_overflow) === -1);
    // Check if auto-overflow is really overflowing // TODO: Check regularly if overflow property changed?
    if(cs_overflow === "auto")
        this.auto_overflow_hidden = false;  // Initially set to false, later checked

    DOMNode.call(this, node, id, 3, cef_hidden);

    // TODO: Deprecated?
    // Disable scrolling for divs which shouldn't be scrolled
    this.hidden_overflow = (cs_overflow === "hidden");

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

    // If overflow is set to auto, check if it is really overflowing before using it in CEF
    this.checkIfOverflown();

}
DOMOverflowElement.prototype = Object.create(DOMNode.prototype);
DOMOverflowElement.prototype.constructor = DOMOverflowElement;
DOMOverflowElement.prototype.Class = "DOMOverflowElement";  // Override base class identifier for this derivated class

DOMOverflowElement.prototype.updateRects = function(){
    DOMNode.prototype.updateRects.call(this);
    this.checkIfOverflown();
}

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

DOMOverflowElement.prototype.cutRectsWith = function(domObj, skip_update=true){
    if(typeof(domObj.getRects) !== "function")
    {
        console.log("Error: Attempted to cut object rects on DOMOverflowElement without a 'getRects' function inside object!");
        return [[]];
    }

    var oeRects = this.getRects();
    var update_rects = !skip_update;
    var objRects = domObj.getUnalteredRects(update_rects);

    // If domObj is fixed, substract current scrolling offsets from overflow rects
    if(domObj.getFixedId() !== -1)
    {
        oeRects.forEach((r) => {
            r[0] -= window.scrollY; // top
            r[1] -= window.scrollX; // left
            r[2] -= window.scrollY; // bottom
            r[3] -= window.scrollX; // right
        });
    }

    return objRects.map(
        (objRect) => {
            return oeRects.reduce(
                (acc, oeRect) => { return CutRectOnRectWindow(acc, oeRect); },
                objRect
            )
        }
    )
}

DOMOverflowElement.prototype.checkIfOverflown = function(){
    if(this.auto_overflow_hidden === undefined)
        return;

    this.setAutoOverflowHidden(
        this.node.scrollHeight <= this.node.clientHeight 
            && this.node.scrollWidth <= this.node.clientWidth
    );
}

DOMOverflowElement.prototype.setAutoOverflowHidden = function(hidden){
    if(hidden === this.auto_overflow_hidden)
        return;

    this.auto_overflow_hidden = hidden;
    // Only hide or show auto-overflow if not already hidden for CEF
    if(!this.cef_hidden)
    {
        if(hidden)
            ConsolePrint("DOM#rem#3#"+this.getId()+"#");
        else
            ConsolePrint("DOM#add#3#"+this.getId()+"#");
    }
}


/*
    ____  ____  __  ____    ___     __         
   / __ \/ __ \/  |/  / |  / (_)___/ /__  ____ 
  / / / / / / / /|_/ /| | / / / __  / _ \/ __ \
 / /_/ / /_/ / /  / / | |/ / / /_/ /  __/ /_/ /
/_____/\____/_/  /_/  |___/_/\__,_/\___/\____/ 
*/
window.domVideos = []
if(window.domNodes[4] !== undefined)
    console.log("Warning! DOMNode list slot 4 already in use!");
window.domNodes[4] = window.domVideos;

function DOMVideo(node, cef_hidden=false)
{
    window.domVideos.push(this);
    var id = window.domVideos.indexOf(this);

    DOMNode.call(this, node, id, 4, cef_hidden);

    /* Currently now attributes, only interaction */
    console.log("DOMVideo node successfully created.");
}
DOMVideo.prototype = Object.create(DOMNode.prototype);
DOMVideo.prototype.constructor = DOMOverflowElement;
DOMVideo.prototype.Class = "DOMVideo";  // Override base class identifier for this derivated class




ConsolePrint("Successfully imported dom_nodes.js!");
