#include "webpage.h"

static const char HTML_PAGE[] = R"HTML(
<!doctype html>
<html>
<head>
  <meta name="viewport" content="width=device-width,initial-scale=1">
  <title>Homer Telemetry</title>
  <style>
    body { font-family: system-ui, sans-serif; background:#0b1020; color:#e8ecff; margin:0; padding:24px; }
    .card { max-width:720px; margin:0 auto; background:#121a33; border:1px solid #243055; border-radius:16px; padding:20px; }
    .row { display:grid; grid-template-columns: 1fr 1fr; gap:12px; }
    .tile { background:#0f1730; padding:14px; border-radius:12px; }
    .label { font-size:12px; opacity:.7; text-transform:uppercase; letter-spacing:.08em; }
    .value { font-size:30px; font-weight:700; margin-top:6px; }
    .small { opacity:.75; font-size:14px; margin-top:10px; }
    .good { color:#8ef0a6; }
    .bad { color:#ff8f8f; }
    @media (max-width: 640px) { .row { grid-template-columns: 1fr; } }
  </style>
</head>
<body>
  <div class="card">
    <h1>Homer Telemetry</h1>
    <div class="row">
      <div class="tile"><div class="label">RC Health</div><div class="value" id="health">--</div></div>
      <div class="tile"><div class="label">Controller Connected</div><div class="value" id="connected">--</div></div>
      <div class="tile"><div class="label">Sample Time</div><div class="value" id="age">--</div></div>
      <div class="tile"><div class="label">CH1 Pulse (L/R)</div><div class="value" id="ch1">--</div></div>
      <div class="tile"><div class="label">CH2 Pulse (F/B)</div><div class="value" id="ch2">--</div></div>
      <div class="tile"><div class="label">CH3 Pulse (Thr)</div><div class="value" id="ch3">--</div></div>
      <div class="tile"><div class="label">CH4 Pulse (Trim)</div><div class="value" id="ch4">--</div></div>
      <div class="tile"><div class="label">Raw G</div><div class="value" id="raw">--</div></div>
      <div class="tile"><div class="label">Zeroed G</div><div class="value" id="zeroed">--</div></div>
      <div class="tile"><div class="label">Avg Zeroed G (30s)</div><div class="value" id="avgzeroed">--</div></div>
      <div class="tile"><div class="label">X</div><div class="value" id="x">--</div></div>
      <div class="tile"><div class="label">Y</div><div class="value" id="y">--</div></div>
      <div class="tile"><div class="label">Z</div><div class="value" id="z">--</div></div>
      <div class="tile"><div class="label">Offset</div><div class="value" id="offset">--</div></div>
    </div>
    <div class="small">Refreshes automatically while you connect over the Seeed's Wi-Fi.</div>
  </div>
  <script>
    const els = {
      health: document.getElementById('health'),
      connected: document.getElementById('connected'),
      age: document.getElementById('age'),
      ch1: document.getElementById('ch1'),
      ch2: document.getElementById('ch2'),
      ch3: document.getElementById('ch3'),
      ch4: document.getElementById('ch4'),
      raw: document.getElementById('raw'),
      zeroed: document.getElementById('zeroed'),
      avgzeroed: document.getElementById('avgzeroed'),
      x: document.getElementById('x'),
      y: document.getElementById('y'),
      z: document.getElementById('z'),
      offset: document.getElementById('offset'),
    };

    async function tick() {
      try {
        const r = await fetch('/data', { cache: 'no-store' });
        const j = await r.json();
        els.health.textContent = j.rc_healthy ? 'OK' : 'BAD';
        els.health.className = 'value ' + (j.rc_healthy ? 'good' : 'bad');
        els.connected.textContent = j.controller_connected ? 'YES' : 'NO';
        els.connected.className = 'value ' + (j.controller_connected ? 'good' : 'bad');
        els.age.textContent = `${j.sample_ms} ms`;
        els.ch1.textContent = `${j.ch1_us} us`;
        els.ch2.textContent = `${j.ch2_us} us`;
        els.ch3.textContent = `${j.ch3_us} us`;
        els.ch4.textContent = `${j.ch4_us} us`;
        els.raw.textContent = j.raw_g.toFixed(4);
        els.zeroed.textContent = j.zeroed_g.toFixed(4);
        els.avgzeroed.textContent = j.avg_zeroed_g.toFixed(4);
        els.x.textContent = j.x;
        els.y.textContent = j.y;
        els.z.textContent = j.z;
        els.offset.textContent = j.offset.toFixed(4);
      } catch (e) {}
    }
    setInterval(tick, 100);
    tick();
  </script>
</body>
</html>
)HTML";

const char *dashboard_html() {
  return HTML_PAGE;
}
