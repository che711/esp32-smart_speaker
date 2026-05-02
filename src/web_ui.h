#pragma once

const char WEB_UI[] PROGMEM = R"rawhtml(<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Smart Speaker</title>
<style>
:root{
  --bg:#111;--s1:#1c1c1c;--s2:#252525;--bd:#303030;
  --ac:#f0a500;--ac2:#c8880a;--tx:#e5e5e5;--mu:#777;
  --ok:#2ecc71;--er:#e74c3c;--r:10px;
}
*{box-sizing:border-box;margin:0;padding:0;}
body{background:var(--bg);color:var(--tx);font-family:'Segoe UI',system-ui,sans-serif;padding:0;min-height:100vh;}
.wrap{max-width:500px;margin:0 auto;padding:16px;}

/* ── Header ─────────────────────────────── */
.hdr{display:flex;justify-content:space-between;align-items:center;padding-bottom:14px;border-bottom:1px solid var(--bd);margin-bottom:16px;}
.hdr h1{font-size:18px;font-weight:700;letter-spacing:.03em;}.hdr h1 em{color:var(--ac);font-style:normal;}
.dot{width:9px;height:9px;border-radius:50%;background:var(--mu);transition:.3s;}
.dot.live{background:var(--ok);box-shadow:0 0 7px var(--ok);}

/* ── Now playing ────────────────────────── */
.player{background:var(--s1);border:1px solid var(--bd);border-radius:var(--r);padding:18px;margin-bottom:14px;}
.src-tag{font-size:10px;font-weight:600;letter-spacing:.12em;text-transform:uppercase;color:var(--mu);margin-bottom:8px;}
.title{font-size:20px;font-weight:700;color:var(--ac);margin-bottom:3px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;}
.subtitle{font-size:12px;color:var(--mu);min-height:16px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;}

.ctrls{display:flex;align-items:center;gap:10px;margin-top:16px;}
.cbtn{background:var(--s2);border:1px solid var(--bd);border-radius:50%;width:42px;height:42px;display:flex;align-items:center;justify-content:center;cursor:pointer;color:var(--tx);font-size:15px;transition:.15s;}
.cbtn:hover{border-color:var(--ac);color:var(--ac);}
.cbtn.play{background:var(--ac);border-color:var(--ac);color:#000;width:52px;height:52px;font-size:20px;}
.cbtn.play:hover{background:var(--ac2);}
.cbtn.play.off{background:var(--s2);border-color:var(--bd);color:var(--tx);}

.vol{display:flex;align-items:center;gap:8px;margin-top:14px;}
.vi{color:var(--mu);font-size:13px;}
input[type=range]{flex:1;appearance:none;height:4px;background:var(--bd);border-radius:2px;outline:none;cursor:pointer;}
input[type=range]::-webkit-slider-thumb{appearance:none;width:16px;height:16px;background:var(--ac);border-radius:50%;cursor:pointer;}
input[type=range]::-moz-range-thumb{width:16px;height:16px;background:var(--ac);border:none;border-radius:50%;}
.vv{font-size:12px;color:var(--mu);min-width:22px;text-align:right;}

/* ── Tabs ───────────────────────────────── */
.tabs{display:flex;background:var(--s1);border:1px solid var(--bd);border-radius:8px;padding:3px;margin-bottom:14px;gap:3px;}
.tab{flex:1;text-align:center;padding:7px;border-radius:6px;cursor:pointer;font-size:12px;font-weight:600;color:var(--mu);border:none;background:none;transition:.15s;}
.tab.on{background:var(--ac);color:#000;}

/* ── Panel ──────────────────────────────── */
.pnl{display:none;}.pnl.on{display:block;}

/* ── Shared list ────────────────────────── */
.sh{display:flex;align-items:center;justify-content:space-between;margin-bottom:9px;}
.sht{font-size:10px;font-weight:700;letter-spacing:.1em;text-transform:uppercase;color:var(--mu);}
.mini{background:none;border:1px solid var(--bd);border-radius:6px;color:var(--mu);font-size:11px;padding:3px 9px;cursor:pointer;transition:.15s;}
.mini:hover{border-color:var(--ac);color:var(--ac);}

.lst{background:var(--s1);border:1px solid var(--bd);border-radius:var(--r);overflow:hidden;margin-bottom:12px;}
.itm{display:flex;align-items:center;padding:10px 13px;border-bottom:1px solid var(--bd);cursor:pointer;transition:.12s;gap:9px;}
.itm:last-child{border-bottom:none;}
.itm:hover{background:var(--s2);}
.itm.cur{background:#1f1500;border-left:3px solid var(--ac);}
.itm.cur .iname{color:var(--ac);}
.inum{font-size:11px;color:var(--mu);min-width:16px;}
.iname{flex:1;font-size:13px;overflow:hidden;white-space:nowrap;text-overflow:ellipsis;}
.isub{font-size:11px;color:var(--mu);}
.idel{background:none;border:none;color:var(--mu);cursor:pointer;padding:2px 6px;font-size:14px;border-radius:4px;transition:.12s;}
.idel:hover{color:var(--er);}
.empty{padding:18px;text-align:center;color:var(--mu);font-size:13px;}

/* ── Add form ───────────────────────────── */
.form{background:var(--s1);border:1px solid var(--bd);border-radius:var(--r);padding:14px;margin-bottom:12px;display:none;}
.form.on{display:block;}
.fr{margin-bottom:9px;}
.fr label{font-size:10px;color:var(--mu);display:block;margin-bottom:3px;text-transform:uppercase;letter-spacing:.06em;}
.fr input{width:100%;background:var(--s2);border:1px solid var(--bd);border-radius:6px;padding:7px 9px;color:var(--tx);font-size:12px;outline:none;transition:.15s;}
.fr input:focus{border-color:var(--ac);}
.fadd{background:var(--ac);border:none;border-radius:6px;color:#000;font-size:12px;font-weight:700;padding:7px 16px;cursor:pointer;}
.fadd:hover{background:var(--ac2);}

/* ── Upload zone ────────────────────────── */
.upload-zone{border:2px dashed var(--bd);border-radius:var(--r);padding:30px 20px;text-align:center;margin-bottom:12px;cursor:pointer;transition:.2s;background:var(--s1);}
.upload-zone:hover,.upload-zone.drag{border-color:var(--ac);background:#1a1200;}
.uz-icon{font-size:36px;margin-bottom:10px;}
.uz-title{font-size:14px;font-weight:600;margin-bottom:4px;}
.uz-sub{font-size:12px;color:var(--mu);}
#fileInput{display:none;}

.progress-wrap{background:var(--s1);border:1px solid var(--bd);border-radius:var(--r);padding:14px;margin-bottom:12px;display:none;}
.progress-wrap.on{display:block;}
.pname{font-size:12px;color:var(--mu);margin-bottom:8px;overflow:hidden;white-space:nowrap;text-overflow:ellipsis;}
.pbar-bg{background:var(--bd);border-radius:3px;height:8px;overflow:hidden;}
.pbar{background:var(--ac);height:100%;width:0%;transition:width .15s;border-radius:3px;}
.ppct{font-size:12px;color:var(--ac);text-align:right;margin-top:4px;}

/* ── Info grid ──────────────────────────── */
.igrid{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:12px;}
.icard{background:var(--s1);border:1px solid var(--bd);border-radius:var(--r);padding:12px;}
.il{font-size:10px;color:var(--mu);text-transform:uppercase;letter-spacing:.07em;margin-bottom:3px;}
.iv{font-size:13px;font-weight:600;}

/* ── Toast ──────────────────────────────── */
.toast{position:fixed;bottom:20px;right:20px;background:var(--s2);border:1px solid var(--bd);border-radius:8px;padding:9px 15px;font-size:12px;opacity:0;transform:translateY(8px);transition:.25s;pointer-events:none;z-index:99;}
.toast.show{opacity:1;transform:none;}
.toast.ok{border-color:var(--ok);color:var(--ok);}
.toast.er{border-color:var(--er);color:var(--er);}
</style>
</head>
<body>
<div class="wrap">

<!-- Header -->
<div class="hdr">
  <h1>Smart <em>Speaker</em></h1>
  <div class="dot" id="dot"></div>
</div>

<!-- Player -->
<div class="player">
  <div class="src-tag" id="srcTag">Radio</div>
  <div class="title" id="npTitle">—</div>
  <div class="subtitle" id="npSub">Выбери станцию или файл</div>
  <div class="ctrls">
    <button class="cbtn" onclick="cmd('/prev')">&#9664;&#9664;</button>
    <button class="cbtn play off" id="btnPlay" onclick="togglePlay()">&#9654;</button>
    <button class="cbtn" onclick="cmd('/next')">&#9654;&#9654;</button>
  </div>
  <div class="vol">
    <span class="vi">&#128264;</span>
    <input type="range" id="volR" min="0" max="21" value="12" oninput="setVol(this.value)">
    <span class="vv" id="volV">12</span>
  </div>
</div>

<!-- Tabs -->
<div class="tabs">
  <button class="tab on" onclick="tab('radio',this)">📻 Радио</button>
  <button class="tab" onclick="tab('files',this)">🎵 Файлы</button>
  <button class="tab" onclick="tab('upload',this)">⬆ Загрузка</button>
  <button class="tab" onclick="tab('info',this)">⚙ Инфо</button>
</div>

<!-- Radio panel -->
<div class="pnl on" id="pnl-radio">
  <div class="sh">
    <span class="sht">Радиостанции</span>
    <button class="mini" onclick="toggleForm('stForm')">+ Добавить</button>
  </div>
  <div class="form" id="stForm">
    <div class="fr"><label>Название</label><input type="text" id="stName" placeholder="My Station"></div>
    <div class="fr"><label>URL потока (MP3/AAC)</label><input type="url" id="stUrl" placeholder="http://..."></div>
    <button class="fadd" onclick="addStation()">Добавить</button>
  </div>
  <div class="lst" id="stList"><div class="empty">Загрузка...</div></div>
</div>

<!-- Files panel -->
<div class="pnl" id="pnl-files">
  <div class="sh"><span class="sht">Файлы на устройстве</span></div>
  <div class="lst" id="fileList"><div class="empty">Загрузка...</div></div>
</div>

<!-- Upload panel -->
<div class="pnl" id="pnl-upload">
  <div class="upload-zone" id="dropZone" onclick="document.getElementById('fileInput').click()"
       ondragover="event.preventDefault();this.classList.add('drag')"
       ondragleave="this.classList.remove('drag')"
       ondrop="onDrop(event)">
    <div class="uz-icon">📁</div>
    <div class="uz-title">Перетащи MP3 сюда</div>
    <div class="uz-sub">или нажми для выбора файла<br>MP3 · AAC · WAV · FLAC (макс. 8 МБ)</div>
  </div>
  <input type="file" id="fileInput" accept=".mp3,.aac,.wav,.flac" onchange="startUpload(this.files[0])">

  <div class="progress-wrap" id="progWrap">
    <div class="pname" id="progName"></div>
    <div class="pbar-bg"><div class="pbar" id="pbar"></div></div>
    <div class="ppct" id="ppct">0%</div>
  </div>

  <div class="sh" style="margin-top:4px"><span class="sht">Загруженные файлы</span></div>
  <div class="lst" id="uploadedList"><div class="empty">Нет файлов</div></div>
</div>

<!-- Info panel -->
<div class="pnl" id="pnl-info">
  <div class="sh"><span class="sht">Система</span></div>
  <div class="igrid">
    <div class="icard"><div class="il">IP адрес</div><div class="iv" id="iIp">—</div></div>
    <div class="icard"><div class="il">Wi-Fi сигнал</div><div class="iv" id="iRssi">—</div></div>
    <div class="icard"><div class="il">Свободно RAM</div><div class="iv" id="iRam">—</div></div>
    <div class="icard"><div class="il">Uptime</div><div class="iv" id="iUp">—</div></div>
    <div class="icard"><div class="il">Хранилище</div><div class="iv" id="iFs">—</div></div>
    <div class="icard"><div class="il">Файлов</div><div class="iv" id="iFiles">—</div></div>
  </div>
</div>

</div><!-- .wrap -->
<div class="toast" id="toast"></div>

<script>
// ── State ────────────────────────────────────────────────────
let S = { playing:false, source:'radio', title:'', sub:'',
          stIdx:-1, fileIdx:-1, vol:12 };
let ws, volT, activeTab='radio';

// ── WebSocket ────────────────────────────────────────────────
function connectWS(){
  ws = new WebSocket('ws://'+location.host+'/ws');
  ws.onopen  = ()=>{ document.getElementById('dot').classList.add('live'); };
  ws.onclose = ()=>{ document.getElementById('dot').classList.remove('live'); setTimeout(connectWS,2500); };
  ws.onmessage = e=>{ try{ apply(JSON.parse(e.data)); }catch(_){} };
}

function apply(d){
  if(!d||d.type!=='state') return;
  S={...S,...d};
  document.getElementById('npTitle').textContent = d.title||'—';
  document.getElementById('npSub').textContent   = d.sub||' ';
  document.getElementById('srcTag').textContent  = d.source==='file'?'Файл':'Radio';
  document.getElementById('volR').value          = d.vol;
  document.getElementById('volV').textContent    = d.vol;
  const btn=document.getElementById('btnPlay');
  btn.innerHTML = d.playing?'&#9646;&#9646;':'&#9654;';
  btn.classList.toggle('off',!d.playing);
  renderStations(d.stIdx);
}

// ── API ──────────────────────────────────────────────────────
function cmd(url){ fetch(url).catch(()=>{}); }

function togglePlay(){
  if(S.playing){ cmd('/stop'); return; }
  if(S.source==='file'&&S.fileIdx>=0) cmd('/play?src=file&i='+S.fileIdx);
  else cmd('/play?src=radio&i='+(S.stIdx>=0?S.stIdx:0));
}

function playStation(i){ cmd('/play?src=radio&i='+i); }
function playFile(i)   { cmd('/play?src=file&i='+i); }

function setVol(v){
  document.getElementById('volV').textContent=v;
  clearTimeout(volT);
  volT=setTimeout(()=>cmd('/vol?v='+v),150);
}

// ── Tabs ─────────────────────────────────────────────────────
function tab(name,el){
  activeTab=name;
  document.querySelectorAll('.tab').forEach(t=>t.classList.remove('on'));
  el.classList.add('on');
  document.querySelectorAll('.pnl').forEach(p=>p.classList.remove('on'));
  document.getElementById('pnl-'+name).classList.add('on');
  if(name==='files'||name==='upload') loadFiles();
}

function toggleForm(id){ document.getElementById(id).classList.toggle('on'); }

// ── Stations ─────────────────────────────────────────────────
function renderStations(active){
  fetch('/stations').then(r=>r.json()).then(arr=>{
    if(!arr.length){
      document.getElementById('stList').innerHTML='<div class="empty">Нет станций</div>';
      return;
    }
    document.getElementById('stList').innerHTML = arr.map(s=>`
      <div class="itm ${s.i===active&&S.source==='radio'?'cur':''}" onclick="playStation(${s.i})">
        <span class="inum">${s.i+1}</span>
        <span class="iname">${esc(s.name)}</span>
        <button class="idel" onclick="event.stopPropagation();delStation(${s.i})" title="Удалить">✕</button>
      </div>`).join('');
  }).catch(()=>{});
}

function addStation(){
  const name=document.getElementById('stName').value.trim();
  const url =document.getElementById('stUrl').value.trim();
  if(!name||!url){toast('Заполни оба поля','er');return;}
  fetch('/stations/add',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({name,url})})
    .then(r=>r.json()).then(d=>{
      if(d.ok){
        toast('Станция добавлена','ok');
        document.getElementById('stName').value='';
        document.getElementById('stUrl').value='';
        document.getElementById('stForm').classList.remove('on');
        renderStations(S.stIdx);
      } else toast(d.err||'Ошибка','er');
    });
}

function delStation(i){
  if(!confirm('Удалить станцию?')) return;
  fetch('/stations/del?i='+i,{method:'DELETE'})
    .then(r=>r.json()).then(d=>{
      if(d.ok){ toast('Удалено','ok'); renderStations(S.stIdx); }
    });
}

// ── Files ─────────────────────────────────────────────────────
function loadFiles(){
  fetch('/files').then(r=>r.json()).then(arr=>{
    // panel "files"
    if(!arr.length){
      document.getElementById('fileList').innerHTML='<div class="empty">Нет загруженных файлов</div>';
      document.getElementById('uploadedList').innerHTML='<div class="empty">Нет файлов</div>';
      document.getElementById('iFiles').textContent='0';
      return;
    }
    document.getElementById('iFiles').textContent=arr.length;
    const html = arr.map(f=>`
      <div class="itm ${f.i===S.fileIdx&&S.source==='file'?'cur':''}" onclick="playFile(${f.i})">
        <span class="inum">${f.i+1}</span>
        <span class="iname">${esc(f.name)}</span>
        <span class="isub">${f.kb} KB</span>
        <button class="idel" onclick="event.stopPropagation();delFile('${esc(f.name)}')" title="Удалить">✕</button>
      </div>`).join('');
    document.getElementById('fileList').innerHTML=html;
    document.getElementById('uploadedList').innerHTML=html;
  }).catch(()=>{});
}

function delFile(name){
  if(!confirm('Удалить '+name+'?')) return;
  fetch('/files/del?name='+encodeURIComponent(name),{method:'DELETE'})
    .then(r=>r.json()).then(d=>{
      if(d.ok){ toast('Файл удалён','ok'); loadFiles(); }
      else toast('Ошибка','er');
    });
}

// ── Upload ────────────────────────────────────────────────────
function onDrop(e){
  e.preventDefault();
  document.getElementById('dropZone').classList.remove('drag');
  const f=e.dataTransfer.files[0];
  if(f) startUpload(f);
}

function startUpload(file){
  if(!file) return;
  const maxBytes = 8*1024*1024;
  if(file.size > maxBytes){ toast('Файл слишком большой (макс 8 МБ)','er'); return; }

  const allowed=['mp3','aac','wav','flac'];
  const ext=file.name.split('.').pop().toLowerCase();
  if(!allowed.includes(ext)){ toast('Неподдерживаемый формат','er'); return; }

  const wrap=document.getElementById('progWrap');
  const bar =document.getElementById('pbar');
  const pct =document.getElementById('ppct');
  document.getElementById('progName').textContent=file.name;
  wrap.classList.add('on');
  bar.style.width='0%'; pct.textContent='0%';

  const fd=new FormData();
  fd.append('file',file,file.name);

  const xhr=new XMLHttpRequest();
  xhr.open('POST','/upload');
  xhr.upload.onprogress=e=>{
    if(e.lengthComputable){
      const p=Math.round(e.loaded/e.total*100);
      bar.style.width=p+'%'; pct.textContent=p+'%';
    }
  };
  xhr.onload=()=>{
    if(xhr.status===200){
      toast('Файл загружен!','ok');
      bar.style.width='100%'; pct.textContent='100%';
      setTimeout(()=>wrap.classList.remove('on'),2000);
      loadFiles();
    } else {
      toast('Ошибка загрузки','er');
      wrap.classList.remove('on');
    }
  };
  xhr.onerror=()=>{ toast('Сетевая ошибка','er'); wrap.classList.remove('on'); };
  xhr.send(fd);
}

// ── Status ───────────────────────────────────────────────────
function loadStatus(){
  fetch('/status').then(r=>r.json()).then(d=>{
    document.getElementById('iIp').textContent   = d.ip  ||'—';
    document.getElementById('iRssi').textContent = d.rssi ? d.rssi+' dBm':'—';
    document.getElementById('iRam').textContent  = d.ram  ? Math.round(d.ram/1024)+' KB':'—';
    document.getElementById('iFs').textContent   = d.fsUsed&&d.fsTotal
      ? Math.round(d.fsUsed)+'/'+Math.round(d.fsTotal)+' KB':'—';
    const u=d.uptime||0;
    document.getElementById('iUp').textContent =
      Math.floor(u/3600)+'h '+Math.floor((u%3600)/60)+'m';
    apply({...d, type:'state'});
  }).catch(()=>{});
}

// ── Toast ─────────────────────────────────────────────────────
let toastT;
function toast(msg,type=''){
  const el=document.getElementById('toast');
  el.textContent=msg; el.className='toast show '+type;
  clearTimeout(toastT); toastT=setTimeout(()=>el.className='toast',2800);
}

function esc(s){ return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;'); }

// ── Init ──────────────────────────────────────────────────────
connectWS();
loadStatus();
setInterval(loadStatus, 6000);
</script>
</body>
</html>)rawhtml";
