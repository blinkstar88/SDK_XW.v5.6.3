/* Localisation support for jQuery through simple map dictionary.
 * Supports java-like ({n}) and sprintf-like (%s, %d) placeholders.
 *
 * Copyright (c) 2010 Ubiquiti Networks, Inc. (http://www.ubnt.com)
 * Dual licensed under the MIT or GPL Version 2 licenses.
 * Tested on jQuery 1.4.2
 */
(function(c){c.extend({l10n:{init:function(e){var f=c.extend({},c.l10n.defaults,e);this.dict=f.dictionary;this.name=f.name;this.set_formatter(f.formatter)},set_formatter:function(e){switch(e){case"java":this.format=b;this.formatter=e;break;case"sprintf":this.format=d;this.formatter=e;break;case"none":default:this.format=a;this.formatter="none"}},get:function(k,g){var j=k;var h=k;if(/\|/.test(k)){var f=k.split("|");j=f[0];f.shift();h=f.join("|")}var e=this.dict[j];h=e||h;return this.format(h,g)},_:function(f,e){return this.get(f,e)}}});function b(h,e){if(!e||e.length==0||!RegExp){return h}var f=["\\{","1","\\}"];for(i=0;i<e.length;++i){f[1]=i;var g=new RegExp(f.join(""),"g");h=h.replace(g,e[i])}return h}function d(j,e){if(!e||e.length==0||!RegExp){return j}var h=/([^%]*)%([sd])(.*)/;var f=0;var g=[];while(f<e.length&&(g=h.exec(j))){g.shift();g[1]=e[f];j=g.join("");f++}return j}function a(f,e){return f}c.l10n.defaults={dictionary:{},name:"default",formatter:"sprintf"}})(jQuery);