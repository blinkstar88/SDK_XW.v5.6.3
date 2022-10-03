#!/sbin/cgi
<?
include("lib/settings.inc");
$cfg = @cfg_load($cfg_file_bak);
include("lib/l10n.inc");
include("lib/misc.inc");
include("lib/help.inc");
$chain_names = get_chain_names($cfg);
$chain1_name = $chain_names[0];
$chain2_name = $chain_names[1];
>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
  <title><? echo get_title($cfg, dict_translate("Antenna Alignment Tool")); ></title>
  <meta http-equiv="Content-Type" content="text/html;charset=utf-8" />
  <link href="FULL_VERSION_LINK/style.css" rel="stylesheet" type="text/css" />
  <link href="FULL_VERSION_LINK/jquery-ui.css" rel="stylesheet" type="text/css">
  <link href="FULL_VERSION_LINK/help.css" rel="stylesheet" type="text/css">
  <script type="text/javascript" language="javascript" src="jsl10n.cgi?l=<? echo $ui_language; >&v=FULL_VERSION_LINK"></script>
  <script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.js"></script>
  <script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/signal.js"></script>
  <script type="text/javascript" language="javascript" src="FULL_VERSION_LINK/sm2/soundmanager2.js"></script>
  <link type="text/css" rel="StyleSheet" href="FULL_VERSION_LINK/bluecurve.css" />
  <script type="text/javascript" src="FULL_VERSION_LINK/js/jquery.ui.js"></script>
  <script type="text/javascript" src="FULL_VERSION_LINK/help.js"></script>
  <script type="text/javascript" language="javascript">
var timerID = null;
var lastSignal = null;

var beeps = {
	count: 5,
	_beeps: new Array(this.count),
	_enabled: false,
	_pauseExcept: function(id) {
		for(var i = 0; i < this.count; i++)
			if(id != i && this._beeps[i] && this._beeps[i].playState == 1) {
				this._beeps[i].pause();
		}
	},
	replay: function(id) {
		if (!this._enabled) {
			this._pauseExcept(-1);
			return;
		}
                
		this._pauseExcept(id);
                
		if (!this._beeps[id]) return;

		this._beeps[id].togglePause();

	},
	add: function(id) {
		if (id >= this.count || id < 0) return;

		ext = '.mp3';
		if (soundManager.canPlayMIME('audio/ogg')) ext = '.ogg';
		this._beeps[id] = soundManager.createSound({
			id: 'beep'+id,
			url: 'sound/beep'+id+ext,
			autoLoad: true,
			onfinish: function(){
				this.position = 0;
			}
		});
	},
	enable: function(state) {
		this._enabled = state;
		if (!this._enabled)
			soundManager.pauseAll();
	},

	setup: function(state) {
		if (state == true && !soundManager.url) {
				soundManager.setup({
				url: 'sm2/',
				// disable debug mode
				debugMode: false,
				onready: function() {
					for(var i = 0; i < beeps.count; i++)
						beeps.add(i);
				},
				ontimeout: function() {
					alert(jsTranslate("err_no_sound|Could not start sound support!\nMake sure mp3 or ogg is supported by WEB browser."));
				}
			});
		}
		beeps.enable(state);
	}
};

function reloadSignal() {
	timerID = null;
	jQuery.getJSON("signal.cgi?"+(new Date().getTime()), update);
}

function refreshDisplay(s)
{
	lastSignal = s;
	$('#signalinfo .switchable').toggle(s != null && s.signal != 0);
	if (typeof updateSignalLevel == 'function' && s != null)
		updateSignalLevel(s.signal, s.rssi, s.noisef, s.rx_chainmask,
		s.chainrssi, s.chainrssiext);
}

function update(s) {
	refreshDisplay(s);
	if (timerID != null)
		clearTimeout(timerID);
	timerID = setTimeout('reloadSignal()', 500);
}

function createSlider() {
	var slider = $("#slider-ui-input-1");
	var rs = $("#rssifield")[0];
	var n = $("#noisef")[0];

	slider.slider({
		range: "min",
		min: 0,
		max: 80,
		step: 1,
		animate: "slow",
		slide: function(event, ui) {
			if (n)
				noise = parseFloat(n.innerHTML);
			else
				noise = -95;
			if (isNaN(noise) || noise >= 0) noise = -95;
			if (rs) rs.value = noise + parseInt(ui.value);
			RSSI_Max = parseInt(ui.value);
			refreshDisplay(lastSignal);
		}
	});
	slider.addClass("ui-widget-content-slider");

	rs.slider = slider;
	rs.onchange = function() {
		var intVal = parseFloat(this.value);
		if (isNaN(intVal)) intVal = -95;
			if (n)
				noise = parseFloat(n.innerHTML);
		else
			noise = -95;
		if (isNaN(noise) || noise >= 0) noise = -95;
			intVal = -1 * (noise - intVal);

		var slmin = this.slider.slider("option", "min");
		var slmax = this.slider.slider("option", "max");

		if (intVal > slmax)
			intVal = slmax;
		else if (intVal < slmin)
			intVal = slmin;

		this.slider.slider("option", "value", intVal);
		if (rs) rs.value = noise + intVal;

		RSSI_Max = parseInt(intVal);
		refreshDisplay(lastSignal);
	}
	rs.onchange();
}
function init() {
	$('#sound').prop("checked", false);
	createSlider();
	soundManager.setup({
		// disable debug mode
		debugMode: false
	});
	reloadSignal();
}
jQuery(document).ready(init);
//-->
</script>

</head>

<body class="popup">
<br />
<form action="#">
<table cellspacing="0" cellpadding="0" align="center" style="width: 490px;" class="popup">
	<tr><th><? echo dict_translate("Antenna Alignment Tool"); ></th></tr>
	<tr>
	  <td>
		<div id="signalinfo" class="row">
		  <span class="label"><? echo dict_translate("Signal Level"); >:</span>
		  <span class="value">
			<span class="percentborder switchable"><div id="signalbar" class="mainbar">&nbsp;</div></span>
			<span class="switchable">&nbsp;</span>
			<span id="signal"></span>
		  </span>
		</div>
		 <div id="signal_chain" class="row initial_hide">
                        <span class="label"><? echo $chain1_name; >&nbsp;/&nbsp;<? echo $chain2_name; >:</span>
                        <span class="value">
                                <span id="signal_0">&nbsp;</span>
                                <span>&nbsp;/&nbsp;</span>
                                <span id="signal_1">&nbsp;</span>
                                <span>&nbsp;dBm</span>
                        </span>
		</div>
		<div class="row">
		  <span class="label"><? echo dict_translate("Noise Floor"); >:</span>
		  <span class="value">
			<span id="noisef"></span>
		  </span>
		</div>
		<div class="row">
		  <span class="label"><? echo dict_translate("Max Signal"); >:</span>
		  <span class="value">
			<div class="horizontal-slider" id="slider-ui-input-1"></div>
			<input type="text" class="std_width" id="rssifield" name="rssifield"
				size="4" value="-65" />
			<span>&nbsp;dBm</span>
		  </span>
		</div>
		<div class="row">
		  <span class="label"><? echo dict_translate("Alignment Beep"); >
                   <span class="help"><a href="<? echo localize_help("beep.html");>" rel="help">[?]</a></span>
                   </span>
		  <span class="value">
	          <input type="checkbox" id="sound" name="sound" onClick="beeps.setup(this.checked);">&nbsp;<? echo dict_translate("Enable"); >
		  </span>
		</div>
	  </td>
	</tr>
</table>
</form>
</body>
</html>
