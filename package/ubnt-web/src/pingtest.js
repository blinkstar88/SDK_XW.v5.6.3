var ping_running=false;var ping_count=0;var pings_started=0;var ping_form=null;var post_data={};var min;var max;var avg;var sent_packets;var received_packets;function updateStatus(){var a="0";if(received_packets<sent_packets){a=Math.round((sent_packets-received_packets)*100/sent_packets)}$("#status_rcv").text(received_packets);$("#status_sent").text(sent_packets);$("#status_loss").text(a);if(received_packets>0){$("#status_min").text(min);$("#status_avg").text(Math.round(avg*100)/100);$("#status_max").text(max)}}function addRow(b){var c=[];c.push('<tr class="pingrow">');var a;for(a=0;a<b.length;++a){c.push('<td class="str">'+b[a]+"</td>")}c.push("</tr>");$("#pingdata > tbody").append(c.join(""));$("#scroll_results").scrollTop($("#scroll_results")[0].scrollHeight);updateStatus()}function addResult(c,b,a){received_packets++;if(b>max){max=b}if(b<min){min=b}avg+=(b-avg)/received_packets;addRow([c,""+b+" ms",a])}function initPing(){var a=$("#dst_addr_select").val();if(a=="0"){a=$("#dst_addr_input").val()}if(a.length==0){return false}post_data.ip_addr=a;post_data.ping_size=$("#ping_size").val();ping_count=$("#ping_count").val();min=9999999.9;max=0;avg=0;sent_packets=0;received_packets=0;return true}function handleResponse(a,c,b){sent_packets++;if(c=="success"&&b.status==200){results=a.split("|");if(results.length>0){rc=parseInt(results[0]);if(rc==0){for(i=1;i<results.length;i+=3){addResult(results[i],parseFloat(results[i+1]),parseInt(results[i+2]))}}else{if(rc==-4){addRow([post_data.ip_addr,pingtest_l10n_timeout,"&nbsp;&nbsp;&nbsp;"])}}}}}function handleError(b,c,a){if(b&&b.status!=200&&b.status!=0){window.location.reload()}else{stopPing()}}function doPing(){pings_started++;if(ping_running){if(pings_started>=ping_count){clearInterval(interval)}}$.ajax({cache:false,url:"/pingtest_action.cgi",data:post_data,success:handleResponse,error:handleError,complete:function(b,a){if(ping_running){if(ping_count<=sent_packets){stopPing()}}}})}function startPing(a){ping_form=a;if(ping_running||!initPing()){return false}ping_running=true;pingStarted();interval=setInterval(function(){doPing()},1000);return true}function runPing(a){if(!ping_running){startPing(a)}else{stopPing()}}function stopPing(){clearInterval(interval);ping_running=false;pingStopped()}function pingStarted(){pings_started=0;$("#ping").val(l10n_stop);$(".pingrow").remove();$("#dst_addr_select, #dst_addr_input, #ping_count, #ping_size").disable();$(".status").text("0")}function pingStopped(){$("#ping").val(l10n_start);if(iplist){iplist.triggerManual(ping_form.dst_addr_select)}$("#dst_addr_select, #dst_addr_input, #ping_count, #ping_size").enable()};