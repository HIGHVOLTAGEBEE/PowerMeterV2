#include "WebPages.h"

// ============================================================================
// Seitenkopf inkl. CSS (gemeinsam fuer alle Seiten)
// ============================================================================
static const char PAGE_HEAD[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<title>XT90 Power Meter</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{background:#111;color:#fff;font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;padding:10px;min-height:100vh;max-width:520px;margin:0 auto}
.nav{display:flex;gap:4px;margin-bottom:12px;flex-wrap:wrap}
.navbtn{flex:1;min-width:56px;padding:10px 4px;background:#222;color:#888;font-size:13px;border-radius:8px;text-align:center;text-decoration:none;transition:all .2s}
.navbtn.active{background:#333;color:#fff;font-weight:600}
.card{background:#1a1a1a;border-radius:12px;padding:14px;margin-bottom:10px}
.row{display:flex;gap:10px;flex-wrap:wrap}
.col{flex:1;min-width:120px}
.val{font-size:28px;font-weight:300;margin:4px 0}
.lbl{font-size:11px;color:#888;text-transform:uppercase;letter-spacing:.5px}
.unit{font-size:14px;color:#666;margin-left:2px}
.btn{display:block;width:100%;padding:16px;border:none;border-radius:10px;font-size:16px;font-weight:600;cursor:pointer;transition:all .2s;margin:6px 0}
.btn-on{background:#2a6b2a;color:#fff}
.btn-off{background:#6b2a2a;color:#fff}
.btn-on:active,.btn-off:active{opacity:.7}
.btn-sm{padding:10px;font-size:13px;width:auto;display:inline-block;margin:3px}
.btn-blue{background:#2a3a6b;color:#fff}
.btn-gray{background:#333;color:#fff}
.btn-red{background:#6b2a2a;color:#fff}
canvas{width:100%;height:200px;background:#111;border-radius:8px;margin:8px 0}
.legend{display:flex;gap:12px;justify-content:center;margin:4px 0 8px 0;flex-wrap:wrap}
.legend-item{display:flex;align-items:center;gap:4px;font-size:11px;color:#888}
.legend-dot{width:8px;height:8px;border-radius:50%;display:inline-block}
.field-group{margin:8px 0}
.field-group label{display:block;font-size:11px;color:#888;margin-bottom:3px}
.field-row{display:flex;gap:8px;align-items:center}
.field-row input{flex:1;padding:10px;background:#222;border:1px solid #333;border-radius:8px;color:#fff;font-size:14px}
.field-row .cur-val{font-size:14px;color:#aaa;min-width:60px;text-align:right}
.status-dot{display:inline-block;width:8px;height:8px;border-radius:50%;margin-right:6px}
.status-dot.green{background:#2a6b2a}
.status-dot.red{background:#6b2a2a}
.status-dot.yellow{background:#6b6b2a}
.grid-2{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.report-card{background:#1a1a1a;border-radius:12px;padding:14px;margin-bottom:10px;border-left:4px solid #6b2a2a}
.report-card.ok{border-left-color:#2a6b2a}
.report-card.trip{border-left-color:#6b6b2a}
.report-row{display:flex;justify-content:space-between;padding:4px 0;font-size:13px;border-bottom:1px solid #222}
.report-label{color:#888}
.report-value{color:#fff;font-weight:500}
.report-value.fault{color:#ff4a4a}
.report-value.ok{color:#4aff6a}
.report-value.warn{color:#ffaa4a}
.info-table{width:100%;font-size:12px}
.info-table td{padding:4px 0;border-bottom:1px solid #222}
.info-table td:first-child{color:#888;width:50%}
.info-table td:last-child{color:#fff;text-align:right}
.gauge-wrap{text-align:center;margin:20px 0 30px 0;position:relative}
.gauge-wrap canvas{width:200px;height:150px;background:transparent}
.gauge-val{position:absolute;top:62px;left:50%;transform:translateX(-50%);font-size:32px;font-weight:300;color:#fff}
.gauge-label{position:absolute;bottom:-18px;left:50%;transform:translateX(-50%);font-size:11px;color:#888;text-transform:uppercase}
.slider-row{display:flex;gap:12px;align-items:center;margin:16px 0}
.slider-row input[type=range]{flex:1;height:6px;-webkit-appearance:none;background:#333;border-radius:3px;outline:none}
.slider-row input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:24px;height:24px;border-radius:50%;background:#4a7a4a;cursor:pointer}
.slider-val{font-size:24px;font-weight:300;min-width:50px;text-align:center}
.dim-sep{height:2px;background:#222;margin:16px 0}
.by{margin-top:16px;text-align:center;font-size:11px;color:#555;letter-spacing:1px}
.sec-title{font-size:12px;font-weight:700;letter-spacing:1px;text-transform:uppercase;margin:14px 0 8px 0;padding:6px 10px;border-radius:6px}
.sec-blue{background:rgba(70,100,220,.16);color:#7a9aff}
.sec-green{background:rgba(50,170,85,.16);color:#4aff6a}
.sec-orange{background:rgba(220,150,45,.16);color:#ffaa4a}
.sec-purple{background:rgba(165,90,210,.16);color:#c98aff}
.sec-gray{background:rgba(140,140,140,.14);color:#aaa}
.tgl{display:flex;gap:8px}
.tgl-btn{flex:1;padding:12px;border:none;border-radius:10px;font-size:14px;font-weight:700;cursor:pointer;transition:all .15s}
.tgl-btn.tgl-on:not(.active),.tgl-btn.tgl-off:not(.active){background:#1c1c1c;color:#555;box-shadow:inset 0 0 0 1px #333}
.tgl-btn.tgl-on.active{background:#2a6b2a;color:#fff}
.tgl-btn.tgl-off.active{background:#6b2a2a;color:#fff}
.storbar{height:12px;background:#222;border-radius:6px;overflow:hidden;margin:6px 0}
.storfill{height:100%;width:0%;background:#2a6b2a;transition:width .3s}
.stor-txt{font-size:12px;color:#888}
.graph-ctl{display:flex;gap:10px;align-items:center;justify-content:center;margin:4px 0 8px 0}
.graph-ctl button{width:auto;padding:8px 12px;font-size:14px;font-weight:700;border:none;border-radius:8px;background:#2a3a6b;color:#fff;cursor:pointer}
.graph-ctl button:active{opacity:.7}
.graph-ctl span{font-size:13px;color:#aaa;min-width:60px;text-align:center}
.prog-wrap{position:fixed;inset:0;background:rgba(0,0,0,.75);display:none;align-items:center;justify-content:center;z-index:99}
.prog-wrap.show{display:flex}
.prog-box{background:#1a1a1a;border-radius:12px;padding:20px;width:82%;max-width:380px}
.prog-title{font-size:14px;font-weight:600;margin-bottom:4px}
.prog-bar{height:14px;background:#222;border-radius:7px;overflow:hidden;margin:10px 0 6px 0}
.prog-fill{height:100%;width:0%;background:#2a6b2a;transition:width .2s}
.prog-txt{font-size:12px;color:#888}
@media(max-width:400px){.val{font-size:22px}.col{min-width:80px}}
</style>
</head>
<body>
)rawliteral";

static const char PAGE_FOOT[] PROGMEM = R"rawliteral(</body>
</html>
)rawliteral";

// Navigationsleiste, active: 0=Live 1=Logger 2=eFuse 3=Dimmer 4=Settings
static String navHtml(int active)
{
  static const char *urls[5]  = {"/", "/logger", "/efuse", "/dimmer", "/config"};
  static const char *names[5] = {"Live", "Logger", "eFuse", "Dimmer", "Settings"};
  String s = F("<div class=\"nav\">");
  for (int i = 0; i < 5; i++)
  {
    s += F("<a href=\"");
    s += urls[i];
    s += F("\" class=\"navbtn");
    if (i == active) s += F(" active");
    s += F("\">");
    s += names[i];
    s += F("</a>");
  }
  s += F("</div>");
  return s;
}

static String assemblePage(int active, const void *body, const void *js)
{
  String page;
  page.reserve(20000);
  page += FPSTR(PAGE_HEAD);
  page += navHtml(active);
  page += FPSTR((const char *)body);
  page += FPSTR((const char *)js);
  page += FPSTR(PAGE_FOOT);
  return page;
}

// ============================================================================
// Seite: Live
// ============================================================================
static const char PAGE_LIVE_BODY[] PROGMEM = R"rawliteral(<div class="card">
<div class="row">
<div class="col"><div class="lbl" id="lblV">Spannung</div><div class="val" id="tv">0.000<span class="unit">V</span></div></div>
<div class="col"><div class="lbl" id="lblA">Strom</div><div class="val" id="ta">0.000<span class="unit">A</span></div></div>
<div class="col"><div class="lbl" id="lblW">Leistung</div><div class="val" id="tw">0.000<span class="unit">W</span></div></div>
</div>
<div class="row" style="margin-top:8px">
<div class="col"><div class="lbl" id="lblT">Temperatur</div><div class="val" id="tt">0.0<span class="unit">°C</span></div></div>
<div class="col"><div class="lbl" id="lblWh">Energie</div><div class="val" id="twh">0.000<span class="unit">Wh</span></div></div>
<div class="col"><div class="lbl" id="lblStatus">Status</div><div class="val" id="tstatus"><span class="status-dot red"></span>OFF</div></div>
</div>
</div>
<div class="card">
<div class="legend">
<span class="legend-item"><span class="legend-dot" style="background:#4a4a7a"></span><span id="lgV">Spannung</span></span>
<span class="legend-item"><span class="legend-dot" style="background:#4a7a4a"></span><span id="lgA">Strom</span></span>
<span class="legend-item"><span class="legend-dot" style="background:#7a4a4a"></span><span id="lgW">Leistung</span></span>
</div>
<canvas id="graph"></canvas>
<div class="graph-ctl">
<button onclick="graphSet(-1000)" style="font-size:20px;padding:6px 16px;font-weight:700">&minus;</button>
<button onclick="graphSet(-100)" style="font-size:12px;padding:4px 10px">&minus;</button>
<span id="lblGraphN">600</span>
<button onclick="graphSet(100)" style="font-size:12px;padding:4px 10px">+</button>
<button onclick="graphSet(1000)" style="font-size:20px;padding:6px 16px;font-weight:700">+</button>
</div>
<div class="graph-ctl" style="justify-content:space-between">
<button class="btn btn-sm" id="btnFreeze" onclick="toggleFreeze()" style="width:auto;margin:0">Freeze</button>
<button class="btn btn-sm btn-blue" onclick="exportGraphPng()" style="width:auto;margin:0">PNG</button>
</div>
</div>
<button class="btn btn-off" id="btnOut" onclick="toggleOutput()">OUTPUT AUS</button>
<div class="by">by HIGHVOLTAGEBEE</div>
)rawliteral";

static const char PAGE_LIVE_JS[] PROGMEM = R"rawliteral(<script>
let ws, graphCtx, wsReconnectTimer = null;
let graphData = [], graphDataV = [], graphDataW = [];
let gMax = 600;
let graphFrozen = false;
let currentData = {};

const TXT = {
de:{V:"Spannung",A:"Strom",W:"Leistung",T:"Temperatur",Wh:"Energie",St:"Status",
 lgV:"Spannung",lgA:"Strom",lgW:"Leistung",ON:"EIN",OFF:"AUS",
 OUT_ON:"OUTPUT EIN",OUT_OFF:"OUTPUT AUS",Freeze:"Freeze",Unfreeze:"Weiter"},
en:{V:"Voltage",A:"Current",W:"Power",T:"Temperature",Wh:"Energy",St:"Status",
 lgV:"Voltage",lgA:"Current",lgW:"Power",ON:"ON",OFF:"OFF",
 OUT_ON:"OUTPUT ON",OUT_OFF:"OUTPUT OFF",Freeze:"Freeze",Unfreeze:"Resume"}
};

function t(key){ const lang = currentData.lang === 1 ? 'en' : 'de'; return (TXT[lang] && TXT[lang][key]) || key; }

function applyLang(){
  const ids=['lblV','lblA','lblW','lblT','lblWh','lblStatus','lgV','lgA','lgW'];
  const keys=['V','A','W','T','Wh','St','lgV','lgA','lgW'];
  ids.forEach((id,i)=>{ const el=document.getElementById(id); if(el) el.textContent=t(keys[i]); });
  const bf=document.getElementById('btnFreeze');
  if(bf) bf.textContent=graphFrozen?t('Unfreeze'):t('Freeze');
  document.getElementById('lblGraphN').textContent=gMax;
}

function connectWS(){
  if(wsReconnectTimer){ clearTimeout(wsReconnectTimer); wsReconnectTimer=null; }
  try{ if(ws) ws.close(); }catch(e){}
  ws=new WebSocket('ws://'+location.host+'/ws');
  ws.onmessage=function(e){
    try{ const d=JSON.parse(e.data); currentData=d; updateUI(d); }catch(ex){}
  };
  ws.onclose=function(){ wsReconnectTimer=setTimeout(connectWS,2000); };
  ws.onerror=function(){ ws.close(); };
}

function toggleOutput(){ ws.send('OUTPUT:'+(currentData.out?0:1)); }

function updateUI(d){
  document.getElementById('tv').innerHTML=d.v.toFixed(3)+'<span class="unit">V</span>';
  document.getElementById('ta').innerHTML=d.a.toFixed(3)+'<span class="unit">A</span>';
  document.getElementById('tw').innerHTML=d.w.toFixed(3)+'<span class="unit">W</span>';
  document.getElementById('tt').innerHTML=d.t.toFixed(1)+'<span class="unit">°C</span>';
  document.getElementById('twh').innerHTML=d.wh.toFixed(3)+'<span class="unit">Wh</span>';
  const se=document.getElementById('tstatus');
  se.innerHTML=d.out?'<span class="status-dot green"></span>'+t('ON'):(d.eft?'<span class="status-dot yellow"></span>'+t('TRIP'):'<span class="status-dot red"></span>'+t('OFF'));
  const ot=d.out?t('OUT_ON'):t('OUT_OFF');
  const oc=d.out?'btn btn-on':'btn btn-off';
  const b=document.getElementById('btnOut');
  b.textContent=ot; b.className=oc;
  if(!graphFrozen){
    graphData.push(d.a); graphDataV.push(d.v); graphDataW.push(d.w);
    while(graphData.length>gMax){ graphData.shift(); graphDataV.shift(); graphDataW.shift(); }
    drawGraph();
  }
  applyLang();
}

function graphSet(d){
  gMax=Math.min(10000,Math.max(100,gMax+d));
  while(graphData.length>gMax){ graphData.shift(); graphDataV.shift(); graphDataW.shift(); }
  applyLang();
  drawGraph();
}

function toggleFreeze(){
  graphFrozen=!graphFrozen;
  const b=document.getElementById('btnFreeze');
  b.textContent=graphFrozen?t('Unfreeze'):t('Freeze');
  b.className='btn btn-sm'+(graphFrozen?' btn-on':'');
}

function exportGraphPng(){
  const c=document.createElement('canvas');
  c.width=1200; c.height=400;
  const keep=graphCtx;
  graphCtx=c.getContext('2d');
  drawGraph();
  graphCtx=keep;
  const a=document.createElement('a');
  a.href=c.toDataURL('image/png');
  a.download='xt90_graph_'+new Date().toISOString().replace(/[:.]/g,'-')+'.png';
  document.body.appendChild(a); a.click(); document.body.removeChild(a);
}

function drawGraph(){
  if(!graphCtx) return;
  const c=graphCtx.canvas;
  const rect=c.parentElement?c.parentElement.getBoundingClientRect():null;
  c.width=rect?(rect.width-28):c.width;
  c.height=200;
  const w=c.width,h=c.height;
  const padL=38,padR=6,padT=10,padB=10;
  graphCtx.fillStyle='#111';
  graphCtx.fillRect(0,0,w,h);
  if(graphData.length<2) return;
  let maxVal=0.1;
  for(let i=0;i<graphData.length;i++){
    maxVal=Math.max(maxVal,Math.abs(graphDataV[i]),Math.abs(graphData[i])*10,Math.abs(graphDataW[i]));
  }
  maxVal=Math.ceil(maxVal*1.2);
  if(maxVal<1) maxVal=1;
  graphCtx.font='10px sans-serif';
  graphCtx.textAlign='right';
  for(let i=0;i<=8;i++){
    const frac=i/8;
    const y=h-padB-frac*(h-padT-padB);
    graphCtx.strokeStyle=i===0?'#333':'#1c1c1c';
    graphCtx.lineWidth=1;
    graphCtx.beginPath();
    graphCtx.moveTo(padL,y);
    graphCtx.lineTo(w-padR,y);
    graphCtx.stroke();
    graphCtx.fillStyle='#666';
    graphCtx.fillText((maxVal*frac).toFixed(1),padL-4,y+3);
  }
  graphCtx.textAlign='left';
  const stepX=(w-padL-padR)/(graphData.length-1);
  function drawLine(data,color){
    graphCtx.strokeStyle=color;
    graphCtx.lineWidth=2;
    graphCtx.beginPath();
    for(let i=0;i<data.length;i++){
      const x=padL+i*stepX;
      const y=h-padB-(Math.abs(data[i])/maxVal)*(h-padT-padB);
      i===0?graphCtx.moveTo(x,y):graphCtx.lineTo(x,y);
    }
    graphCtx.stroke();
  }
  drawLine(graphDataV,'#4a4a7a');
  drawLine(graphData,'#4a7a4a');
  drawLine(graphDataW,'#7a4a4a');
}

graphCtxInit();
function graphCtxInit(){ graphCtx=document.getElementById('graph').getContext('2d'); }
connectWS();
applyLang();
</script>
)rawliteral";

// ============================================================================
// Seite: Logger
// ============================================================================
static const char PAGE_LOGGER_BODY[] PROGMEM = R"rawliteral(<div class="card">
<div class="grid-2">
<div><div class="lbl" id="lblPV">Peak Spannung</div><div class="val" id="lpv">0.000<span class="unit">V</span></div></div>
<div><div class="lbl" id="lblPA">Peak Strom</div><div class="val" id="lpa">0.000<span class="unit">A</span></div></div>
<div><div class="lbl" id="lblMV">Min Spannung</div><div class="val" id="lmv">0.000<span class="unit">V</span></div></div>
<div><div class="lbl" id="lblMA">Min Strom</div><div class="val" id="lma">0.000<span class="unit">A</span></div></div>
</div>
<div style="margin-top:8px">
<div class="lbl" id="lblWh2">Gesamtenergie</div>
<div class="val" id="lwh">0.000<span class="unit">Wh</span></div>
</div>
<div style="margin-top:8px">
<div class="lbl" id="lblUp">Laufzeit</div>
<div class="val" id="lup">0s</div>
</div>
</div>
<button class="btn btn-off" id="btnLog" onclick="toggleLogging()">LOGGING AUS</button>
<div class="card">
<div class="field-group">
<label id="lblLogInt">Log Intervall (s)</label>
<div class="field-row">
<input type="number" id="logInterval" min="0.1" max="3600" step="0.1">
<button class="btn btn-sm btn-blue" onclick="saveLogInterval()" id="btnSaveLogInt">Speichern</button>
</div>
</div>
<div class="field-group">
<label id="lblLogCnt">Log Einträge</label>
<div class="field-row"><span class="cur-val" id="logCount">0</span></div>
</div>
<div class="field-group">
<label id="lblStor">Log Speicher</label>
<div class="storbar"><div class="storfill" id="storFill"></div></div>
<div class="stor-txt" id="storTxt">0 / 0 MB (0%)</div>
</div>
<button class="btn btn-sm btn-blue" onclick="exportLog()" id="btnExport">Excel Export</button>
<button class="btn btn-sm btn-red" onclick="clearLog()" id="btnClearLog">Log löschen</button>
</div>
<div class="prog-wrap" id="progWrap">
<div class="prog-box">
<div class="prog-title" id="progTitle">Exportiere Logs...</div>
<div class="prog-bar"><div class="prog-fill" id="progFill"></div></div>
<div class="prog-txt" id="progTxt">0 / 0</div>
</div>
</div>
<div class="by">by HIGHVOLTAGEBEE</div>
)rawliteral";

static const char PAGE_LOGGER_JS[] PROGMEM = R"rawliteral(<script>
let ws, wsReconnectTimer = null;
let currentData = {};
let exporting = false;

const TXT = {
de:{PV:"Peak Spannung",PA:"Peak Strom",MV:"Min Spannung",MA:"Min Strom",
 Wh2:"Gesamtenergie",Up:"Laufzeit",LOG_ON:"LOGGING EIN",LOG_OFF:"LOGGING AUS",
 CLR:"Log löschen",LogInt:"Log Intervall (s)",LogCnt:"Log Einträge",Stor:"Log Speicher",
 SaveInt:"Speichern",Exporting:"Exportiere Logs...",Entries:"Einträge",ErrExport:"Export fehlgeschlagen"},
en:{PV:"Peak Voltage",PA:"Peak Current",MV:"Min Voltage",MA:"Min Current",
 Wh2:"Total Energy",Up:"Uptime",LOG_ON:"LOGGING ON",LOG_OFF:"LOGGING OFF",
 CLR:"Clear Log",LogInt:"Log Interval (s)",LogCnt:"Log Entries",Stor:"Log Storage",
 SaveInt:"Save",Exporting:"Exporting logs...",Entries:"entries",ErrExport:"Export failed"}
};

function t(key){ const lang = currentData.lang === 1 ? 'en' : 'de'; return (TXT[lang] && TXT[lang][key]) || key; }

function applyLang(){
  const ids=['lblPV','lblPA','lblMV','lblMA','lblWh2','lblUp','btnLog','lblLogInt','lblLogCnt','lblStor','btnExport','btnClearLog','btnSaveLogInt'];
  const keys=['PV','PA','MV','MA','Wh2','Up','LOG_OFF','LogInt','LogCnt','Stor','Export','CLR','SaveInt'];
  ids.forEach((id,i)=>{ const el=document.getElementById(id); if(el) el.textContent=t(keys[i]); });
}

function fmtTime(s){
  const h=Math.floor(s/3600),m=Math.floor((s%3600)/60),sec=s%60;
  return (h>0?h+'h ':'')+(m>0?m+'m ':'')+sec+'s';
}

function connectWS(){
  if(wsReconnectTimer){ clearTimeout(wsReconnectTimer); wsReconnectTimer=null; }
  try{ if(ws) ws.close(); }catch(e){}
  ws=new WebSocket('ws://'+location.host+'/ws');
  ws.onmessage=function(e){
    try{ const d=JSON.parse(e.data); currentData=d; updateUI(d); }catch(ex){}
  };
  ws.onclose=function(){ wsReconnectTimer=setTimeout(connectWS,2000); };
  ws.onerror=function(){ ws.close(); };
}

function toggleLogging(){ ws.send('LOG:'+(currentData.log?0:1)); }

function updateUI(d){
  document.getElementById('lpv').innerHTML=d.pv.toFixed(3)+'<span class="unit">V</span>';
  document.getElementById('lpa').innerHTML=d.pa.toFixed(3)+'<span class="unit">A</span>';
  document.getElementById('lmv').innerHTML=d.mv.toFixed(3)+'<span class="unit">V</span>';
  document.getElementById('lma').innerHTML=d.ma.toFixed(3)+'<span class="unit">A</span>';
  document.getElementById('lwh').innerHTML=d.wh.toFixed(3)+'<span class="unit">Wh</span>';
  document.getElementById('lup').innerHTML=fmtTime(d.up);
  document.getElementById('logCount').textContent=d.lc;
  const b=document.getElementById('btnLog');
  b.textContent=d.log?t('LOG_ON'):t('LOG_OFF');
  b.className=d.log?'btn btn-on':'btn btn-off';
  applyLang();
}

function clearLog(){ ws.send('CLEAR_LOG'); loadLogInfo(); }

function saveLogInterval(){
  const v=document.getElementById('logInterval').value;
  fetch('/settings',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'logInterval='+v}).catch(()=>{});
}

function loadLogInfo(){
  fetch('/loginfo').then(r=>r.json()).then(d=>{
    document.getElementById('logCount').textContent=d.entries;
    document.getElementById('storFill').style.width=d.pct+'%';
    document.getElementById('storFill').style.background=d.pct>85?'#6b2a2a':'#2a6b2a';
    document.getElementById('storTxt').textContent=(d.usedKB/1024).toFixed(2)+' / '+(d.totalKB/1024).toFixed(2)+' MB ('+d.pct+'%)';
  }).catch(()=>{});
  fetch('/settings').then(r=>r.json()).then(d=>{
    document.getElementById('logInterval').value=d.logInterval;
  }).catch(()=>{});
}

function progShow(on){ document.getElementById('progWrap').className=on?'prog-wrap show':'prog-wrap'; }
function progUpdate(pct,txt){
  document.getElementById('progFill').style.width=Math.min(100,pct)+'%';
  document.getElementById('progTxt').textContent=txt;
}

async function exportLog(){
  if(exporting) return;
  let info=null;
  try{ info=await fetch('/loginfo').then(r=>r.json()); }catch(e){}
  if(!info||!info.entries) return;
  exporting=true;
  document.getElementById('progTitle').textContent=t('Exporting');
  progShow(true);
  const t0=Date.now();
  let offset=0,rows=[],fetched=0,failed=false;
  while(true){
    let txt;
    try{ txt=await fetch('/logchunk?o='+offset+'&c=100').then(r=>r.text()); }
    catch(e){ failed=true; break; }
    const nl=txt.indexOf('\n');
    let next=-1,done='1';
    if(nl>=0){
      const m=txt.substring(0,nl).match(/#OFF:(-?\d+),(\d)/);
      if(m){ next=parseInt(m[1]); done=m[2]; }
      const body=txt.substring(nl+1);
      const lines=body.split('\n').filter(l=>l.length>0);
      rows=rows.concat(lines);
      fetched+=lines.length;
    }
    const elapsed=(Date.now()-t0)/1000;
    const rate=elapsed>0.5?fetched/elapsed:0;
    const eta=rate>0?Math.max(0,(info.entries-fetched)/rate):0;
    progUpdate(info.entries>0?(fetched/info.entries)*100:0,fetched+' / '+info.entries+' '+t('Entries')+'  ·  ETA '+eta.toFixed(0)+'s');
    if(done==='1'||next<0||nl<0) break;
    offset=next;
  }
  progShow(false);
  exporting=false;
  if(failed||rows.length===0){ if(failed) alert(t('ErrExport')); return; }
  let html='<html xmlns:x="urn:schemas-microsoft-com:office:excel"><head><meta charset="UTF-8"></head><body><table border="1">';
  html+='<tr><th>Timestamp</th><th>V</th><th>A</th><th>W</th><th>PeakV</th><th>PeakA</th><th>PeakW</th><th>MinV</th><th>MinA</th><th>Wh</th></tr>';
  for(let i=0;i<rows.length;i++){
    html+='<tr>';
    const cols=rows[i].split(',');
    for(let j=0;j<cols.length;j++) html+='<td>'+cols[j]+'</td>';
    html+='</tr>';
  }
  html+='</table></body></html>';
  const blob=new Blob(['\ufeff'+html],{type:'application/vnd.ms-excel'});
  const a=document.createElement('a');
  a.href=URL.createObjectURL(blob);
  a.download='xt90_log_'+new Date().toISOString().replace(/[:.]/g,'-')+'.xls';
  document.body.appendChild(a); a.click(); document.body.removeChild(a);
  URL.revokeObjectURL(a.href);
}

connectWS();
loadLogInfo();
applyLang();
setInterval(loadLogInfo,2000);
</script>
)rawliteral";

// ============================================================================
// Seite: eFuse
// ============================================================================
static const char PAGE_EFUSE_BODY[] PROGMEM = R"rawliteral(<div class="card">
<div class="field-group">
<label id="lblEFMc">Max Strom (A)</label>
<div class="field-row">
<input type="number" id="efMc" step="0.1" placeholder="Aktuell">
<span class="cur-val" id="efMcV">0.0A</span>
</div>
</div>
<div class="field-group">
<label id="lblEFMv">Max Spannung (V)</label>
<div class="field-row">
<input type="number" id="efMv" step="0.1" placeholder="Aktuell">
<span class="cur-val" id="efMvV">0.0V</span>
</div>
</div>
<div class="field-group">
<label id="lblEFMnv">Min Spannung (V)</label>
<div class="field-row">
<input type="number" id="efMnv" step="0.1" placeholder="Aktuell">
<span class="cur-val" id="efMnvV">0.0V</span>
</div>
</div>
<div class="field-group">
<label id="lblEFMt">Max Temperatur (°C)</label>
<div class="field-row">
<input type="number" id="efMt" step="1" placeholder="Aktuell">
<span class="cur-val" id="efMtV">0.0°C</span>
</div>
</div>
<div class="field-group">
<label id="lblEFDelay">Auslöseverzögerung (s)</label>
<div class="field-row">
<input type="number" id="efTt" step="0.1" placeholder="Aktuell">
<span class="cur-val" id="efTtV">0.0s</span>
</div>
</div>
<button class="btn btn-blue" onclick="saveEFuse()" id="btnSaveEF">Speichern</button>
</div>
<div id="efReport"></div>
<button class="btn btn-off" id="btnEf" onclick="toggleEFuse()">eFUSE AUS</button>
<button class="btn btn-red" onclick="resetEFuse()" id="btnResetEF">Reset eFuse</button>
<button class="btn btn-on" id="btnOutEf" onclick="toggleOutput()">OUTPUT EIN</button>
<div class="by">by HIGHVOLTAGEBEE</div>
)rawliteral";

static const char PAGE_EFUSE_JS[] PROGMEM = R"rawliteral(<script>
let ws, wsReconnectTimer = null;
let currentData = {};

const TXT = {
de:{EFMc:"Max Strom (A)",EFMv:"Max Spannung (V)",EFMnv:"Min Spannung (V)",
 EFMt:"Max Temperatur (°C)",EFDelay:"Auslöseverzögerung (s)",Save:"Speichern",
 ResetEF:"Reset eFuse",Active:"Aktiv",Deact:"Deaktiviert",Trip:"AUSGELÖST",
 Reason:"Grund",FaultVal:"Fehlerwert",OutOn:"Ausgang an für",TripAgo:"Ausgelöst vor",
 Mon:"eFuse überwacht",OffDesc:"eFuse ausgeschaltet",
 EF_ON:"eFUSE EIN",EF_OFF:"eFUSE AUS",OUT_ON:"OUTPUT EIN",OUT_OFF:"OUTPUT AUS"},
en:{EFMc:"Max Current (A)",EFMv:"Max Voltage (V)",EFMnv:"Min Voltage (V)",
 EFMt:"Max Temperature (°C)",EFDelay:"Trip Delay (s)",Save:"Save",
 ResetEF:"Reset eFuse",Active:"Active",Deact:"Deactivated",Trip:"TRIPPED",
 Reason:"Reason",FaultVal:"Fault Value",OutOn:"Output was on for",TripAgo:"Tripped",
 Mon:"eFuse monitoring",OffDesc:"eFuse disabled",
 EF_ON:"eFUSE ON",EF_OFF:"eFUSE OFF",OUT_ON:"OUTPUT ON",OUT_OFF:"OUTPUT OFF"}
};

function t(key){ const lang = currentData.lang === 1 ? 'en' : 'de'; return (TXT[lang] && TXT[lang][key]) || key; }

function applyLang(){
  const ids=['lblEFMc','lblEFMv','lblEFMnv','lblEFMt','lblEFDelay','btnSaveEF','btnResetEF'];
  const keys=['EFMc','EFMv','EFMnv','EFMt','EFDelay','Save','ResetEF'];
  ids.forEach((id,i)=>{ const el=document.getElementById(id); if(el) el.textContent=t(keys[i]); });
}

function fmtTime(s){
  const h=Math.floor(s/3600),m=Math.floor((s%3600)/60),sec=s%60;
  return (h>0?h+'h ':'')+(m>0?m+'m ':'')+sec+'s';
}

function connectWS(){
  if(wsReconnectTimer){ clearTimeout(wsReconnectTimer); wsReconnectTimer=null; }
  try{ if(ws) ws.close(); }catch(e){}
  ws=new WebSocket('ws://'+location.host+'/ws');
  ws.onmessage=function(e){
    try{ const d=JSON.parse(e.data); currentData=d; updateUI(d); }catch(ex){}
  };
  ws.onclose=function(){ wsReconnectTimer=setTimeout(connectWS,2000); };
  ws.onerror=function(){ ws.close(); };
}

function toggleOutput(){ ws.send('OUTPUT:'+(currentData.out?0:1)); }
function toggleEFuse(){ ws.send('EFUSE:'+(currentData.ef?0:1)); }
function resetEFuse(){ ws.send('RESET_EFUSE'); }

function saveEFuse(){
  const mc=document.getElementById('efMc').value;
  const mv=document.getElementById('efMv').value;
  const mnv=document.getElementById('efMnv').value;
  const mt=document.getElementById('efMt').value;
  const tt=document.getElementById('efTt').value;
  ws.send('SET:mc='+mc+',mv='+mv+',mnv='+mnv+',mt='+mt+',tt='+tt+',');
  document.getElementById('efMcV').textContent=parseFloat(mc).toFixed(1)+'A';
  document.getElementById('efMvV').textContent=parseFloat(mv).toFixed(1)+'V';
  document.getElementById('efMnvV').textContent=parseFloat(mnv).toFixed(1)+'V';
  document.getElementById('efMtV').textContent=parseFloat(mt).toFixed(1)+'°C';
  document.getElementById('efTtV').textContent=parseFloat(tt).toFixed(1)+'s';
}

function updateUI(d){
  const be=document.getElementById('btnEf');
  be.textContent=d.ef?t('EF_ON'):t('EF_OFF');
  be.className=d.ef?'btn btn-on':'btn btn-off';
  const bo=document.getElementById('btnOutEf');
  bo.textContent=d.out?t('OUT_ON'):t('OUT_OFF');
  bo.className=d.out?'btn btn-on':'btn btn-off';
  if(d.eft){
    let rh='<div class="report-card trip"><div style="font-size:14px;font-weight:600;color:#ffaa4a;margin-bottom:8px">'+t('Trip')+'</div>';
    rh+='<div class="report-row"><span class="report-label">'+t('Reason')+'</span><span class="report-value fault">'+d.fr+'</span></div>';
    rh+='<div class="report-row"><span class="report-label">'+t('FaultVal')+'</span><span class="report-value fault">'+d.fc.toFixed(2)+'A / '+d.fv.toFixed(2)+'V / '+d.ft.toFixed(1)+'°C</span></div>';
    rh+='<div class="report-row"><span class="report-label">'+t('OutOn')+'</span><span class="report-value warn">'+fmtTime(d.ot)+'</span></div>';
    rh+='<div class="report-row"><span class="report-label">'+t('TripAgo')+'</span><span class="report-value">'+fmtTime(d.ts)+'</span></div></div>';
    document.getElementById('efReport').innerHTML=rh;
  } else if(d.ef){
    document.getElementById('efReport').innerHTML='<div class="report-card ok"><div style="font-size:14px;font-weight:600;color:#4aff6a;margin-bottom:4px">'+t('Active')+'</div><div style="font-size:12px;color:#888">'+t('Mon')+'</div></div>';
  } else {
    document.getElementById('efReport').innerHTML='<div class="report-card"><div style="font-size:14px;font-weight:600;color:#888;margin-bottom:4px">'+t('Deact')+'</div><div style="font-size:12px;color:#666">'+t('OffDesc')+'</div></div>';
  }
  applyLang();
}

connectWS();
applyLang();

// Gespeicherte eFuse-Werte in die Eingabefelder laden
fetch('/settings').then(r=>r.json()).then(d=>{
  document.getElementById('efMc').value=d.maxCurrent;
  document.getElementById('efMcV').textContent=d.maxCurrent.toFixed(1)+'A';
  document.getElementById('efMv').value=d.maxVoltage;
  document.getElementById('efMvV').textContent=d.maxVoltage.toFixed(1)+'V';
  document.getElementById('efMnv').value=d.minVoltage;
  document.getElementById('efMnvV').textContent=d.minVoltage.toFixed(1)+'V';
  document.getElementById('efMt').value=d.maxTemperature;
  document.getElementById('efMtV').textContent=d.maxTemperature.toFixed(1)+'°C';
  document.getElementById('efTt').value=d.tripTime;
  document.getElementById('efTtV').textContent=d.tripTime.toFixed(1)+'s';
}).catch(()=>{});
</script>
)rawliteral";

// ============================================================================
// Seite: Dimmer
// ============================================================================
static const char PAGE_DIMMER_BODY[] PROGMEM = R"rawliteral(<div class="card">
<div class="row">
<div class="col"><div class="lbl" id="lblDV">Spannung</div><div class="val" id="dv">0.000<span class="unit">V</span></div></div>
<div class="col"><div class="lbl" id="lblDA">Strom</div><div class="val" id="da">0.000<span class="unit">A</span></div></div>
<div class="col"><div class="lbl" id="lblDW">Leistung</div><div class="val" id="dw">0.000<span class="unit">W</span></div></div>
</div>
</div>
<div class="card">
<div class="gauge-wrap">
<canvas id="gaugeCanvas" width="200" height="150"></canvas>
<div class="gauge-val" id="gaugePct">0%</div>
<div class="gauge-label" id="gaugeLbl">Aktuell</div>
</div>
<div class="dim-sep"></div>
<div class="lbl" id="lblDimmer">Dimmer</div>
<div class="slider-row">
<input type="range" id="dimmerSlider" min="0" max="100" value="100" oninput="dimmerSlide(this.value)">
<span class="slider-val" id="dimmerVal">100%</span>
</div>
</div>
<div class="card">
<div class="sec-title sec-purple" id="lblReg">Regler</div>
<div class="field-group">
<label id="lblRegMode">Modus</label>
<div class="tgl" id="tglRegMode">
<button class="tgl-btn tgl-off" style="flex:1" id="regBtnOff" onclick="setRegMode('off')">OFF</button>
<button class="tgl-btn tgl-off" style="flex:1" id="regBtnCC" onclick="setRegMode('cc')">CC (A)</button>
<button class="tgl-btn tgl-off" style="flex:1" id="regBtnCP" onclick="setRegMode('cp')">CP (W)</button>
</div>
</div>
<div class="field-group" id="fgRegA">
<label id="lblRegA">Soll-Strom (A)</label>
<div class="field-row">
<input type="number" id="regA" step="0.01" min="0">
<span class="cur-val" id="regAV">A</span>
</div>
</div>
<div class="field-group" id="fgRegW">
<label id="lblRegW">Soll-Leistung (W)</label>
<div class="field-row">
<input type="number" id="regW" step="0.01" min="0">
<span class="cur-val" id="regWV">W</span>
</div>
</div>
<button class="btn btn-blue" onclick="saveReg()" id="btnSaveReg">Speichern</button>
<div style="font-size:11px;color:#666" id="lblRegHint">Regler aktiv nur wenn Dimmer EIN und Output AN.</div>
</div>
<button class="btn btn-off" id="btnDimEn" onclick="toggleDimmer()">DIMMER AUS</button>
<button class="btn btn-off" id="btnOutDim" onclick="toggleOutput()">OUTPUT AUS</button>
<div class="by">by HIGHVOLTAGEBEE</div>
)rawliteral";

static const char PAGE_DIMMER_JS[] PROGMEM = R"rawliteral(<script>
let ws, gaugeCtx, wsReconnectTimer = null;
let lastDimSend = 0;
let gaugeDisp = 0, gaugeTarget = 0, gaugeStarted = false;
let currentData = {};
let currentRegMode = 'off';

const TXT = {
de:{DV:"Spannung",DA:"Strom",DW:"Leistung",GaugeLbl:"Aktuell",Dimmer:"Dimmer",
 DimON:"DIMMER EIN",DimOFF:"DIMMER AUS",OUT_ON:"OUTPUT EIN",OUT_OFF:"OUTPUT AUS",
 Reg:"Regler",RegMode:"Modus",RegA:"Soll-Strom (A)",RegW:"Soll-Leistung (W)",
 RegHint:"Regler aktiv nur wenn Dimmer EIN und Output AN."},
en:{DV:"Voltage",DA:"Current",DW:"Power",GaugeLbl:"Current",Dimmer:"Dimmer",
 DimON:"DIMMER ON",DimOFF:"DIMMER OFF",OUT_ON:"OUTPUT ON",OUT_OFF:"OUTPUT OFF",
 Reg:"Regulator",RegMode:"Mode",RegA:"Current setpoint (A)",RegW:"Power setpoint (W)",
 RegHint:"Regulator active only when Dimmer ON and Output ON."}
};

function t(key){ const lang = currentData.lang === 1 ? 'en' : 'de'; return (TXT[lang] && TXT[lang][key]) || key; }

function applyLang(){
  const ids=['lblDV','lblDA','lblDW','gaugeLbl','lblDimmer','lblReg','lblRegMode','lblRegA','lblRegW','btnSaveReg','lblRegHint'];
  const keys=['DV','DA','DW','GaugeLbl','Dimmer','Reg','RegMode','RegA','RegW','Save','RegHint'];
  ids.forEach((id,i)=>{ const el=document.getElementById(id); if(el) el.textContent=t(keys[i]); });
}

function connectWS(){
  if(wsReconnectTimer){ clearTimeout(wsReconnectTimer); wsReconnectTimer=null; }
  try{ if(ws) ws.close(); }catch(e){}
  ws=new WebSocket('ws://'+location.host+'/ws');
  ws.onmessage=function(e){
    try{ const d=JSON.parse(e.data); currentData=d; updateUI(d); }catch(ex){}
  };
  ws.onclose=function(){ wsReconnectTimer=setTimeout(connectWS,2000); };
  ws.onerror=function(){ ws.close(); };
}

function toggleOutput(){ ws.send('OUTPUT:'+(currentData.out?0:1)); }
function toggleDimmer(){ ws.send('DIMMER:EN:'+(currentData.de?0:1)); }

function dimmerSlide(val){
  document.getElementById('dimmerVal').textContent=val+'%';
  if(currentRegMode!=='off') return; // Slider gesperrt wenn Regler aktiv
  const now=Date.now();
  if(ws && ws.readyState===1 && now-lastDimSend>=100){
    ws.send('DIMMER:'+val);
    lastDimSend=now;
  }
}

function syncSliderLock(){
  const locked=currentRegMode!=='off';
  const s=document.getElementById('dimmerSlider');
  s.disabled=locked;
  s.style.opacity=locked?0.35:1;
  document.getElementById('dimmerVal').style.color=locked?'#555':'#fff';
}

function updateUI(d){
  document.getElementById('dv').innerHTML=d.v.toFixed(3)+'<span class="unit">V</span>';
  document.getElementById('da').innerHTML=d.a.toFixed(3)+'<span class="unit">A</span>';
  document.getElementById('dw').innerHTML=d.w.toFixed(3)+'<span class="unit">W</span>';
  const bd=document.getElementById('btnDimEn');
  bd.textContent=d.de?t('DimON'):t('DimOFF');
  bd.className=d.de?'btn btn-on':'btn btn-off';
  const bo=document.getElementById('btnOutDim');
  bo.textContent=d.out?t('OUT_ON'):t('OUT_OFF');
  bo.className=d.out?'btn btn-on':'btn btn-off';
  gaugeTarget=d.dp;
  startGaugeAnim();
  // Regler-Modus live vom Server synchronisieren
  const m=d.cc?'cc':(d.cp?'cp':'off');
  if(m!==currentRegMode) setRegMode(m,false);
  applyLang();
}

function drawGaugeLoop(){
  gaugeDisp+=(gaugeTarget-gaugeDisp)*0.12;
  if(Math.abs(gaugeTarget-gaugeDisp)<0.05) gaugeDisp=gaugeTarget;
  drawGauge(gaugeDisp);
  requestAnimationFrame(drawGaugeLoop);
}

function startGaugeAnim(){
  if(!gaugeStarted){ gaugeStarted=true; requestAnimationFrame(drawGaugeLoop); }
}

function drawGauge(pct){
  if(!gaugeCtx) return;
  const w=200,h=150;
  const cx=w/2,r=72;
  const cy=h-6-r*Math.SQRT1_2;
  gaugeCtx.clearRect(0,0,w,h);
  const startAngle=Math.PI*0.75;
  const endAngle=Math.PI*2.25;
  const range=endAngle-startAngle;
  gaugeCtx.strokeStyle='#222';
  gaugeCtx.lineWidth=12;
  gaugeCtx.lineCap='round';
  gaugeCtx.beginPath();
  gaugeCtx.arc(cx,cy,r,startAngle,endAngle);
  gaugeCtx.stroke();
  const angle=startAngle+range*(pct/100);
  gaugeCtx.strokeStyle='#4a7a4a';
  gaugeCtx.lineWidth=12;
  gaugeCtx.beginPath();
  gaugeCtx.arc(cx,cy,r,startAngle,angle);
  gaugeCtx.stroke();
  document.getElementById('gaugePct').textContent=Math.round(pct)+'%';
}

function setRegMode(mode,send){
  if(send===undefined) send=true;
  currentRegMode=mode;
  document.getElementById('regBtnOff').className='tgl-btn tgl-off'+(mode==='off'?' active':'');
  document.getElementById('regBtnCC').className='tgl-btn tgl-on'+(mode==='cc'?' active':'');
  document.getElementById('regBtnCP').className='tgl-btn tgl-on'+(mode==='cp'?' active':'');
  document.getElementById('fgRegA').style.display=(mode==='cc')?'block':'none';
  document.getElementById('fgRegW').style.display=(mode==='cp')?'block':'none';
  syncSliderLock();
  if(send) saveReg(mode);
}

function saveReg(mode){
  if(mode===undefined) mode=currentRegMode;
  const body='ccMode='+(mode==='cc'?1:0)+
             '&cpMode='+(mode==='cp'?1:0)+
             '&ccSetpoint='+(parseFloat(document.getElementById('regA').value)||0)+
             '&cpSetpoint='+(parseFloat(document.getElementById('regW').value)||0);
  fetch('/settings',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:body}).catch(()=>{});
  document.getElementById('regAV').textContent=(parseFloat(document.getElementById('regA').value)||0).toFixed(2)+'A';
  document.getElementById('regWV').textContent=(parseFloat(document.getElementById('regW').value)||0).toFixed(2)+'W';
}

fetch('/settings').then(r=>r.json()).then(d=>{
  document.getElementById('regA').value=d.ccSetpoint;
  document.getElementById('regW').value=d.cpSetpoint;
  document.getElementById('regAV').textContent=parseFloat(d.ccSetpoint).toFixed(2)+'A';
  document.getElementById('regWV').textContent=parseFloat(d.cpSetpoint).toFixed(2)+'W';
  setRegMode(d.ccMode?'cc':(d.cpMode?'cp':'off'),false);
}).catch(()=>{});

gaugeCtx=document.getElementById('gaugeCanvas').getContext('2d');
connectWS();
applyLang();
</script>
)rawliteral";

// ============================================================================
// Seite: Settings
// ============================================================================
static const char PAGE_SETTINGS_BODY[] PROGMEM = R"rawliteral(<div class="card">
<div class="sec-title sec-blue" id="lblSecWLAN">WLAN</div>
<div class="field-group">
<label id="lblSSID">AP SSID</label>
<div class="field-row">
<input type="text" id="sSsid" placeholder="SSID">
</div>
</div>
<div class="field-group">
<label id="lblPass">AP Passwort</label>
<div class="field-row">
<input type="text" id="sPass" placeholder="Passwort">
</div>
</div>
<div class="field-group">
<label id="lblMdns">DNS Server (meter.local)</label>
<div class="tgl" id="tglMdns">
<button class="tgl-btn tgl-on" onclick="sendTgl('mdnsEnabled',1,'tglMdns',true)">ON</button>
<button class="tgl-btn tgl-off" onclick="sendTgl('mdnsEnabled',0,'tglMdns',false)">OFF</button>
</div>
</div>
<div class="sec-title sec-blue" id="lblSecSystem">System</div>
<div class="field-group">
<label id="lblPWM">PWM Frequenz (Hz)</label>
<div class="field-row">
<input type="number" id="sPwm" min="100" max="30000" step="100" placeholder="1000">
</div>
</div>
<div class="field-group">
<label id="lblDefOut">Default Output</label>
<div class="tgl" id="tglDefOut">
<button class="tgl-btn tgl-on" onclick="sendTgl('defaultOutputOn',1,'tglDefOut',true)">ON</button>
<button class="tgl-btn tgl-off" onclick="sendTgl('defaultOutputOn',0,'tglDefOut',false)">OFF</button>
</div>
</div>
<div class="field-group">
<label id="lblLang">Sprache / Language</label>
<div class="tgl" id="tglLang">
<button class="tgl-btn tgl-on" style="font-size:13px" onclick="sendTgl('language',0,'tglLang',true)">DEUTSCH</button>
<button class="tgl-btn tgl-off" style="font-size:13px" onclick="sendTgl('language',1,'tglLang',false)">ENGLISH</button>
</div>
</div>
<div class="sec-title sec-green" id="lblSecSmooth">Glättung</div>
<div class="field-group">
<label id="lblSmoothV">Spannungs-Glättung (0-100%)</label>
<div class="field-row">
<input type="number" id="sSmoothV" min="0" max="100" step="5" placeholder="30">
<span class="cur-val" id="sSmoothVV">30%</span>
</div>
</div>
<div class="field-group">
<label id="lblSmoothA">Strom-Glättung (0-100%)</label>
<div class="field-row">
<input type="number" id="sSmoothA" min="0" max="100" step="5" placeholder="30">
<span class="cur-val" id="sSmoothAV">30%</span>
</div>
</div>
<div class="sec-title sec-purple" id="lblSecLED">Status LED</div>
<div class="field-group">
<label id="lblLed">Status LED</label>
<div class="tgl" id="tglLed">
<button class="tgl-btn tgl-on" onclick="sendTgl('ledEnabled',1,'tglLed',true)">ON</button>
<button class="tgl-btn tgl-off" onclick="sendTgl('ledEnabled',0,'tglLed',false)">OFF</button>
</div>
</div>
<div class="field-group">
<label id="lblLedB">LED Max Helligkeit (%)</label>
<div class="field-row">
<input type="number" id="sLedB" min="0" max="100" step="5" placeholder="50">
</div>
</div>
<div class="sec-title sec-orange" id="lblSecBtn">Boot Button</div>
<div class="field-group">
<label id="lblBtn">Boot Button</label>
<div class="tgl" id="tglBtn">
<button class="tgl-btn tgl-on" onclick="sendTgl('buttonEnabled',1,'tglBtn',true)">ON</button>
<button class="tgl-btn tgl-off" onclick="sendTgl('buttonEnabled',0,'tglBtn',false)">OFF</button>
</div>
</div>
<div class="sec-title sec-orange" id="lblSecLog">Logger</div>
<div class="field-group">
<label id="lblLogInt2">Log Intervall (s)</label>
<div class="field-row">
<input type="number" id="sLogInt" min="0.1" max="3600" step="0.1" placeholder="1">
</div>
</div>
<div class="field-group">
<label id="lblAutoLog">Auto-Start Logging (Boot)</label>
<div class="tgl" id="tglAutoLog">
<button class="tgl-btn tgl-on" onclick="sendTgl('autoLog',1,'tglAutoLog',true)">ON</button>
<button class="tgl-btn tgl-off" onclick="sendTgl('autoLog',0,'tglAutoLog',false)">OFF</button>
</div>
</div>
<button class="btn btn-blue" onclick="window.open('/calibrate','_blank')" id="btnCalib">Kalibrierung</button>
<button class="btn btn-blue" onclick="saveSettings()" id="btnSaveSet">Speichern</button>
<button class="btn btn-red" onclick="resetSettings()" id="btnResetSet">Reset</button>
</div>
<div class="card">
<div class="lbl" id="lblInfo">Geräteinfo</div>
<table class="info-table" style="margin-top:6px">
<tr><td id="lblFW">Firmware</td><td id="diVer">XT90 PM v1.3</td></tr>
<tr><td id="lblSW">Software Version</td><td>1.3</td></tr>
<tr><td id="lblLat">Webserver Latenz</td><td id="diLat">0ms</td></tr>
<tr><td id="lblHz">Updates</td><td id="diHz">0 Hz</td></tr>
<tr><td id="lblUp2">Betriebszeit</td><td id="diUp">0s</td></tr>
<tr><td id="lblIP">IP Adresse</td><td id="diIP">-</td></tr>
</table>
</div>
<div class="by">by HIGHVOLTAGEBEE</div>
)rawliteral";

static const char PAGE_SETTINGS_JS[] PROGMEM = R"rawliteral(<script>
let ws, wsReconnectTimer = null;
let currentData = {};
let lastMsgTime = 0, hzCount = 0, hzTimer = 0;

const TXT = {
de:{SSID:"AP SSID",Pass:"AP Passwort",Mdns:"DNS Server (meter.local)",
 DefOut:"Default Output",Lang:"Sprache / Language",PWM:"PWM Frequenz (Hz)",
 SmoothV:"Spannungs-Glättung (0-100%)",SmoothA:"Strom-Glättung (0-100%)",
 Led:"Status LED",LedB:"LED Max Helligkeit (%)",Btn:"Boot Button",
 LogInt2:"Log Intervall (s)",AutoLog:"Auto-Start Logging (Boot)",
 Calib:"Kalibrierung",SaveSet:"Speichern",ResetSet:"Reset",Info:"Geräteinfo",
 FW:"Firmware",SW:"Software Version",Lat:"Webserver Latenz",Hz:"Updates",
 Up2:"Betriebszeit",IP:"IP Adresse",
 SecWLAN:"WLAN",SecSystem:"System",SecSmooth:"Glättung",SecLED:"Status LED",
 SecBtn:"Boot Button",SecLog:"Logger"},
en:{SSID:"AP SSID",Pass:"AP Password",Mdns:"DNS Server (meter.local)",
 DefOut:"Default Output",Lang:"Language",PWM:"PWM Frequency (Hz)",
 SmoothV:"Voltage smoothing (0-100%)",SmoothA:"Current smoothing (0-100%)",
 Led:"Status LED",LedB:"LED Max Brightness (%)",Btn:"Boot Button",
 LogInt2:"Log Interval (s)",AutoLog:"Auto-Start Logging (Boot)",
 Calib:"Calibration",SaveSet:"Save",ResetSet:"Reset",Info:"Device Info",
 FW:"Firmware",SW:"Software Version",Lat:"Webserver Latency",Hz:"Updates",
 Up2:"Uptime",IP:"IP Address",
 SecWLAN:"WLAN",SecSystem:"System",SecSmooth:"Smoothing",SecLED:"Status LED",
 SecBtn:"Boot Button",SecLog:"Logger"}
};

function t(key){ const lang = currentData.lang === 1 ? 'en' : 'de'; return (TXT[lang] && TXT[lang][key]) || key; }

function applyLang(){
  const ids=['lblSSID','lblPass','lblMdns','lblDefOut','lblLang','lblPWM',
    'lblSmoothV','lblSmoothA','lblLed','lblLedB','lblBtn','lblLogInt2','lblAutoLog',
    'btnCalib','btnSaveSet','btnResetSet','lblInfo','lblFW','lblSW','lblLat','lblHz','lblUp2','lblIP',
    'lblSecWLAN','lblSecSystem','lblSecSmooth','lblSecLED','lblSecBtn','lblSecLog'];
  const keys=['SSID','Pass','Mdns','DefOut','Lang','PWM',
    'SmoothV','SmoothA','Led','LedB','Btn','LogInt2','AutoLog',
    'Calib','SaveSet','ResetSet','Info','FW','SW','Lat','Hz','Up2','IP',
    'SecWLAN','SecSystem','SecSmooth','SecLED','SecBtn','SecLog'];
  ids.forEach((id,i)=>{ const el=document.getElementById(id); if(el) el.textContent=t(keys[i]); });
}

function fmtTime(s){
  const h=Math.floor(s/3600),m=Math.floor((s%3600)/60),sec=s%60;
  return (h>0?h+'h ':'')+(m>0?m+'m ':'')+sec+'s';
}

function connectWS(){
  if(wsReconnectTimer){ clearTimeout(wsReconnectTimer); wsReconnectTimer=null; }
  try{ if(ws) ws.close(); }catch(e){}
  ws=new WebSocket('ws://'+location.host+'/ws');
  ws.onmessage=function(e){
    try{
      const d=JSON.parse(e.data);
      currentData=d;
      const now=Date.now();
      if(now-hzTimer>1000){
        document.getElementById('diHz').textContent=hzCount+' Hz';
        hzCount=0; hzTimer=now;
      }
      hzCount++;
      if(lastMsgTime>0) document.getElementById('diLat').textContent=(now-lastMsgTime)+'ms';
      lastMsgTime=now;
      document.getElementById('diUp').textContent=fmtTime(d.up);
      document.getElementById('diIP').textContent=location.host;
      applyLang();
    }catch(ex){}
  };
  ws.onclose=function(){ wsReconnectTimer=setTimeout(connectWS,2000); };
  ws.onerror=function(){ ws.close(); };
}

function setTgl(id,on){
  const b=document.getElementById(id);
  if(!b) return;
  b.querySelector('.tgl-on').className='tgl-btn tgl-on'+(on?' active':'');
  b.querySelector('.tgl-off').className='tgl-btn tgl-off'+(on?'':' active');
}

function sendTgl(param,val,id,on){
  if(param==='autoLog'){
    fetch('/logging',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'state='+(val?1:0)}).catch(()=>{});
  } else {
    fetch('/settings',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:param+'='+val}).catch(()=>{});
  }
  setTgl(id,on);
}

function loadSettings(){
  fetch('/settings').then(r=>r.json()).then(d=>{
    document.getElementById('sSsid').value=d.apSSID||'';
    document.getElementById('sPass').value=d.apPassword||'';
    document.getElementById('sPwm').value=d.pwmFrequency;
    document.getElementById('sSmoothV').value=Math.round(d.vSmooth*100);
    document.getElementById('sSmoothVV').textContent=Math.round(d.vSmooth*100)+'%';
    document.getElementById('sSmoothA').value=Math.round(d.cSmooth*100);
    document.getElementById('sSmoothAV').textContent=Math.round(d.cSmooth*100)+'%';
    document.getElementById('sLedB').value=d.ledMaxBrightness;
    document.getElementById('sLogInt').value=d.logInterval;
    setTgl('tglDefOut',d.defaultOutputOn);
    setTgl('tglLang',!d.language);
    setTgl('tglLed',d.ledEnabled);
    setTgl('tglBtn',d.buttonEnabled);
    setTgl('tglAutoLog',d.loggingEnabled);
    setTgl('tglMdns',d.mdnsEnabled);
  }).catch(()=>{});
}

function saveSettings(){
  fetch('/settings',{
    method:'POST',
    headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:'apSSID='+encodeURIComponent(document.getElementById('sSsid').value)+
         '&apPassword='+encodeURIComponent(document.getElementById('sPass').value)+
         '&pwmFrequency='+document.getElementById('sPwm').value+
         '&vSmooth='+(parseFloat(document.getElementById('sSmoothV').value)||0)/100+
         '&cSmooth='+(parseFloat(document.getElementById('sSmoothA').value)||0)/100+
         '&ledMaxBrightness='+document.getElementById('sLedB').value+
         '&logInterval='+document.getElementById('sLogInt').value
  }).catch(()=>{});
  document.getElementById('sSmoothVV').textContent=document.getElementById('sSmoothV').value+'%';
  document.getElementById('sSmoothAV').textContent=document.getElementById('sSmoothA').value+'%';
}

function resetSettings(){ fetch('/reset',{method:'POST'}).then(()=>{location.reload();}).catch(()=>{}); }

connectWS();
loadSettings();
applyLang();
</script>
)rawliteral";

// ============================================================================
// Kalibrierungs-Wizard (Sprache folgt der Einstellung, per /settings geladen)
// ============================================================================
static const char PAGE_CAL[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="de">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<title>Kalibrierung</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{background:#111;color:#fff;font-family:-apple-system,BlinkMacSystemFont,'Segoe UI',Roboto,sans-serif;padding:10px;max-width:480px;margin:0 auto}
h2{font-size:18px;margin:14px 0 10px 0;font-weight:600}
.card{background:#1a1a1a;border-radius:12px;padding:16px;margin-bottom:10px}
.lbl{font-size:11px;color:#888;text-transform:uppercase;letter-spacing:.5px;margin-bottom:4px}
.val{font-size:30px;font-weight:300;margin:6px 0}
.unit{font-size:14px;color:#666;margin-left:2px}
.btn{display:block;width:100%;padding:14px;border:none;border-radius:10px;font-size:15px;font-weight:600;cursor:pointer;margin:6px 0;color:#fff}
.btn-blue{background:#2a3a6b}.btn-green{background:#2a6b2a}.btn-gray{background:#333}.btn:disabled{opacity:.4;cursor:default}
select,input{width:100%;padding:12px;background:#222;border:1px solid #333;border-radius:8px;color:#fff;font-size:16px;margin:4px 0}
.step{display:none}.step.active{display:block}
.steps{display:flex;gap:6px;margin:12px 0}
.stepdot{flex:1;height:4px;background:#333;border-radius:2px}.stepdot.active{background:#4a6b4a}
.msg{padding:10px;border-radius:8px;font-size:13px;margin:8px 0;display:none}
.msg.ok{display:block;background:#1a2b1a;color:#4aff6a}
.msg.err{display:block;background:#2b1a1a;color:#ff6a6a}
.hint{font-size:12px;color:#888;line-height:1.5;margin:8px 0}
a{color:#6a9aff}
.by{margin-top:16px;text-align:center;font-size:11px;color:#555;letter-spacing:1px}
</style>
</head>
<body>
<h2 id="calTitle">Kalibrierung</h2>
<div class="steps">
<div class="stepdot active" id="d1"></div>
<div class="stepdot" id="d2"></div>
<div class="stepdot" id="d3"></div>
</div>

<div class="step active" id="s1">
<div class="card">
<div class="lbl" id="lblChan">Kanal wählen</div>
<select id="chan">
<option value="v" id="optV">Spannung (V)</option>
<option value="a" id="optA">Strom (A)</option>
</select>
<div class="lbl" style="margin-top:10px" id="lblRef">Referenzwert (gemessen mit Referenzgerät)</div>
<input type="number" id="ref" step="0.001" min="0" placeholder="z.B. 24.000">
<div class="msg" id="msg1"></div>
<button class="btn btn-blue" onclick="toStep2()" id="btnNext">Weiter</button>
</div>
</div>

<div class="step" id="s2">
<div class="card">
<div class="lbl" id="lblRaw">Aktueller Roh-Messwert (unkalibriert)</div>
<div class="val" id="live">0<span class="unit" id="liveUnit">V</span></div>
<div class="hint" id="hintMeas"></div>
<button class="btn btn-green" id="btnMeas" onclick="measure()">Messung starten</button>
<div class="msg" id="msgMeas"></div>
</div>
<button class="btn btn-gray" onclick="toStep1()" id="btnBack">Zurück</button>
</div>

<div class="step" id="s3">
<div class="card">
<div class="lbl" id="lblResult">Ergebnis</div>
<table style="width:100%;font-size:14px">
<tr><td style="color:#888;padding:4px 0" id="rRefL">Referenzwert</td><td style="text-align:right" id="rRef">-</td></tr>
<tr><td style="color:#888;padding:4px 0" id="rMeasL">Gemittelt gemessen</td><td style="text-align:right" id="rMeas">-</td></tr>
<tr><td style="color:#888;padding:4px 0" id="rFacL">Kalibrierfaktor</td><td style="text-align:right" id="rFac">-</td></tr>
</table>
<div class="msg" id="msg3"></div>
<button class="btn btn-green" onclick="saveCal()" id="btnSaveCal">Speichern im EEPROM</button>
<button class="btn btn-blue" onclick="toStep2()" id="btnMeasAgain">Neu messen</button>
</div>
<button class="btn btn-gray" onclick="resetCal()" id="btnResetCal">Kalibrierung zurücksetzen (Faktor 1.0)</button>
</div>

<div style="text-align:center;margin-top:14px;font-size:13px">
<a href="/" id="linkBack">Zurück zum Hauptmenü</a>
</div>
<div class="by">by HIGHVOLTAGEBEE</div>

<script>
let chan='v', refVal=0, measured=0, factor=1, pollTimer=null, samples=[], measuring=false;
let calLang=0;

const CALT={
de:{title:"Kalibrierung",chan:"Kanal wählen",optV:"Spannung (V)",optA:"Strom (A)",
 ref:"Referenzwert (gemessen mit Referenzgerät)",refPh:"z.B. 24.000",errRef:"Bitte gültigen Referenzwert eingeben.",
 next:"Weiter",raw:"Aktueller Roh-Messwert (unkalibriert)",
 hint:"Referenzquelle anschließen und stabilisieren. Der angezeigte Rohwert wird für 3 Sekunden gemittelt, wenn du „Messung starten“ drückst.",
 meas:"Messung starten",measuring:"Messe... (3s)",back:"Zurück",result:"Ergebnis",
 rRef:"Referenzwert",rMeas:"Gemittelt gemessen",rFac:"Kalibrierfaktor",
 save:"Speichern im EEPROM",again:"Neu messen",reset:"Kalibrierung zurücksetzen (Faktor 1.0)",
 backMain:"Zurück zum Hauptmenü",errSamples:"Zu wenige Messwerte, bitte erneut versuchen.",
 okSave:"Kalibrierung gespeichert im EEPROM.",errSave:"Fehler beim Speichern.",errConn:"Verbindungsfehler.",
 okReset:"Kalibrierfaktor auf 1.0 zurückgesetzt."},
en:{title:"Calibration",chan:"Select channel",optV:"Voltage (V)",optA:"Current (A)",
 ref:"Reference value (measured with reference instrument)",refPh:"e.g. 24.000",errRef:"Please enter a valid reference value.",
 next:"Next",raw:"Current raw value (uncalibrated)",
 hint:"Connect the reference source and let it stabilize. The raw value shown is averaged over 3 seconds when you press \"Start measurement\".",
 meas:"Start measurement",measuring:"Measuring... (3s)",back:"Back",result:"Result",
 rRef:"Reference value",rMeas:"Averaged measurement",rFac:"Calibration factor",
 save:"Save to EEPROM",again:"Measure again",reset:"Reset calibration (factor 1.0)",
 backMain:"Back to main menu",errSamples:"Too few samples, please try again.",
 okSave:"Calibration saved to EEPROM.",errSave:"Error while saving.",errConn:"Connection error.",
 okReset:"Calibration factor reset to 1.0."}
};

function ct(k){ return CALT[calLang===0?'de':'en'][k]; }

function applyCalLang(){
  const T=CALT[calLang===0?'de':'en'];
  const set=(id,txt)=>{const e=document.getElementById(id); if(e) e.textContent=txt;};
  set('calTitle',T.title); document.title=T.title;
  set('lblChan',T.chan); set('optV',T.optV); set('optA',T.optA);
  set('lblRef',T.ref); document.getElementById('ref').placeholder=T.refPh;
  set('btnNext',T.next); set('lblRaw',T.raw); set('hintMeas',T.hint);
  if(!measuring) set('btnMeas',T.meas);
  set('btnBack',T.back); set('lblResult',T.result);
  set('rRefL',T.rRef); set('rMeasL',T.rMeas); set('rFacL',T.rFac);
  set('btnSaveCal',T.save); set('btnMeasAgain',T.again); set('btnResetCal',T.reset);
  set('linkBack',T.backMain);
}

fetch('/settings').then(r=>r.json()).then(d=>{ calLang=d.language||0; applyCalLang(); }).catch(()=>{ applyCalLang(); });

function show(n) {
  [1,2,3].forEach(i=>{
    document.getElementById('s'+i).className='step'+(i===n?' active':'');
    document.getElementById('d'+i).className='stepdot'+(i<=n?' active':'');
  });
  if (pollTimer) { clearInterval(pollTimer); pollTimer=null; }
  if (n===2) startPoll();
}

function unit() { return chan==='v' ? 'V' : 'A'; }
function prec() { return chan==='v' ? 3 : 4; }

function toStep1() { show(1); }
function toStep2() {
  chan = document.getElementById('chan').value;
  refVal = parseFloat(document.getElementById('ref').value);
  const m1 = document.getElementById('msg1');
  if (!(refVal > 0)) { m1.className='msg err'; m1.textContent=ct('errRef'); return; }
  m1.className='msg'; m1.style.display='none';
  document.getElementById('liveUnit').textContent = unit();
  show(2);
}

function startPoll() {
  pollTimer = setInterval(()=>{
    fetch('/caldata').then(r=>r.json()).then(d=>{
      const v = chan==='v' ? d.v : d.a;
      document.getElementById('live').innerHTML = v.toFixed(prec()) + '<span class="unit">' + unit() + '</span>';
      if (measuring) samples.push(v);
    }).catch(()=>{});
  }, 100);
}

function measure() {
  samples = [];
  measuring = true;
  const btn = document.getElementById('btnMeas');
  btn.disabled = true;
  btn.textContent = ct('measuring');
  const m = document.getElementById('msgMeas');
  m.className='msg'; m.style.display='none';
  setTimeout(()=>{
    measuring = false;
    btn.disabled = false;
    btn.textContent = ct('meas');
    if (samples.length < 3) { m.className='msg err'; m.textContent=ct('errSamples'); return; }
    measured = samples.reduce((a,b)=>a+b,0)/samples.length;
    factor = refVal / measured;
    document.getElementById('rRef').textContent = refVal.toFixed(prec()) + ' ' + unit();
    document.getElementById('rMeas').textContent = measured.toFixed(prec()) + ' ' + unit();
    document.getElementById('rFac').textContent = factor.toFixed(6);
    show(3);
  }, 3000);
}

function saveCal() {
  fetch('/calsave', {
    method:'POST',
    headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:'chan='+chan+'&factor='+factor
  }).then(r=>{
    const m = document.getElementById('msg3');
    if (r.ok) { m.className='msg ok'; m.textContent=ct('okSave'); }
    else { m.className='msg err'; m.textContent=ct('errSave'); }
  }).catch(()=>{
    const m = document.getElementById('msg3');
    m.className='msg err'; m.textContent=ct('errConn');
  });
}

function resetCal() {
  fetch('/calsave', {
    method:'POST',
    headers:{'Content-Type':'application/x-www-form-urlencoded'},
    body:'chan='+chan+'&factor=1.0'
  }).then(()=>{
    const m = document.getElementById('msg3');
    m.className='msg ok'; m.textContent=ct('okReset');
  }).catch(()=>{});
}
</script>
</body>
</html>
)rawliteral";

// ============================================================================
// Seiten-Export
// ============================================================================
String webPageLive()     { return assemblePage(0, PAGE_LIVE_BODY,     PAGE_LIVE_JS); }
String webPageLogger()   { return assemblePage(1, PAGE_LOGGER_BODY,   PAGE_LOGGER_JS); }
String webPageEfuse()    { return assemblePage(2, PAGE_EFUSE_BODY,    PAGE_EFUSE_JS); }
String webPageDimmer()   { return assemblePage(3, PAGE_DIMMER_BODY,   PAGE_DIMMER_JS); }
String webPageSettings() { return assemblePage(4, PAGE_SETTINGS_BODY, PAGE_SETTINGS_JS); }

String webPageCalibrate()
{
  String page;
  page.reserve(12000);
  page += FPSTR(PAGE_CAL);
  return page;
}
