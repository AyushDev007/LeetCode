# Flappy Bird

A polished browser version of Flappy Bird. No build step, no dependencies, no
image or audio files — the bird, pipes, skyline and sound effects are all
generated at runtime.

**Play it:** open `index.html` in any modern browser.

```
flappy-bird/
├── index.html   # markup + overlay screens
├── style.css    # UI chrome (menus, HUD, buttons)
└── game.js      # engine, physics and canvas rendering
```

---

## Controls

| Action | Input |
|--------|-------|
| Flap   | `Space` · `↑` · `W` · click · tap |
| Pause  | `P` · `Esc` · pause button |
| Mute   | `M` · speaker button |

The game also pauses itself if the tab loses focus.

---

## What's in it

**Feel**

- Fixed 60 Hz physics step, decoupled from the render loop, so the game runs at
  the same speed on any refresh rate.
- Velocity-driven bird rotation — snaps up on a flap, tips over slowly on the
  way down — with the wing beating faster while climbing.
- Slightly forgiving hitbox (10 px against a 12 px bird) so near-misses read as
  near-misses.
- The ceiling bounces you instead of killing you.
- Difficulty ramps with the score: the gap narrows from 168 px to 130 px and
  scroll speed rises from 2.5 to 4.3.

**Look**

- Three palettes — day, dusk, night — that cross-fade every 12 points. Colours
  are interpolated per channel, so the whole scene shifts together: sky, pipes,
  grass, dirt and clouds.
- Four parallax layers (skyline, near skyline, clouds, hills) scrolling at
  different rates over a scrolling ground.
- The sun grows craters and the city windows light up as night comes in;
  stars fade in and twinkle.
- Particles for flaps, scoring and crashes, plus screen shake and a white flash
  on impact.

**UI**

- Glass panels for the menu, pause and game-over screens.
- Bronze / silver / gold / platinum medals at 10, 20, 30 and 50 points.
- Best score kept in `localStorage`, with a "New Best!" callout.
- Overlay typography is sized in container-query units, so the panels stay
  proportional whether the frame is 200 px or 420 px wide.

---

## Tuning

The constants at the top of `game.js` are the whole difficulty model:

```js
const GRAVITY  = 0.42;   // downward acceleration per step
const FLAP_V   = -7.15;  // upward velocity applied on a flap
const PIPE_W   = 62;     // pipe width
const BASE_GAP = 168;    // starting vertical gap, shrinks with score
const MIN_GAP  = 130;    // floor on that gap
```

A flap lifts the bird about 57 px before gravity wins, which is the number to
keep in mind when changing `BASE_GAP`.
