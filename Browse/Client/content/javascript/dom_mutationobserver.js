window.domLinks = [];
window.domTextInputs = [];
window.overflowElements = [];


/* TODOs
        - Write method 'InformCEF' -- DONE
        - Delete methods with equal name in old approach
            > UpdateDOMRects()
            > ...
        - Adjust RenderProcessHandler to new approach
*/

/**
 * Constructor
 */
function DOMObject(node, nodeType)
{
    /* Attributes */
        this.node = node;
        this.nodeType = nodeType;
        this.rects = AdjustClientRects(this.node.getClientRects());
        this.visible = true;    // default value, call DOMObj.checkVisibility() after object is created!
        this.fixed = (node.hasAttribute("childFixedId")) ? true : false;
        this.overflowParent = undefined;
        this.text = "";
        this.isPassword = false;

    /* Methods */ 
        // Update member variable for Rects and return true if an update has occured 
        this.updateRects = function(){
            this.checkVisibility();

            // Get new Rect data
            var updatedRectsData = AdjustClientRects(this.node.getClientRects());

            if(this.fixed)
            {
                updatedRectsData.map( function(rectData){ rectData = SubstractScrollingOffset(rectData);} );
            }

            // Compare new and old Rect data
            var equal = CompareClientRectsData(updatedRectsData, this.rects);

            // Rect updates occured and new Rect data is accessible
            if(!equal && updatedRectsData !== undefined)
            {
                this.rects = updatedRectsData;
                InformCEF(this, ['update', 'rects']); 
            }
            
            return !equal;
        };

        

        // Returns float[4] for each Rect with adjusted coordinates
        this.getRects = function(){
            if(this.visible && this.overflowParent !== null && this.overflowParent !== undefined)
            {
                // TODO: Work on OverflowElemen Objects and their getRects method instead!
                var bb_overflow = this.overflowParent.getBoundingClientRect();
                if(bb_overflow.height > 0 && bb_overflow.width > 0)
                {
                    var oRect = AdjustClientRects([bb_overflow])[0];

                    var rects = AdjustClientRects(this.node.getClientRects());
                    var new_rects = [];
                    for(var j = 0, n = rects.length; j < n; j++)
                    {
                        var rect = [];
                        rect.push(Math.max(rects[j][0], oRect[0]));
                        rect.push(Math.max(rects[j][1], oRect[1]));
                        rect.push(Math.min(rects[j][2], oRect[2]));
                        rect.push(Math.min(rects[j][3], oRect[3]));
                        new_rects.push(rect);
                    }

                    // ConsolePrint("Cut-off on OverflowElements doesn't work correctly!");
                    return new_rects;
                }
                else
                {
                    return this.rects;
                }

            }

            // Return rects as list of float lists with adjusted coordinates
            return this.rects;
        };

        this.setFixed = function(fixed){
            if(this.fixed != fixed)
            {
                this.fixed = fixed;
                InformCEF(this, ['update', 'fixed']);
                this.updateRects();
            }
        };

        this.setVisibility = function(visible){
            if(this.visible != visible)
            {
                this.visible = visible;
                InformCEF(this, ['update', 'visible']);
                if(visible) this.updateRects();
            }
        };

        this.checkVisibility = function(){
            var visibility = window.getComputedStyle(this.node, null).getPropertyValue('visibility');

            // Set visibility to hidden if bounding box is of resolution is 0 in any direction
            var bb = this.node.getBoundingClientRect();
            if(bb.width == 0 || bb.height == 0) { visibility = 'hidden'; }


            // Check if any parent node has opacity near zero, if yes, child (this node) might not be visible
            var root = this.node;
            while(root !== document.documentElement && root && root !== undefined)
            {
                if(window.getComputedStyle(root, null).getPropertyValue('opacity') < 0.0001)
                {
                    visibility = 'hidden';
                    break;
                }
                root = root.parentNode;
            }

            switch(visibility)
            {
                case 'hidden': { this.setVisibility(false); return; }
                case '':
                case 'visible': { /*this.setVisibility(true);*/ break; }
                default:
                { 
                    ConsolePrint("DOMObj.checkVisibility() - visibility="+visibility+" currently handled as 'visible'.");
                    // this.setVisibility(true);
                }
            }

            if(this.overflowParent)
            {
                var overflowRect = this.overflowParent.getBoundingClientRect();
                var nodeRect = this.node.getBoundingClientRect();

                // DEBUG
                // ConsolePrint("Comparing node's bounding box with overflow parent's bounding box...");
                // ConsolePrint("overflow box: "+overflowRect.top+","+overflowRect.left+","+overflowRect.bottom+","+overflowRect.right);
                // ConsolePrint("node's   box: "+nodeRect.top+","+nodeRect.left+","+nodeRect.bottom+","+nodeRect.right);

                // Test if overflow box is more than a thin line
                if( (overflowRect.height > 0 && overflowRect.width > 0) &&
                    // // Test if node's Rect lies completely inside of overflow Rect, then node is visible
                    // !(overflowRect.left <= nodeRect.left && overflowRect.right >= nodeRect.right && 
                    // overflowRect.top <= nodeRect.top && overflowRect.bottom >= nodeRect.bottom))
                    (overflowRect.top >= nodeRect.bottom || overflowRect.bottom <= nodeRect.top 
                        || overflowRect.left >= nodeRect.right || overflowRect.right <= nodeRect.left) )
                    {
                        this.setVisibility(false);
                        // DEBUG
                        // ConsolePrint("Node's box is outside, so it's not visible!");
                        return;
                    }
                // ConsolePrint("Node's box is inside, so node is visible!");

            }
            this.setVisibility(true);

        }

        this.searchOverflows = function (){
            // NOTE: Assuming there aren't a any overflows in an overflow
            var parent = this.node.parentNode;
            while(parent != null || parent === document.documentElement)
            {
                if(parent.nodeType == 1)
                {
                    // style.overflow = {visible|hidden|scroll|auto|initial|inherit}
                    var overflowProp = window.getComputedStyle(parent, null).getPropertyValue('overflow');
                    if(overflowProp == 'hidden')
                    {
                        // Add node as overflow parent and compute own visibility with Rect of overflow parent
                        this.overflowParent = parent;
                        // DEBUG
                        // ConsolePrint("Found an overflow parent!");
                        return;
                    }
                }
                parent = parent.parentNode;
            }
        }

        this.setTextInput = function(text, submit){
            ConsolePrint("setTextInput called with text='"+text+"' and submit="+submit);

            // Only executable if DOMNode is TextInput field
            if(this.nodeType === 0)
            {
                if(this.node.tagName == "TEXTAREA")
                {
                    this.node.value = text;
                }
                else if (this.node.tagName == 'INPUT')
                {
                    // GOOGLE FIX
                    var inputs = this.node.parentNode.getElementsByTagName("INPUT");
                    var n = inputs.length;
                    var zIndex = window.getComputedStyle(this.node, null).getPropertyValue('z-index');
                    for(var i = 0; i < n && n > 1; i++)
                    {
                        if(inputs[i] !== this.node && inputs[i].type == this.node.type)
                        {
                            if(zIndex < window.getComputedStyle(inputs[i], null).getPropertyValue('z-index'))
                            {
                                inputs[i].value = text;
                                ConsolePrint("Set text input on another input field with higher z-index");
                            }
                        }
                    }
                    
                    this.node.value = text;

                }
                else
                {
                    this.node.textContent = text;
                    ConsolePrint("Set input's value to given text");
                }
                
                // Assuming text input works in any case (no feedback needed), so that input text is now displayed
                this.text = text;
                InformCEF(this, ["update", "text"]);
                
                // ConsolePrint("Input text was set!");

                if(submit)
                {
                    // NOTE: Emulate pressing Enter in input field?

                    var parent = this.node.parentNode;
                    var no_form_found = false;
                    while(parent.nodeName != 'FORM')
                    {
                        parent = parent.parentNode;
                        if(parent === document.documentElement)
                        {
                            ConsolePrint('Could not submit text input: No child of any form element.');
                            no_form_found = true;
                            break;
                        }
                    }
                    if(!no_form_found)
                    {
                        parent.submit();
                        ConsolePrint("Input text was submitted.");
                    }
                
                }

            }
        };

/* -------------- Code, executed on object construction -------------- */
        
        // Push to list and determined by DOMObjects position in type specific list
        var domObjList = GetDOMObjectList(this.nodeType);

        // This should never fail
        if(domObjList !== undefined)
        {
            domObjList.push(this);
            var nodeID = domObjList.length - 1;
            // Add attributes to given DOM node
            this.node.setAttribute('nodeID', nodeID);
            this.node.setAttribute('nodeType', this.nodeType);
            
            // Create empty DOMNode object on C++ side
            InformCEF(this, ['added']);

            // Setup of attributes
            this.checkVisibility();
            this.searchOverflows();


            // Send msg if already fixed on creation!
            if(this.fixed)
            {
                InformCEF(this, ['update', 'fixed']);
            }

            // Set displayed text, depending on given node type
            // TODO/IDEA: (External) function returning node's text attribute, corresponding to node's tagName & type
            if(this.nodeType == 0)   // Only needed for text inputs
            {
                if(this.node.tagName == "TEXTAREA" || this.node.tagName == "INPUT")
                {
                    if(this.node.value !== undefined && this.node.value !== null)
                        this.text = this.node.value;
                }
                else
                {
                    if(this.node.textContent !== undefined && this.node.textContent !== null)
                    this.text = this.node.textContent;
                }
                InformCEF(this, ["update", "text"]);
            }

            if(this.node.tagName == "INPUT" && this.node.type == "password")
            {
                // Update attribute 4 aka (bool) isPasswordField=true, the last 1 for 'true' could be skipped, 
                // but MsgRouter needs 5 arguments in encoded string for attribute updates
                ConsolePrint("DOM#upd#"+this.nodeType+"#"+nodeID+"#4#1#");
            }


        }
        else
        {
            ConsolePrint("ERROR: No DOMObjectList found for adding node with type="+nodeType+"!");
        }



}

/**
 * Create a DOMObject of given type for node and add it to the global list
 * Also, automatically inform CEF about added node and in case of Rect updates
 * 
 * args:    node : DOMNode, nodeType : int
 * returns: void
 */
function CreateDOMObject(node, nodeType)
{
    // Only add DOMObject for node if there doesn't exist one yet
    if(!node.hasAttribute('nodeID'))
    {
        // Create DOMObject, which encapsulates the given DOM node
        var domObj = new DOMObject(node, nodeType);
    }
    else
    {
        // ConsolePrint("Useless call of CreateDOMObject");
    }
}

function CreateDOMTextInput(node) { CreateDOMObject(node, 0); }
function CreateDOMLink(node) { CreateDOMObject(node, 1); }




/**
 * Adjusts given DOMRects to window properties in order to get screen coordinates
 * 
 * args:    rects : [DOMRect]
 * return:  [[float]] - top, left, bottom, right coordinates of each DOMRect in rects
 */
function AdjustClientRects(rects)
{
	// function RectToFloatList(rect){ return [rect.top, rect.left, rect.bottom, rect.right]; };

    // .getClientRects() may return an empty DOMRectList{}
    if(rects.length === 0)
    {
        return [[0,0,0,0]];
    }

    var adjRects = [];
    for(var i = 0, n = rects.length; i < n; i++)
    {
        adjRects.push(
            AdjustRectCoordinatesToWindow(rects[i])
        );
    }

    return adjRects;
}

/**
 * Compares two lists of DOMRect objects and returns true if all values are equal
 * 
 * args:    rects1, rects2 : [DOMRect]
 * returns: bool
 */
function CompareClientRects(rects1, rects2)
{
	var n = rects1.length;

	if(n != rects2.length)
		return false;

	// Check if width and height of each Rect are identical
	for(var i = 0; i < n; i++)
	{
		if(rects1[i].width != rects2[i].width || rects1[i].height != rects2[i].height)
			return false;
	}

	// Check if Rect coordinates are identical
	for(var i = 0; i < n; i++)
	{
		// Note: It's enough to check x & y if width & height are identical
		if(rects1[i].x != rects2[i].x || rects1[i].y != rects2[i].y)		
			return false;
	}

	return true;
}

/**
 * Compares two lists of type [[float]] and returns true if all values are equal
 * 
 * args:    rects1, rects2 : [[float]]
 * returns: bool
 */
function CompareClientRectsData(rects1, rects2)
{
    if(rects2 === undefined || rects2 === null)
        return false;

    var n = rects1.length;

	if(n !== rects2.length)
		return false;

	// Check if width and height of each Rect are identical
	for(var i = 0; i < n; i++)
	{
		for(var j = 0; j < 4; j++)
        {
            if(rects1[i][j] !== rects2[i][j])
                return false;
        }
	}

	return true;
}

/**
 * Triggers update of DOMRects of each DOMObject by using DOMObjects updateRects() method
 * 
 * args:    -/-
 * returns: void
 */
function UpdateDOMRects()
{
    // DEBUG
    // ConsolePrint("UpdateDOMRects() called");

    // Trigger update of Rects for every domObject
    window.domTextInputs.forEach(
        function (domObj) { domObj.updateRects(); }
    );
    window.domLinks.forEach(
        function (domObj) { domObj.updateRects(); }
    );

    // ... and all OverflowElements
    window.overflowElements.forEach(
        function (overflowObj) {
            overflowObj.updateRects(); 
        }
    );

    // ... and all FixedElements
    window.domFixedElements.forEach(
        function(fixedObj){ if(fixedObj !== undefined){fixedObj.updateRects();} }
    );

   
    // Update visibility of each DOM object
    window.domTextInputs.forEach(
        function (domObj) { domObj.searchOverflows(); domObj.checkVisibility(); }
    );
    window.domLinks.forEach(
        function (domObj) { domObj.searchOverflows(); domObj.checkVisibility(); }
    );


}

function UpdateChildrensDOMRects(parent)
{
    ForEveryChild(parent, function(child){
        if(child.nodeType == 1)
        {
            if((nodeType = child.getAttribute("nodeType")) !== undefined && nodeType !== null)
            {
                var nodeID = child.getAttribute("nodeID");
                if((domObj = GetDOMObject(nodeType, nodeID)) !== undefined)
                {
                    domObj.searchOverflows(); 
                    domObj.checkVisibility(); 
                    domObj.updateRects();
                } 
            }

            if((overflowId = child.getAttribute("overflowId")) !== undefined && overflowId !== null)
            {
                if((overflowObj = GetOverflowElement(overflowId)) !== undefined)
                {
                    overflowObj.updateRects();
                }
            }

        }
    });

}

/**
 * Transform natural language to encoded command to send to CEF
 * Relies on existing nodeId in domObj.node!
 * 
 * args:    domObj : DOMObject, operation : [string]
 * returns: void
 */
function InformCEF(domObj, operation)
{
    var id = domObj.node.getAttribute('nodeID');
    var type = domObj.nodeType;

    if(id !== undefined && type !== undefined)
    {
        // Encoding uses only first 3 chars of natural language operation
        var op = operation[0].substring(0,3);

        var encodedCommand = 'DOM#'+op+'#'+type+'#'+id+'#';

        if(op == 'upd')
        {
            if(operation[1] == 'rects')
            {
                var rectsData = domObj.getRects();
                // Encode changes in 'rect' as attribute '0'
                encodedCommand += '0#';
                // Encode list of floats to strings, each value separated by ';'
                for(var i = 0, n = rectsData.length; i < n; i++)
                {
                    for(var j = 0; j < 4; j++)
                    {
                        encodedCommand += (rectsData[i][j]+';');
                    }
                }
                // Add '#' at the end to mark ending of encoded command
                encodedCommand = encodedCommand.substr(0,encodedCommand.length-1)+'#';
            }

            if(operation[1] == 'fixed')
            {
                // If fixed attribute doesn't exist, node is not fixed
                var status = (domObj.node.hasAttribute('fixedId')|| domObj.node.hasAttribute("childFixedId")) ? 1 : 0;
                // Encode changes in 'fixed' as attribute '1'
                encodedCommand += ('1#'+status+'#');

            }

            if(operation[1] == 'visible')
            {
                var status = (domObj.visible) ? 1 : 0;
                
                encodedCommand += ('2#'+status+'#');
                // ConsolePrint("encodedCommand: "+encodedCommand);
            }

            if(operation[1] == "text")
            {
                encodedCommand += ("3#"+domObj.text+"#");
            }
        }

        // Send encoded command to CEF
        ConsolePrint(encodedCommand);
    }
    else
    {
        ConsolePrint("ERROR: No DOMObject given to perform informing of CEF! id: "+id+", type: "+type);
    }
}

/**
 * Get global list of DOMObjects for specific node 
 * 
 * args:    nodeType : int
 * returns: [DOMObject]
 */
function GetDOMObjectList(nodeType)
{

    switch(nodeType)
    {
        case 0:
        case '0': { return window.domTextInputs; };
        case 1:
        case '1': { return window.domLinks; };
        case 2:
        case '2': { return window.overflowElements; }
        // NOTE: Add more cases if new nodeTypes are added
        default:
        {
            ConsolePrint('ERROR: No DOMObjectList for nodeType='+nodeType+' exists!');
            return null;
        }
    }
}


/**
 * Get DOMObject by using node's type and ID
 * 
 * args:    nodeType, nodeID : int
 * returns: DOMObject
 */
function GetDOMObject(nodeType, nodeID)
{
    var targetList = GetDOMObjectList(nodeType);

    // Catch error case
    if(nodeID >= targetList.length || targetList == undefined || nodeID === undefined || nodeID === null)
    {
        ConsolePrint('ERROR: Node with id='+nodeID+' does not exist for type='+nodeType+'!');
        return null;
    }

    return targetList[nodeID];
}

// ATTENTION: V8 doesn't seem to work with polymorphism of functions!

// /**
//  * Get corresponding DOMObject to a given node, if it doesn't exist 'undefined' is returned
//  * 
//  * args:    node : DOMNode
//  * return:  DOMObject
//  */
// function GetDOMObject(node)
// {
//     var id = node.getAttribute('nodeID');
//     var type = node.getAttribute('nodeType');

//     if(!id || !type)
//         return undefined;

//     return GetDOMObject(type, id);
// }


function OverflowElement(node)
{
    /* Attributes */
        this.node = node;
        this.rects = AdjustClientRects(this.node.getClientRects());
        this.fixed = false;

        // this.overflowParent = undefined;

    /* Methods */
        this.getMaxTopScrolling = function(){
            return (this.node.scrollHeight - this.node.getBoundingClientRect().width);
        }
        this.getMaxLeftScrolling = function(){
            return (this.node.scrollWidth - this.node.getBoundingClientRect().height);
        }
        this.getTopScrolling = function(){
            return this.node.scrollTop;
        }
        this.getLeftScrolling = function(){
            return this.node.scrollLeft;
        }
        this.scroll = function(gazeX, gazeY){
            // DEBUG
            // ConsolePrint("OverflowElement: Scrolling request received x: "+x+", y: "+y+". Executing...");
            
            // Update scrolling position according to current gaze coordinates
            // Idea: Only scroll if gaze is somewhere near the overflow elements edges
            if(this.rects.length > 0)
            {
                var rect = this.rects[0];
                var centerX = rect[1] + Math.round(rect.width / 2);
                var centerY =  rect[0] + Math.round(rect.height / 2);

                var distLeft = gazeX - rect[1];   // negative values imply gaze outside of element
                var distRight = rect[3] - gazeX;
                var distTop = gazeY - rect[0];
                var distBottom = rect[2] - gazeY;

                // Treshold for actual scrolling taking place, maximum distance to border where scrolling takes place
                var tresholdX = 1 / 5 * ((rect[3]-rect[1]) / 2);
                var tresholdY = 1 / 5 * ((rect[2]-rect[0]) / 2);

                var maxScrollingPerFrame = 10;
                // Actual scrolling, added afterwards
                var scrollX = 0;
                var scrollY = 0;

                if(distLeft >= 0 && distLeft < tresholdX)
                {
                    scrollX -= (maxScrollingPerFrame * ( 1 - (distLeft / tresholdX) ));
                }
                if(distRight >= 0 && distRight < tresholdX)
                {
                    scrollX += (maxScrollingPerFrame * ( 1 - (distRight / tresholdX) ));
                }
                if(distTop >= 0 && distTop < tresholdY)
                {
                    scrollY -= (maxScrollingPerFrame * (1 - (distTop / tresholdY)));
                }
                if(distBottom >= 0 && distBottom < tresholdY)
                {
                    scrollY += (maxScrollingPerFrame * (1 - (distBottom / tresholdY)));
                }

                // ConsolePrint("Executing OverflowElement scrolling by (x, y) = ("+scrollX+", "+scrollY+").");
                
                // // DEBUG 
                // var id = this.node.getAttribute("overflowId");
                // ConsolePrint(id+" before: scrollLeft: "+this.node.scrollLeft+" scrollTop: "+this.node.scrollTop);

                // Execute scrolling
                this.node.scrollLeft += scrollX;
                this.node.scrollTop += scrollY;

                // // DEBUG
                // ConsolePrint(id+"after : ("+scrollX+", "+scrollY+")\t-- scrollLeft: "+this.node.scrollLeft+" scrollTop: "+this.node.scrollTop);
                // ConsolePrint("class: "+this.node.className);

                // Update Rects of all child elements
                ForEveryChild(this.node, function(child){
                    if(child.nodeType == 1)
                    {
                        if((nodeType = child.getAttribute("nodeType")) !== undefined && nodeType !== null)
                        {
                            var nodeID = child.getAttribute("nodeID");
                            if((domObj = GetDOMObject(nodeType, nodeID)) !== undefined)
                            {
                                domObj.updateRects();
                            } 
                        }

                        if((overflowId = child.getAttribute("overflowId")) !== undefined && overflowId !== null)
                        {
                            if((overflowObj = GetOverflowElement(overflowId)) !== undefined)
                            {
                                overflowObj.updateRects();
                            }
                        }

                        
                        // TODO: Update child OEs as well. Idea: Update Method which can be called for one node and checks if DomObj or OE
                        // and executes updateRects
                        // Maybe better: Add function pointer to node s.t. node.updateRects() is callable
                    }
                });

                // Return current scrolling position as feedback
                return [this.node.scrollLeft, this.node.scrollTop];
            }
         
        }

        this.getRects = function(){
            // TODO: Also check if maximal scrolling limits changed if Rect width or height changed
            return this.rects;
        }

        this.updateRects = function(){

            // this.checkVisibility(); // doesn't exist (yet?) for OverflowElements!

            // Get new Rect data
            var updatedRectsData = AdjustClientRects(this.node.getClientRects());

            
            if(this.fixed)
            {
                updatedRectsData.map( function(rectData){ rectData = SubstractScrollingOffset(rectData);} );
            }


            // Compare new and old Rect data
            var equal = CompareClientRectsData(updatedRectsData, this.rects);

            var id = this.node.getAttribute("overflowId");

            // Rect updates occured and new Rect data is accessible
            if(!equal && updatedRectsData !== undefined)
            {
                this.rects = updatedRectsData;

                var encodedCommand = "#ovrflow#upd#"+id+"#rect#";
                
                for (var i = 0; i < 4; i++)
                {                 
                    encodedCommand += this.rects[0][i];
                    if(i < 3) 
                    {
                        encodedCommand += ";";
                    }
                }
                encodedCommand += "#";                
                ConsolePrint(encodedCommand);
            }

            return !equal;
        };

        this.setFixed = function(fixed){
            if(this.fixed !== fixed)
            {
                this.fixed = fixed;
                
                // Inform CEF about changes in fixed attribute
                var id = this.node.getAttribute("overflowId");
                var numFixed = (fixed) ? 1 : 0;
                var encodedCommand = "#ovrflow#upd#"+id+"#fixed#"+numFixed+"#";
                ConsolePrint(encodedCommand);

                this.updateRects();
            }
        };

/* ------------ CODE EXECUTED ON CONSTRUCTION OF OBJECT ---------------- */

        

}


function CreateOverflowElement(node)
{
    if(!node.getAttribute("overflowId"))
    {
        var overflowObj = new OverflowElement(node);

        window.overflowElements.push(overflowObj);

        // Prepare informing CEF about added OverflowElement
        var outStr = "#ovrflow#add#";


        var id = window.overflowElements.length - 1;
        node.setAttribute("overflowId", id);

        var zero = (id < 10) ? "0" : "";
        outStr += (zero + id + "#");
        // #ovrflow#add#[0]id#


        // Note: Ignoring multiple Rects at this point...
        var rects = overflowObj.getRects();
        var rect = (rects.length > 0) ? rects[0] : [0,0,0,0];


        for(var i = 0; i < 4; i++)
        {
            outStr += rect[i];
            if(i !== 3) outStr += ";";  // Note: if-statement misses in DOMObjects --> different decoding atm
        }
        outStr += "#";
        // #ovrflow#add#[0]id#rect0;rect1;rect2;rect3#

        outStr +=  overflowObj.getMaxLeftScrolling();
        outStr += ";";
        outStr += overflowObj.getMaxTopScrolling();
        outStr += "#";
        // #ovrflow#add#[0]id#rect0;rect1;rect2;rect3#maxLeft;maxTop#

        ConsolePrint(outStr);

        //DEBUG
        // ConsolePrint("### OverflowElement created: "+outStr);
    }
   
}

// Called from CEF Handler
function GetOverflowElement(id)
{
    if(id < window.overflowElements.length && id >= 0)
        return window.overflowElements[id];     // This may return undefined
    else
    {
        ConsolePrint("ERROR in GetOverflowElement: id="+id+", valid id should be in [0, "+(window.overflowElements.length-1)+"]!");
        return null;
    }
        
}

function RemoveOverflowElement(id)
{
    if(id < window.overflowElements.length && id >= 0)
    {
        delete window.overflowElements[id]; // TODO: Keep list space empty or fill when new OE is created?

        // Inform CEF about removed overflow element
        ConsolePrint("#ovrflow#rem#"+id);

    }
    else
    {
        ConsolePrint("ERROR: Couldn't remove OverflowElement with id="+id);
    }
}

function SubstractScrollingOffset(rectData)
{
	// Translate rectData by (-scrollX, -scrollY)
	rectData[0] -= window.scrollY;
	rectData[1] -= window.scrollX;
	rectData[2] -= window.scrollY;
	rectData[3] -= window.scrollX;
	return rectData;
}