/*
RotatorAD V3.7 2009-5-14
Author: Dakular <shuhu@staff.sina.com.cn>
格式: new RotatorAD(商业广告数组, 非商业广告数组, 层id)
说明: 第一次访问随机出现，以后访问顺序轮播；自动过滤过期广告；cookie时间24小时；商业广告数量不足时，从非商业广告中补充
	  限制最少轮播数量，广告少于限制数时，才从垫底里补充，否则不补垫底
*/
if(typeof(RotatorAD)!='function'){
var RotatorAD = function(rad,nad,div_id){

var date = new Date();
var id = 0;
var max = 99;
var url = document.location.href;
var cookiename = 'SinaRot'+escape(url.substr(url.indexOf('/',7),2)+url.substring(url.lastIndexOf('/')));
var timeout = 1440; //24h
var w = rad.width;
var h = rad.height;
var num = rad.num;
var num2 = rad.num2;
var ary = new Array();
//过滤无效商广
for(var i=0; i<rad.length; i++){
	var start = strToDate(rad[i][2].replace('<startdate>','').replace('</startdate>',''));
	var end = strToDate(rad[i][3].replace('<enddate>','').replace('</enddate>',''),true);
	if(date>start && date<end){
		ary.push([rad[i][0], rad[i][1], rad[i][4], rad[i][5]?rad[i][5]:'0']);
	}
}
//过滤无效垫底
var vnad = new Array();
for(var i=0; i<nad.length; i++){
	if(nad[i][2]==null){
		vnad.push([nad[i][0], nad[i][1], '', '0']);
	}else{
		var start = strToDate(nad[i][2].replace('<startdate>','').replace('</startdate>',''));
		var end = strToDate(nad[i][3].replace('<enddate>','').replace('</enddate>',''),true);
		if(date>start && date<end){
			vnad.push([nad[i][0], nad[i][1], '', '0']);
		}
	}
}
//补位
var nn = 0;
if(vnad.length>0 && (num2==null || ary.length<num2)){
	for(var i=0; i<(num2==null?rad.num:num2); i++){
		if(i>ary.length-1){
			ary.push([vnad[nn][0], vnad[nn][1], '', '0']);
			if(++nn > nad.length-1) nn = 0;
		}
	}
}
num = ary.length<num?ary.length:num;
//排序(同步有序号的广告)
ary.sort(function(x,y){return x[3]-y[3];});
//取id
if(typeof(globalRotatorId)=='undefined' || globalRotatorId==null || isNaN(globalRotatorId)){
	curId = G(cookiename);
	curId = curId==''?Math.floor(Math.random()*max):++curId;
	if(curId>max || curId==null || isNaN(curId)) curId=0;
	S(cookiename,curId,timeout);
	globalRotatorId = curId;
}
id=globalRotatorId%num+1;
//Show AD
if(id==0 || ary.length==0) return; //如果没有广告则不显示
if(id==-1) id=1; //当只有一个广告时：始终显示第一个
var n = id-1;
var type = ary[n][0].substring(ary[n][0].length-3).toLowerCase();
var od = document.getElementById(div_id);
if(type=='swf'){
	var of = new sinaFlash(ary[n][0], div_id+'_swf', w, h, "7", "", false, "High");
	of.addParam("wmode", "opaque");
	of.addParam("allowScriptAccess", "always");
	of.addVariable("adlink", escape(ary[n][1]));
	of.write(div_id);
	document.write('<iframe src="" style="display:none"></iframe>'); //for FF bug
}else if(type=='jpg' || type=='gif'){
	od.innerHTML = '<a href="'+ary[n][1]+'" target="_blank"><img src="'+ary[n][0]+'" border="0" width="'+w+'" height="'+h+'" /></a>';
	document.write('<iframe src="" style="display:none"></iframe>'); //for FF bug
}else if(type=='htm' || type=='tml'){
	od.innerHTML = '<iframe id="ifm_'+div_id+'" frameborder="0" scrolling="no" width="'+w+'" height="'+h+'"></iframe>';
	document.getElementById('ifm_'+div_id).src = ary[n][0];
}else if(type=='.js'){ //js
	document.write('<script language="javascript" type="text/javascript" src="'+ary[n][0]+'"></scr'+'ipt>');
	document.write('<iframe src="" style="display:none"></iframe>'); //for FF bug
}else{ //textlink
	document.write('<a href="'+ary[n][1]+'"  target="_blank">'+ary[n][0]+'</a>');
}
if(ary[n][2]!="" && ary[n][2]!=null){ //ad tracker
	var oImg = new Image();
	oImg.src = ary[n][2];
}
function G(N){
	var c=document.cookie.split("; ");
	for(var i=0;i<c.length;i++){
		var d=c[i].split("=");
		if(d[0]==N)return unescape(d[1]);
	}return '';
};
function S(N,V,Q){
	var L=new Date();
	var z=new Date(L.getTime()+Q*60000);
	document.cookie=N+"="+escape(V)+";path=/;expires="+z.toGMTString()+";";
};
function strToDate(str,ext){
	var arys = new Array();
	arys = str.split('-');
	var newDate = new Date(arys[0],arys[1]-1,arys[2],9,0,0);
	if(ext){
		newDate = new Date(newDate.getTime()+1000*60*60*24);
	}
	return newDate;
}

}
}