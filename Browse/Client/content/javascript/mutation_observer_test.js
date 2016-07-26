// https://developer.mozilla.org/de/docs/Web/API/MutationObserver


// Helper function for console output
function consolePrint(msg)
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
	var doc = document.documentElement;
	var zoomFactor = 1;
	var offsetX = (window.pageXOffset || doc.scrollLeft) - (doc.clientLeft || 0); 
	var offsetY = (window.pageYOffset || doc.scrollTop) - (doc.clientTop || 0); 

	var docRect = document.body.getBoundingClientRect();

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



// node.fixedID defines position, where to store bounding rectangle coordinates in |window.fixed_coordinates|
window.elements = [];

window.fixed_elements = new Set();
window.fixed_IDlist = [];				// Position |i| contains boolean information if ID |i| is currently used
window.fixed_coordinates = [[]];		// 2D list of bounding rectangle coordinates, fill with bounding rect coordinates of fixed node and its children (if position == 'relative')

window.dom_links = [];
window.dom_links_rect = [[]];

// Used to get coordinates as array of size 4 in RenderProcessHandler
// function GetRect(node){ return AdjustRectCoordinatesToWindow(node.getBoundingClientRect()); }


// DEBUG
window.debug_node;

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

		// Write node's (and its children's) bounding rectangle coordinates to List
		SaveBoundingRectCoordinates(node);
	}

	// TODO: Add attribute 'fixedID' to every child node



	var zero = '';
	if(id < 10) zero = '0';
	// Tell CEF that fixed node was added
	consolePrint('#fixElem#add#'+zero+id);
}


function SaveBoundingRectCoordinates(node)
{
	var rect = node.getBoundingClientRect();
	// Only add coordinates if width and height are greater zero
	// if(rect.width && rect.height)
	// {
	var id = node.getAttribute('fixedID');

	// 1D array with top, left, bottom, right values of bounding rectangle
	var rect_coords = AdjustRectToZoom(node.getBoundingClientRect());

	// Add empty 1D array to 2D array, if needed
	while(window.fixed_coordinates.length <= id)
	{
		window.fixed_coordinates.push([]);
	}
	// Add rect coordinates to list of fixed coordinates at position |id|
	window.fixed_coordinates[id] = rect_coords;

	// Compute bounding rect containing all child nodes
	var tree_rect_coords = ComputeBoundingRectOfAllChilds(node, 0, id);

	// Save tree rect, if different than fixed nodes bounding rect
	var equal = true;
	for(var i = 0; i < 4; i++)
	{
		equal &= (rect_coords[i] == tree_rect_coords[i]);
	}
	if(!equal)
	{
		window.fixed_coordinates[id] = window.fixed_coordinates[id].concat(
											ComputeBoundingRect(rect_coords, tree_rect_coords)
										);
	}
	// }
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


function RemoveFixedElement(node)
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
		consolePrint('#fixElem#rem#'+zero+id);
		// Remove 'fixedID' attribute
		node.removeAttribute('fixedID');
	}
}

// NOT IN USE
function UpdateBoundingRects()
{
	consolePrint('UpdateBoundingRects()');
	for (var node of fixed_elements)
	{
		var id = node.getAttribute('fixedID');

		consolePrint('Checking #'+id+' for updates...');

		// Read out old rectangle coordinates
		var old_coords = window.fixed_coordinates[id];
		// Update bounding rectangles
		SaveBoundingRectCoordinates(node);
		// Read out updated rectangle coordinates in order to compare old and new
		var new_coords = window.fixed_coordinates[id];

		consolePrint('new: '+new_coords);
		consolePrint('old: '+old_coords);

		// Inform CEF that coordinates have to be updated
		if(new_coords.length !== old_coords.length)
		{
			// alert('new: '+new_coords+'; old: '+old_coords);

			var zero = '';
			if(id < 10) zero = '0';
			// Tell CEF that fixed node's coordinates were updated
			consolePrint('#fixElem#add#'+zero+id);
		}


	}
}

// Traverse subtree starting with given childNode as root
function UpdateSubtreesBoundingRect(childNode)
{

	var id = childNode.getAttribute('fixedID');

	// consolePrint('Checking #'+id+' for updates...');

	// Read out old rectangle coordinates
	var old_coords = window.fixed_coordinates[id];
	// Update bounding rectangles (only for subtree, starting with node where changes happend)
	var child_coords = ComputeBoundingRectOfAllChilds(childNode, 0, id);

	// consolePrint('child: '+child_coords);
	// consolePrint('old  : '+old_coords);
	var inform_about_changes = false;
	var parent_rect = old_coords.slice(0, 4);

	// Parent rect hasn't been containing all child rects
	if(old_coords.length > 4)
	{
		var old_child_rect = old_coords.slice(4, 8);

		// Inform CEF that coordinates have to be updated
		if(!CompareArrays(child_coords, parent_rect) && !CompareArrays(child_coords, old_child_rect))
		{

			// Update childrens' combined bounding rect 
			window.fixed_coordinates[id] = parent_rect.concat(child_coords);
			// alert('new: '+new_coords+'; old: '+old_coords);
			// consolePrint("Updated subtree's bounding rect: "+window.fixed_coordinates[id]);

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
	}


	if(inform_about_changes)
	{
		var zero = '';
		if(id < 10) zero = '0';
		// Tell CEF that fixed node's coordinates were updated
		consolePrint('#fixElem#add#'+zero+id);
		// DEBUG
		// consolePrint("UpdateSubtreesBoundingRect()");
	}

}

function CompareArrays(array1, array2)
{
	if(array1.length != array2.length)
		return false;

	for(var i = 0, n = array1.length; i < n; i++)
	{
		if(array1[i] != array2[i])
			return false;
	}

	return true;
}

function AddDOMTextLink(node)
{
	var rect = node.getBoundingClientRect(node);

	// Only add DOMTextLinks which are visible and contain an URL
	if(rect.width && rect.height && node.href)
	{
		window.dom_links.push(node);
		// window.dom_links_rect.push(AdjustRectCoordinatesToWindow(node));
		// var coords = AdjustRectCoordinatesToWindow(rect); //[rect.top, rect.left, rect.bottom, rect.right];
		var coords = [rect.top, rect.left, rect.bottom, rect.right];
		window.dom_links_rect.push(coords);

		// DEBUG
		// consolePrint("Added DOMTextLink #"+(window.dom_links.length-1)+", coords: "+coords);

		// Tell CEF message router, that DOM Link was added
		consolePrint('#DOMLink#'+(window.dom_links.length-1)); // TODO: #DOMLink#add/rem?
	}
	
}

// TODO: Better: Add EventListener?
// document.onclick = function(){ UpdateBoundingRects(); };

// DEBUG
document.onreadystatechange = function()
{
	consolePrint("### DOCUMENT STATECHANGE! ###");

	if(document.readyState == 'complete')
	{
		consolePrint("### Updating Link Rects ###");

		for(var i = 0, n = window.dom_links.length; i < n; i++)
		{
			var node = window.dom_links[i];
			AddDOMTextLink(node);
		}
	}
}

// alert(window.elements.length);
// eine Instanz des Observers erzeugen
function MutationObserverInit()
{ 
	consolePrint('Trying to initialize Mutation Observer ... ');

	window.observer = new MutationObserver(
		function(mutations) {
		  	mutations.forEach(
		  		function(mutation)
		  		{
			  		var node;

		  			if(mutation.type == 'attributes')
		  			{
		  				node = mutation.target;
			  			var attr; // attribute name of attribute which has changed

		  				// TODO: How to identify node whose attributes changed? --> mutation.target?
		  				if(node.nodeType == 1) // 1 == ELEMENT_NODE
		  				{
		  					attr = mutation.attributeName;

		  					// usage example: observe changes in input field text
		  					// if (node.tagName == 'INPUT' && node.type == 'text')
		  					// 	alert('tagname: \''+node.tagName+'\' attr: \''+attr+'\' value: \''+node.getAttribute(attr)+'\' oldvalue: \''+mutation.oldValue+'\'');

		  					if(attr == 'style' || attr == 'class') // TODO: example: uni-koblenz.de - node.id='header': class changes from 'container' to 'container fixed'
		  					{
		  						// alert('attr: \''+attr+'\' value: \''+node.getAttribute(attr)+'\' oldvalue: \''+mutation.oldValue+'\'');
		  						if(window.getComputedStyle(node, null).getPropertyValue('position') == 'fixed')
		  						// if(node.style.position && node.style.position == 'fixed')
		  						{
		  							if(!window.fixed_elements.has(node)) // TODO: set.add(node) instead of has sufficient?
		  							{
		  								//DEBUG
		  								// consolePrint("Attribut "+attr+" changed, calling AddFixedElement");

		  								AddFixedElement(node);
		  							}
		  				
		  						}
		  						else // case: style.position not fixed
		  						{
		  							// If contained, remove node from set of fixed elements
		  							if(window.fixed_elements.has(node))
		  							{
		  								RemoveFixedElement(node);
		  							}
		  						}
		  					}

		  					// Changes in attribute 'class' may indicate that bounding rect update is needed, if node is child node of a fixed element
		  					if(attr == 'class')
		  					{
		  						if(node.hasAttribute('fixedID'))
		  						{
		  							//DEBUG
		  							// consolePrint("class changed, updating subtree");

		  							UpdateSubtreesBoundingRect(node);


		  							// TODO: Triggered quite often... (while scrolling)
		  						}
		  					}

		  					// if(attr == 'href')
		  					// {
		  					// 	AddFixedElement(node);
		  					// 	consolePrint("Changes in attribute |href|, adding link..");
		  					// }

		  				}

		  			}





		  			if(mutation.type == 'childList') // TODO: Called upon each appending of a child node??
		  			{
		  				// Check if fixed nodes have been added as child nodes
			  			var nodes = mutation.addedNodes;

			  			for(var i=0, n=nodes.length; i < n; i++)
			  			{
			  				node = nodes[i]; // TODO: lots of data copying here??
			  				var rect;

			  				if(node.nodeType == 1) // 1 == ELEMENT_NODE
			  				{

			  					// if(node.style.position && node.style.position == 'fixed')
		  						if(window.getComputedStyle(node, null).getPropertyValue('position') == 'fixed')
		  						{
		  							// consolePrint('position: '+node.style.position);
		  							if(!window.fixed_elements.has(node)) // TODO: set.add(node) instead of has sufficient?
		  							{
		  								//DEBUG
		  								// consolePrint("New fixed node added to DOM tree");

		  								AddFixedElement(node);
		  							}
		  						}


		  						// EXPERIMENTAL
		  						// Find text links
		  						if(node.tagName == 'A')
		  						{
		  							// AddDOMTextLink(node);
		  							// DEBUG
		  							window.dom_links.push(node);
		  							window.dom_links_rect.push([0,0,0,0]);

		  						}

		  						// node.text = node.tagName;
		  						// EXPERIMENTAL END


			  				}
			  			}





			  			// Check if fixed nodes have been removed as child nodes and if yes, delete them from list of fixed elements
			  			var removed_nodes = mutation.removedNodes;
			  			for(var i=0, n=removed_nodes.length; i < n; i++)
			  			{
			  				node = removed_nodes[i];

			  				if(node.nodeType == 1)
			  				{
			  					// DEBUG
			  					// if(window.getComputedStyle(node, null).getPropertyValue('position') == 'fixed')
			  					// 	alert('Observed removal of fixed child node!');

			  					RemoveFixedElement(node);
			  				}
			  			}

		  			}

		  		} // end of function(){...}


		 	);    
		}
	);



	// TODO:

	// characterData vs. attributes - one of those not neccessary?

	// attributeFilter -
	// Mit dieser Eigenschaft kann ein Array mit lokalen Attributnamen angegeben werden (ohne Namespace), wenn nicht alle Attribute beobachtet werden sollen.
	// --> nur relevante Attribute beobachten!

	consolePrint('MutationObserver successfully created! Telling him what to observe... ');
	consolePrint('Trying to start observing... ');
		
	// Konfiguration des Observers: alles melden - Änderungen an Daten, Kindelementen und Attributen
	var config = { attributes: true, childList: true, characterData: true, subtree: true, characterDataOldValue: false, attributeOldValue: false};
	
	// eigentliche Observierung starten und Zielnode und Konfiguration übergeben
	window.observer.observe(window.document, config);

	consolePrint('MutationObserver was told what to observe.');
	
} // END OF MutationObserverInit()

function MutationObserverShutdown()
{
	window.observer.disconnect(); 

	delete window.observer;

	consolePrint('Disconnected and deleted MutationObserver! ');
}

		  				// https://developer.mozilla.org/de/docs/Web/API/Node
		  				// https://developer.mozilla.org/de/docs/Web/API/Element
		  				// http://stackoverflow.com/questions/9979172/difference-between-node-object-and-element-object
		  				// read also: http://www.backalleycoder.com/2013/03/18/cross-browser-event-based-element-resize-detection/

		  												// offtopic: check if attributes exist
								// if(nodes[i].hasOwnProperty('style'))

			  					// http://ryanmorr.com/understanding-scope-and-context-in-javascript/
