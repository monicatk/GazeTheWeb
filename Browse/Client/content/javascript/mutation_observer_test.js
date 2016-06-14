// https://developer.mozilla.org/de/docs/Web/API/MutationObserver

// zu überwachende Zielnode (target) auswählen
var target = document.documentElement;
 
// eine Instanz des Observers erzeugen
var observer = new MutationObserver(
	function(mutations) {
	  	mutations.forEach(
	  		function(mutation)
	  		{
	  			if(mutation.type == 'childList')
	  			{

		  			var nodes = mutation.addedNodes;
		  			for(i=0; i < nodes.length; i++)
		  			{
		  				// https://developer.mozilla.org/de/docs/Web/API/Node
		  				// https://developer.mozilla.org/de/docs/Web/API/Element
		  				// GOAL: map Node to Element?
		  				// var target = nodes[i].target;

		  				nodes[i].nodeValue = nodes[i].id;

		  			}
	  			}
	  			else if(mutation.type == 'attributes')
	  			{
	  				// TODO: Wie heißt das geaenderte Attribut?
	  				
	  				alert('attributName: '+mutation.attributeName+', attributeNamespace: '+mutation.attributeNamespace+'');
	  			}
	  			else if(mutation.type == 'characterData')
	  			{
	  				alert('vorher: '+mutation.target.oldValue+', nachher: '+mutation.target.nodeValue);
	  			}
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