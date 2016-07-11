// https://developer.mozilla.org/de/docs/Web/API/MutationObserver


// DEBUG - Helper function for console output
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
	};

	var output = [];
	output.push(rect.top*zoomFactor + offsetY);
	output.push(rect.left*zoomFactor + offsetX);
	output.push(rect.bottom*zoomFactor + offsetY);
	output.push(rect.right*zoomFactor + offsetX);

	return output;
}

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


// zu überwachende Zielnode (target) auswählen
var target = document.documentElement;
 

// node.fixedID defines position, where to store bounding rectangle coordinates in |window.fixed_coordinates|
window.elements = [];

window.fixed_elements = new Set();
window.fixed_IDlist = [];				// Position |i| contains boolean information if ID |i| is currently used
window.fixed_coordinates = [[]];		// 2D list of bounding rectangle coordinates, fill with bounding rect coordinates of fixed node and its children (if position == 'relative')

// Iterate over Set of already used fixedIDs to find the next ID not yet used and assign it to node as attribute
function AddFixedElement(node)
{
	// Add node to set of fixed elements, if position is fixed
	window.fixed_elements.add(node);

	// Find smallest ID not yet in use
	var id;
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

	var zero = '';
	if(id < 10) zero = '0';
	// Tell CEF that fixed node was added
	consolePrint('#fixElem#add#'+zero+id);
}


function SaveBoundingRectCoordinates(node)
{
	var rect = node.getBoundingClientRect();
	// Only add coordinates if width and height are greater zero
	if(rect.width && rect.height)
	{
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
		var tree_rect_coords = ComputeBoundingRectOfAllChilds(node, 0);

		// Save tree rect, if different than fixed nodes bounding rect
		var equal = true;
		for(var i = 0; i < 4; i++)
		{
			equal &= (rect_coords[i] == tree_rect_coords[i]);
		}
		if(!equal)
		{
			window.fixed_coordinates[id] = window.fixed_coordinates[id].concat(
												tree_rect_coords
											);
		}
	}
}

// parent & child are 1D arrays of length 4
function ComputeBoundingRect(parent, child)
{
	// if(parent == [-1,-1,-1,-1])
	// 	return child;

	return [Math.min(parent[0], child[0]),
			Math.min(parent[1], child[1]),
			Math.max(parent[2], child[2]),
			Math.max(parent[3], child[3])];
}

function ComputeBoundingRectOfAllChilds(node, depth)
{
	// Check if node's bounding rectangle is outside of the current union of rectangles in |window.fixed_coordinates[id]|
	if(node.nodeType == 1) // 1 == ELEMENT_NODE
	{
		var rect = node.getBoundingClientRect();

		// Compare nodes to current bounding rectangle of all child nodes
		var rect_coords = AdjustRectToZoom(rect);


		var out = 'No children';
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
						ComputeBoundingRectOfAllChilds(node.children.item(i), depth+1)
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

// alert(window.elements.length);
// eine Instanz des Observers erzeugen
var observer = new MutationObserver(
	function(mutations) {
	  	mutations.forEach(
	  		function(mutation)
	  		{
	  		// for(j = 0; j < mutations.length; j++)
	  			// var mutation = mutations[j];
		  		var node;
		  		var attr; // attribute name of attribute which has changed

	  			if(mutation.type == 'attributes')
	  			{
	  				node = mutation.target;

	  				//DEBUG
	  				if(node.id == 'social-floater')
	  				{
	  					consolePrint('Attribute changes in social floater');
	  				}

	  				// TODO: How to identify node whose attributes changed? --> mutation.target?
	  				if(node.nodeType == 1) // 1 == ELEMENT_NODE
	  				{
	  					attr = mutation.attributeName;

	  					// usage example: observe changes in input field text
	  					// if (node.tagName == 'INPUT' && node.type == 'text')
	  					// 	alert('tagname: \''+node.tagName+'\' attr: \''+attr+'\' value: \''+node.getAttribute(attr)+'\' oldvalue: \''+mutation.oldValue+'\'');

	  					if(attr == 'style')
	  					{
	  						// alert('attr: \''+attr+'\' value: \''+node.getAttribute(attr)+'\' oldvalue: \''+mutation.oldValue+'\'');
	  						if(node.style.position && node.style.position == 'fixed')
	  						{
	  							if(!window.fixed_elements.has(node)) // TODO: set.add(node) instead of has sufficient?
	  							{
	  								alert('Found fixed element!');
	  								AddFixedElement(node);
	  							}
	  						}
	  						else // case: style.position not fixed
	  						{
	  							// If contained, remove node from set of fixed elements
	  							if(window.fixed_elements.has(node))
	  							{
	  								RemoveFixedElement(node);

	  								// TODO: case fixed element like accepting cookies: node may disappear and no attribute changes!
	  								// Observe node removals and check if it was a fixed element
	  							}
	  						}
	  					}

						// // if(node.style && node.style.position && node.style.position =='fixed')
		  		// // 		{
		  		// // 			alert('Found fixed element '+node.tagName+' '+node.type);
						// // 		// var rect = node.getBoundingClientRect();
						// // 		// alert('Found fixed element! top: '+rect.top+', left: '+rect.left+', bottom: '+rect.bottom+', right: '+rect.right);
						// // }
	  				}

	  			}

	  			if(mutation.type == 'childList') // TODO: Called upon each appending of a child node??
	  			{
		  			var nodes = mutation.addedNodes;

		  			for(var i=0, n=nodes.length; i < n; i++)
		  			{
		  				node = nodes[i]; // TODO: lots of data copying here??
		  				var rect;

		  				// https://developer.mozilla.org/de/docs/Web/API/Node
		  				// https://developer.mozilla.org/de/docs/Web/API/Element
		  				// http://stackoverflow.com/questions/9979172/difference-between-node-object-and-element-object
		  				// read also: http://www.backalleycoder.com/2013/03/18/cross-browser-event-based-element-resize-detection/

		  				// GOAL: map Node to Element?
		  				// var target = nodes[i].target;

		  				// nodes[i].nodeValue = nodes[i].id;
		  				if(node.id && node.id == 'social-floater')
		  				{
		  					consolePrint('Social floater was added!');
		  				}

		  				if(node.nodeType == 1) // 1 == ELEMENT_NODE
		  				{
		  					if(node.style.position && node.style.position == 'fixed')
	  						{
	  							consolePrint('position: '+node.style.position);
	  							if(!window.fixed_elements.has(node)) // TODO: set.add(node) instead of has sufficient?
	  							{
	  								alert('Found fixed element! (node added to DOM tree)');
	  								AddFixedElement(node);
	  							}
	  						}

		  					if(node.tagName == 'INPUT' && (node.type == 'text' || node.type == 'search' || node.type == 'email' || node.type == 'password') )
		  					{
		  						// node.value = node.type;


		  						// var rect = node.getBoundingClientRect();
		  						// alert('input! top: '+rect.top+', left: '+rect.left+', bottom: '+rect.bottom+', right: '+rect.right);
		  				// 		rect = node.getBoundingClientRect();
								// alert('Found input field! top: '+rect.top+', left: '+rect.left+', bottom: '+rect.bottom+', right: '+right);
		  					}
		  					// nodes[i].value = nodes[i].tagName;
		  					// elements.push(nodes[i].tagName);
		  					// alert(window.elements.length);

		  					// http://ryanmorr.com/understanding-scope-and-context-in-javascript/


		  			// 		/* Fixed element observation */
		  			// 		if(node.style && node.style.position && node.style.position =='fixed')
		  			// 		{
		  			// 			alert('Found fixed element '+node.tagName+' '+node.type);
							// 	// var rect = node.getBoundingClientRect();
							// 	// alert('Found fixed element! top: '+rect.top+', left: '+rect.left+', bottom: '+rect.bottom+', right: '+rect.right);
							// }

							// offtopic: check if attributes exist
							// if(nodes[i].hasOwnProperty('style'))

		  				}

		  			}
	  			}
	  			// else if(mutation.type == 'attributes')
	  			// {
	  			// 	// TODO: Wie heißt das geaenderte Attribut?
	  				
	  			// 	alert('attributName: '+mutation.attributeName+', attributeNamespace: '+mutation.attributeNamespace+'');
	  			// }
	  			// else if(mutation.type == 'characterData')
	  			// {
	  			// 	alert('vorher: '+mutation.target.oldValue+', nachher: '+mutation.target.nodeValue);
	  			// }
	  		}
	  	// 	function(mutation) {
	   //  		alert('target: '+mutation.target.type+', type: '+mutation.type);
	  	// 	}
	 	);    
	}
);

// Konfiguration des Observers: alles melden - Änderungen an Daten, Kindelementen und Attributen
var config = { attributes: true, childList: true, characterData: true, subtree: true, characterDataOldValue: true, attributeOldValue:true};
 
// eigentliche Observierung starten und Zielnode und Konfiguration übergeben
observer.observe(target, config);
consolePrint('MutationObserver successfully created!');
