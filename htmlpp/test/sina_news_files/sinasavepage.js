/**
 * save page
 * @version 1.0
 * @update 2008-12-19
 * @example
	<body>
	...
	<script type="text/javascript" src="sinasavepage.js"></script>
	...
	<input type="button" onclick="SinaSavePage.save();" value="save" />
	...
	</body>
*/

var SinaSavePage = {
	html:'\
<form action="http://roll.news.sina.com.cn/savepage/save.php" method="post" target="sina_save_page_iframe" id="sina_save_page_form">\
	<input type="hidden" name="url" value="" />\
</form>\
<iframe style="display:none;" id="sina_save_page_iframe" name="sina_save_page_iframe"></iframe>',
	/**
	 * @param string url url of page
	 * @return void
	 */
	save:function(url){
		var form = document.getElementById('sina_save_page_form');
		if(typeof url != 'string'){
			url = document.location;	
		}
		form.elements['url'].value = url;
		form.submit();
	}
};
document.write(SinaSavePage.html);