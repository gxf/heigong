// comment-sounter应用接口
// settings
var ON = true; // 是否启用本功能
var Q = "http://counter.sina.com.cn/querylist?format=js&entry=g_clist&id=0&key="; // Counter接口
var U = "http://comment4.news.sina.com.cn/comment/comment4.html?channel=_CHANNEL_&newsid=_NEWSID_&style=_STYLE_"; // 留言板URL
var V = "<A HREF='_URL_' target=_blank>_COUNT_</A>"; // 留言板入口
// depend
var undefined; // for IE5
var g_clist = new Array();
function get_elements_by_tagname( tag, name ) {
     var elem = document.getElementsByTagName( tag );
     var arr = new Array();
     for( i=0,iarr=0; i<elem.length; ++i ) {
          var att = elem[i].getAttribute( "name" );
          if( att == name ) {
               arr[iarr] = elem[i];
               ++iarr;
          }
     }
     return arr;
}
function valid( str ) {
	if ( str==null || str=="" ) {
		return false;
	} else {
		return true;
	}
}
function replace( origin, from, to ) {
	var re = new RegExp( from, "g" );
	return ( origin.replace(re,to) );
}
function uri_encode( str ) {
	if ( !valid(str) ) {
		return "";
	}
	var toescape = ";/?:@&=+ \"#%<>'`[],~!$^(){}|\\";
	var newstr="", chr="";
	for ( var i=0; i<str.length; i++ ) {
		chr = str.charAt(i);
		if ( toescape.indexOf(chr) == -1 ) {
			newstr += chr;
		} else {
			newstr += escape( chr );
		}
	}
	return newstr;
}
function write_back( clist, nlist ) {
	for ( var c=0; c<clist.length; ++c ) {
		var KEY = clist[c][0];
		var COUNT = parseInt( clist[c][1] );
		for ( var n=0; n<nlist.length; ++n ) {
			var NODE = nlist[n];
			// id
			var id = NODE.getAttribute( "cmnt_id" );
			if (  id != KEY ) {
				continue;
			}
			// limit
			var limit = NODE.getAttribute( "show_limit" );
			if ( valid(limit) ) {
				var border = limit.split(",");
				if ( (border[0]!=undefined && border[0]!="0" && COUNT<parseInt(border[0])) ||
					 (border[1]!=undefined && border[1]!="0" && COUNT>parseInt(border[1])) ) {
					continue;
				}
			}	
			// url
			var params = id.split(":");
			var url = replace( U, "_CHANNEL_", params[0] );
			url = replace( url, "_NEWSID_", params[1] );
			url = replace( url, "_STYLE_", params[2] );
			// count string 
			var count_string = COUNT;
			if ( NODE.getAttribute("cmnt_url") == "true" ) {
				count_string = replace( V, "_URL_", url );
				count_string = replace( count_string, "_COUNT_", COUNT );
			}
			// write
			if ( typeof(cmnt_count_renderer) != "undefined" ) {
				NODE.innerHTML = cmnt_count_renderer( COUNT, url );
			} else {
				NODE.innerHTML = replace( NODE.innerHTML, "_COUNT_", count_string );
			}
			NODE.style.display = "";
			// refresh
			var refresh = NODE.getAttribute( "auto_refresh" );
			// ...
		}
	}
}
function _load( url ) {
	var js_doc = document.createElement( "script" ); 
	js_doc.setAttribute( "type", "text/javascript" );
	js_doc.setAttribute( "charset", "GBK" );
	var head = document.getElementsByTagName("head")[0];
	if ( head == undefined ) {
		return;
	}
	head.insertBefore( js_doc, null );
	js_doc.src = url;
}
function load_cmntcount() {
	// make query string
	var keys="", query;
	for ( var i=0; i<g_nlist.length; ++i ) {
		keys += ( g_nlist[i].getAttribute("cmnt_id") + "," );
	}
	query = Q + keys;
	// update pv, ONLY PROCESS THE FIRST cmnt_count NODE
	var c1 = g_nlist[0];
	var pv_id = c1.getAttribute("p_id");
	var pv_key = c1.getAttribute("p_key");
	var pv_url = c1.getAttribute("p_url");
	if ( valid(pv_id) && valid(pv_key) && valid(pv_url) ) {
		query += ( "&p_id=" + uri_encode(pv_id) );
		query += ( "&p_key=" + uri_encode(pv_key) );
		query += ( "&p_url=" + uri_encode(pv_url) );
		query += ( "&p_expara=" + uri_encode(c1.getAttribute("p_expara")) );
	}
	// read count
	_load( query );
}
// callback
function counter_callback() {
	write_back( g_clist, g_nlist );
}
// start
var g_nlist = new Array();
function reload_cmntcount() {
	g_nlist = get_elements_by_tagname( "span", "cmnt_count" );
	if ( ON && g_nlist.length>0 ) {
		load_cmntcount();
	}
}
reload_cmntcount();
