
//Sina iAsk Shunqing

//对象引用
function iask_keywords_getElement(id){return document.getElementById(id)};

//发送请求
function iask_keywords_send(mod, value){
	var sender = new Image();
	sender.src = 'http://js.iask.com/' + mod + '?' + value;
/*	sender.onload = function(){clear(this);};
	sender.onerror = function(){clear(this);};
	sender.onabort = function(){clear(this);};
	function clear(obj){
		obj.onerror = null;
		obj.onload = null;
		obj.onabort = null;
		obj = null;
	}*/
}

//获取参数
function iask_keywords_getParameter(key){
	var result = null;
	var rs = new RegExp(key + "=([^&]+)","g").exec(self.location.toString());
	if(rs)
		result = rs[1];
	rs = null;
	return result;
}

//设置对象是否显示
function iask_keywords_setDisplay(obj, bool){
	if(bool)
		obj.style.display = '';
	else
		obj.style.display = 'none';
}

//字符长度
function iask_keywords_strLength(str){
	return str.replace(/[^\x00-\xff]/g, "__").length;
}

//初始化
function iask_keywords_ADInit(ADdata){

	var iaskkeyword = iask_keywords_getElement(iask_keywords_lid);

	var show = false;

	if(iaskkeyword && ADdata.length > 0){
		var output = '';

		//根据长度确定显示个数
		var keynum = 0;
		var tmpword = '';
		for(var i = 0; i < ADdata.length; i ++, keynum ++){
			tmpword += ADdata[i].key + '　';
			if(iask_keywords_strLength(tmpword) > iask_keywords_len)
				break;
		}

		//最小长度限制
		show = iask_keywords_strLength(tmpword) > iask_keywords_min;
		if(show){

			for(var i = 0; i < keynum; i ++){
				if(i > 0)
					output += '　';
				output += '<a href="http://www.google.cn/search?lr=&client=aff-sina&ie=utf8&oe=utf8&hl=zh-CN&channel=contentrelatedsearch&q='
				output += encodeURIComponent(ADdata[i].key);
				output += '" target="_blank" onclick=iask_keywords_send("iAskeyword.png","key=' + ADdata[i].key + '&idx=' + i + '")>';
				output += ADdata[i].key;
				output += '</a>';
			}

			iaskkeyword.innerHTML = output;
			iask_keywords_setDisplay(iask_keywords_getElement(iask_keywords_fid), true);

		}

	}

	iask_keywords_send("iAskeyword.png","show=" + show + "&wnum=" + ADdata.length);
}

function getcook(url){
	var cook = getCookie("SU");
	if(cook == ""){
		//iask_keywords_send("iAskUserLog.png","SU=null&url=" + url);
		//alert("null");
	}
	else{
		iask_keywords_send("iAskUserLog.png","SU=" + cook + "&url=" + url);
		//alert(cook);
	}
}

function setcook(url){
	var s = url.lastIndexOf("/") + 1;
	var e = url.lastIndexOf("\.");
	var urlid = url.substring(s,e);
	//alert(urlid);
	var i = document.cookie.indexOf("SIC=");
	if(i <= 0){
		alert("no SIC");
		document.cookie = "SIC=" + urlid;
		alert(document.cookie);
	}
	else{
		alert(i);
		var cook = getCookie("SIC");
		if(cook == ""){
			document.cookie = "SIC=" + urlid;
			//setCookie("SIC",urlid);
		}
		else{
			document.cookie = "SIC=" + cook + urlid;
			//setCookie("SIC",cook + urlid);
		}
	}
}

var iask_keywords_url = window.location.href;
var iask_keywords_server = "http://keyword.sina.com.cn/iaskkeywords.php?url=" + iask_keywords_url + "&bid=" + iask_keywords_bid + "&sign=xx&sid=1&dpc=1";
//var iask_keywords_server = "http://keyword.sina.com.cn/match/newskeys.php?url=" + iask_keywords_url;

getcook(iask_keywords_url);
//setcook(iask_keywords_url);
//var ADScript = document.createElement('script');
//ADScript.setAttribute("type", "text/javascript");
//ADScript.setAttribute("src", iask_keywords_server);
//document.body.appendChild(ADScript);
document.write("<script type='text/javascript' src='" + iask_keywords_server + "'></script>");

