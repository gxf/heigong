// Netratings SiteCensus v52.js
// COPYRIGHT 2006 Nielsen//Netratings

function random()
{
    random.seed = (random.seed*random.a + random.c) % random.m;
    return random.seed / random.m;
}
random.m=714025;
random.a=4096;
random.c=150889;
random.seed = (new Date()).getTime()%random.m;


function _rsEH(){}
window.onerror=_rsEH;

var _rsLP=location.protocol.indexOf('https')>-1?'https:':'http:';
var _rsND=_rsLP+_rsDN;

var _rsRD=(new Date()).getTime();

if (typeof(_rsCC)=="undefined") {var _rsCC=1;}     // cookie check
if (typeof(_rsDT)=="undefined") {var _rsDT=0;}     // doc.title
if (typeof(_rsSE)=="undefined") {var _rsSE=0;}     // surveys enabled
if (typeof(_rsSV)=="undefined") {var _rsSV="";}    // survey id
if (typeof(_rsSM)=="undefined") {var _rsSM=0;}     // sample rate
if (typeof(_rsSS)=="undefined") {var _rsSS=1;}     // sample size
if (typeof(_rsUT)=="undefined") {var _rsUT=0;}     // use custom tags
if (typeof(_rsMP)=="undefined") {var _rsMP=1;}     // measure page
if (typeof(_rsIP)=="undefined") {var _rsIP=0;}     // use invisible pixel
if (typeof(_rsTC)=="undefined") {var _rsTC=500;}   // click timeout (_rsClick)
if (typeof(_rsSI)=="undefined") {var _rsSI=escape(window.location);}
if (typeof(_rsC0)=="undefined") {var _rsC0;}
if (typeof(_rsC1)=="undefined") {var _rsC1;}
if (typeof(_rsC2)=="undefined") {var _rsC2;}
if (typeof(_rsC3)=="undefined") {var _rsC3;}
if (typeof(_rsC4)=="undefined") {var _rsC4;}
if (typeof(_rsC5)=="undefined") {var _rsC5;}
if (typeof(_rsC6)=="undefined") {var _rsC6;}
if (typeof(_rsC7)=="undefined") {var _rsC7;}
if (typeof(_rsC8)=="undefined") {var _rsC8;}
if (typeof(_rsC9)=="undefined") {var _rsC9;}

var _rsClickDst;
function _rsClick(_clickDest)
{
	var _pixelSrc;
	var _pixel;
	var _eClickDest = escape(_clickDest);
	_rsClickDst = _clickDest;

	_pixelSrc  = _rsND+'cgi-bin/m?rnd='+(new Date()).getTime();
	_pixelSrc  = _pixelSrc+'&ci='+_rsCI;
	_pixelSrc  = _pixelSrc+'&cg='+escape(_rsCG);
	_pixelSrc  = _pixelSrc+'&cc='+_rsCC;
	_pixelSrc  = _pixelSrc+'&si='+_rsCI+'-ctgw-'+_eClickDest;
	_pixelSrc  = _pixelSrc+'&rp='+escape(window.location);
	_pixel     = new Image(1,1);
	_pixel.src = _pixelSrc;

	setTimeout("window.location = _rsClickDst", _rsTC);
}

function rsCi()
{
	var _rsUA=navigator.appName+" "+navigator.appVersion;
	var _rsRUA=navigator.userAgent;
	var _rsWS=window.screen;
	var _rsBV=navigator.appVersion.substring(0, 1);
	var _rsNN=(_rsUA.indexOf('Netscape'));
	var _rsMC=(_rsUA.indexOf('Mac'));
	var _rsIE=(_rsUA.indexOf('MSIE'));
	var _rsOP=(_rsRUA.indexOf('Opera'));
	var _rsIEV=(parseInt(_rsUA.substr(_rsIE+5)));
	var _rsRP=escape(document.referrer);
	var _rsSR;
	var _rsCD;
	var _rsLG;
	var _rsJE;
	var _rsCK;
	var _rsTZ;
	var _rsCT;
	var _rsHP;
	var _rsTL;
	var _rsSW;
	var _rsSH;

	if (_rsMP==0) {return;}

        _rsJE=(navigator.javaEnabled()==true)?"y":"n";
        if (_rsDT==1) {
		_rsTL=escape(document.title);
	}
	if((_rsIE>0)||((_rsNN!=-1)&&(_rsBV >=5))) {
		_rsCK=(navigator.cookieEnabled==true)?"y":"n";
	}
	if((_rsIE>=0)&&(_rsIEV>=5)&&(_rsMC==-1)&&(_rsOP==-1)) {
		document.body.addBehavior("#default#clientCaps");
		_rsCT=document.body.connectionType;
		document.body.addBehavior("#default#homePage");
		_rsHP=(document.body.isHomePage(location.href))?"y":"n";
	}
	var _rsD = new Date();
	_rsTZ = _rsD.getTimezoneOffset()/-60;
	if((typeof(_rsWS)!="undefined")&&(_rsWS!=null)) {
		_rsSW=_rsWS.width;
		_rsSH=_rsWS.height;
		_rsCD=_rsWS.colorDepth;
		_rsSR=_rsSW+'x'+_rsSH;
		if((_rsNN!=-1)&&(_rsBV >=4)) {
			_rsCD=_rsWS.pixelDepth;
		}
	}
        if((_rsNN!=-1)&&(_rsBV >=4)||(_rsOP>=0)) {
		_rsLG=navigator.language;
	}
	if((_rsIE!=-1)&&(_rsBV >=4)&&(_rsOP==-1)) {
		_rsLG=navigator.userLanguage;
	}

	
        var _rsPR="";
	_rsPR='<img src="';
	_rsPR=_rsPR+_rsND+'cgi-bin/m?rnd='+(new Date()).getTime();
	_rsPR=_rsPR+'&ci='+_rsCI;
	_rsPR=_rsPR+'&cg='+escape(_rsCG);
	_rsPR=_rsPR+'&cc='+_rsCC;
	if (_rsSR!=null) {_rsPR=_rsPR+'&sr='+_rsSR;}
	if (_rsCD!=null) {_rsPR=_rsPR+'&cd='+_rsCD;}
	if (_rsLG!=null) {_rsPR=_rsPR+'&lg='+_rsLG;}
	if (_rsJE!=null) {_rsPR=_rsPR+'&je='+_rsJE;}
	if (_rsCK!=null) {_rsPR=_rsPR+'&ck='+_rsCK;}
	if (_rsTZ!=null) {_rsPR=_rsPR+'&tz='+_rsTZ;}
	if (_rsCT!=null) {_rsPR=_rsPR+'&ct='+_rsCT;}
	if (_rsHP!=null) {_rsPR=_rsPR+'&hp='+_rsHP;}
	if (_rsTL!=null) {_rsPR=_rsPR+'&tl='+_rsTL;}
	if (_rsUT==1) {
	   if (_rsC0!=null) {_rsPR=_rsPR+'&c0='+escape(_rsC0);}
	   if (_rsC1!=null) {_rsPR=_rsPR+'&c1='+escape(_rsC1);}
	   if (_rsC2!=null) {_rsPR=_rsPR+'&c2='+escape(_rsC2);}
	   if (_rsC3!=null) {_rsPR=_rsPR+'&c3='+escape(_rsC3);}
	   if (_rsC4!=null) {_rsPR=_rsPR+'&c4='+escape(_rsC4);}
	   if (_rsC5!=null) {_rsPR=_rsPR+'&c5='+escape(_rsC5);}
	   if (_rsC6!=null) {_rsPR=_rsPR+'&c6='+escape(_rsC6);}
	   if (_rsC7!=null) {_rsPR=_rsPR+'&c7='+escape(_rsC7);}
	   if (_rsC8!=null) {_rsPR=_rsPR+'&c8='+escape(_rsC8);}
	   if (_rsC9!=null) {_rsPR=_rsPR+'&c9='+escape(_rsC9);}
	}
	_rsPR=_rsPR+'&si='+_rsSI;
	_rsPR=_rsPR+'&rp='+_rsRP;
	if (_rsIP==1) {_rsPR=_rsPR+'" style="visibility:hidden;position:absolute;left:0px;top:0px;z-index:-1';}
	_rsPR=_rsPR+'" width="1" height="1" alt=""/>';

	document.write(_rsPR);
}

if((_rsSE)&&(random() <= _rsSM)) {
        var _rsIM='<scr'+'ipt language="JavaScript" type="text/javascript" src="'+_rsND+'cgi-bin/j?ci='+_rsCI+'&ss='+_rsSS+'&cc='+_rsCC+'&rd='+_rsRD+'&se='+_rsSE+'&sv='+_rsSV+'"><\/scr'+'ipt>';
    document.write(_rsIM);
}
else {
    rsCi();
}

