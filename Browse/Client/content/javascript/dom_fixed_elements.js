ConsolePrint("Importing JS code for FixedElements...");



// new version of window.fixed_elements
window.domFixedElements = [];

/** Constructor */
function FixedElement(node)
{
    /* Attributes */
    this.node = node;
    this.rects = [];
    this.id = -1;

    /* Methods */
    this.getRects = function()
    {
        return this.rects;
    }

    this.updateRects = function()
    {
        // Compute fitting bounding box for fixed node and all its children
        // Starting with ClientRects of this parent node
        var domRectList = [];
        for(var i = 0, rects = this.node.getClientRects(); i < rects.length; i ++)
        {
            domRectList.push(rects[i]);
        }
        

        // Check if child is already contained in one of the DOMRects
        ForEveryChild(this.node,
            function(child)
            {
                if(child.nodeType === 1)
                {
                    var rects = child.getClientRects();
                    for(var i = 0, n = rects.length; i < n; i++)
                    {
                        var rect = rects[i];
                        var contained = false;

                        // Iterate over all current bounding rects
                        domRectList.forEach(
                            function(container)
                            {
                                if(!contained) // Stop checking if rect is contained, when already contained in another rect
                                {
                                    contained = IsRectContained(rect, container);
                                }
                            }
                        ); // domRectList.forEach

                        // Another rect must be added if current rect isn't contained in any 
                        if(!contained)
                        {
                            domRectList.push(rects[i]);
                        }

                    } // for rects.length
                } // if nodeType === 1
            }
        ); // ForEveryChild

        var updatedRectsData = [];
        // Convert DOMRects to [t,l,b,r] float lists and adjust coordinates if zoomed
        domRectList.forEach(
            function(domRect)
            { 
                // ConsolePrint(AdjustRectToZoom(domRect));
                updatedRectsData.push(AdjustRectToZoom(domRect));
            }
        );

        // Check if Rect data changed, if yes, inform CEF about changes
        var n = this.rects.length;
        var changed = (updatedRectsData.length !== n);
        if(!changed)
        {
            for(var i = 0; i < n && !changed; i++)
            {
                changed = (updatedRectsData[i] !== this.rects[i]);
            }
        }

        // Save updated rect data in FixedElement objects attribute
        this.rects = updatedRectsData;

        if(changed)
        {
            // Inform CEF that fixed element has been updated
            ConsolePrint("#fixElem#add#"+this.id);
            //   ConsolePrint("-----> #fixElem#add#"+this.id); // DEBUG
        }
        
    }

    /* Code executed on constructor call ... */
    // Add FixedElement Object to list and determine its id
    window.domFixedElements.push(this);
    this.id = window.domFixedElements.length - 1;
    

    this.node.setAttribute("fixedId", this.id);
    var scope_id = this.id;
    // Note corresponding fixed element ID in an attribute
    ForEveryChild(this.node, function(child)
    { 
        if(child.nodeType === 1) 
        {
            child.setAttribute("childFixedId", scope_id);

            // If child corresponds to DOMObject or OverflowElement, inform about its fixation
            SetFixationStatus(child, true);
        }
    });

    // Update fixed Rects and inform CEF if changes happened
    this.updateRects();
}

function GetFixedElement(node)
{
    if(node.nodeType === 1)
    {
        var fixedId = node.getAttribute("fixedId");
        if(fixedId >= 0 && fixedId < window.domFixedElements.length)
        {
            return window.domFixedElements[fixedId];
        }
        else if(fixedId !== null)
        {
            console.log("Fetched fixedId, which doesn't seem to be supported: id="+fixedId);
           
        }
    }
    // default return value
    return undefined;
}

function AddFixedElement(node)
{
    if(node.nodeType === 1)
    {
        if(id = node.getAttribute("fixedId"))
        {
            // Trigger rect updates of whole subtree, just in case
            window.domFixedElements[id].updateRects();

            ConsolePrint("AddFixedElement: fixedId = "+id+" already linked to FixedObj!");
            return false;
        }
        else
        {
            ConsolePrint("Constructing new FixedElement Object...");
            // Create new FixedElement object, which will be added to global fixed element list
            new FixedElement(node);
            ConsolePrint("DONE - Constructing new FixedElement Object");

            //DEBUG
            // ConsolePrint("Added fixed element, currently available fixedIDs...");
            // window.domFixedElements.forEach(function(obj){ if(obj) ConsolePrint(obj.id);});

            return true;
        }
    }
}

function RemoveFixedElement(node)
{
    if(fixedObj = GetFixedElement(node) && fixedObj !== undefined)
    {
        var id = fixedObj.id;
        // Delete object in its list slot, slot will be left empty (undefined) at the moment
        if(fixedObj.id > 0 && fixedObj.id < window.domFixedElements.length)
        {
            delete window.domFixedElements[fixedObj.id];
        }
        node.removeAttribute("fixedId");

        // Also remove fixed ID from every child
        ForEveryChild(node, 
            function(child)
            {
                if(child.nodeType === 1)
                {
                    // NOTE: What if there are multiple fixed elements in that hierarchy
                    // and only one gets removed? Child might still be fixed?
                    // --> Added childFixedId instead

                    child.removeAttribute("childFixedId");

                    SetFixationStatus(child, false);
                }
            }
        ) // ForEveryChild

        // Just in case
        UpdateDOMRects();

        ConsolePrint("Successfully removed FixedElement with ID="+id);
    }
}



// Inform CEF about the current fixation status of a already known nodes
function SetFixationStatus(node, status)
{
	var type = node.getAttribute('nodeType');
	var nodeId = node.getAttribute('nodeID');

	// Node might be linked to DOMObject
	if(type !== null && nodeId !== null)
	{

		var domObj = GetDOMObject(type, nodeId);
		if(domObj !== null && domObj !== undefined)
		{
			domObj.setFixed(status);
		}
	}


	// Node might be linked to OverflowElement object
	var overflowId = node.getAttribute("overflowId");
	if(overflowId !== null)
	{
		var overflowObj = GetOverflowElement(overflowId);
		if(overflowObj !== null)
		{
			//DEBUG
			// ConsolePrint("Changing fixation status to "+status+" for OE id="+overflowId);

			overflowObj.setFixed(status);
		}
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


ConsolePrint("Successfully finished importing JS code for FixedElements!");
