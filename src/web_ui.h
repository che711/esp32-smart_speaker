#pragma once

const char WEB_UI[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html lang="ru">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Smart Speaker</title>
<style>
  :root {
    --bg: #0f0f0f;
    --surface: #1a1a1a;
    --surface2: #242424;
    --border: #2e2e2e;
    --accent: #e8a020;
    --accent2: #c07818;
    --text: #e8e8e8;
    --muted: #888;
    --danger: #c0392b;
    --success: #27ae60;
    --radius: 10px;
  }
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body { background: var(--bg); color: var(--text); font-family: 'Segoe UI', system-ui, sans-serif; min-height: 100vh; padding: 16px; }

  .app { max-width: 480px; margin: 0 auto; }

  /* Header */
  .header { display: flex; align-items: center; justify-content: space-between; margin-bottom: 20px; padding-bottom: 14px; border-bottom: 1px solid var(--border); }
  .header h1 { font-size: 18px; font-weight: 600; letter-spacing: .04em; }
  .header h1 span { color: var(--accent); }
  .status-dot { width: 8px; height: 8px; border-radius: 50%; background: var(--muted); transition: background .3s; }
  .status-dot.connected { background: var(--success); box-shadow: 0 0 8px var(--success); }

  /* Now playing card */
  .now-playing { background: var(--surface); border: 1px solid var(--border); border-radius: var(--radius); padding: 20px; margin-bottom: 16px; }
  .np-label { font-size: 10px; font-weight: 600; letter-spacing: .12em; text-transform: uppercase; color: var(--muted); margin-bottom: 10px; }
  .np-station { font-size: 20px; font-weight: 600; color: var(--accent); margin-bottom: 4px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
  .np-track { font-size: 13px; color: var(--muted); min-height: 18px; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }

  /* Playback controls */
  .controls { display: flex; align-items: center; gap: 12px; margin-top: 18px; }
  .btn-ctrl { background: var(--surface2); border: 1px solid var(--border); border-radius: 50%; width: 42px; height: 42px; display: flex; align-items: center; justify-content: center; cursor: pointer; transition: all .15s; color: var(--text); font-size: 16px; }
  .btn-ctrl:hover { border-color: var(--accent); color: var(--accent); }
  .btn-play { background: var(--accent); border-color: var(--accent); color: #000; width: 52px; height: 52px; font-size: 20px; }
  .btn-play:hover { background: var(--accent2); border-color: var(--accent2); color: #000; }
  .btn-play.paused { background: var(--surface2); border-color: var(--border); color: var(--text); }

  /* Volume */
  .volume-row { display: flex; align-items: center; gap: 10px; margin-top: 14px; }
  .vol-icon { color: var(--muted); font-size: 14px; width: 20px; text-align: center; }
  input[type=range] { flex: 1; -webkit-appearance: none; height: 4px; border-radius: 2px; background: var(--border); outline: none; cursor: pointer; }
  input[type=range]::-webkit-slider-thumb { -webkit-appearance: none; width: 16px; height: 16px; border-radius: 50%; background: var(--accent); cursor: pointer; }
  input[type=range]::-moz-range-thumb { width: 16px; height: 16px; border-radius: 50%; background: var(--accent); border: none; cursor: pointer; }
  .vol-val { color: var(--muted); font-size: 13px; min-width: 24px; text-align: right; }

  /* Stations */
  .section-header { display: flex; align-items: center; justify-content: space-between; margin-bottom: 10px; }
  .section-title { font-size: 11px; font-weight: 600; letter-spacing: .1em; text-transform: uppercase; color: var(--muted); }
  .btn-sm { background: none; border: 1px solid var(--border); border-radius: 6px; color: var(--muted); font-size: 12px; padding: 4px 10px; cursor: pointer; transition: all .15s; }
  .btn-sm:hover { border-color: var(--accent); color: var(--accent); }

  .stations-list { background: var(--surface); border: 1px solid var(--border); border-radius: var(--radius); overflow: hidden; margin-bottom: 16px; }
  .station-item { display: flex; align-items: center; padding: 12px 16px; cursor: pointer; transition: background .12s; border-bottom: 1px solid var(--border); gap: 12px; }
  .station-item:last-child { border-bottom: none; }
  .station-item:hover { background: var(--surface2); }
  .station-item.active { background: #1e1600; border-left: 3px solid var(--accent); }
  .station-item.active .s-name { color: var(--accent); }
  .s-num { font-size: 11px; color: var(--muted); min-width: 18px; }
  .s-name { flex: 1; font-size: 14px; }
  .s-del { background: none; border: none; color: var(--muted); cursor: pointer; padding: 2px 6px; font-size: 16px; border-radius: 4px; transition: color .15s; }
  .s-del:hover { color: var(--danger); }

  /* Add station form */
  .add-form { background: var(--surface); border: 1px solid var(--border); border-radius: var(--radius); padding: 16px; margin-bottom: 16px; display: none; }
  .add-form.open { display: block; }
  .form-row { margin-bottom: 10px; }
  .form-row label { font-size: 11px; color: var(--muted); display: block; margin-bottom: 4px; text-transform: uppercase; letter-spacing: .06em; }
  .form-row input { width: 100%; background: var(--surface2); border: 1px solid var(--border); border-radius: 6px; padding: 8px 10px; color: var(--text); font-size: 13px; outline: none; transition: border-color .15s; }
  .form-row input:focus { border-color: var(--accent); }
  .btn-add { background: var(--accent); border: none; border-radius: 6px; color: #000; font-size: 13px; font-weight: 600; padding: 8px 18px; cursor: pointer; transition: background .15s; }
  .btn-add:hover { background: var(--accent2); }

  /* System info */
  .info-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; margin-bottom: 16px; }
  .info-card { background: var(--surface); border: 1px solid var(--border); border-radius: var(--radius); padding: 12px; }
  .info-card .i-label { font-size: 10px; color: var(--muted); text-transform: uppercase; letter-spacing: .08em; margin-bottom: 4px; }
  .info-card .i-val { font-size: 14px; font-weight: 500; }

  /* Toast */
  .toast { position: fixed; bottom: 20px; right: 20px; background: var(--surface2); border: 1px solid var(--border); border-radius: 8px; padding: 10px 16px; font-size: 13px; opacity: 0; transform: translateY(10px); transition: all .25s; pointer-events: none; z-index: 100; }
  .toast.show { opacity: 1; transform: translateY(0); }
  .toast.ok { border-color: var(--success); color: var(--success); }
  .toast.err { border-color: var(--danger); color: var(--danger); }
</style>
</head>
<body>
<div class="app">

  <div class="header">
    <h1>Smart <span>Speaker</span></h1>
    <div class="status-dot" id="statusDot"></div>
  </div>

  <!-- Now playing -->
  <div class="now-playing">
    <div class="np-label">Now playing</div>
    <div class="np-station" id="npStation">—</div>
    <div class="np-track" id="npTrack">Нажми станцию чтобы начать</div>

    <div class="controls">
      <button class="btn-ctrl" onclick="prevStation()" title="Предыдущая">&#9664;&#9664;</button>
      <button class="btn-ctrl btn-play" id="btnPlay" onclick="togglePlay()">&#9654;</button>
      <button class="btn-ctrl" onclick="nextStation()" title="Следующая">&#9654;&#9654;</button>
    </div>

    <div class="volume-row">
      <span class="vol-icon">&#128264;</span>
      <input type="range" id="volSlider" min="0" max="21" value="10" oninput="setVolume(this.value)">
      <span class="vol-val" id="volVal">10</span>
    </div>
  </div>

  <!-- Stations -->
  <div class="section-header">
    <span class="section-title">Станции</span>
    <button class="btn-sm" onclick="toggleAddForm()">+ Добавить</button>
  </div>

  <div class="add-form" id="addForm">
    <div class="form-row">
      <label>Название</label>
      <input type="text" id="newName" placeholder="My Radio Station">
    </div>
    <div class="form-row">
      <label>URL потока</label>
      <input type="url" id="newUrl" placeholder="http://stream.example.com/live.mp3">
    </div>
    <button class="btn-add" onclick="addStation()">Добавить</button>
  </div>

  <div class="stations-list" id="stationsList"></div>

  <!-- System info -->
  <div class="section-header" style="margin-top:4px">
    <span class="section-title">Система</span>
  </div>
  <div class="info-grid">
    <div class="info-card"><div class="i-label">IP адрес</div><div class="i-val" id="infoIp">—</div></div>
    <div class="info-card"><div class="i-label">Сигнал Wi-Fi</div><div class="i-val" id="infoRssi">—</div></div>
    <div class="info-card"><div class="i-label">Свободная память</div><div class="i-val" id="infoFree">—</div></div>
    <div class="info-card"><div class="i-label">Uptime</div><div class="i-val" id="infoUptime">—</div></div>
  </div>

</div>

<div class="toast" id="toast"></div>

<script>
let state = { playing: false, station: '', track: '', volume: 10, currentIndex: -1 };
let ws;

function connectWS() {
  ws = new WebSocket('ws://' + location.host + '/ws');
  ws.onopen = () => document.getElementById('statusDot').classList.add('connected');
  ws.onclose = () => {
    document.getElementById('statusDot').classList.remove('connected');
    setTimeout(connectWS, 2000);
  };
  ws.onmessage = (e) => {
    const d = JSON.parse(e.data);
    if (d.type === 'state') applyState(d);
  };
}

function applyState(d) {
  state = { ...state, ...d };
  document.getElementById('npStation').textContent = d.station || '—';
  document.getElementById('npTrack').textContent   = d.track   || ' ';
  document.getElementById('volSlider').value = d.volume;
  document.getElementById('volVal').textContent    = d.volume;

  const btn = document.getElementById('btnPlay');
  if (d.playing) {
    btn.innerHTML = '&#9646;&#9646;';
    btn.classList.remove('paused');
  } else {
    btn.innerHTML = '&#9654;';
    btn.classList.add('paused');
  }
  renderStations(d.currentIndex);
}

function renderStations(activeIdx) {
  fetch('/stations').then(r => r.json()).then(list => {
    const el = document.getElementById('stationsList');
    el.innerHTML = list.map(s => `
      <div class="station-item ${s.index === activeIdx ? 'active' : ''}" onclick="playStation(${s.index})">
        <span class="s-num">${s.index + 1}</span>
        <span class="s-name">${s.name}</span>
        <button class="s-del" onclick="event.stopPropagation();removeStation(${s.index})" title="Удалить">&#10005;</button>
      </div>`).join('');
  });
}

function playStation(idx) {
  fetch('/play?index=' + idx);
}

function togglePlay() {
  fetch(state.playing ? '/stop' : '/play?index=' + (state.currentIndex >= 0 ? state.currentIndex : 0));
}

function nextStation() { fetch('/next'); }
function prevStation() { fetch('/prev'); }

let volTimer;
function setVolume(v) {
  document.getElementById('volVal').textContent = v;
  clearTimeout(volTimer);
  volTimer = setTimeout(() => fetch('/volume?v=' + v), 150);
}

function toggleAddForm() {
  document.getElementById('addForm').classList.toggle('open');
}

function addStation() {
  const name = document.getElementById('newName').value.trim();
  const url  = document.getElementById('newUrl').value.trim();
  if (!name || !url) { showToast('Заполни оба поля', 'err'); return; }
  fetch('/stations/add', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ name, url })
  }).then(r => r.json()).then(d => {
    if (d.ok) {
      showToast('Станция добавлена', 'ok');
      document.getElementById('newName').value = '';
      document.getElementById('newUrl').value  = '';
      document.getElementById('addForm').classList.remove('open');
      renderStations(state.currentIndex);
    } else {
      showToast(d.error || 'Ошибка', 'err');
    }
  });
}

function removeStation(idx) {
  if (!confirm('Удалить станцию?')) return;
  fetch('/stations/remove?index=' + idx, { method: 'DELETE' })
    .then(r => r.json()).then(d => {
      if (d.ok) { showToast('Удалено', 'ok'); renderStations(state.currentIndex); }
    });
}

function loadStatus() {
  fetch('/status').then(r => r.json()).then(d => {
    document.getElementById('infoIp').textContent     = d.ip     || '—';
    document.getElementById('infoRssi').textContent   = d.rssi ? d.rssi + ' dBm' : '—';
    document.getElementById('infoFree').textContent   = d.freeHeap ? Math.round(d.freeHeap/1024) + ' KB' : '—';
    const u = d.uptime || 0;
    const h = Math.floor(u/3600), m = Math.floor((u%3600)/60), s = u%60;
    document.getElementById('infoUptime').textContent = `${h}h ${m}m ${s}s`;
    applyState(d);
  });
}

let toastTimer;
function showToast(msg, type = '') {
  const t = document.getElementById('toast');
  t.textContent = msg;
  t.className = 'toast show ' + type;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => t.className = 'toast', 2500);
}

connectWS();
loadStatus();
setInterval(loadStatus, 5000);
</script>
</body>
</html>
)rawhtml";
