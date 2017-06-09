ConsolePrint("Starting to import dom_nodes_interaction.js ...");




function SetSelectionIndex(id, idx)
{
	var obj = GetDOMSelectField(id);
	if(obj === undefined)
	{
		ConsolePrint("ERROR: Function call of 'SetSelectionIndex' failed.")
		return;
	}

	obj.setSelectionIdx(index);
}

DOMTextInput.prototype.setTextInput = function(text, submit){
	// TODO: To be refactored!
	if(this.node.tagName == "TEXTAREA")
	{
		this.node.value = text;
	}
	else if (this.node.tagName == 'INPUT')
	{
		// GOOGLE FIX
		var inputs = this.node.parentNode.getElementsByTagName("INPUT");
		var n = inputs.length;
		var zIndex = window.getComputedStyle(this.node, null).getPropertyValue('z-index');
		for(var i = 0; i < n && n > 1; i++)
		{
			if(inputs[i] !== this.node && inputs[i].type == this.node.type)
			{
				if(zIndex < window.getComputedStyle(inputs[i], null).getPropertyValue('z-index'))
				{
					inputs[i].value = text;
					ConsolePrint("Set text input on another input field with higher z-index");
				}
			}
		}
		this.node.value = text;
	}
	else
	{
		this.node.textContent = text;
		ConsolePrint("Set input's value to given text");
	}
	
	this.setText(text);

	if(!submit)
		return {'command': "Success"};

	// Used for Enter-Button emulation, when submitting text
	if(this.rects.length > 0)
	{
		var rect = this.getRects()[0];
		var response = {  
					'command' : "EmulateEnterKey",
					'x': rect[1] + (rect[3]-rect[1])/2, 
					'y': rect[0] + (rect[2]-rect[0])/2
		};
		ConsolePrint("Returning rect's center: "+response.x+", "+response.y);
		return response;
	}
	else
		return {
			'command' : "Error",
			'msg'	: "Input's rect w: 0, h: 0, can't perfom Enter emulation!"
		}
		;
	}

// DEPRECATED
function PerformTextInput(id, text, submit)
{
	console.log("PerfomTextInput: "+[id, text, submit]);
    var domObj = GetDOMTextInput(id);
    if(domObj === undefined)
	{
		ConsolePrint("ERROR: Couldn't input text to DOMTextInput with id "+id+".");
		return false;
	}
    
	return domObj.setTextInput(text, submit);
}


DOMOverflowElement.prototype.scroll = function(gazeX, gazeY, fixedIds){
	// TODO: Refactore this method?
	if(fixedIds.length > 0 && fixedIds.indexOf(this.getFixedId()) === -1)
		return;

	// Do not use cut-off rect and keep scrolling velocity untouched if partially hidden
	var rects = AdjustClientRects(this.node.getClientRects());
	//  var rect = this.getRects()[0];

	// Update scrolling position according to current gaze coordinates
	// Idea: Only scroll if gaze is somewhere near the overflow elements edges
	if(rects.length > 0)
	{
		var rect = rects[0];

		var centerX = rect[1] + Math.round(rect.width / 2);
		var centerY =  rect[0] + Math.round(rect.height / 2);

		var distLeft = gazeX - rect[1];   // negative values imply gaze outside of element
		var distRight = rect[3] - gazeX;
		var distTop = gazeY - rect[0];
		var distBottom = rect[2] - gazeY;

		// Treshold for actual scrolling taking place, maximum distance to border where scrolling takes place
		var tresholdX = 1 / 2.5 * ((rect[3]-rect[1]) / 2);
		var tresholdY = 1 / 2.5 * ((rect[2]-rect[0]) / 2);

		var maxScrollingPerFrame = 10;
		// Actual scrolling, added afterwards
		var scrollX = 0;
		var scrollY = 0;

		if(distLeft >= 0 && distLeft < tresholdX)
		{
			scrollX -= (maxScrollingPerFrame * ( 1 - (distLeft / tresholdX) ));
		}
		if(distRight >= 0 && distRight < tresholdX)
		{
			scrollX += (maxScrollingPerFrame * ( 1 - (distRight / tresholdX) ));
		}
		if(distTop >= 0 && distTop < tresholdY)
		{
			scrollY -= (maxScrollingPerFrame * (1 - (distTop / tresholdY)));
		}
		if(distBottom >= 0 && distBottom < tresholdY)
		{
			scrollY += (maxScrollingPerFrame * (1 - (distBottom / tresholdY)));
		}

		// Execute scrolling
		this.node.scrollLeft += scrollX;
		this.node.scrollTop += scrollY;

		// Return current scrolling position as feedback
		return [this.node.scrollLeft, this.node.scrollTop];
	}
	
}

// DEPRECATED
function ScrollOverflowElement(elemId, gazeX, gazeY, fixedIds)
{
    var overflowObj = GetDOMOverflowElement(elemId);
	if(overflowObj === undefined)
		return;
	
	// TODO: Move scrolling computation to C++ Tab::ScrollOverflowElement
	// Add solution for scrolling if edge of overflow is covered by a fixed element and scroll at the edge of fixed element
	if(fixedIds !== null && fixedIds !== undefined && fixedIds.length > 0)
	{
		var childFixedId = overflowObj.getFixedId();
		
		if(fixedIds.indexOf(childFixedId) === -1)  // child id not contained in list
		{
			ConsolePrint("Skipping scrolling!");
			// Skip scrolling, because overflow is hidden by fixed element with "fixedId"
			return;
		}
	}
	overflowObj.scroll(gazeX,gazeY);
}

ConsolePrint("Successfully imported dom_nodes_interaction.js!");