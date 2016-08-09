window.domLinks = [];
window.domTextInputs = [];

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
        this.visible = true;
        this.fixed = false;

    /* Methods */ 
        // Update member variable for Rects and return if an update has occured 
        this.updateRects = function(){
            var updatedRectsData = AdjustClientRects(this.node.getClientRects());
            if(this.fixed)
            {
                updatedRects.map( function(rectData){rectData = SubstractScrollingOffset(rectData);} );
            }

            var equal = CompareClientRectsData(updatedRectsData, this.rects);

            if(!equal)
            {
                this.rects = updatedRectsData;
                InformCEF(this, ['update', 'rects']); 
            }
            // ConsolePrint(''+this.rects)
            
            return !equal;
        };

        // Returns float[4] for each Rect with adjusted coordinates
        this.getRects = function(){
            // Update rects if changes occured
            // this.updateRects();

            // Return rects as list of float lists with adjusted coordinates
            return this.rects;
        };

        this.setFixed = function(fixed){
            this.fixed = fixed;
            // DEBUG
            ConsolePrint("Informing CEF about fixation status change...");
            InformCEF(this, ['update', 'fixed']);
        };

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

        // Push to list and determined by DOMObjects position in type specific list
        var domObjList = GetDOMObjectList(nodeType);
        if(domObjList != undefined)
        {
            domObjList.push(domObj);
            var nodeID = domObjList.length - 1;

            // Add attributes to given DOM node
            node.setAttribute('nodeID', nodeID);
            node.setAttribute('nodeType', nodeType);

            InformCEF(domObj, ['added']);
        }
        else
        {
            ConsolePrint("ERROR: No DOMObjectList found for adding node with type="+nodeType+"!");
        }

    }
    else
    {
        ConsolePrint("Useless call of CreateDOMObject");
    }
}

function CreateDOMTextInput(node) { ConsolePrint("CreateDOMTextInput called!"); CreateDOMObject(node, 0); }
function CreateDOMLink(node) { CreateDOMObject(node, 1); }


/**
 * Adjusts given DOMRects to window properties in order to get screen coordinates
 * 
 * args:    rects : [DOMRect]
 * return:  [[float]] - top, left, bottom, right coordinates of each DOMRect in rects
 */
function AdjustClientRects(rects)
{
	function RectToFloatList(rect){ return [rect.top, rect.left, rect.bottom, rect.right]; };

	var lists = [];
	for(var i = 0, n = rects.length; i < n; i++)
	{
		lists.push(
            AdjustRectCoordinatesToWindow(rects[i])
        );
	}

	// TODO!: Adjust coordinates to window settings

	return lists;
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
    var n = rects1.length;

	if(n != rects2.length)
		return false;

	// Check if width and height of each Rect are identical
	for(var i = 0; i < n; i++)
	{
		for(var j = 0; j < 4; j++)
        {
            if(rects1[i][j] != rects2[i][j])
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
    ConsolePrint("UpdateDOMRects() called");

    // Trigger update of Rects for every domObject
    window.domTextInputs.forEach(
        function (domObj) { domObj.updateRects(); }
    );
    window.domLinks.forEach(
        function (domObj) { domObj.updateRects(); }
    );
}

/**
 * Transform natural language to encoded command to send to CEF
 * 
 * args:    domObj : DOMObject, operation : [string]
 * returns: void
 */
function InformCEF(domObj, operation)
{
    var id = domObj.node.getAttribute('nodeID');
    var type = domObj.nodeType;

    if(id != undefined && type != undefined)
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
                var status = (domObj.node.getAttribute('fixedID') != undefined) ? 1 : 0;
                // Encode changes in 'fixed' as attribute '1'
                encodedCommand += ('1#'+status+'#');

                // DEBUG
                ConsolePrint('command: '+encodedCommand);
            }
        }



        // Send encoded command to CEF
        ConsolePrint(encodedCommand);
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
        // NOTE: Add more cases if new nodeTypes are added
        default:
        {
            ConsolePrint('ERROR: No DOMObjectList for nodeType='+nodeType+' exists!');
            return undefined;
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
    if(nodeID >= targetList.length || targetList == undefined)
    {
        ConsolePrint('ERROR: Node with id='+nodeID+' does not exist for type='+nodeType+'!');
        return undefined;
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