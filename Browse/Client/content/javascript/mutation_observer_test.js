// https://developer.mozilla.org/de/docs/Web/API/MutationObserver

// Global variables in which DOM relevant information is stored
window.fixed_elements = new Set();
window.fixed_IDlist = [];				// Position |i| contains boolean information if ID |i| is currently used
window.fixed_coordinates = [[]];		// 2D list of bounding rectangle coordinates, fill with bounding rect coordinates of fixed node and its children (if position == 'relative')

window.dom_links = [];
window.dom_links_rect = [[]];

window.dom_textinputs = [];
window.dom_textinputs_rect = [[]];

window.checkChildrens = new Set();


// TESTING CEF V8 ExecuteFunction
// window.myObject = {val: '8', called: '0', getVal: function(){ConsolePrint('Objects function called!'); return 7;} };



// Helper function for console output
function ConsolePrint(msg)
{
	window.cefQuery({ request: msg, persistent : false, onSuccess : function(response) {}, onFailure : function(error_code, error_message){} });
}

// TODO: Add as global function and also use it in DOM node work
/**
	Adjust bounding client rectangle coordinates to window, using scrolling offset and zoom factor.

	@param: DOMRect, as returned by Element.getBoundingClientRect()
	@return: Double array with length = 4, containing coordinates as follows: top, left, bottom, right
*/
function AdjustRectCoordinatesToWindow(rect)
{
	if(rect == undefined)
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

// Only used for fixed elements
function AdjustRectToZoom(rect)
{
	var zoomFactor = 1;
	if(document.body.style.zoom)
	{
		zoomFactor = document.body.style.zoom;
	};

	var output = [];
	output.push(rect.top*zoomFactor);
	output.push(rect.left*zoomFactor);
	output.push(rect.bottom*zoomFactor);
	output.push(rect.right*zoomFactor);

	return output;
}



// Used to get coordinates as array of size 4 in RenderProcessHandler
// function GetRect(node){ return AdjustRectCoordinatesToWindow(node.getBoundingClientRect()); }


// function CompareRectData(node_list, rect_list, NotificationFunc)
// {
// 	var n = node_list.length;

// 	for(var i = 0; i < n; i++)
// 	{
// 		var new_rect = AdjustRectCoordinatesToWindow(
// 			node_list[i].getBoundingClientRect()
// 		);

// 		// Adjust coordinates to fixed screen position if needed
// 		if(node_list[i].hasAttribute('fixedID'))
// 		{
// 			SubstractScrollingOffset(new_rect);
// 		}

// 		var old_rect = rect_list[i];

// 		if(new_rect[0] != old_rect[0] || new_rect[1] != old_rect[1] || new_rect[2] != old_rect[2] || new_rect[3] != old_rect[3])
// 		{
// 			NotificationFunc(i, new_rect);
// 		}

// 	}
// }

// // being lazy
// function RectToString(rect)
// {
// 	return ''+rect[0]+';'+rect[1]+';'+rect[2]+';'+rect[3];
// }

	/* NOTES
		Encoding of request strings for DOM node operations:

		DOM#{add | rem | upd}#nodeType#nodeID{#attribute#data}#

		Definitions:
		nodeType	: int
			0 : TextInput
			1 : TextLink
		nodeID		: int
		add --> send process message to Renderer to fetch V8 data
		rem --> request removal of given node
		upd --> update data of an specific node attribute
		attribute	: int
			0	: Rect
		data	: depends on attribute
	*/

// function UpdateDOMRects()
// {
// 	ConsolePrint("OLD UpdateDOMRects() function called!");
// 	// // Compare node's current Rect with data in Rect list
// 	// CompareRectData(
// 	// 	window.dom_links, 
// 	// 	window.dom_links_rect, 
// 	// 	function Notify(i, rect) { 
// 	// 		ConsolePrint('DOM#upd#1#'+i+'#0#'+RectToString(rect)+'#');
// 	// 	} 
// 	// );

// 	// CompareRectData(
// 	// 	window.dom_textinputs, 
// 	// 	window.dom_textinputs_rect, 
// 	// 	function Notify(i, rect) { 
// 	// 		ConsolePrint('DOM#upd#0#'+i+'#0#'+RectToString(rect)+'#');
// 	// 	} 
// 	// );
// }

function UpdateFixedElementRects()
{
	window.fixed_elements.forEach(
		function (node){
			var id = node.getAttribute('fixedID');

			var preUpdate = window.fixed_coordinates[id];

			window.fixed_coordinates[id] = AdjustRectToZoom(
				node.getBoundingClientRect()
			);

			UpdateSubtreesBoundingRect(node);

			// UpdateSubtreesBoundingRects only informs CEF if child nodes' Rect exists or changes
			// So, check if updates took place has to be done here
			var equal = (preUpdate.length == window.fixed_coordinates.length);
			for(var i = 0, n = preUpdate.length; i < n && equal; i++)
			{
				equal &= (preUpdate[i] == window.fixed_coordinates[i]);
			}
			
			if(!equal)
			{
				var zero = '';
				if(id < 10) zero = '0';
				// Tell CEF that fixed node was updated
				ConsolePrint('#fixElem#add#'+zero+id);	// TODO: Change String encoding, get rid of 'zero'
			}

		}
	);


}

function AddDOMTextInput(node)
{
	// DEBUG
	ConsolePrint("AddDOMTextInput should not be called anymore!");

	// // Add text input node to list of text inputs
	// window.dom_textinputs.push(node);

	// // Add text input's Rect to list
	// window.dom_textinputs_rect.push(
	// 	AdjustRectCoordinatesToWindow(
	// 		node.getBoundingClientRect()
	// 	)
	// );

	// // Add attribute in order to recognize already discovered DOM nodes later
	// node.setAttribute('nodeType', '0');
	// node.setAttribute('nodeID', (window.dom_textinputs.length-1));

	// // Inform CEF, that new text input is available
	// var node_amount = window.dom_textinputs.length - 1;
	// ConsolePrint('DOM#add#0#'+node_amount+'#');

	// DEBUG
	//ConsolePrint("END adding text input");
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

// Iterate over Set of already used fixedIDs to find the next ID not yet used and assign it to node as attribute
function AddFixedElement(node)
{
	// Determine fixed element ID
	var id;

	if(node.hasAttribute('fixedID'))
	{
		// node already in set, use existing ID
		id = node.getAttribute('fixedID');

		// Check if bounding Rect changes happened
		UpdateSubtreesBoundingRect(node);
	}
	else
	{
		// Add node to set of fixed elements, if position is fixed
		window.fixed_elements.add(node);

		// Find smallest ID not yet in use

		var found = false;
		for(var i=0, n=window.fixed_IDlist.length; i < n; i++)
		{
			if(!window.fixed_IDlist[i])
			{
				window.fixed_IDlist[i] = true;
				id = i;
				found = true;
			}
		}
		if(!found)
		{
			// Extend ID list by one entry
			window.fixed_IDlist.push(true);
			// Use new last position as ID
			id = window.fixed_IDlist.length-1;
		}

		// Create attribute in node and store ID there
		node.setAttribute('fixedID', id);

		// DEBUG
		// ConsolePrint("Adding fixed node #"+id);

		// Write node's (and its children's) bounding rectangle coordinates to List
		SaveBoundingRectCoordinates(node);
	}

	// TODO: Add attribute 'fixedID' to every child node



	var zero = '';
	if(id < 10) zero = '0';
	// Tell CEF that fixed node was added
	ConsolePrint('#fixElem#add#'+zero+id);
}


function SaveBoundingRectCoordinates(node)
{
	// DEBUG
	// ConsolePrint('Start of SaveBoundingRectCoordinates...');

	var rect = node.getBoundingClientRect();
	// Only add coordinates if width and height are greater zero
	// if(rect.width && rect.height)
	// {
	var id = node.getAttribute('fixedID');

	
	// Add empty 1D array to 2D array, if needed
	while(window.fixed_coordinates.length <= id)
	{
		window.fixed_coordinates.push([]);
	}

	// 1D array with top, left, bottom, right values of bounding rectangle
	var rect_coords = AdjustRectToZoom(
		node.getBoundingClientRect()
	);
	
	// Add rect coordinates to list of fixed coordinates at position |id|
	window.fixed_coordinates[id] = rect_coords;

	// DEBUG
	// ConsolePrint('Start of ComputeBoundingRectOfAllChilds...');

	// Compute bounding rect containing all child nodes
	var tree_rect_coords = ComputeBoundingRectOfAllChilds(node, 0, id);

		// DEBUG
	// ConsolePrint('End of ComputeBoundingRectOfAllChilds.');

	// Save tree rect, if different than fixed nodes bounding rect
	var equal = true;
	for(var i = 0; i < 4 && equal; i++)
	{
		equal &= (rect_coords[i] == tree_rect_coords[i]);
	}

	if(!equal)
	{
		window.fixed_coordinates[id] = 
			window.fixed_coordinates[id].concat(
				ComputeBoundingRect(rect_coords, tree_rect_coords)
			);
	}
	 ConsolePrint('Computed fixed#'+id+": "+window.fixed_coordinates[id]);

	 // DEBUG
	// ConsolePrint('End of SaveBoundingRectCoordinates.');
}

// Inform CEF about the current fication status of a already known node
function SetFixationStatus(node, status)
{
	var type = node.getAttribute('nodeType');
	var id = node.getAttribute('nodeID');

	if(type && id)
	{
		// DEBUG
		// ConsolePrint("Getting DOMObject...");
		
		var domObj = GetDOMObject(type, id);
		if(domObj)
		{
			domObj.setFixed(status);
		}
		// DEBUG
		// ConsolePrint("Done with getting & configuring DOMObject.");
		// // Inform about updates in node's attribute |1| aka |_fixed : bool|
		// // _fixed = status;
		// var intStatus = (status) ? 1 : 0;
		// ConsolePrint('DOM#upd#'+type+'#'+id+'#1#'+intStatus+'#');

		// //DEBUG
		// //ConsolePrint('Setting fixation status to '+intStatus);
	}
}

// parent & child are 1D arrays of length 4
function ComputeBoundingRect(parent, child)
{
	// if(parent.height == 0 || parent.width == 0)
	if(parent[2]-parent[0] == 0 || parent[3]-parent[1] == 0)
		return child;

	return [Math.min(parent[0], child[0]),
			Math.min(parent[1], child[1]),
			Math.max(parent[2], child[2]),
			Math.max(parent[3], child[3])];
}

function ComputeBoundingRectOfAllChilds(node, depth, fixedID)
{
	// Check if node's bounding rectangle is outside of the current union of rectangles in |window.fixed_coordinates[id]|
	if(node.nodeType == 1) // 1 == ELEMENT_NODE
	{

		// Add attribute 'fixedID' in order to indicate being a child of a fixed node
		if(!node.hasAttribute('fixedID'))
		{
			node.setAttribute('fixedID', fixedID);
		}

		// Inform CEF that DOM node is child of a fixed element
		if(node.hasAttribute('nodeType'))
		{
			SetFixationStatus(node, true);
		}

		var rect = node.getBoundingClientRect();

		// Compare nodes to current bounding rectangle of all child nodes
		var rect_coords = AdjustRectToZoom(rect);

		// Traverse all child nodes
		if(node.children && node.children.length > 0)
		{
			var n = node.children.length;

			for(var i=0; i < n ; i++)
			{		
				// Compare previously computed bounding rectangle with the one computed by traversing the child node
				rect_coords = 
					ComputeBoundingRect(
						rect_coords,
						ComputeBoundingRectOfAllChilds(node.children.item(i), depth+1, fixedID)
					);
			}
		}


		return rect_coords;
	}

	// Error case
	return [-1,-1,-1,-1]; // TODO: nodeType != 1 possible? May ruin the whole computation
}

// If resetChildren=true, traverse through all child nodes and remove attribute 'fixedID'
// May not be neccessary if node (and its children) were completely removed from DOM tree
function RemoveFixedElement(node, resetChildren)
{
	// Remove node from Set of fixed elements
	window.fixed_elements.delete(node);

	if(node.hasAttribute('fixedID'))
	{
		var id = node.getAttribute('fixedID');
		// Remove fixedID from ID List
		window.fixed_IDlist[id] = false;
		// Delete bounding rectangle coordinates for this ID
		window.fixed_coordinates[id] = [];

		var zero = '';
		if(id < 10) zero = '0';

		// Tell CEF that fixed node with ID was removed
		ConsolePrint('#fixElem#rem#'+zero+id);
		// Remove 'fixedID' attribute
		node.removeAttribute('fixedID');

		if(resetChildren && node.children)
		{
			UnfixChildNodes(node.children);
		}
	}
}

function UnfixChildNodes(childNodes)
{
	for(var i = 0, n = childNodes.length; i < n; i++)
	{
		childNodes[i].removeAttribute('fixedID');
		
		SetFixationStatus(childNodes[i], false);

		if(childNodes[i].children)
		{
			UnfixChildNodes(childNodes[i].children);
		}
	}
}

// For fixed elements: Traverse subtree starting with given childNode as root
function UpdateSubtreesBoundingRect(childNode)
{

	var id = childNode.getAttribute('fixedID');

	// ConsolePrint('Checking #'+id+' for updates...');

	// Read out old rectangle coordinates
	var old_coords = window.fixed_coordinates[id];
	// Update bounding rectangles (only for subtree, starting with node where changes happend)
	var child_coords = ComputeBoundingRectOfAllChilds(childNode, 0, id);

	// ConsolePrint('child: '+child_coords);
	// ConsolePrint('old  : '+old_coords);
	var inform_about_changes = false;
	var parent_rect = old_coords.slice(0, 4);

	// Parent rect hasn't been containing all child rects
	if(old_coords.length > 4)
	{
		var old_child_rect = old_coords.slice(4);

		// Check if new child rect is contained in parent rect
		child_coords = ComputeBoundingRect(parent_rect, child_coords);

		// Inform CEF that coordinates have to be updated
		if(!CompareArrays(child_coords, parent_rect) && !CompareArrays(child_coords, old_child_rect))
		{

			// Update childrens' combined bounding rect 
			window.fixed_coordinates[id] = parent_rect.concat(child_coords);
			// alert('new: '+new_coords+'; old: '+old_coords);
			// ConsolePrint("Updated subtree's bounding rect: "+window.fixed_coordinates[id]);

			inform_about_changes = true;
		}
	}
	else // Parent rect has been containing all child rects
	{
		// Check if new child rect is contained in parent rect
		child_coords = ComputeBoundingRect(parent_rect, child_coords);

		if(!CompareArrays(parent_rect, child_coords))
		{
			window.fixed_coordinates[id] = parent_rect.concat(child_coords);

			inform_about_changes = true;
		}
		// else: Parent rect still contains updated child rect


		// TODO: Rewrite code here.. confusing af. Also, it seems to inform_about_changes, although none have taken place!
	}


	if(inform_about_changes)
	{
		var zero = '';
		if(id < 10) zero = '0';
		// Tell CEF that fixed node's coordinates were updated
		ConsolePrint('#fixElem#add#'+zero+id);

		// //DEBUG
		// ConsolePrint('Updated a fixed element');
		// for(var i = 0, n = window.fixed_coordinates.length; i < n ; i++)
		// {
		// 	var str = (i == id) ? '<---' : '';
		// 	ConsolePrint(i+': '+window.fixed_coordinates[i]+str);
		// }

		// DEBUG
		// ConsolePrint("UpdateSubtreesBoundingRect()");
	}

}

function CompareArrays(array1, array2)
{
	var n = array1.length;

	if(n != array2.length)
		return false;

	for(var i = 0; i < n; i++)
	{
		if(array1[i] != array2[i])
			return false;
	}

	return true;
}

// OLD APPROACH
function AddDOMTextLink(node)
{
	// DEBUG
	//ConsolePrint("START adding text link");

	window.dom_links.push(node);

	var rect = node.getBoundingClientRect(node);
	var coords = AdjustRectCoordinatesToWindow(rect); //[rect.top, rect.left, rect.bottom, rect.right];
	window.dom_links_rect.push(coords);


	// Add attribute in order to recognize already discovered DOM nodes later
	node.setAttribute('nodeType', '1');
	node.setAttribute('nodeID', (window.dom_links.length-1));

	// Tell CEF message router, that DOM Link was added
	ConsolePrint('DOM#add#1#'+(window.dom_links.length-1)+'#');

		// DEBUG
	//ConsolePrint("END adding text link");
}

// TESTING PURPOSE
document.onclick = function(){
	ConsolePrint("### document.onclick() triggered! ###");
	UpdateDOMRects();
	// UpdateFixedElementRects();

		// var divs = document.getElementsByTagName('DIV');
		// for (var i = 0, n = divs.length; i < n; i++)
		// {
		// 	var role = divs[i].getAttribute('role');
		// 	ConsolePrint('role: '+role);
		// 	if(role == 'button')
		// 	{
		// 		ConsolePrint("Found DIV which behaves as Button!");
		// 		CreateDOMLink(divs[i]);
		// 	}
		// 	if(role == 'textbox')
		// 	{
		// 		ConsolePrint("Found DIV which behaves as Textbox!");
		// 		CreateDOMTextInput(divs[i]);
		// 	}
		// }
	
}

window.maxDepth = 0;
function UpdateMaxDepth(node)
{
	ConsolePrint("UpdateMaxDepth...");
	var depth = 0;
	var current = node;
	while(current != document.documentElement)
	{
		ConsolePrint("depth: "+depth);
		current = current.parentNode;
		if(current) depth++;
	}
	ConsolePrint('Result: depth='+depth);
	window.maxDepth = Math.max(window.maxDepth, depth);
}

var targetNode;

// Trigger DOM data update on changing document loading status
document.onreadystatechange = function()
{
	// ConsolePrint("### DOCUMENT STATECHANGE! ###");

	if(document.readyState == 'loading')
	{
		ConsolePrint('### document.readyState == loading ###'); // Never triggered
	}

	if(document.readyState == 'interactive')
	{
		//ConsolePrint("document reached interactive state: Updating DOM Rects...");

		UpdateDOMRects();

		//ConsolePrint("... done with Updating DOM Rects.");

		var links = document.getElementsByTagName('A');
		ConsolePrint('Found '+links.length+' links on an alternative way...');

		// var additional = 0;
		// for(var i = 0, n = links.length; i < n; i++){
		// 	// ConsolePrint(links[i].textContent+' -- ELEMENT_NODE? '+(links[i].nodeType == 1));
		// 	if( links[i].nodeType == 1 && !links[i].hasAttribute('nodeID'))
		// 	{
		// 		CreateDOMLink(links[i]);
		// 		additional++;
		// 	}
		// }
		// ConsolePrint("Added "+additional+" additional Link nodes!");


	}

	if(document.readyState == 'complete')
	{
		UpdateDOMRects();

		ConsolePrint('Page fully loaded. #TextInputs='+window.domTextInputs.length+', #Links='+window.domLinks.length);

		var links = document.getElementsByTagName('A');
		ConsolePrint('Found '+links.length+' links on an alternative way...');

		// ConsolePrint("DEBUGGING: maxDepth="+window.maxDepth);
		// var additional = 0;
		// for(var i = 0, n = links.length; i < n; i++){
		// 	// ConsolePrint(links[i].textContent+' -- ELEMENT_NODE? '+(links[i].nodeType == 1));
		// 	if( links[i].nodeType == 1 && !links[i].hasAttribute('nodeID'))
		// 	{
		// 		CreateDOMLink(links[i]);
		// 		additional++;
		// 	}
		// }
		// ConsolePrint("Added "+additional+" additional Link nodes!");

		if(targetNode)
		{
			ConsolePrint("innerHTML: "+targetNode.innerHTML);
		}


		var divs = document.getElementsByTagName('DIV');
		
		/* Experimenting with CSS events */

		// forEach extension for NodeList divs
		Array.prototype.forEach.call(divs, function(node){
				if(window.getComputedStyle(node, null).getPropertyValue('transition-property') == 'opacity')
				{
					node.addEventListener('webkitTransitionEnd', function(event)
					{
						ConsolePrint('Added event listener: Transition!');
						// ConsolePrint("Transition ended: opacity="+window.getComputedStyle(node, null).getPropertyValue('opacity'));
					}, false);
					// ConsolePrint("EventListener for transition end added!");
				}	

				if(window.getComputedStyle(node, null).getPropertyValue('overflow') == 'hidden')
				{
					node.addEventListener('overflow', function(event)
					{
						ConsolePrint('Added event listener: Overflow!');
						// ConsolePrint("Transition ended: opacity="+window.getComputedStyle(node, null).getPropertyValue('opacity'));
					}, false);
					// ConsolePrint("EventListener for overflow end added!");

					node.addEventListener('underflow', function(event)
					{
						ConsolePrint('Added event listener: Underflow!');
						// ConsolePrint("Transition ended: opacity="+window.getComputedStyle(node, null).getPropertyValue('opacity'));
					}, false);
					// ConsolePrint("EventListener for underflow end added!");
				}
			}
		);


		ConsolePrint("Amount of nodes, whose children have to be checked on another way: "+window.checkChildrens.size);
		window.checkChildrens.forEach(function(node){
			ConsolePrint("class: "+node.getAttribute('class')+" tag: "+node.tagName);
		});

		// TODO: >> WHEN << to check not observed child nodes??
		
		
	}
}

// http://stackoverflow.com/questions/18323757/how-do-you-detect-that-a-script-was-loaded-and-executed-in-a-chrome-extension
document.addEventListener('beforeload', function(event) {
    var target = event.target;
    if (target.nodeName.toUpperCase() !== 'SCRIPT') return;
    var dispatchEvent = function(name, bubbles, cancelable) {
        var evt = new CustomEvent(name, {
            bubbles: bubbles,
            cancelable: cancelable
        });
        target.dispatchEvent(evt);
        if (evt.defaultPrevented) {
            event.preventDefault();
        }
    };
    var onload = function() {
        cleanup();
        dispatchEvent('afterscriptexecute', true, false);
    };
    var cleanup = function() {
        target.removeEventListener('load', onload, true);
        target.removeEventListener('error', cleanup, true);
    }
    target.addEventListener('error', cleanup, true);
    target.addEventListener('load', onload, true);

    dispatchEvent('beforescriptexecute', true, true);
}, true);

document.addEventListener('beforescriptexecute', function() {
    ConsolePrint("beforescriptexecute fired!");
});
document.addEventListener('afterscriptexecute', function() {
    ConsolePrint("afterscriptexecute fired!");
});


window.onresize = function()
{
	//UpdateDOMRects();
	// TODO: Update fixed elements' Rects too?
	ConsolePrint("Javascript detected window resize, update of fixed element Rects.");

	// UpdateFixedElementRects(); // TODO: Already triggered by CEF's JS injection?
}

window.scriptExecuting = false;
window.executedScript;
var scriptOrder = 0;

// Instantiate and initialize the MutationObserver
function MutationObserverInit()
{ 
	ConsolePrint('Initializing Mutation Observer ... ');

	window.observer = new MutationObserver(
		function(mutations) {
		  	mutations.forEach(
		  		function(mutation)
		  		{
					// DEBUG
					// ConsolePrint("Mutation occured..."+mutation.type);

			  		var node;
					
		  			if(mutation.type == 'attributes')
		  			{
		  				node = mutation.target;

						//   UpdateMaxDepth(node);

			  			var attr; // attribute name of attribute which has changed

		  				// TODO: How to identify node whose attributes changed? --> mutation.target?
		  				if(node.nodeType == 1) // 1 == ELEMENT_NODE
		  				{
		  					attr = mutation.attributeName;

							  if(node === document.documentElement && attr == 'style')
							  {
								//   ConsolePrint("DEBUG: zoom = "+window.getComputedStyle(document.documentElement, null).getPropertyValue('zoom'));
							  }

		  					// usage example: observe changes in input field text
		  					// if (node.tagName == 'INPUT' && node.type == 'text')
		  					// 	alert('tagname: \''+node.tagName+'\' attr: \''+attr+'\' value: \''+node.getAttribute(attr)+'\' oldvalue: \''+mutation.oldValue+'\'');

		  					if(attr == 'style' || 
							   (document.readyState != 'loading' && attr == 'class') ) // TODO: example: uni-koblenz.de - node.id='header': class changes from 'container' to 'container fixed'
		  					{
									// alert('attr: \''+attr+'\' value: \''+node.getAttribute(attr)+'\' oldvalue: \''+mutation.oldValue+'\'');
									if(window.getComputedStyle(node, null).getPropertyValue('position') == 'fixed')
									// if(node.style.position && node.style.position == 'fixed')
									{
										if(!window.fixed_elements.has(node))
										{
											//DEBUG
											ConsolePrint("Attribut "+attr+" changed, calling AddFixedElement");

											AddFixedElement(node);
											
											UpdateDOMRects();
										}
							
									}
									else // case: style.position not fixed
									{
										// If contained, remove node from set of fixed elements
										if(window.fixed_elements.has(node))
										{
											RemoveFixedElement(node, true);

											UpdateDOMRects();
										}
									}


									// Visibility a): style.visibility of given node (and its children)
									// Check visibility of all children, only top most node called because of changes in node.style?
									function CheckVisibility(child){
										var id = child.getAttribute('nodeID');
										// Current node is already known to C++, so check visibility
										if(id)
										{
											var type = child.getAttribute('nodeType');
											var domObj = GetDOMObject(type, id);
											// And there really exists a corresponding DOMObject
											if(domObj)
											{	
												domObj.checkVisibility();
											}

										}
										// Call function for all children
										var children = child.children, n = children.length;
										for(var i = 0; i < n; i++)
										{
											CheckVisibility(children[i]);
										}
									}
									// Call recursively for all child nodes
									CheckVisibility(node);
								
									


		  					}

								// Changes in attribute 'class' may indicate that bounding rect update is needed, if node is child node of a fixed element
								if(attr == 'class')
								{
									if(node.hasAttribute('fixedID'))
									{
										//DEBUG
										// ConsolePrint("class changed, updating subtree");
										UpdateDOMRects();

										UpdateSubtreesBoundingRect(node);

										


										// TODO: Triggered quite often... (while scrolling)
									}

									// Testing: GMail, username entered, pw field appears, Rects not in place...
									UpdateDOMRects();
								}


  
								if(attr == 'href')
								{
									CreateDOMLink(node);
									ConsolePrint("Changes in attribute |href|, adding link..");
								}



								if(attr == 'role')
								{
									var attrVal = node.getAttribute('role');
									if(attrVal == 'button')
									{
										ConsolePrint("Found DIV with role = button, by observing attribute role");
										CreateDOMLink(node);
									}
									if(attrVal == 'textbox')
									{
										ConsolePrint("Found DIV with role = textbox, by observing attribute role");
										CreateDOMTextInput(node);
									}
								}

								if(attr == 'innerHTML')
								{
									ConsolePrint("innerHTML changed!");
									// ConsolePrint('----------------------------------');
									// ConsolePrint('old.innerHTML: '+mutation.oldValue);
									// ConsolePrint('new.innerHTML: '+node.innerHTML);
									
								}


							
		  				}
						
					

		  			}





		  			if(mutation.type == 'childList') // TODO: Called upon each appending of a child node??
		  			{
		  				// Check if fixed nodes have been added as child nodes
			  			var nodes = mutation.addedNodes; // concat(mutation.target.childNodes);
						//   var nodes = Array.prototype.slice.call(mutation.addedNodes).concat(mutation.target.childNodes);
						//   ConsolePrint("#addedNodes: "+mutation.addedNodes.length+", new: "+nodes.length);

						// DEBUGGING
						// var parent = mutation.target;
						// if(parent.className == "tag-home__wrapper")
						// {
						// 	ConsolePrint("Added child nodes: "+nodes.length);
						// 	for(var i=0, n = nodes.length; i < n; i++)
						// 		ConsolePrint(nodes[i].textContent+"# with "+nodes[i].childNodes.length+" additional children #"+nodes[i].childNodes[0].textContent);
							
								
						// // } 
						// ConsolePrint("Childs added for node: class="+mutation.target.getAttribute('class'));
						// if(mutation.target.getAttribute('class') == "tag-home__wrapper")
						// // if(mutation.target.getAttribute('class') == 'tag-home__item  js-tag-item')
						// {
						// 	ConsolePrint("Undetected Link should be appended right now... (#childs: "+nodes.length+")");
						// 	for(var i = 0, n = nodes.length; i < n; i++)
						// 	{
						// 		ConsolePrint(i+": tagName="+nodes[i].tagName+"   class: "+nodes[i].getAttribute('class'));
						// 	}
						// 	ConsolePrint("Other childs (not added): ");
						// 	for(var i = 0, n = mutation.target.childNodes.length; i < n; i++)
						// }


			  			for(var i=0, n=nodes.length; i < n; i++)
			  			{
			  				node = nodes[i]; // TODO: lots of data copying here??
							// if(node.style) node.style.backgroundColor = 'red';

							//   UpdateMaxDepth(node);

							// if(node === document.documentElement)
							// {
							// 	// ConsolePrint("### documentElement found!");

							// 	// Set up eventListener for nodeInserted CSS event
							// 	document.addEventListener(
							// 		"webkitAnimationStart", 
							// 		function(event){
							// 			if(event.animationName == "nodeInserted")
							// 			{
							// 				ConsolePrint(event.target.tagName+" was inserted in DOM");
							// 			}
							// 		}, 
							// 		false); 
							// 	ConsolePrint("EventListener for CSS 'nodeInserted' events was successfully set up!");

							// 	// Add style with CSS keyFrame, defining pseudo-animation in order to detect node insertion via CSS events
							// 	var keyFrame = document.createElement("STYLE");
							// 	keyFrame.appendChild(
							// 		document.createTextNode(
							// 			"@keyframes nodeInserted {\
							// 				from { opacity: 0.999; }\
							// 				to { opacity: 1; }\
							// 			}"
							// 		)
							// 	);
							// 	document.head.appendChild(keyFrame);
							// 	ConsolePrint("Added CSS keyframe in order to fire 'nodeInserted' CSS events.");

							// 	document.documentElement.style.WebkitAnimation = "nodeInserted 10s 1";
							// 	ConsolePrint("Added CSS animation to document's root node!");
							// }


						
			  				if(node.nodeType == 1) // 1 == ELEMENT_NODE
			  				{
								  var parent = mutation.target;
								//   // Remove parent from list of nodes, whose children have to be checked on another way
								//   window.checkChildrens.delete(parent);
							
								//   // Keep in mind which added nodes have children for which a childList mutation has to be detected in the future
								//   // otherwise, those child nodes have to be checked on another way
								//   if(node.children.length > 0)
								//   {
								// 	  window.checkChildrens.add(node);
								//   }

								//   if (window.scriptExecuting && node !== window.executedScript)
								//   {
								// 	//   window.scriptExecuting = false;
								// 	//   ConsolePrint("Script finished!")// tag: "+node.tagName+" class: "+node.getAttribute('class')+" text: "+node.textContent);
								//   }
								//   if(node.tagName == "SCRIPT")
								//   {
								// 	//   node.text = '';
								// 	//   ConsolePrint("Appending Script!");
								// 	//   node.onload = function(){ConsolePrint(scriptOrder+" Script's onload!");};
								// 	// //   node.load = function(){ConsolePrint("Script's load!");};
								// 	  node.text = "ConsolePrint('"+scriptOrder+" hrhr, code start');" + node.text + "ConsolePrint('"+scriptOrder+" hrhr, code end');";
								// 	scriptOrder++;
								// 	//   window.scriptExecuting = true;
								// 	//   window.executedScript = node;
								//   }
								//   function depth(node){
								// 	  var curr = node;
								// 	  var depth = 0;
								// 	  while(curr !== document.documentElement)
								// 	  {
								// 		  depth++;
								// 		  curr = curr.parentNode;
								// 	  }
								// 	  return depth;
								// 	}
								// 	var str = node.tagName+" depth: "+depth(node);
								// 	if(node.tagName == 'SCRIPT') str = (scriptOrder-1)+" "+str;//+node.text.substr(0,100);
								// ConsolePrint(str);//+"   siblings: "+parent.childNodes.length+" shadowRoot? "+node.shadowRoot);
								  
								//   if(parent.getAttribute('class') == "tag-home  tag-home--slide  no-js__hide  js-tag-home") ConsolePrint("This ONE node...");

								//   	// if(parent.nodeType == 1 && parent.getAttribute('class') == "tag-home  tag-home--slide  no-js__hide  js-tag-home")
								// 	// {
								// 	// 	ConsolePrint("Child has "+node.children.length);
								// 	// }
								// 	// {
								// 	// 	targetNode = node;
								// 	// 	ConsolePrint('Found target node!');
								// 	// 	ConsolePrint("innerHTML: "+node.innerHTML);
								// 	// }

								// 	// if(mutation.target.nodeType == 1 && mutation.target.getAttribute('class') == "tag-home  tag-home--slide  no-js__hide  js-tag-home")
								// 	// {
								// 	// 	ConsolePrint('Found target node as new parent! #children: '+targetNode.childNodes.length+', legally added: '+nodes.length);
								// 	// 	ConsolePrint("innerHTML: "+mutation.target.innerHTML);
								// 	// 	ConsolePrint('Children of only child: '+node.childNodes.length);
										
								// 	// }
								// 	// if(mutation.target.nodeType == 1 && mutation.target.getAttribute('class') == "tag-home__wrapper")
								// 	// {
								// 	// 	ConsolePrint("Only-child used as parent! #childs: "+nodes.length);
								// 	// 	ConsolePrint("child: "+mutation.target.innerHTML);
								// 	// 	ConsolePrint("child-child: "+nodes[0].innerHTML);
								// 	// 	ConsolePrint("But real childs: "+mutation.target.childNodes.length);
								// 	// }

								// 	// if((node.getAttribute('class') == 'tag-home__item  js-tag-item') ||
								// 	// (mutation.target.nodeType == 1 && mutation.target.getAttribute('class') == 'tag-home__item  js-tag-item'))
								// 	// {
								// 	// 	ConsolePrint("Those are the nodes I am looking for.");
								// 	// }



			  					// if(node.style.position && node.style.position == 'fixed')
		  						if(window.getComputedStyle(node, null).getPropertyValue('position') == 'fixed') 
		  						{
		  							// ConsolePrint('position: '+node.style.position);
		  							if(!window.fixed_elements.has(node)) // TODO: set.add(node) instead of has sufficient?
		  							{
		  								//DEBUG
		  								// ConsolePrint("New fixed node added to DOM tree");

										AddFixedElement(node);

										UpdateDOMRects();
		  							}
		  						}


		  						// EXPERIMENTAL
		  						// Find text links
		  						if(node.tagName == 'A' )
		  						{
									// AddDOMTextLink(node); // OLD
									// ConsolePrint("Found link: "+node.textContent);
									// New approach
									CreateDOMLink(node);

		  							// DEBUG
		  							//window.dom_links.push(node);
		  							//window.dom_links_rect.push([0,0,0,0]);

		  						}

		  						// node.text = node.tagName;
		  						// EXPERIMENTAL END

		  						if(node.tagName == 'INPUT' || 
								//   node.role == 'textbox' || 
								  node.tagName == 'TEXTAREA' ||
								  (node.tagName == 'DIV' && node.getAttribute('role') == 'textbox') )
		  						{
		  							// Identify text input fields
		  							if(node.type == 'text' || node.type == 'search' || node.type == 'email' || node.type == 'password')
		  							{
		  								// AddDOMTextInput(node); // OLD

										// New approach
										CreateDOMTextInput(node);
		  							}
		  						}

								// NEW: Buttons
								if(node.tagName == 'INPUT'  || (node.tagName == 'DIV' && node.getAttribute('role') == 'button'))
								{
									if(node.type == 'button' || node.type == 'submit' || node.type == 'reset' || !node.hasAttribute('type'))
									{
										// TODO: CreateDOMButton!
										CreateDOMLink(node);
									}
								}


			  				}
			  			}





			  			// Check if fixed nodes have been removed as child nodes and if yes, delete them from list of fixed elements
			  			var removed_nodes = mutation.removedNodes;
			  			for(var i=0, n=removed_nodes.length; i < n; i++)
			  			{
			  				node = removed_nodes[i];

			  				if(node.nodeType == 1)
			  				{
								if(window.fixed_elements.has(node))
								{
									RemoveFixedElement(node, false);

									UpdateDOMRects();
								}

								// TODO: Removal of relevant DOM node types??
			  				}
			  			}

		  			}

					// DEBUG
					//ConsolePrint("END Mutation occured...");
		  		} // end of function(){...}


		 	);    
		}
	);



	// TODO:

	// characterData vs. attributes - one of those not neccessary?

	// attributeFilter -
	// Mit dieser Eigenschaft kann ein Array mit lokalen Attributnamen angegeben werden (ohne Namespace), wenn nicht alle Attribute beobachtet werden sollen.
	// --> nur relevante Attribute beobachten!

	ConsolePrint('MutationObserver successfully created! Telling him what to observe... ');
	ConsolePrint('Trying to start observing... ');
		
	// Konfiguration des Observers: alles melden - Änderungen an Daten, Kindelementen und Attributen
	var config = { attributes: true, childList: true, characterData: true, subtree: true, characterDataOldValue: false, attributeOldValue: true};
	
	// eigentliche Observierung starten und Zielnode und Konfiguration übergeben
	window.observer.observe(document, config);

	ConsolePrint('MutationObserver was told what to observe.');

	// TODO: Tweak MutationObserver by defining a more specific configuration


	// document.documentElement.addEventListener(
	// 	"webkitAnimationStart", 
	// 	function(event){
	// 		if(event.animationName == "nodeInserted")
	// 		{
	// 			ConsolePrint(this.tagName+" noticed: "+event.target.tagName+" was inserted in DOM");
	// 		}
	// 	}, 
	// 	false); 
	// ConsolePrint("EventListener for CSS 'nodeInserted' events was successfully set up!");

	// // Add style with CSS keyFrame, defining pseudo-animation in order to detect node insertion via CSS events
	// var keyFrame = document.createElement("STYLE");
	// keyFrame.appendChild(
	// 	document.createTextNode(
	// 		// "@-webkit-keyframes nodeInserted {\
	// 		"@keyframes nodeInserted {\
	// 			from { opacity: 0; }\
	// 			to { opacity: 1; }\
	// 		}"
	// 	)
	// );
	// document.head.appendChild(keyFrame);
	// ConsolePrint("Added CSS keyframe in order to fire 'nodeInserted' CSS events.");

	// document.documentElement.style.WebkitAnimation = "nodeInserted 3s infinite";
	// // document.documentElement.style.webkitAnimation = "nodeInserted";
	// // document.documentElement.style.webkitDuration = "5s";
	// ConsolePrint("Added CSS animation to document's root node!");


	/*
	documentObserver = new MutationObserver(
		function(mutations) {
		  	mutations.forEach(
		  		function(mutation)
		  		{
					// DEBUG
					//ConsolePrint("START Mutation occured...");

			  		//var node;
					
		  			if(mutation.type == 'attributes')
		  			{
		  				var attr = mutation.attributeName;
						ConsolePrint('DOCUMENT OBSERVER!   '+attr);

					}
				}
			);
		}
	);

	var config = { attributes: true, childList: false, characterData: true, subtree: false, characterDataOldValue: false, attributeOldValue: false};
	
	// eigentliche Observierung starten und Zielnode und Konfiguration übergeben
	documentObserver.observe(window.document.documentElement, config);
	*/
	
} // END OF MutationObserverInit()

function MutationObserverShutdown()
{
	window.observer.disconnect(); 

	delete window.observer;

	ConsolePrint('Disconnected and deleted MutationObserver! ');
}

		  				// https://developer.mozilla.org/de/docs/Web/API/Node
		  				// https://developer.mozilla.org/de/docs/Web/API/Element
		  				// http://stackoverflow.com/questions/9979172/difference-between-node-object-and-element-object
		  				// read also: http://www.backalleycoder.com/2013/03/18/cross-browser-event-based-element-resize-detection/

		  												// offtopic: check if attributes exist
								// if(nodes[i].hasOwnProperty('style'))

			  					// http://ryanmorr.com/understanding-scope-and-context-in-javascript/
