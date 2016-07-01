// https://developer.mozilla.org/de/docs/Web/API/MutationObserver


// DEBUG - Helper function for console output
function consolePrint(msg)
{
	window.cefQuery({ request: msg, persistent : false, onSuccess : function(response) {}, onFailure : function(error_code, error_message){} });
}

function RemovedFixedElement(fixedID)
{
	consolePrint('#fixElem#rem#'+fixedID);
}

function AddedFixedElement(fixedID)
{
	consolePrint('#fixElem#add#'+fixedID);
}

function CheckRectResolution(node)
{
	var rect = node.getBoundingClientRect();
	// true, if both values greater than zero
	return rect.height && rect.width;
}

// zu überwachende Zielnode (target) auswählen
var target = document.documentElement;
 
window.elements = [];
window.fixed_elements = new Set();
window.fixed_current_key = 0;			// Inkrement this key, when adding new elements to set & map
window.fixed_ID_to_node = new Map();	// Map IDs of fixed elements to node objects, access nodes from C++ with given ID

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
	  						if(node.style.position && node.style.position == 'fixed' && !window.fixed_elements.has(node))
	  						{
	  							// Add node to set of fixed elements, if position is fixed
	  							window.fixed_elements.add(node);
	  							// Add node as value to fixed element map with a given ID as key
	  							window.fixed_ID_to_node.set(window.fixed_current_key, node);
	  							// Tell CEF about adding fixed element
	  							AddedFixedElement(window.fixed_current_key);
	  							// Increment current key to avoid duplicates
	  							window.fixed_current_key++;
	  						}
	  						else // style.position not fixed
	  						{
	  							// consolePrint('position attribute removed or not fixed (anymore)!');
	  							// Remove node from set of fixed elements, if contained
	  							if(window.fixed_elements.has(node))
	  							{
	  								window.fixed_elements.delete(node);
	  								consolePrint('Removed node from set of fixed elements');

	  								// Get fixedID, delete it from Map and tell CEF that fixed element with this ID was removed
	  								for(var id of window.fixed_ID_to_node.keys())
	  								{
	  									if (window.fixed_ID_to_node.get(id) == node)
	  									{
	  										window.fixed_ID_to_node.delete(id);
	  										// Tell CEF about the removal
	  										RemovedFixedElement(id);
	  									}
	  								}
	  							}
	  							// NOTE: Deleting node as value from Map is ignored, because it would be too much overhead
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
		  						node.value = node.type;
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