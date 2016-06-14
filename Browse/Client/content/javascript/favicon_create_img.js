var favIconImg = new Image();
var canvas = document.createElement('canvas');
var ctx = canvas.getContext('2d');
favIconImg.onload = function(){
	//alert('favIconImg.onload()');
	canvas.width = favIconImg.width;
	canvas.height = favIconImg.height+1;	// see experimental above
	ctx.drawImage(favIconImg, 0, 0);

	//alert('Setting V8 favicon res to ('+favIconImg.height+','+favIconImg.width+')');
	window.favIconHeight = favIconImg.height;
	window.favIconWidth = favIconImg.width;

	// experimental: draw 1 row in RGBA to force encoding, don't read this line afterwards
	ctx.fillStyle = 'rgba(255, 0, 0, 255)';
	ctx.fillRect(0, favIconImg.height, favIconImg.width, favIconImg.height+1);
	//alert('DEBUG: Drew 1px line on canvas bottom');
	// alert('Updated favIconImg!');
};