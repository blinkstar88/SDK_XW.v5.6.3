var ipSubnetCalc={calculateIpRange:function(g,c){if((g==="")||(g===null)||(g===false)||(c==="")||(c===null)||(c===false)){return null}var e=ipSubnetCalc.toDec(g);var b=ipSubnetCalc.toDec(c);var d=0;var f=0;var a;if(b<4294967040){b=4294967040}for(a=0;a<32;a++){f=(d+(1<<(32-(a+1))))>>>0;if(((b&f)>>>0)!==f){break}d=f}return ipSubnetCalc.getMaskRange(e,a,b)},toDec:function(a){var b=a.split(".");return((((((+b[0])*256)+(+b[1]))*256)+(+b[2]))*256)+(+b[3])},getMaskRange:function(f,a,b){var g=ipSubnetCalc.getPrefixMask(a);var e=ipSubnetCalc.getMask(32-a);var d=(f&g)>>>0;var c=(((f&g)>>>0)+e)>>>0;d++;c--;if(f+1<c){d=f+1}else{c=f-1}return{rangeStart:ipSubnetCalc.toStr(d),rangeEnd:ipSubnetCalc.toStr(c),netmask:ipSubnetCalc.toStr(b)}},getMask:function(c){var a=0;var b;for(b=0;b<c;b++){a+=(1<<b)>>>0}return a},getPrefixMask:function(b){var a=0;var c;for(c=0;c<b;c++){a+=(1<<(32-(c+1)))>>>0}return a},toStr:function(b){var c=b%256;for(var a=3;a>0;a--){b=Math.floor(b/256);c=b%256+"."+c}return c}};