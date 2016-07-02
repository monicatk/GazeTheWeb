// https://developer.mozilla.org/de/docs/Web/API/MutationObserver


// DEBUG - Helper function for console output
function consolePrint(msg)
{
	window.cefQuery({ request: msg, persistent : false, onSuccess : function(response) {}, onFailure : function(error_code, error_message){} });
}

function CheckRectResolution(node)
{
	var rect = node.getBoundingClientRect();
	// true, if both values greater than zero
	return rect.height && rect.width;
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
	for(i=0, n=window.fixed_IDlist.length; i < n; i++)
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

	// Tell CEF that fixed node was added
	var zero = '';
	if(id < 10)
	{
		zero = '0';
	}
	consolePrint('#fixElem#add#'+zero+id);
}


function SaveBoundingRectCoordinates(node)
{
	var rect = node.getBoundingClientRect();
	// Only add coordinates if width and height are greater zero
	if(rect.width && rect.height)
	{
		var id = node.getAttribute('fixedID');

		var coords = [];
		coords.push(rect.top);
		coords.push(rect.left);
		coords.push(rect.bottom);
		coords.push(rect.right);

		// TODO: Include zoom factor!

		// TODO: Include child nodes with position == 'relative' to fixed node

		while(window.fixed_coordinates.length <= id)
		{
			window.fixed_coordinates.push([]);
		}

		window.fixed_coordinates[id] = coords;

		consolePrint('DEBUG: fixed coords: '+rect.top+' '+rect.left+' '+rect.bottom+' '+rect.right);
	}
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
		if(id < 10)
		{
			zero = '0';
		}
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
	  				// TODO: How to identify node whose attributes changed? --> mutation.target?
	  				if(mutation.target.nodeType == 1) // 1 == ELEMENT_NODE
	  				{
	  					node = mutation.target;
	  					attr = mutation.attributeName;

	  					// usage example: observe changes in input field text
	  					// if (node.tagName == 'INPUT' && node.type == 'text')
	  					// 	alert('tagname: \''+node.tagName+'\' attr: \''+attr+'\' value: \''+node.getAttribute(attr)+'\' oldvalue: \''+mutation.oldValue+'\'');

	  					if(attr == 'style')
	  					{
	  						// alert('attr: \''+attr+'\' value: \''+node.getAttribute(attr)+'\' oldvalue: \''+mutation.oldValue+'\'');
	  						if(node.style.position && node.style.position == 'fixed')
	  						{
	  							if(!window.fixed_elements.has(node))
	  							{
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

						// // if(node.style && node.style.position && node.style.position =='fixed')
		  		// // 		{
		  		// // 			alert('Found fixed element '+node.tagName+' '+node.type);
						// // 		// var rect = node.getBoundingClientRect();
						// // 		// alert('Found fixed element! top: '+rect.top+', left: '+rect.left+', bottom: '+rect.bottom+', right: '+rect.right);
						// // }
	  				}

	  			}

	  			if(mutation.type == 'childList')
	  			{
		  			var nodes = mutation.addedNodes;

		  			for(i=0, n=nodes.length; i < n; i++)
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


		  				if(node.nodeType == 1) // 1 == ELEMENT_NODE
		  				{

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


// (function (global) {
//     "use strict";

//     if (typeof global.MutationObserver !== "function") {
//         global.MutationObserver = global.WebKitMutationObserver || global.MozMutationObserver;
//     }

//     var watch = document.getElementById("watch");

//     function whenClicked() {
//         var li = document.createElement("li");

//         li.textContent = "List element";
//         watch.appendChild(li);
//     }

//     document.getElementById("button").addEventListener("click", whenClicked, false);

//     if (typeof global.MutationObserver !== "function") {
//         watch.addEventListener("DOMNodeInserted", function (evt) {
//             console.log("New element detected", evt.target);
//         }, false);
//     } else {
//         var observer = new global.MutationObserver(function (mutations) {
//             mutations.forEach(function (mutation) {
//                 if (mutation.type === 'childList') {
//                     console.log("New element detected", mutation.addedNodes[0]);
//                 }
//             });
//         });

//         observer.observe(watch, {
//             childList: true,
//             characterData: true,
//             subtree: true
//         });
//     }
// }(window));