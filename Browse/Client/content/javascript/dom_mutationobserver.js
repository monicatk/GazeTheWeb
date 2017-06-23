//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================
ConsolePrint("Starting to import dom_mutationobserver.js ...");

window.appendedSubtreeRoots = new Set();

// TODO: Deprecated?
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

// TESTING PURPOSE
document.onclick = function(){
	// ConsolePrint("### document.onclick() triggered! Calling UpdateDOMRects! ###");
	// UpdateDOMRects();
	
}
// TESTING TWITTER, no updates through clicking right now
// document.addEventListener("click", function(e){
// 	ConsolePrint("JS Debug: Realized that click event was fired! Target is listed in DevTools Console.");
// 	console.log("Clicked target: "+e.target);
// 	UpdateDOMRects();
// });

// Trigger DOM data update on changing document loading status
document.onreadystatechange = function()
{
	// ConsolePrint("### DOCUMENT STATECHANGE! ###");

	// if(document.readyState == 'loading')
	// {
	// 	// ConsolePrint('### document.readyState == loading ###'); // Never triggered
	// }

	if(document.readyState == 'interactive')
	{
		// GMail fix
		ForEveryChild(document.documentElement, AnalyzeNode);

		UpdateDOMRects();

		// GMail
		// ForEveryChild(document.documentElement, AnalyzeNode);
	}

	if(document.readyState == 'complete')
	{
		// GMail fix
		ForEveryChild(document.documentElement, AnalyzeNode);

		UpdateDOMRects();

		//ConsolePrint('<----- Page fully loaded. -------|');

		// GMail
		// ForEveryChild(document.documentElement, AnalyzeNode);
		
	}
}


window.onresize = function()
{
	//UpdateDOMRects();
	// TODO: Update fixed elements' Rects too?
	ConsolePrint("Javascript detected window resize, update of fixed element Rects.");
}

// Refactored!
// Update rects if CSS transition took place (TODO: Needed if parent's rects didn't change?)
document.addEventListener('transitionend', function(event){
	// Tree, whose children have to be check for rect updates
	var root = event.target;

	var fixedElem = GetFixedElementByNode(root);
	if(fixedElem !== undefined)
	{
		// If root is fixed element, subtree will be updated by simply calling updateRects
		fixedElem.updateRects();
	}
	else
	{
		// Update rects of given root node
		var obj = GetCorrespondingDOMObject(root);
		if(obj !== undefined)
		{
			obj.updateRects();
		}
		// Update rects of whole subtree beneath root node
		ForEveryChild(root, 
			(child) => {
				var fixedElem = GetFixedElementByNode(child);
				if(fixedElem === undefined)
				{
					var obj = GetCorrespondingDOMObject(child);
					if (obj !== undefined)
					{
						obj.updateRects();
					}
				}
				else
				{
					fixedElem.updateRects();
				}
			},
			(child) => {
				// Abort rect update of childs subtree, if child is fixed element
				// With child as fixed element, rect updates for subtree will be triggered anyway
				return (GetFixedElementByNode(child) !== undefined);
			}
		); // ForEveryChild end
	}

}, false);



var docFrags = [];

var origCreateElement = Document.prototype.createElement;
Document.prototype.createElement = function(tag)
{
	if(arguments[0] === "iframe" || arguments[0] === "IFRAME") 
	{
		// ConsolePrint("<iframe> created.");
	}
	// ConsolePrint("Creating element with tagName="+tag);

	var elem = origCreateElement.apply(this, arguments);
	// if(arguments[0] === "iframe") elem.style.backgroundColor = "#0000ff";
	return elem ;
}


/* Modify appendChild in order to get notifications when this function is called */
var originalAppendChild = Element.prototype.appendChild;
Element.prototype.appendChild = function(child){
	// appendChild extension: Check if root is already part of DOM tree
    if(this.nodeType == 1 || this.nodeType > 8)
    {
		var subtreeRoot = this;

		// Stop going up the tree when parentNode is documentElement or doesn't exist (null or undefined)
		while(subtreeRoot !== document.documentElement && subtreeRoot.parentNode && subtreeRoot.parentNode !== undefined)
			subtreeRoot = subtreeRoot.parentNode;

		// Register subtree roots, whose children have to be checked outside of MutationObserver
        if(subtreeRoot !== document.documentElement) 
		{

			// When DocumentFragments get appended to DOM, they "lose" all their children and only their children are added to DOM
			if(subtreeRoot.nodeType == 11) // 11 == DocumentFragment
			{
				// Mark all 1st generation child nodes of DocumentFragments as subtree roots
				for(var i = 0, n = subtreeRoot.childNodes.length; i < n; i++)
				{
					window.appendedSubtreeRoots.add(subtreeRoot.childNodes[i]);
					// ForEveryChild(subtreeRoot.childNodes[i], function(childNode){
						// if(childNode.style) childNode.style.backgroundColor = '#0000ff';
					// });	
				}
			}
			else 
			{	
				// Add subtree root to Set of subtree roots
				window.appendedSubtreeRoots.add(subtreeRoot);	

				// Remove children of this subtree root from subtree root set --> prevent double-checking of branches
				ForEveryChild(subtreeRoot, function(childNode){
						// if(childNode.nodeType == 1 && childNode.style) childNode.style.backgroundColor = '#ffff00';
						window.appendedSubtreeRoots.delete(childNode);
					}
				);
			} // else
		}

    }  

	// DocumentFragment as parent: children disappear when fragment is appended to DOM tree
	if(child.nodeType === 11)
	{
		for(var i = 0, n = child.childNodes.length; i < n; i++)
		{
			ForEveryChild(child.childNodes[i], 
				(childNode) => { window.appendedSubtreeRoots.delete(childNode); }
			);

			window.appendedSubtreeRoots.add(child.childNodes[i]);
		}
	}

	// Finally: Execute appendChild
    return originalAppendChild.apply(this, arguments); // Doesn't work with child, why?? Where does arguments come from?
};

Document.prototype.createDocumentFragment = function() {
	var fragment = new DocumentFragment();

	// TODO: childList in config not needed? Or later for children, added to DOM, relevant?
	if(window.observer !== undefined)
	{
		var config = { attributes: true, childList: true, characterData: true, subtree: true, characterDataOldValue: false, attributeOldValue: true};
		window.observer.observe(fragment, config);
	}
	else
		console.log("Custom createDocumentFragment: MutationObserver doesn't seem to have been set up properly.");

	return fragment;
};




function AnalyzeNode(node)
{
	if( (node.nodeType == 1 || node.nodeType > 8) && (node.hasAttribute && !node.hasAttribute("nodeType")) && (node !== window)) // 1 == ELEMENT_NODE
	{

		// If node is possible DocFrag subtree root node, remove it from set and analyze its subtree too
		if(window.appendedSubtreeRoots.delete(node))
			ForEveryChild(node, AnalyzeNode);

		var computedStyle = window.getComputedStyle(node, null);

		// Identify fixed elements on appending them to DOM tree
		if(
			// computedStyle.getPropertyValue('display') !==  "none" && // NOTE: if display == 'none' -> rect is zero
			(
				computedStyle.getPropertyValue('position') == 'fixed'
				//  ||	node.tagName == "DIV" && node.getAttribute("role") == "dialog"	// role == dialog isn't sufficient
			)
		) 
		{
			// Returns true if new FixedElement was added; false if already linked to FixedElement Object
			if(AddFixedElement(node))
			{
				UpdateDOMRects();
			}
		}


		if(node.tagName === "SELECT")
		{
			CreateDOMSelectField(node);
		}


		// Find text links
		if(node.tagName == 'A' )
		{
			CreateDOMLink(node);
		}

		if(node.tagName == 'INPUT' || node.tagName == "BUTTON") // Fun fact: There exists the combination of tag "BUTTON" and type "submit"
		{
			// Identify text input fields
			if(node.type == 'text' || node.type == 'search' || node.type == 'email' || node.type == 'password')
			{
				CreateDOMTextInput(node);
			}

			// TODO: Handle other kinds of <input> elements
			if(node.type == 'button' || node.type == 'submit' || node.type == 'reset' || !node.hasAttribute('type'))
			{
				// TODO: CreateDOMButton!
				CreateDOMLink(node);
			}
		}
		// textareas or DIVs, who are treated as text fields
		if(node.tagName == 'TEXTAREA' || (node.tagName == 'DIV' && node.getAttribute('role') == 'textbox'))
		{
			CreateDOMTextInput(node);
		}

		// NEW: Buttons
		if(node.tagName == 'DIV' && node.getAttribute('role') == 'button')
		{
			CreateDOMLink(node);
		}

		// GMail
		if(node.tagName == 'DIV' && (node.getAttribute('role') == 'link' || node.getAttribute('role') == 'tab') )
		{
			CreateDOMLink(node);
		}
		if(node.tagName == "SPAN" && node.hasAttribute('email'))
		{
			CreateDOMLink(node);
		}

		// GMail: Trying to make mail receiver clickable (again)
		if(node.tagName === "DIV" && node.className === "vR")
		{
			console.log("GMail mail receiver fix: Found <div> with class 'vR'. I will assume it will be clickable.");
			CreateDOMLink(node);
		}
		if(node.tagName === "SPAN" && node.tagName === "aQ2")
		{
			console.log("GMail mail receiver fix: Found <span> with class 'aQ2'. I will assume it will be clickable.");
			CreateDOMLink(node);
		}
		if(node.tagName == "IMG" && node.getAttribute("data-tooltip") !== null)
		{
			CreateDOMLink(node);
		}

		var rect = node.getBoundingClientRect();

		// Detect scrollable elements inside of webpage
		if((node.tagName === "DIV" || node.tagName === "P") && rect.width > 0 && rect.height > 0)
		{
			var overflow = computedStyle.getPropertyValue("overflow");
			if(overflow === "scroll" || overflow == "auto")
			{
				CreateDOMOverflowElement(node);
			}
			// else if (overflow === "auto")
			// {
			// 	// TODO: Element size might change over time! Keep node as potential overflow in mind?
			// 	// OR add it anyway and only scroll if height and scroll height aren't the same?
			// 	if(node.scrollHeight !== rect.height || node.scrollWidth !== rect.width)
			// 	{
			// 		CreateDOMOverflowElement(node);
			// 	}
			// }
			else
			{
				var overflowX = computedStyle.getPropertyValue("overflow-x");
				var overflowY = computedStyle.getPropertyValue("overflow-y");
				if(overflowX === "auto" || overflowX === "scroll" || overflowY === "auto" || overflowY === "scroll")
				{
					CreateDOMOverflowElement(node);
				}
			}
		}


		// Update whole <form> if transition happens in form's subtree
		// (For shifting elements in form (e.g. Google Login) )
		if(node.tagName == 'FORM')
		{

			node.addEventListener('webkitTransitionEnd', function(event){
				// ConsolePrint("FORM transition event detected"); //DEBUG
				ForEveryChild(node, function(child){
					if(child.nodeType == 1)
					{
						var nodeType = child.getAttribute('nodeType');
						if(nodeType)
						{
							var nodeID = child.getAttribute('nodeID');
							var domObj = GetDOMObject(nodeType, nodeID);
							if(domObj)
							{
								domObj.updateRects();
							}
						}
					}
					
				});
			}, false);
		}


	}
}

window.debug = false;

// Instantiate and initialize the MutationObserver
function MutationObserverInit()
{ 
	ConsolePrint('Initializing Mutation Observer ... ');

	window.observer = new MutationObserver(
		function(mutations) {
		  	mutations.forEach(
		  		function(mutation)
		  		{
					// TODO: Refactoring:
					// When attributes changed:
					// 		- Fetch corresponding DOMNode object at the beginning, if it exists
					// 		- Pipe attribute type to object and get corresponding setter, if it exists
					// 		- Call setter with given (updated) data
					// Recognition of needed rect updates:
					// 		- As before
					// Adding & removal of nodes:
					// 		- Try to access existing DOMNode object, first

					if(debug)
					console.log(mutation.type, "\t", mutation.attributeName, "\t", mutation.oldValue, "\t", mutation.target);
					
					var working_time_start = Date.now();

			  		var node;
					
		  			if(mutation.type === 'attributes')
		  			{
		  				node = mutation.target;
			  			var attr; // attribute name of attribute which has changed


		  				if(node.nodeType == 1) // 1 == ELEMENT_NODE
		  				{
		  					attr = mutation.attributeName;

							// ### FIXED HIERARCHY HANDLING ###
							// 1.) Cascading copy of current childFixedId for whole subtree
							// 2.) Update of fixObj in each DOM object in this subtree
							if(attr === "childfixedid")
							{
								var id = node.getAttribute(attr);

								// Set childFixedId for each child of altered node
								// Works also with fixObj === undefined
								var fixObj = GetFixedElementById(id);

								node.childNodes.forEach((child) => {
									// Extend node by given attribute
									if(typeof(child.setAttribute) === "function")
										child.setAttribute("childFixedId", id);
										// Update fixObj in DOMObject
										var domObj = GetCorrespondingDOMObject(child);
										if(domObj !== undefined)
											domObj.setFixObj(fixObj);	// fixObj may be undefined
								});
							}



							// ### OVERFLOW HIERARCHY HANDLING ###
							// Set childrens' overflowIds when node gets tagged as part of overflow subtree 
							if(attr == "overflowid")
							{
								var id = node.getAttribute("overflowid");
								var domObj = GetCorrespondingDOMObject(node);
								var skip_subtree = false;
								if(domObj !== undefined)
								{
								// id of null or -1 will reset overflow object in domObj
									domObj.setOverflowViaId(id);

									if(domObj.Class === "DOMOverflowElement")
										skip_subtree = true;
								}
								// TODO: This node should automatically get an overflowId (on Overflow test site), but it doesn't --> DocFrag!
								// if(node.className === "scroll-wrapper scrollbar-inner")
								// 	console.log("overflowId changed to: "+id);

								if(!skip_subtree)
								{
									for(var i = 0, n = node.childNodes.length; i < n; i++)
									{			
										var child = node.childNodes[i];	

										if(child === undefined || typeof(child.setAttribute) !== "function")
										{			
											// console.log(i+"/"+n+": Skipped.");
											continue;
										}
										// console.log(i+"/"+n+": "+child.className);

										if(id !== null)
											child.setAttribute("overflowId", id);
										else
											child.removeAttribute("overflowid");	// TODO: Search for hierachically higher overflows!
									}
								}
							}


							
							// ### CREATION AND REMOVAL OF FIXED ELEMENTS ###
		  					if(attr == 'style' ||  (document.readyState != 'loading' && attr == 'class') )
		  					{
								if(window.getComputedStyle(node, null).getPropertyValue('position') === 'fixed')
								{
									if(AddFixedElement(node))
										// Update every Rect, just in case anything changed due to an additional fixed element
										UpdateDOMRects();
								}
								else 
								{
									// Checks if node corresponds to fixedObj and removes it, when true
									RemoveFixedElement(node);
								}
							}

						
							if(attr == 'class')
							{
								// Update (if it exists) DOM object's rects if node's class changed
								var domObj = GetCorrespondingDOMObject(node);
								if(domObj !== undefined)
									domObj.updateRects();

								// ### FIXED ELEMENT BOUNDING BOX UPDATES ###
								// Changes in attribute 'class' may indicate that a fixed element's union of bounding rects needs to be updated
								// Trigger fixed parent update, if it exists. All of its children will be updated
								var childFixedId = node.getAttribute("childFixedId");
								if(childFixedId !== null && childFixedId !== null)
								{
									var fixObj = GetFixedElementById(childFixedId);
									if(fixObj !== undefined)
									{
										fixObj.updateRects();
										return;
									}
								}

								// Else, if node is fixed, subtree will be updated too
								var fixObj = GetFixedElementByNode(node);
								if(fixObj !== undefined)
								{
									fixObj.updateRects();
									return;
								}

								// Update subtree if class changes
								ForEveryChild(node, (child) => { UpdateNodesRect(child); });
							
							}

							if(attr == "style")
							{
								// Goal: Recognise changes in e.g. style.display
								// 'solution': Trigger rect update if changes in style took place. Direct change in style would be
								// value assignment, which will be recognised in MutationObserver
								UpdateNodesRect(node);
								ForEveryChild(node, (child) => { UpdateNodesRect(child); });
								// TODO: Changes in style may occure when scrolling some elements ... might be a lot of Rect Update calls!
		  					} // END attr == 'style'

		  				} // END node.nodeType == 1
		  			} // END mutation.type == 'attributes'





		  			if(mutation.type === 'childList') // TODO: Called upon each appending of a child node??
		  			{
						// console.log("type: ", mutation.type,"\ttarget: ", mutation.target, "\tnodeType: ", mutation.target.nodeType);

		  				// Check if fixed nodes have been added as child nodes
			  			var nodes = mutation.addedNodes;
						var parent = mutation.target;
						
						// Handle every appended child node
			  			nodes.forEach((node) => {
							if(node === undefined)
								return;

							// Mark child nodes of DocumentFragment, in order to being able to analyze their subtrees later
							if(mutation.target.nodeType === 11)
							{
								// Add node as possible root, which might not be recognized when DocFrag disappears
								window.appendedSubtreeRoots.add(node);
								// Remove knowledge about every possible root due to a DocFrag in whole subtree
								ForEveryChild(node, function(child){ window.appendedSubtreeRoots.delete(child); });
							}

							// DEBUG
							if(node.className === "scroll-wrapper scrollbar-inner")
								console.log("Target node added to dom tree!");
							
							
							// Set children (and their subtree) to fixed, if parent is also fixed
							if(parent !== undefined && parent.nodeType === 1 && node.nodeType === 1)
							{
								var id = parent.getAttribute("childfixedid");
								if(id === null || id === undefined)
									id = parent.getAttribute("fixedid");

								if(id !== null && id !== undefined)
								{
									node.setAttribute("childfixedid", id);
									var domObj = GetCorrespondingDOMObject(node);
									if(domObj !== undefined)
										domObj.setFixObj(GetFixedElementById(id));
								}
							}

							// If parent is contained in overflow element, then child will also be
							if(parent !== undefined && typeof(parent.getAttribute) === "function" && typeof(node.setAttribute) === "function")
							{
								var overflowId = parent.getAttribute("overflowid");
								if(overflowId !== null)
									node.setAttribute("overflowid", overflowId);
							}

			  				AnalyzeNode(node);
						}); // END of forEach

						// Update rects of fixed parent's subtree after having appended child nodes 
						// TODO: Partial rect update for fixed elements?
						if(parent.nodeType === 1)
						{
							var fixId = parent.getAttribute("childFixedId");
							if(fixId !== null && fixId !== undefined)
							{
								var fixObj = GetFixedElementById(fixId);
								if(fixObj !== null && fixObj !== undefined)
								{
									fixObj.updateRects();
								}
							}
						}

						

			  			
			  			mutation.removedNodes.forEach((node) => {
			  				if(node !== undefined && node.nodeType === 1)
			  				{
								RemoveFixedElement(node, false);

								var overflowId = node.getAttribute("overflowId");
								if(overflowId !== null)
								{
									RemoveDOMOverflowElement(overflowId);
								}

			  				}

							UpdateNodesRect(node);
							// Trigger Rect Updates after removal of (several) node(s)
							ForEveryChild(node, (child) => { UpdateNodesRect(child); });
						});


		  			} // END mutation.type == 'childList'

					mutation_observer_working_time += (Date.now() - working_time_start);

						
		  		} // END forEach mutation


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
	window.observer.observe(window.document, config);

	ConsolePrint('MutationObserver was told what to observe.');

	// TODO: Tweak MutationObserver by defining a more specific configuration



	
} // END OF MutationObserverInit()


function MutationObserverShutdown()
{
	window.observer.disconnect(); 

	delete window.observer;

	ConsolePrint('Disconnected and deleted MutationObserver! ');
}




var mutation_observer_working_time = 0;
var load_starting_time;

function StartPageLoadingTimer()
{
	load_starting_time = Date.now();
}

function StopPageLoadingTimer()
{
	var page_load_duration = Date.now() - load_starting_time;
	ConsolePrint("Page load took "+page_load_duration+"ms");
	ConsolePrint("MutationObserver operations took "+mutation_observer_working_time+"ms, "+
		(100*mutation_observer_working_time/page_load_duration)+"% of page load.");
}

function CreateCustomMutationObserver(node, attrFunc, childFunc)
{
	var observer = new MutationObserver(
		function(mutations) {
		  	mutations.forEach(
		  		function(mutation){
					if(mutation.type === 'attributes')
						attrFunc(mutation);
					if(mutation.type == 'childList')
						childFunc(mutation);

				  }
			);
		}
	);
	var config = {
		attributes: true, childList: true, characterData: true, 
		subtree: true, characterDataOldValue: true, attributeOldValue: true
	};
	observer.observe(node, config);
	return observer;
}


ConsolePrint("Successfully imported dom_mutationobserver.js!");


// window.onchange = function(e){ConsolePrint("Window changes: "+e);};

// ConsolePrint("Creating window_observer...");
// window.window_observer = new MutationObserver(
// 		function(mutations) {
// 		  	mutations.forEach(
// 		  		function(mutation)
// 		  		{
// 					ConsolePrint("Mutation in window object detected!");

// 		  			if(mutation.attributeName == "document")
// 					{
// 						ConsolePrint("window object's 'document' attribute changed!");

// 						var config = { attributes: true, childList: true, characterData: true, subtree: true, characterDataOldValue: false, attributeOldValue: true};
// 						window.observer.observe(window.document, config);
// 					}
// 				}
// 			);
// 		}
// );
// var config = {attributes: true}
// window.window_observer.observe(window, config );
// ConsolePrint("... done");

