var iYQIndx = 1;
var oYQObject;
var sYQBGColor = "";
var oYQTimeout;
var boolYQerr=false;
var boolYQreq=false;
var a = navigator.userAgent.toLowerCase();
var iaskUrl ='http://iask.com/innews/r';
if (typeof(HTMLElement)!="undefined") {
      HTMLElement.prototype.__defineGetter__("currentStyle", function() { return document.defaultView.getComputedStyle(this, null); });
}
function activateYQinl(poEl)
{
	
	if (a.indexOf('msie') =='-1' || a.indexOf("opera") !=-1 )   
        {
		var oF;
        	oF = poEl.parentNode.getElementsByTagName("form").item(0)
                var arg = getArg(oF);
                var U ='http://iask.com/n' + arg;
                document.window.open(U); 
                return true;
        }
	if(activateYQ(poEl)){
		poEl.parentNode.getElementsByTagName("form").item(0).submit();
	};		
}
function activateYQ(poEl) 
{
	if(boolYQreq) return false;
	if (!document.getElementById) return true;
	var oF;
	oF = poEl.parentNode.getElementsByTagName("form").item(0)
	yqRemoveOpenResults();
	
	yqSetZIndex(oF);
	yqFetchResults(oF);
	oP = oF.parentNode;
	while (oP && (oP.className!="yqcontext" && oP.className!="yqlink")  && oP.tagName.toUpperCase()!="BODY") {
		oP = oP.parentNode;
	}
	if (oP && (oP.className=='yqcontext' || oP.className=='yqlink')) {
		if(oP.currentStyle){ 
			if( oP.currentStyle.backgroundColor){
				sYQBGColor = oP.currentStyle.backgroundColor;
			}
		}
	}
	iYQIndx++;
	return false;
}
function yqFetchResults(poF) 
{
	var oResult		= document.createElement('div');
	oResult.id		= 'yschyqresult';
	oResult.className	= 'yschyqresult';
	oResult.className +=' yschyqresultinl';
	poF.appendChild(oResult);	
	var oResultIn		= document.createElement('div');
	oResultIn.id		= 'yschyqresultin';
	oResultIn.className	= 'yschyqresultin';
	modifyEventListener("add","onmouseup",oResult,yqStopEventBubble);
	var sT			= '<h3 id=yschyqresulthead class=drag>爱问(iAsk)</strong>搜索 </h3>\n';
	var sCURL = 'http://image2.sina.com.cn/cha/news/closewin.gif';	
	var sCBtn		= '<div id=yschyqresultclose><a href=http://iask.com/help/help_index.html target=_blank class=help>帮助？</a> <a href="" onclick="yqRemoveOpenResults(); boolYQreq=false;  return false" class=but><img src="'+sCURL+'" height=14 width=14 border=0></a></div>\n';
	var sL		= '<div id=yschyqloading class=yschyqloading><p>正在搜索中 . . .</p></div>\n';
	var sIFrame		= '<iframe id=yschyqif class=yschyqif name=yschyqif scrolling="no" frameBorder=0 allowtransparency="true"></iframe>\n';
	var sRs		= '<div id=yschyqresults class=yschyqresults></div>\n';
	var sFeedback		= '<div id=yschyqfeedback class=yschyqfeedback><p><a href="http://iask.com">联系我们</a></p></div>\n';	
	if(a.indexOf('safari')=='-1'){
		oResultIn.innerHTML	= sT + sCBtn + sL + sRs + sFeedback;
		oResult.appendChild(oResultIn);
		var oJSResults = document.createElement('script');
		oJSResults.setAttribute("language","JavaScript");
		oJSResults.setAttribute("src", yqAssembleQuery(poF));
		if (boolYQreq) return false;
		document.getElementsByTagName("head").item(0).appendChild(oJSResults);
		boolYQreq = !boolYQreq;
	} 
	else { 
		document.domain='iask.com';
		oResultIn.innerHTML=sT + sCBtn + sL + sIFrame + sFeedback;
		oResult.appendChild(oResultIn);		
		var oIFrame= document.getElementById('yschyqif');
		poF.target=oIFrame.id;
		poF.action=iaskUrl;
		poF.method="post";
		poF.submit();
	}
	var oTitle = document.getElementById('yschyqresulthead');
	modifyEventListener("add","onmousedown",oTitle,yqDoOnMouseDown);
	modifyEventListener("add","onmouseup",oTitle,yqDoOnMouseUp);	
	modifyEventListener("add","onselectstart",document,returnFalse);
	oYQObject = oIFrame;
	oYQTimeout = setTimeout('yqCheckForResults()',20000);
	return false;
}

function modifyEventListener(psAction, psEventName, poElement, poFunction){
	var isAdd = ((psAction == "add")||(psAction == "attach"))?true:false;
	psEventName = psEventName.toLowerCase();
	if (psEventName.substring(0,2) != "on") {
		psEventName = "on"+psEventName;
	}
	if ( document.getElementById ) {
		if ( poElement.captureEvents ) {
			var sFunc = (isAdd)?"captureEvents":"releaseEvents";
			Eval("poElement."+sFunc+"(Event."+psEventName.substr(2).toUpperCase()+");");
		}
		var sFunc = (isAdd)?(poFunction.toString().split(' ')[1].split('(')[0]):"null";
		eval("poElement."+psEventName+" = "+sFunc+";");
		return true;
		} else if ( poElement.attachEvent ) { 
	if (isAdd) {
		poElement.attachEvent(psEventName,poFunction);
	} else {
		poElement.detachEvent(psEventName,poFunction);
	}
	return true;
	} else if ( poElement.addEventListener ) { 
	if (isAdd) {
		poElement.addEventListener(psEventName.substr(2),poFunction,false);
	} else {
		poElement.removeEventListener(psEventName.substr(2),poFunction,false);
	}
	return true;
	}
	return false;
}

function yqRemoveOpenResults() 
{
	var oResults = document.getElementById('yschyqresult');
	if (oResults) {
		var oOpenForm = oResults.parentNode;
		var oOpenContext = oOpenForm.parentNode;
		oOpenForm.removeChild(oResults);
		if (sYQBGColor!="") {
			oOpenContext.style.backgroundColor = sYQBGColor;
		}
		window.clearTimeout(oYQTimeout);
		
	}
	modifyEventListener("remove","onselectstart",document,returnFalse);
}
function yqCheckForResults() 
{
	var oLoading = document.getElementById('yschyqloading');
	if (oLoading) {
		boolYQerr=true;
		boolYQreq = !boolYQreq;
		if (oYQObject)
			oYQObject.parentNode.removeChild(oYQObject);
		oLoading.innerHTML = '<p class=error>连接超时, 请<a href="#" onclick="yqRemoveOpenResults(); boolYQreq=false; return false;" >关闭窗口</a>在试一次.</p>';
	}
}	
function yqSetZIndex(poF) 
{
	var aYQForms;	
	var i=0;		
	aYQForms = document.getElementsByTagName('form');
	for (i=0; i < aYQForms.length; i++) {
		if (aYQForms[i].className=="yq") {
			aYQForms[i].style.zIndex = 999 + iYQIndx;
		}
	}
	poF.style.zIndex = 1000 + iYQIndx;
}
function yqAssembleQuery(poF) 
{
	boolYQerr=false;
	var aInputs = poF.getElementsByTagName('input');
	var sArgs='?';
	for (var i=0; i<aInputs.length; i++) {
		sArgs += aInputs[i].name +'='+ aInputs[i].value;			
		if (i!=aInputs.length-1) sArgs += '&';
	}
	return iaskUrl +sArgs;
}
function yqStopEventBubble(evt){
	var evt = (evt) ? evt : ((window.event) ? window.event : "");
	evt.cancelBubble=true;
}
var draggedElem=null; 
var x,y; 
function spDoOnMouseMove(evt){
	evt = getEventObject(evt);
	if ( evt && draggedElem ) {
		draggedElem.style.marginLeft=0 + "px";
		draggedElem.style.marginTop=0 + "px";
		draggedElem.style.left=(temp1+evt.clientX-x)+"px"; 
		draggedElem.style.top=(temp2+evt.clientY-y)+"px";
		return false;
	}
}
function yqDoOnMouseDown(evt){
	evt = getEventObject(evt);
	var elem = getEventObjectsElement(evt);
	elem = getEventObjectsElement(evt);
	while ((elem.className!="yschyqresult" && elem.className!="yschyqresult yschyqresultinl") && (elem.className.indexOf("yq")!="-1" || elem.className.indexOf("drag")!="-1") ) {
		elem = elem.parentNode;
	}
	if ( elem && (elem.className=="yschyqresult" || elem.className=="yschyqresult yschyqresultinl")  && evt.button!=2 ) {
		draggedElem=elem; 	
		temp1=(elem.offsetLeft); 
		temp2=(elem.offsetTop);
		x=evt.clientX; 
		y=evt.clientY;
		modifyEventListener("add","onmousemove",document,spDoOnMouseMove);
	}
}
function yqDoOnMouseUp(){
	if ( draggedElem ) {
		draggedElem=null;
		modifyEventListener("remove","onmousemove",document,spDoOnMouseMove);
	}
}
function returnFalse() {
	return false;
}	
function getEventObject(evt) {
    return (evt) ? evt : ((window.event) ? event : null);
}
function getEventObjectsElement (evt){
    evt = (evt) ? evt : ((window.event) ? event : null);
    if ( evt ) {
        var elem = (evt.target) ? evt.target : ((evt.srcElement) ? evt.srcElement:null);
        while ( elem.nodeType == 3 ) {
            elem = elem.parentNode;
        }
        return elem;
    }
    return null;
}
var b="<link rel=STYLESHEET type=text/css href=http://image2.sina.com.cn/cha/news/newstheme.css ><style>#yschyqresult {white-space:normal !important; }\n.yqin {display:inline; height:0px; width:0px; position:absolute; text-align:left;}\n.yqlink {display:inline-block;}</style>\n";
document.write(b);