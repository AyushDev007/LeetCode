/* ===============================================================
   Flappy Bird
   Everything is drawn procedurally — no sprite sheets, no assets.
   =============================================================== */
(() => {
  'use strict';

  const canvas = document.getElementById('game');
  const ctx = canvas.getContext('2d');

  /* --- logical resolution; the canvas is CSS-scaled to fit ------ */
  const W = 400;
  const H = 600;
  const GROUND_H = 92;
  const SKY_H = H - GROUND_H;

  /* --- tuning (values are per 1/60s step) ---------------------- */
  const GRAVITY = 0.42;
  const FLAP_V = -7.15;
  const MAX_FALL = 11;
  const BIRD_X = 112;
  const BIRD_R = 12;
  const HIT_R = 10;          // slightly forgiving hitbox
  const PIPE_W = 62;
  const PIPE_SPACING = 198;
  const BASE_SPEED = 2.5;
  const BASE_GAP = 168;
  const MIN_GAP = 130;
  const MAX_SPEED = 4.3;

  const rand = (a, b) => a + Math.random() * (b - a);
  const clamp = (v, a, b) => (v < a ? a : v > b ? b : v);
  const lerp = (a, b, t) => a + (b - a) * t;

  /* ============================================================ */
  /*  Palettes                                                    */
  /* ============================================================ */

  const hex2rgb = (h) => [
    parseInt(h.slice(1, 3), 16),
    parseInt(h.slice(3, 5), 16),
    parseInt(h.slice(5, 7), 16),
  ];

  const THEMES = [
    { // clear day
      skyTop: '#4ec0ea', skyBot: '#bdeaf7', sun: '#fff6c8',
      far: '#8fd0e6', near: '#6bb7d6', cloud: '#ffffff',
      hill: '#7ec850', grass: '#9fe06a', dirt: '#dcc287', dirtDark: '#c9a86a',
      pipe: '#5cc63f', pipeDark: '#2f7d22', pipeLight: '#a5e88a', night: 0,
    },
    { // dusk
      skyTop: '#33417e', skyBot: '#ff9a6b', sun: '#ffd08a',
      far: '#6a5b91', near: '#4a3f6d', cloud: '#ffd9c9',
      hill: '#4e6b52', grass: '#6d8f5c', dirt: '#c39a72', dirtDark: '#a37c58',
      pipe: '#59a83f', pipeDark: '#2b5f22', pipeLight: '#93cf78', night: .35,
    },
    { // night
      skyTop: '#0b1230', skyBot: '#28356b', sun: '#f4f1de',
      far: '#1b2450', near: '#141a3a', cloud: '#6d7cb8',
      hill: '#22333f', grass: '#2f4a3a', dirt: '#6d5c46', dirtDark: '#544736',
      pipe: '#3d8a35', pipeDark: '#1c4a1a', pipeLight: '#65b45a', night: 1,
    },
  ].map((t) => {
    const out = { night: t.night };
    for (const k in t) if (k !== 'night') out[k] = hex2rgb(t[k]);
    return out;
  });

  const THEME_EVERY = 12;   // points per palette swap
  const rgb = (c) => `rgb(${c[0] | 0},${c[1] | 0},${c[2] | 0})`;
  const rgba = (c, a) => `rgba(${c[0] | 0},${c[1] | 0},${c[2] | 0},${a})`;

  /** Live palette — a blend between the previous and target theme. */
  const sky = {};
  let themeIndex = 0;
  let themeMix = 1;         // 0 = fully previous, 1 = fully target

  function blendThemes() {
    const to = THEMES[themeIndex];
    const from = THEMES[(themeIndex - 1 + THEMES.length) % THEMES.length];
    const t = themeMix;
    for (const k in to) {
      if (k === 'night') { sky.night = lerp(from.night, to.night, t); continue; }
      const a = from[k], b = to[k];
      sky[k] = [lerp(a[0], b[0], t), lerp(a[1], b[1], t), lerp(a[2], b[2], t)];
    }
  }
  blendThemes();

  /* ============================================================ */
  /*  Audio — tiny WebAudio synth, no files                       */
  /* ============================================================ */

  const audio = {
    ctx: null,
    master: null,
    muted: localStorage.getItem('flappy.muted') === '1',

    ensure() {
      if (this.ctx) {
        if (this.ctx.state === 'suspended') this.ctx.resume();
        return;
      }
      const AC = window.AudioContext || window.webkitAudioContext;
      if (!AC) return;
      this.ctx = new AC();
      this.master = this.ctx.createGain();
      this.master.gain.value = this.muted ? 0 : 1;
      this.master.connect(this.ctx.destination);
    },

    setMuted(m) {
      this.muted = m;
      localStorage.setItem('flappy.muted', m ? '1' : '0');
      if (this.master) this.master.gain.value = m ? 0 : 1;
    },

    tone(type, f0, f1, dur, vol, delay = 0) {
      if (!this.ctx || this.muted) return;
      const t = this.ctx.currentTime + delay;
      const osc = this.ctx.createOscillator();
      const g = this.ctx.createGain();
      osc.type = type;
      osc.frequency.setValueAtTime(f0, t);
      osc.frequency.exponentialRampToValueAtTime(Math.max(f1, 1), t + dur);
      g.gain.setValueAtTime(0, t);
      g.gain.linearRampToValueAtTime(vol, t + 0.008);
      g.gain.exponentialRampToValueAtTime(0.0001, t + dur);
      osc.connect(g).connect(this.master);
      osc.start(t);
      osc.stop(t + dur + 0.02);
    },

    flap()  { this.tone('square', 620, 380, 0.09, 0.05); },
    score() { this.tone('sine', 880, 880, 0.07, 0.07); this.tone('sine', 1320, 1320, 0.09, 0.06, 0.07); },
    hit()   { this.tone('sawtooth', 260, 70, 0.22, 0.10); },
    die()   { this.tone('triangle', 300, 45, 0.5, 0.08, 0.1); },
  };

  /* ============================================================ */
  /*  World                                                       */
  /* ============================================================ */

  const bird = { y: 0, v: 0, rot: 0, wing: 0, flapT: 0 };
  let pipes = [];
  let particles = [];
  let clouds = [];
  let stars = [];

  let state = 'menu';        // menu | ready | playing | falling | dead
  let score = 0;
  let best = +(localStorage.getItem('flappy.best') || 0);
  let speed = BASE_SPEED;
  let tick = 0;
  let shake = 0;
  let groundX = 0;      // wraps, tiles the ground texture
  let worldX = 0;       // never wraps, drives parallax layers
  let paused = false;

  /* --- parallax skylines: one seamless tile, drawn repeatedly -- */
  function makeSkyline(tileW, minH, maxH, minW, maxW) {
    const list = [];
    let x = 0;
    while (x < tileW) {
      const w = Math.round(rand(minW, maxW));
      const h = Math.round(rand(minH, maxH));
      const cols = Math.max(1, Math.floor(w / 13));
      const rows = Math.max(1, Math.floor(h / 16));
      const lights = [];
      for (let i = 0; i < cols * rows; i++) lights.push(Math.random() < 0.45);
      list.push({ x, w, h, cols, rows, lights });
      x += w + rand(2, 9);
    }
    return { tileW: x, list };
  }

  const skylineFar = makeSkyline(W, 60, 130, 26, 52);
  const skylineNear = makeSkyline(W, 40, 95, 34, 66);

  function seedClouds() {
    clouds = [];
    for (let i = 0; i < 6; i++) {
      clouds.push({ x: rand(-60, W + 60), y: rand(40, 230), s: rand(0.55, 1.25), sp: rand(0.12, 0.34) });
    }
  }

  function seedStars() {
    stars = [];
    for (let i = 0; i < 55; i++) {
      stars.push({ x: rand(0, W), y: rand(0, SKY_H * 0.8), r: rand(0.6, 1.5), ph: rand(0, 6.28) });
    }
  }

  seedClouds();
  seedStars();

  function gapFor(s) { return Math.max(MIN_GAP, BASE_GAP - s * 1.5); }

  function addPipe(x) {
    const gap = gapFor(score);
    const margin = 62;
    const top = rand(margin, SKY_H - margin - gap);
    pipes.push({ x, top, gap, scored: false });
  }

  function resetWorld() {
    pipes = [];
    particles = [];
    score = 0;
    speed = BASE_SPEED;
    shake = 0;
    themeIndex = 0;
    themeMix = 1;
    blendThemes();
    bird.y = SKY_H * 0.45;
    bird.v = 0;
    bird.rot = 0;
    bird.flapT = 0;
    ui.setScore(0);
  }

  function startRound() {
    resetWorld();
    state = 'ready';
    hud.classList.add('live');
  }

  function beginPlay() {
    state = 'playing';
    for (let i = 0; i < 3; i++) addPipe(W + 140 + i * PIPE_SPACING);
    flap();
  }

  function flap() {
    bird.v = FLAP_V;
    bird.flapT = 9;
    audio.flap();
    for (let i = 0; i < 5; i++) {
      particles.push({
        x: BIRD_X - 8, y: bird.y + 6,
        vx: rand(-2.2, -0.6), vy: rand(0.1, 1.5),
        life: 1, decay: rand(0.03, 0.06), r: rand(1.5, 3.6),
        c: [255, 255, 255], grav: 0.02,
      });
    }
  }

  function burst(x, y) {
    for (let i = 0; i < 22; i++) {
      const a = rand(0, Math.PI * 2);
      const sp = rand(1, 5.5);
      particles.push({
        x, y,
        vx: Math.cos(a) * sp, vy: Math.sin(a) * sp - 1,
        life: 1, decay: rand(0.015, 0.035), r: rand(1.8, 4.2),
        c: Math.random() < 0.6 ? [255, 214, 92] : [255, 255, 255], grav: 0.16,
      });
    }
  }

  function sparkle(x, y) {
    for (let i = 0; i < 10; i++) {
      const a = rand(0, Math.PI * 2);
      particles.push({
        x, y,
        vx: Math.cos(a) * rand(0.5, 2.4), vy: Math.sin(a) * rand(0.5, 2.4),
        life: 1, decay: rand(0.03, 0.05), r: rand(1.2, 2.6),
        c: [255, 236, 150], grav: 0.01,
      });
    }
  }

  function die(hitPipe) {
    if (state !== 'playing') return;
    state = 'falling';
    shake = 14;
    audio.hit();
    audio.die();
    burst(BIRD_X, bird.y);
    flashEl.classList.remove('hit');
    void flashEl.offsetWidth;          // restart the CSS animation
    flashEl.classList.add('hit');
    if (hitPipe) bird.v = Math.min(bird.v, -2);
  }

  function land() {
    state = 'dead';
    if (score > best) {
      best = score;
      localStorage.setItem('flappy.best', String(best));
      ui.newBest = true;
    } else {
      ui.newBest = false;
    }
    setTimeout(ui.showGameOver, 320);
  }

  /* ============================================================ */
  /*  Update                                                      */
  /* ============================================================ */

  function update() {
    if (paused) return;
    tick++;

    /* palette drift */
    const wantTheme = Math.floor(score / THEME_EVERY) % THEMES.length;
    if (wantTheme !== themeIndex) { themeIndex = wantTheme; themeMix = 0; }
    if (themeMix < 1) { themeMix = Math.min(1, themeMix + 0.008); blendThemes(); }

    const scrolling = state === 'ready' || state === 'playing';
    if (scrolling) {
      worldX += speed;
      groundX = worldX % 48;
    }

    for (const c of clouds) {
      c.x -= c.sp * (scrolling ? 1 : 0.35);
      if (c.x < -70) { c.x = W + rand(20, 90); c.y = rand(40, 230); c.s = rand(.55, 1.25); }
    }

    if (state === 'ready') {
      bird.y = SKY_H * 0.45 + Math.sin(tick * 0.07) * 8;
      bird.rot = Math.sin(tick * 0.07) * 0.12;
      bird.wing += 0.22;
    }

    if (state === 'playing' || state === 'falling') {
      bird.v = Math.min(bird.v + GRAVITY, MAX_FALL);
      bird.y += bird.v;

      /* nose up fast, nose down slowly */
      const target = bird.v < 0 ? -0.5 : clamp(bird.v / 11, 0, 1) * 1.55;
      bird.rot += (target - bird.rot) * (bird.v < 0 ? 0.22 : 0.09);

      bird.flapT = Math.max(0, bird.flapT - 1);
      bird.wing += bird.flapT > 0 ? 0.55 : 0.16;
    }

    if (state === 'playing') {
      speed = Math.min(MAX_SPEED, BASE_SPEED + score * 0.025);

      /* ceiling: bonk, don't die */
      if (bird.y < BIRD_R) { bird.y = BIRD_R; bird.v = Math.max(bird.v, 1.2); }

      for (const p of pipes) p.x -= speed;
      if (pipes.length && pipes[0].x < -PIPE_W - 4) pipes.shift();

      const lastPipe = pipes[pipes.length - 1];
      if (!lastPipe || lastPipe.x < W - PIPE_SPACING) addPipe((lastPipe ? lastPipe.x : W) + PIPE_SPACING);

      for (const p of pipes) {
        if (!p.scored && BIRD_X > p.x + PIPE_W) {
          p.scored = true;
          score++;
          ui.setScore(score);
          audio.score();
          sparkle(BIRD_X + 6, bird.y);
        }
        if (hitsPipe(p)) { die(true); break; }
      }
    }

    if ((state === 'playing' || state === 'falling') && bird.y + HIT_R >= SKY_H) {
      bird.y = SKY_H - HIT_R;
      if (state === 'playing') die(false);
      else if (state === 'falling') { bird.v = 0; bird.rot = 1.57; land(); }
    }

    if (state === 'falling') bird.rot = Math.min(1.57, bird.rot + 0.06);

    /* particles */
    for (let i = particles.length - 1; i >= 0; i--) {
      const p = particles[i];
      p.x += p.vx;
      p.y += p.vy;
      p.vy += p.grav;
      p.vx *= 0.99;
      p.life -= p.decay;
      if (p.life <= 0) particles.splice(i, 1);
    }

    if (shake > 0) shake *= 0.86;
    if (shake < 0.2) shake = 0;
  }

  function hitsPipe(p) {
    if (BIRD_X + HIT_R < p.x || BIRD_X - HIT_R > p.x + PIPE_W) return false;
    const inGap = bird.y - HIT_R > p.top && bird.y + HIT_R < p.top + p.gap;
    if (inGap) return false;
    /* circle vs. the two rectangles, using the nearest corner */
    return near(p.x, 0, PIPE_W, p.top) || near(p.x, p.top + p.gap, PIPE_W, SKY_H - p.top - p.gap);
  }

  function near(rx, ry, rw, rh) {
    const cx = clamp(BIRD_X, rx, rx + rw);
    const cy = clamp(bird.y, ry, ry + rh);
    const dx = BIRD_X - cx, dy = bird.y - cy;
    return dx * dx + dy * dy < HIT_R * HIT_R;
  }

  /* ============================================================ */
  /*  Draw                                                        */
  /* ============================================================ */

  function draw() {
    ctx.save();
    if (shake > 0) ctx.translate(rand(-shake, shake), rand(-shake, shake));

    drawSky();
    drawStars();
    drawSunMoon();
    drawSkyline(skylineFar, 0.22, sky.far, SKY_H - 34, false);
    drawSkyline(skylineNear, 0.42, sky.near, SKY_H - 18, true);
    drawClouds();
    drawHills();
    for (const p of pipes) drawPipe(p);
    drawGround();
    drawBird();
    drawParticles();

    ctx.restore();
    drawVignette();
    if (state === 'ready') drawReadyHint();
  }

  function drawSky() {
    const g = ctx.createLinearGradient(0, 0, 0, SKY_H);
    g.addColorStop(0, rgb(sky.skyTop));
    g.addColorStop(1, rgb(sky.skyBot));
    ctx.fillStyle = g;
    ctx.fillRect(0, 0, W, SKY_H);
  }

  function drawStars() {
    if (sky.night < 0.03) return;
    for (const s of stars) {
      const tw = 0.55 + 0.45 * Math.sin(tick * 0.05 + s.ph);
      ctx.fillStyle = `rgba(255,255,255,${sky.night * tw * 0.9})`;
      ctx.beginPath();
      ctx.arc(s.x, s.y, s.r, 0, 6.284);
      ctx.fill();
    }
  }

  function drawSunMoon() {
    const x = 316, y = 92, r = 30;
    ctx.save();
    ctx.shadowColor = rgba(sky.sun, 0.75);
    ctx.shadowBlur = 40;
    ctx.fillStyle = rgb(sky.sun);
    ctx.beginPath();
    ctx.arc(x, y, r, 0, 6.284);
    ctx.fill();
    ctx.restore();

    /* craters fade in with the night palette */
    if (sky.night > 0.2) {
      ctx.fillStyle = `rgba(190,190,180,${(sky.night - 0.2) * 0.7})`;
      const craters = [[-9, -6, 7], [7, 3, 5], [-3, 11, 4], [11, -11, 3]];
      for (const [dx, dy, cr] of craters) {
        ctx.beginPath();
        ctx.arc(x + dx, y + dy, cr, 0, 6.284);
        ctx.fill();
      }
    }
  }

  function drawSkyline(sk, par, color, baseY, lit) {
    const off = (worldX * par) % sk.tileW;
    ctx.fillStyle = rgb(color);
    /* solid base band, so no sky shows through the gaps between blocks */
    ctx.fillRect(0, baseY, W, SKY_H - baseY);
    for (let rep = -1; rep <= Math.ceil(W / sk.tileW); rep++) {
      const ox = rep * sk.tileW - off;
      for (const b of sk.list) {
        const bx = Math.round(ox + b.x);
        if (bx > W || bx + b.w < 0) continue;
        ctx.fillRect(bx, baseY - b.h, b.w, b.h);
        if (!lit || sky.night < 0.15) continue;
        ctx.fillStyle = `rgba(255,214,120,${sky.night * 0.75})`;
        for (let c = 0; c < b.cols; c++) {
          for (let r = 0; r < b.rows; r++) {
            if (!b.lights[c * b.rows + r]) continue;
            ctx.fillRect(bx + 5 + c * 13, baseY - b.h + 7 + r * 16, 5, 7);
          }
        }
        ctx.fillStyle = rgb(color);
      }
    }
  }

  function drawClouds() {
    for (const c of clouds) {
      const s = c.s;
      ctx.fillStyle = rgba(sky.cloud, 0.85);
      ctx.beginPath();
      ctx.arc(c.x, c.y, 16 * s, 0, 6.284);
      ctx.arc(c.x + 18 * s, c.y + 4 * s, 12 * s, 0, 6.284);
      ctx.arc(c.x - 18 * s, c.y + 5 * s, 11 * s, 0, 6.284);
      ctx.arc(c.x + 6 * s, c.y - 10 * s, 12 * s, 0, 6.284);
      ctx.fill();
    }
  }

  function drawHills() {
    const off = (worldX * 0.6) % 160;
    ctx.fillStyle = rgb(sky.hill);
    ctx.beginPath();
    ctx.moveTo(-10, SKY_H);
    for (let x = -160 - off; x < W + 160; x += 160) {
      ctx.quadraticCurveTo(x + 40, SKY_H - 46, x + 80, SKY_H - 8);
      ctx.quadraticCurveTo(x + 120, SKY_H - 40, x + 160, SKY_H - 6);
    }
    ctx.lineTo(W + 10, SKY_H);
    ctx.closePath();
    ctx.fill();
  }

  function drawPipe(p) {
    const bottomY = p.top + p.gap;
    pipeBody(p.x, 0, p.top);
    pipeCap(p.x, p.top - 26);
    pipeBody(p.x, bottomY, SKY_H - bottomY);
    pipeCap(p.x, bottomY);
  }

  function pipeBody(x, y, h) {
    if (h <= 0) return;
    const g = ctx.createLinearGradient(x, 0, x + PIPE_W, 0);
    g.addColorStop(0, rgb(sky.pipeDark));
    g.addColorStop(0.18, rgb(sky.pipe));
    g.addColorStop(0.42, rgb(sky.pipeLight));
    g.addColorStop(0.62, rgb(sky.pipe));
    g.addColorStop(1, rgb(sky.pipeDark));
    ctx.fillStyle = g;
    ctx.fillRect(x, y, PIPE_W, h);
    ctx.strokeStyle = rgba(sky.pipeDark, 0.85);
    ctx.lineWidth = 2;
    ctx.strokeRect(x + 1, y - 1, PIPE_W - 2, h + 2);
  }

  function pipeCap(x, y) {
    const cw = PIPE_W + 10, cx = x - 5;
    const g = ctx.createLinearGradient(cx, 0, cx + cw, 0);
    g.addColorStop(0, rgb(sky.pipeDark));
    g.addColorStop(0.2, rgb(sky.pipe));
    g.addColorStop(0.45, rgb(sky.pipeLight));
    g.addColorStop(0.68, rgb(sky.pipe));
    g.addColorStop(1, rgb(sky.pipeDark));
    ctx.fillStyle = g;
    roundRect(cx, y, cw, 26, 5);
    ctx.fill();
    ctx.strokeStyle = rgba(sky.pipeDark, 0.9);
    ctx.lineWidth = 2;
    ctx.stroke();
  }

  function drawGround() {
    const y = SKY_H;

    /* grass strip */
    ctx.fillStyle = rgb(sky.grass);
    ctx.fillRect(0, y, W, 14);
    ctx.fillStyle = rgba(sky.pipeDark, 0.35);
    for (let x = -groundX; x < W; x += 24) ctx.fillRect(x, y + 10, 12, 4);

    /* dirt */
    const g = ctx.createLinearGradient(0, y + 14, 0, H);
    g.addColorStop(0, rgb(sky.dirt));
    g.addColorStop(1, rgb(sky.dirtDark));
    ctx.fillStyle = g;
    ctx.fillRect(0, y + 14, W, GROUND_H - 14);

    /* diagonal hatching, scrolling with the world */
    ctx.save();
    ctx.beginPath();
    ctx.rect(0, y + 14, W, GROUND_H - 14);
    ctx.clip();
    ctx.strokeStyle = rgba(sky.dirtDark, 0.55);
    ctx.lineWidth = 6;
    for (let x = -groundX - 60; x < W + 60; x += 24) {
      ctx.beginPath();
      ctx.moveTo(x, H);
      ctx.lineTo(x + 34, y + 14);
      ctx.stroke();
    }
    ctx.restore();

    ctx.fillStyle = 'rgba(0,0,0,.18)';
    ctx.fillRect(0, y + 14, W, 3);
  }

  function drawBird() {
    const x = BIRD_X, y = bird.y;
    ctx.save();
    ctx.translate(x, y);
    ctx.rotate(bird.rot);

    /* soft drop shadow */
    ctx.fillStyle = 'rgba(0,0,0,.18)';
    ctx.beginPath();
    ctx.ellipse(2, 4, BIRD_R + 2, BIRD_R - 1, 0, 0, 6.284);
    ctx.fill();

    /* body */
    const bg = ctx.createLinearGradient(0, -BIRD_R, 0, BIRD_R);
    bg.addColorStop(0, '#ffe066');
    bg.addColorStop(0.55, '#fbc02d');
    bg.addColorStop(1, '#e59400');
    ctx.fillStyle = bg;
    ctx.beginPath();
    ctx.ellipse(0, 0, BIRD_R + 2, BIRD_R, 0, 0, 6.284);
    ctx.fill();
    ctx.lineWidth = 2;
    ctx.strokeStyle = '#8a5a00';
    ctx.stroke();

    /* belly */
    ctx.fillStyle = 'rgba(255,255,255,.45)';
    ctx.beginPath();
    ctx.ellipse(1, 5, 8, 5, 0, 0, 6.284);
    ctx.fill();

    /* wing — flaps up on input, glides otherwise */
    const wing = Math.sin(bird.wing) * (bird.flapT > 0 ? 9 : 4);
    ctx.save();
    ctx.translate(-3, 1);
    ctx.rotate(-wing * 0.05);
    ctx.fillStyle = '#fff4d0';
    ctx.strokeStyle = '#8a5a00';
    ctx.beginPath();
    ctx.ellipse(0, -wing * 0.35, 8, 5.5, -0.3, 0, 6.284);
    ctx.fill();
    ctx.stroke();
    ctx.restore();

    /* eye */
    ctx.fillStyle = '#fff';
    ctx.beginPath();
    ctx.arc(6, -5, 5, 0, 6.284);
    ctx.fill();
    ctx.strokeStyle = '#8a5a00';
    ctx.lineWidth = 1.5;
    ctx.stroke();
    ctx.fillStyle = '#20232b';
    ctx.beginPath();
    ctx.arc(7.6, -5, 2.3, 0, 6.284);
    ctx.fill();
    ctx.fillStyle = '#fff';
    ctx.beginPath();
    ctx.arc(8.4, -6, 0.8, 0, 6.284);
    ctx.fill();

    /* beak */
    ctx.fillStyle = '#ff8c1a';
    ctx.strokeStyle = '#c25e00';
    ctx.lineWidth = 1.5;
    ctx.beginPath();
    ctx.moveTo(10, -1);
    ctx.lineTo(20, 1.5);
    ctx.lineTo(10, 5);
    ctx.closePath();
    ctx.fill();
    ctx.stroke();

    /* tail */
    ctx.fillStyle = '#e59400';
    ctx.beginPath();
    ctx.moveTo(-12, -2);
    ctx.lineTo(-19, -6);
    ctx.lineTo(-17, 3);
    ctx.closePath();
    ctx.fill();

    ctx.restore();
  }

  function drawParticles() {
    for (const p of particles) {
      ctx.fillStyle = rgba(p.c, Math.max(0, p.life) * 0.85);
      ctx.beginPath();
      ctx.arc(p.x, p.y, p.r * (0.4 + p.life * 0.6), 0, 6.284);
      ctx.fill();
    }
  }

  function drawVignette() {
    const g = ctx.createRadialGradient(W / 2, H / 2, H * 0.32, W / 2, H / 2, H * 0.78);
    g.addColorStop(0, 'rgba(0,0,0,0)');
    g.addColorStop(1, 'rgba(0,0,0,.28)');
    ctx.fillStyle = g;
    ctx.fillRect(0, 0, W, H);
  }

  function drawReadyHint() {
    const bob = Math.sin(tick * 0.09) * 5;
    ctx.save();
    ctx.textAlign = 'center';
    ctx.fillStyle = 'rgba(0,0,0,.28)';
    roundRect(W / 2 - 92, 300 + bob, 184, 46, 14);
    ctx.fill();
    ctx.fillStyle = '#fff';
    ctx.font = '800 19px "Trebuchet MS", system-ui, sans-serif';
    ctx.fillText('TAP TO FLAP', W / 2, 330 + bob);
    ctx.restore();
  }

  function roundRect(x, y, w, h, r) {
    ctx.beginPath();
    ctx.moveTo(x + r, y);
    ctx.arcTo(x + w, y, x + w, y + h, r);
    ctx.arcTo(x + w, y + h, x, y + h, r);
    ctx.arcTo(x, y + h, x, y, r);
    ctx.arcTo(x, y, x + w, y, r);
    ctx.closePath();
  }

  /* ============================================================ */
  /*  UI wiring                                                   */
  /* ============================================================ */

  const $ = (id) => document.getElementById(id);
  const hud = $('hud');
  const flashEl = $('flash');
  const scoreEl = $('score');
  const screens = { start: $('screenStart'), over: $('screenOver'), pause: $('screenPause') };

  const MEDALS = [
    { at: 50, name: 'Platinum', icon: '💎', color: '#7fe7e0' },
    { at: 30, name: 'Gold', icon: '🥇', color: '#ffd046' },
    { at: 20, name: 'Silver', icon: '🥈', color: '#d7dde4' },
    { at: 10, name: 'Bronze', icon: '🥉', color: '#cd7f32' },
  ];

  const ui = {
    newBest: false,

    setScore(v) {
      scoreEl.textContent = v;
      scoreEl.classList.add('pop');
      setTimeout(() => scoreEl.classList.remove('pop'), 130);
    },

    show(name) {
      for (const k in screens) screens[k].classList.toggle('show', k === name);
    },

    hideAll() {
      for (const k in screens) screens[k].classList.remove('show');
    },

    showGameOver() {
      $('finalScore').textContent = score;
      $('finalBest').textContent = best;
      $('newBest').classList.toggle('show', ui.newBest);

      const medal = MEDALS.find((m) => score >= m.at);
      const el = $('medal');
      el.classList.toggle('earned', !!medal);
      el.style.setProperty('--medal-color', medal ? medal.color : '#888');
      $('medalIcon').textContent = medal ? medal.icon : '—';
      $('medalLabel').textContent = medal ? `${medal.name} medal` : `No medal — reach 10`;

      ui.show('over');
    },

    showMenu() {
      $('startBest').textContent = best;
      hud.classList.remove('live');
      state = 'menu';
      resetWorld();
      ui.show('start');
    },
  };

  /* --- input --------------------------------------------------- */

  function tap() {
    audio.ensure();
    if (paused || state === 'menu' || state === 'dead') return;
    if (state === 'ready') beginPlay();
    else if (state === 'playing') flap();
  }

  function setPaused(p) {
    if (state !== 'playing' && state !== 'ready') p = false;
    paused = p;
    $('btnPause').classList.toggle('alt', p);
    if (p) ui.show('pause');
    else if (screens.pause.classList.contains('show')) ui.hideAll();
  }

  canvas.addEventListener('pointerdown', (e) => { e.preventDefault(); tap(); });

  window.addEventListener('keydown', (e) => {
    if (e.code === 'Space' || e.code === 'ArrowUp' || e.code === 'KeyW') {
      e.preventDefault();
      if (state === 'menu') { audio.ensure(); startRound(); ui.hideAll(); }
      else if (state === 'dead') restart();
      else tap();
    } else if (e.code === 'KeyP' || e.code === 'Escape') {
      e.preventDefault();
      setPaused(!paused);
    } else if (e.code === 'KeyM') {
      toggleSound();
    }
  });

  document.addEventListener('visibilitychange', () => {
    if (document.hidden && state === 'playing') setPaused(true);
  });
  window.addEventListener('blur', () => { if (state === 'playing') setPaused(true); });

  function restart() {
    ui.hideAll();
    startRound();
  }

  function toggleSound() {
    audio.ensure();
    audio.setMuted(!audio.muted);
    $('btnSound').classList.toggle('alt', audio.muted);
  }

  $('btnPlay').addEventListener('click', () => { audio.ensure(); ui.hideAll(); startRound(); });
  $('btnRetry').addEventListener('click', restart);
  $('btnMenu').addEventListener('click', ui.showMenu);
  $('btnResume').addEventListener('click', () => setPaused(false));
  $('btnQuit').addEventListener('click', () => { setPaused(false); ui.showMenu(); });
  $('btnPause').addEventListener('click', (e) => { e.stopPropagation(); setPaused(!paused); });
  $('btnSound').addEventListener('click', (e) => { e.stopPropagation(); toggleSound(); });

  $('btnSound').classList.toggle('alt', audio.muted);
  $('startBest').textContent = best;

  /* --- crisp rendering on hi-dpi displays ---------------------- */

  function fitCanvas() {
    const dpr = Math.min(window.devicePixelRatio || 1, 3);
    canvas.width = W * dpr;
    canvas.height = H * dpr;
    ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  }
  window.addEventListener('resize', fitCanvas);
  fitCanvas();

  /* --- fixed-step loop ----------------------------------------- */

  const STEP = 1000 / 60;
  let last = performance.now();
  let acc = 0;

  function frame(now) {
    requestAnimationFrame(frame);
    let dt = now - last;
    last = now;
    if (dt > 250) dt = STEP;        // returning from a background tab
    acc += dt;
    let steps = 0;
    while (acc >= STEP && steps < 5) { update(); acc -= STEP; steps++; }
    if (steps === 5) acc = 0;
    draw();
  }

  resetWorld();
  bird.y = SKY_H * 0.45;
  requestAnimationFrame(frame);
})();
