function getSnr(b){var a=b*2;if(a<10){a=10}else{if(a>100){a=100}}return a}function refreshTable(d){if(!d){return}$("#sats > tbody").empty();var a=[];for(var b=0;b<d.length;++b){var c=d[b];var e=[];e.push("<tr>");e.push('<td class="centered">'+((c.sat>=10)?c.sat:"0"+c.sat)+"</td>");e.push('<td style="line-height: 1;"><span class="percentborder"><div class="mainbar" style="width: '+getSnr(c.snr)+'%;">&nbsp;</div></span></td>');if(b+1<d.length){c=d[++b];e.push('<td class="centered">'+((c.sat>=10)?c.sat:"0"+c.sat)+"</td>");e.push('<td style="line-height: 1;"><span class="percentborder"><div class="mainbar" style="width: '+getSnr(c.snr)+'%;">&nbsp;</div></span></td>')}else{e.push("<td>&nbsp;</td><td>&nbsp;</td>")}e.push("</tr>");a.push(e.join(""))}if(d.length==0||typeof d.length=="undefined"){a.push('<tr><td colspan="'+$("#sats >thead >tr >th").length+'">'+jsTranslate("No data available")+"</td></tr>")}$("#sats > tbody").append(a.join(""))}function refreshInfo(){var a={};a.load="y";$.ajax({type:"GET",url:"gpsstats.cgi",dataType:"json",cache:false,data:a,success:refreshTable,complete:requestCompleted})}function requestCompleted(b,a){if(typeof requestCompleted.timeout=="undefined"){requestCompleted.timeout=0}if(requestCompleted.timeout){clearTimeout(requestCompleted.timeout)}requestCompleted.timeout=setTimeout(refreshInfo,1000)}function refreshAll(){if(typeof reloadStatus=="function"){reloadStatus()}refreshInfo()}$(document).ready(function(){refreshInfo()});