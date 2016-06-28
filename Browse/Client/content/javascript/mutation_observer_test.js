// https://developer.mozilla.org/de/docs/Web/API/MutationObserver

// zu überwachende Zielnode (target) auswählen
var target = document.documentElement;
 
window.elements = [];
alert(window.elements.length);
// eine Instanz des Observers erzeugen
var observer = new MutationObserver(
	function(mutations) {
	  	mutations.forEach(
	  		function(mutation)
	  		{
	  		// for(j = 0; j < mutations.length; j++)
	  			// var mutation = mutations[j];
	  			if(mutation.type == 'childList')
	  			{
		  			var nodes = mutation.addedNodes;
		  			for(i=0; i < nodes.length; i++)
		  			{
		  				// https://developer.mozilla.org/de/docs/Web/API/Node
		  				// https://developer.mozilla.org/de/docs/Web/API/Element
		  				// http://stackoverflow.com/questions/9979172/difference-between-node-object-and-element-object
		  				// read also: http://www.backalleycoder.com/2013/03/18/cross-browser-event-based-element-resize-detection/

		  				// GOAL: map Node to Element?
		  				// var target = nodes[i].target;

		  				// nodes[i].nodeValue = nodes[i].id;


		  				if(nodes[i].nodeType == 1) // 1 == ELEMENT_NODE
		  				{
		  					nodes[i].value = nodes[i].tagName;
		  					// elements.push(nodes[i].tagName);
		  					alert(window.elements.length);

		  					// http://ryanmorr.com/understanding-scope-and-context-in-javascript/
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