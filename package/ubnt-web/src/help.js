function helpClick(){var d=$(this).position();var a=d.left+$(this).outerWidth();var c=d.top-$(document).scrollTop();var b=$("<div/>").load($(this).attr("href"));b.attr("title","Help");b.dialog({bgiframe:true,modal:true,height:"auto",dialogClass:"help",width:300,resizable:false,position:[a,c],minHeight:"none",buttons:{Close:function(){$(this).dialog("close")}}}).parent().wrapAll('<div class="help"></div>');return false}$(function(){$("a[rel=help]").click(helpClick)});