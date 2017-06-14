//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

ConsolePrint("Starting to import dom_fixed_elements.js ...");


// new version of window.fixed_elements
window.domFixedElements = [];
debugFixedNodes = []; // DEBUG

/** Constructor */
function FixedElement(node)
{
    /* Code executed on constructor call ... */
    // Add FixedElement Object to list and determine its id
    if(GetFixedElementByNode(node) === undefined)
    {
        window.domFixedElements.push(this);
        this.id = window.domFixedElements.indexOf(this);
        // console.log("Creating FixedElement with id="+this.id);
    }
    else
    {
        console.log("Node already marked as fixed element! Aborting object creation!");
        debugFixedNodes.push(node); // DEBUG
        return;
    }


    /* Attributes */
    this.node = node;
    this.rects = [];



    /**
     * REFACTORING TODOs
     *  - Automatically add FixedElement to list of fixed elements after it's creation
     *  - On creation: Check if node already belongs to an fixed element object? OR Use CreateFixedElement function
     */

    /* Methods */
    // Refactoring: Added for DOMNode objects
    this.getId = () => { return this.id; }

    this.getRects = function()
    {
        return this.rects;
    }

    this.updateRects = function()
    {
        var updatedRectsData = [];

        // NOTE: Fix for GMail. There exist DIVs without any children, whose rects cover the whole page and they are fixed
        // although this does not seem to influence anything
        if((this.node.tagName === "DIV" && this.node.children.length === 0)
            || ( window.getComputedStyle(this.node, null).getPropertyValue("display") === "none")) // 9gag inactivity overlay
        {
            this.rects = [[0,0,0,0]];
        }
        else
        {
            // Compute fitting bounding box for fixed node and all its children
            // Starting with ClientRects of this parent node
            var domRectList = [];
            for(var i = 0, rects = this.node.getClientRects(); i < rects.length; i ++)
            {
                domRectList.push(rects[i]);
            }
            
            
            // Check if child is already contained in one of the DOMRects
            ForEveryChild(
                this.node, 
                // Main function
                (child) =>{
                    if(child.nodeType === 1 && 
                        window.getComputedStyle(child, null).getPropertyValue("opacity") !== "0" &&
                        window.getComputedStyle(child, null).getPropertyValue("visibility") !== "hidden")
                    {
                        child.getClientRects().forEach((rect) => {
                            var contained = false;

                            // Iterate over all current bounding rects
                            domRectList.forEach((container) => {
                                    if(!contained) // Stop checking if rect is contained, when already contained in another rect
                                        contained = IsRectContained(rect, container);
                                }
                            );

                            // Another rect must be added if current rect isn't contained in any 
                            if(!contained)
                                domRectList.push(rects[i]);

                        }); // rects.forEach
                    } // if nodeType === 1
                },
                // Abort function, executed after main function
                (node) => { 
                    if (node.nodeType === 1)
                    {
                        // Skip children if invisible
                        if(window.getComputedStyle(node, null).getPropertyValue("opacity") === 0)
                            return true;
                        // ... or overflow element, which would cover children anyway outside of rect
                        var css_overflow = window.getComputedStyle(node, null).getPropertyValue("overflow");
                        return (node.hasAttribute("overflowId") || ( css_overflow !== null && css_overflow !== "visible"))
                    } 
                    else 
                        return true;
                }
            ); // ForEveryChild

            // Convert DOMRects to [t,l,b,r] float lists and adjust coordinates if zoomed
            domRectList.forEach((domRect) => { 
                    updatedRectsData.push(AdjustRectToZoom(domRect));
                }
            );
        }
        



        // Check if Rect data changed, if yes, inform CEF about changes
        var n = this.rects.length;
        var changed = (updatedRectsData.length !== n);
        for(var i = 0; i < n && !changed; i++)
        {
            for(var j = 0; j < 4 && !changed; j++)
            {
                 changed = (updatedRectsData[i][j] !== this.rects[i][j]);
            }
        }

        // Save updated rect data in FixedElement objects attribute
        this.rects = updatedRectsData;

        if(changed)
        {
            // Inform CEF that fixed element has been updated
            // var debug = (window.domFixedElements[this.id] === undefined) ? "undefined" : "ok";
            ConsolePrint("#fixElem#add#"+this.id+"#");

            // Give feedback if other rects might have to get updated too
            return true;
        }
        else
        {
            return false;
        }
        
    }

  

    this.node.setAttribute("fixedId", this.id);

    var scoped_id = this.id;
    ForEveryChild(this.node, (child) => {
        if(typeof(child.setAttribute) !== "function")
            return;
        
        child.setAttribute("childFixedId", scoped_id);
    });
    
    // Update fixed Rects and inform CEF if changes happened
    this.updateRects();
}

// TODO: Other Getters for DOM objects work with numbers, not with nodes. Rename this one?
// INFO: Added another Getter working with ID instead, temporary solution?
function GetFixedElementByNode(node)
{
    if(node.nodeType === 1)
    {
        var fixedId = node.getAttribute("fixedId");
        if(fixedId !== null && fixedId !== undefined && 
            fixedId >= 0 && fixedId < window.domFixedElements.length)
        {
            return window.domFixedElements[fixedId];
        }
        else if(fixedId !== null && fixedId !== undefined)
        {
            console.log("Fetched fixedId, which doesn't seem to be supported: id="+fixedId);
           
        }
    }
    // default return value
    return undefined;
}
function GetFixedElementById(id)
{
    if(id !== null && id >= 0 && id < window.domFixedElements.length)
    {
        return window.domFixedElements[id];
    }
    return undefined;
}

function AddFixedElement(node)
{

    if(node.nodeType === 1)
    {
        if(node.hasAttribute("childFixedId"))
        {
            return false;
        }

        if(node.hasAttribute("fixedId"))
        {
            var id = node.getAttribute("fixedId")
            var fixedObj = window.domFixedElements[id];
            // Trigger rect updates of whole subtree, just in case
            if(fixedObj !== undefined && fixedObj !== null)
            {
                fixedObj.updateRects();
            }
            else
            {
                ConsolePrint("Tried to access fixedObj with id="+id+", but it seems to be already deleted. But corresponding node still has its ID as attribute!");
            }
            

            // ConsolePrint("AddFixedElement: fixedId = "+id+" already linked to FixedObj!");
            return false;
        }
        else
        {
            // Create new FixedElement object, which will be added to global fixed element list
            new FixedElement(node);

            return true;
        }
    }
}

function RemoveFixedElement(node)
{
    var fixedObj = GetFixedElementByNode(node);
    if(fixedObj !== undefined)
    {
        // Needed for output at the end
        var id = fixedObj.id;

        // Delete object in its list slot, slot will be left empty (undefined) at the moment
        if(id >= 0 && id < window.domFixedElements.length)
        {
            delete window.domFixedElements[id];

            ConsolePrint("#fixElem#rem#"+id);
            // console.log("Removed fixedObj with id="+id);
        }

        node.removeAttribute("fixedId");

        // TODO: Set childrens fixedIds to this node's parent fixed id, if any
        var childFixedId = node.getAttribute("childFixedId");
        if(childFixedId)
        {
            node.childNodes.forEach((child) => {
                if(typeof(child.setAttribute) === "function")
                    child.setAttribute("childFixedId", childFixedId);
            });
        }
        else
        {
            node.childNodes.forEach((child) => {
                if(typeof(child.removeAttribute) === "function")
                    child.removeAttribute("childFixedId");
            });
        }
        
        // Just in case
        UpdateDOMRects();

        ConsolePrint("Successfully removed FixedElement with ID="+id);
    }
    else
    {
        // ConsolePrint("fixedObj === undefined")
        // ConsolePrint("node.fixedId="+node.getAttribute("fixedId"));
    }
}


function AdjustRectToZoom(rect)
{

	var zoomFactor = 1.0;

	if(document.body.style.zoom)
	{
		zoomFactor = document.body.style.zoom;
	}

	var output = [];
	output.push(rect.top*zoomFactor);
	output.push(rect.left*zoomFactor);
	output.push(rect.bottom*zoomFactor);
	output.push(rect.right*zoomFactor);

	return output;
}

function IsRectContained(rect, container)
{
    if(rect.width === 0 || rect.height === 0)
        return true;
    
    if(container.left <= rect.left && container.right >= rect.right
        && container.top <= rect.top && container.bottom >= rect.bottom)
        return true;

    // TODO: Idea: If rect partially contained, "cut off" not contained part and check if other containers contain it
    // if not: add "cut off" part to containers

    return false;
}
ConsolePrint("Successfully imported dom_fixed_elements.js!");