var css = document.createElement('style');
css.type = 'text/css';
css.innerHTML = 'body::-webkit-scrollbar { width:0px !important; }';
if(document.getElementsByTagName('head').length > 0)
{
    var head = document.getElementsByTagName('head')[0];
    if(head && head !== undefined)
    {
        head.appendChild(css);
    }
}
