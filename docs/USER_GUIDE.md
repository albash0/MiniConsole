# MiniConsole user guide

These controls apply to the OLED and TFT profiles unless a section specifies otherwise. Hardware validation described in the source project's development records does not establish every third-party board's compatibility.

### Pocket Partners (two consoles)

Select **Pocket Partners** from the library. Choose **Host** on one console and
**Join** on the other, press A to open, then A on both to ready up. The same room
code confirms the pair. No router is required. Coral is the host, cyan is the
joining player; a small arrow identifies your character.

- Left/Right: walk. A: jump (release between jumps).
- Hold B while walking against a crate to push it.
- Up: pause or resume both consoles. While paused, A resumes, Down retries the
  room, and B returns both players to the library.
- Reach a shared key and get both characters into the doorway. Stand on your
  partner to reach higher platforms; pressure plates can be held by a player
  or crate. A short gate grace period allows safe passage.
- A after a fall restarts the room for both. A after clearing advances both.

Ten original single-screen rooms introduce stacking, switch relays, crate
weights, gaps and a final combined puzzle. Completed rooms unlock for replay;
Up/Down selects an unlocked starting room before hosting. Unlocks persist in a
separate NVS key (`pocketLevel`) and do not replace solo checkpoints. The host
runs fixed-step shared physics; clients send repeated input counters and receive
world snapshots. Brief link stalls freeze the simulation; an eight-second
outage shows Link Lost. Pause/exit/retry apply to both players.

### Two-console Block Battle

On the Tetris title screen, press **A** to open Block Battle (Up still starts solo).
Choose Host on one console and Join on the other, then A to open the room.
Check that both show the same room number and press A on each to Ready.
The match starts after a three-second countdown. Both receive the same piece bag.
Doubles send one garbage row, triples two, Tetrises four; clears cancel queued
incoming rows before attacking. Garbage arrives between pieces and has one hole
per row. The side bars show queued incoming rows and the opponent's stack height.
A pauses/resumes both games; B while paused forfeits. After results, Up/A opens
the rematch lobby and B returns to the library. Battle runs do not replace solo
checkpoints or high scores. Wireless runs only in the battle lobby/match and is
stopped on exit and before Doom. No router or internet is needed (ESP-NOW ch6).


### TFT wiring and display

| TFT label | GPIO / connection |
| --- | --- |
| GND / VCC | GND / 3.3V |
| SCL / SDA | SPI clock 8 / MOSI 9 |
| RES / DC | 1 / 6 |
| CS | GND; driver CS=-1 |
| BLK | 43, active high |
| Separate IIC expansion SDA / SCL | 5 / 44 |

The physical panel is initialized as **135×240, SPI mode 3**, with landscape base
rotation 3. Mode 0 produced a lit but blank panel on the user's module; mode 3
was confirmed working. The TFT alone uses its SPI bus. The existing Flip screen
setting adds 180° to the rendered image; Tetris keeps its D-pad-at-bottom turn.

Menus, settings, instructions, pause screens and party games use the full
**240×135** resolution, with proportional fonts, rounded cards and vector icons.
Tetris menus use a native 135×240 portrait canvas. Snake and Tetris have
native-resolution playfields and HUDs. Snake interpolates
its rounded body between grid moves, with animated fruit and seamless edge
wrapping. Tetris uses 10-pixel textured tiles, a tinted landing outline, a
native next-piece preview, and native flash/wipe/collapse effects. Sokoban also uses a native 15-pixel tile board with themed walls, boxes,
targets and player graphics. The remaining arcade playfields retain their pixel art, scaled to 240×120 with native HUDs.
Color distinguishes pieces, sprites, bricks, puzzle elements and confetti.
Doom composes a native **240×135 RGB565** frame with its original palette and
native-resolution HUD/pause overlay. Its display task yields one FreeRTOS tick
after each frame so the CPU 0 idle task can run and service its watchdog.
Display transfers use 40MHz SPI;
framebuffers are allocated without large global SRAM arrays, with the full-panel
and Doom color buffers in PSRAM. No battery-charger controls are changed.

For first installation on a new TFT board, including Doom's shareware assets:

```sh
python3 MiniConsole/tools/build_doom.py --display tft --port /dev/cu.usbmodem1101 --assets
```

Use the actual serial port; omit `--assets` for later application updates.
`DISPLAY_PROFILE=tft sh MiniConsole/tests/run_tests.sh` runs controller/UI/save
tests through the TFT backend as well as the dedicated RGB565/rotation tests.

| Connection | OLED GPIO | TFT GPIO |
| --- | --- | --- |
| A | 7 | 2 |
| B | 2 | 7 |
| Up | 10 | 10 |
| Right | 11 | 11 |
| Left | 12 | 12 |
| Down | 13 | 13 |
| OLED SDA | 9 | — |
| OLED SCL | 8 | — |

The TFT A/B assignments were swapped after checking the assembled console.
This is a firmware mapping; the existing PCB copper and manufacturing files
are unchanged.

Buttons connect their GPIO to GND and use internal pull-ups. OLED power is
3V3 and GND. The firmware checks both 0x3C and 0x3D display addresses.

## Controls

- Library: Left/Right (or Up/Down) selects a cartridge; A opens; B opens System.
- System: Up/Down selects Settings, How to Play or Button Test; A opens; B returns to the library.
- Snake: D-pad steers. A or B pauses.
- Tetris: turn the console 90° clockwise, placing the D-pad below the
  screen. The game uses a 64×128 portrait layout. Up (regular Left, GPIO12)
  rotates clockwise, B drops instantly, Left/Right moves (with hold repeat),
  Down drops faster, A pauses/resumes.
- Tetris start/game over: Up (regular Left, GPIO12) starts or retries; B returns home.
- Paused: A resumes; B returns home.
- Snake game over: A retries; B returns home.
- Breakout: Left/Right moves the paddle, A serves, B pauses. Three lives, successive
  brick racks and a bonus for clearing each rack. Bricks award 10–40 points by row;
  a completed rack awards 100 × its level. The ball gets faster as levels advance.
- Invaders: Left/Right moves the ship, hold A to fire, B pauses. Three lives,
  destructible shields, enemy shots and faster successive waves. Aliens award
  10/20/30 points by row; clearing a wave awards 100 × its wave number.
- Sokoban: D-pad walks/pushes; A undoes up to the last 128 moves; B pauses.
  Push every box onto a dot. Filled targets have a check mark. Ten original
  levels unlock in order, with completed progress saved. Left/Right on its start
  screen selects an unlocked level. After a clear, A advances; B returns home.
- Breakout/Invaders/Sokoban paused: A resumes, Left restarts, B returns home.
- Breakout/Invaders game over: A retries; B returns home.
- Doom: Up/Down walks; Left/Right turns; A fires and confirms menus; B uses
  doors/switches and goes back in menus, including when turning. To change
  weapon, hold B for at least 0.4 seconds, then tap Left/Right. Pressing B while
  already turning keeps the normal Use action and does not change weapons.
  Hold A+B for 0.3 seconds to pause/save; paused A resumes, B returns to the
  library, and Up opens Doom's original menu. Hold A+B for two seconds to return
  home from any Doom screen. Choose New Game, episode 1 and a skill with A to start.
- Micro Motorways: keep the console horizontal. D-pad moves the cursor. A begins
  a road, D-pad draws, A confirms. Tap B to open the frozen tool picker (discarding an unfinished preview); hold B for 0.6 seconds to pause and save.
  The three tool icons appear vertically on the left; Up/Down highlights a tool,
  A confirms and closes, and B closes. Traffic stays frozen while the picker is open.
  In the full pause menu, Up/Down selects; Left/Right on Tool switches Roads/Erase/Express;
  A resumes, cycles the tool, changes the palette, restarts or returns home. B returns home.
- Settings: Up/Down selects; Left/Right or A changes; B returns to System.
- Button test: all six buttons highlight; hold B for 0.8 seconds to return to System.
- How to Play: Left/Right switches between all seven games and the Tetris scoring page.

Only Tetris rotates; the library, system screens and other games stay horizontal.
Portrait D-pad mapping after the physical turn is Up=GPIO12, Right=GPIO10,
Down=GPIO11, Left=GPIO13. A/B keep their profile's GPIOs. The flip-screen setting adds
180° to either layout for an OLED mounted upside down.

## Micro Motorways

An original traffic-planning game on a 20×10 map. Connect the small colored homes
to the entrances of matching 2×2 destinations. Colors also have distinct symbols:
red circles, blue triangles and yellow squares. The entrance is the lower-left
cell of each destination; selecting any part of it snaps to that entrance.
Each run begins with two randomly placed home/destination pairs and two random
colors. Their queues and request timers are staggered. Normal/Fast starts require
a river crossing. The generator checks complete routes sharing one bridge and
leaves at least four spare roads after an affordable opening; no roads are
pre-built. New layouts take effect when starting a new run.

Cars choose shortest routes, return home after each trip and queue at busy
junctions. Each completed delivery earns one point. Destination dots show waiting
demand. At six waiting trips, a warning timer surrounds the building; clear the
backlog before it fills. Difficulty follows the system Speed setting.

Each new road edge uses one supply, including a diagonal edge. Hold two adjacent D-pad directions together to move/draw diagonally. A one-cell vertical step followed by a one-cell horizontal step (or the reverse) automatically becomes one diagonal, costing one road. Existing junctions are retained. Crossing diagonals form a center intersection where cars can turn and yield to crossing traffic. Building corners still block diagonal cuts. Bridges can be straight or diagonal; each crossing water tile still uses one bridge token. Traffic freezes during a road preview. Draw back
over the previous tile to shorten a preview, or cancel without spending anything.
Erase refunds roads and bridge tokens; occupied road cells cannot be erased.
Express links connect two existing, non-building road cells directly, using one
express token. Their crossings do not create intersections. Select that tool,
press A at the first road, move to another road and press A again.

Homes and destinations arrive faster on the higher Speed settings, up to the
active population limits. The city then continues through destination replacement.
Every 60 seconds, choose roads, a bridge plus some roads, or an express link plus
some roads. Up/Down chooses; A claims once. At the three-link cap, the express
choice gives the normal road supply instead.

| Speed | Starting roads | Road reward | Bridge + roads | Express + roads | Home / destination growth | New-site grace | Overload timer |
| --- | ---: | ---: | ---: | ---: | --- | ---: | ---: |
| Easy | 36 | 16 | 1 + 8 | 1 + 6 | 20 / 45 seconds | 15 seconds | 9 seconds |
| Normal | 28 | 16 | 1 + 6 | 1 + 4 | 18 / 35 seconds | 10 seconds | 6 seconds |
| Fast | 24 | 16 | 1 + 4 | 1 + 3 | 15 / 30 seconds | 8 seconds | 4.5 seconds |

Roads cannot pass through a building to make a shortcut.

There is no final week or forced completion. Every three weeks, the oldest
remaining destination closes and its replacement is outlined in advance. The
lot is reserved, so you can build to its lower-left entrance before it opens.
Existing cars get at least ten seconds to return; after thirty seconds any
stranded cars belonging to that destination are recalled. Other traffic and roads
stay in place. If the board has no free plot, the old plot is reused, with a new
color where the existing neighborhoods support it. Reopened destinations get the
connection grace period listed above before new demand accumulates. Closures,
reserved lots and grace timers survive pause/power-off. This cycle continues
with fixed memory use instead of filling the map and ending growth permanently.

Requests start more frequently, ramp up over the first few weeks, and sometimes
arrive two at a time. After 30 seconds a RUSH marker shows the busy color, rotating
every 20 seconds. Capacity estimates combine each home's contribution to each
matching destination instead of letting the farthest house slow the whole color.
The sustainable demand bound still depends on the fleet and geographic distances,
independent of road quality; it does not slow down to rescue a poor road network.
Three/four-way junctions pass at most one car every 0.4/0.6/0.7 seconds on
Easy/Normal/Fast, so merging traffic can cause real bottlenecks. Warnings recede at
the same rate they fill, rather than clearing twice as fast. Good
networks can survive indefinitely; disconnected or congested networks still lose.
Dispatch prioritizes unmet demand and rotates home priority to avoid starving
later destinations. Time uses a saved 64-bit counter; score saturates instead of
wrapping. Weekly supplies and high-score tracking continue throughout the run.

The TFT renderer uses the full 240×135 panel with rounded roads, interpolated cars,
queue indicators and warning animations. Micro Motorways defaults to its own
Classic palette: warm cream land, blue-green water, gray roads, and red/blue/yellow
buildings and cars. Inspired by Mini Motorways by Dinosaur Polo Club; Micro Motorways is an original implementation.
In its pause menu, Palette can switch to System, which honors Match system theme.
The palette choice is saved separately in flash and applies to new runs and resumed games. Classic is the initial default. Buildings and cars retain their identifying colors in either palette. The OLED profile has a compact
monochrome version. Sound effects respect Mute. High scores are stored separately.
Pausing saves the committed map, active traffic, supplies, city-growth random
state and any pending weekly choice. Weekly choices also save automatically;
preview roads are never saved. Existing game checkpoints remain compatible, including both earlier Micro
Motorways formats. Older congestion warnings retain their completion fraction
when migrated to the shorter timers. Existing runs retain their map and progress;
the revised balance applies immediately, while randomized openings require a new run.


## Appearance (TFT)

Open **System → Settings → Appearance**. Settings scrolls when its last item
is selected, including while the hidden mode's goal setting is available.
The OLED profile keeps its existing monochrome settings.

- **Preset:** Left/Right or A cycles Cyan, Amber, Violet, Mint, Mono, Light,
  Rose, Lavender, Sage, Sky and Peach. The last five pair pastel backgrounds
  and pale panels with muted accents and dark text.
- **Background, Panels, Accent, Text:** A opens the RGB editor. Up/Down selects
  R, G or B; Left/Right adjusts, with hold repeat. A keeps that color edit;
  B undoes it. Changes update the sample card immediately.
- **Default:** stages the original cyan colors.
- **Apply:** saves the staged colors and returns to Settings. B from Appearance
  cancels all unapplied changes. Scroll down to reach Default and Apply.

The editor keeps its controls readable even if a custom palette has poor
contrast. Background, panels, accent and text are customizable; borders and
secondary text are derived. Menus and pause screens use the chosen palette.
**Match system theme** in Settings controls the backgrounds of Snake, Tetris
and Sokoban. On uses the system background/panel colors and readable themed HUDs;
Off restores classic game backgrounds. It defaults to On and persists across
power cycles. The snake and apples, seven Tetris pieces, and Sokoban's player,
boxes, filled targets, goal dots and walls retain their own gameplay colors in
every theme, including Mono. All seven Tetris piece textures are retained. Invaders uses a true black gameplay background, including its HUD and
letterbox margins. Breakout and Doom's world palette retain their colors.

Themes persist in one versioned, checksummed 16-byte NVS record. Version 2 uses
a permanent Custom ID; version-1 presets and custom colors load unchanged when
upgrading. Previewing
and drawing frames never writes flash. Apply verifies the saved record before
activating it; a failed save keeps the previous active theme and offers retry.
Missing or invalid theme data falls back to Cyan without removing other settings
or saved games. No additional framebuffer is allocated.

## Native Snake and Tetris (TFT)

Snake keeps the existing 30×12 rules grid and collision/turn timing. Its TFT
renderer uses 7-pixel cells with sub-cell body movement, rounded outlines,
directional eyes, a tapered tail, and pulsing fruit. A consumed fruit stays
visible until the interpolated head reaches it; the displayed score follows
that visual arrival. The underlying score and saved game are unchanged. Only
the previous tail and fruit are retained for animation; no extra framebuffer
or body-history array is allocated. Pausing freezes the animation; loading a
checkpoint resumes from its exact rules position before interpolation restarts.

Tetris keeps the 10×20 grid, clockwise portrait orientation, controls, Game Boy
scoring, lock delay and save formats. The TFT board uses 10×10-pixel cells with
bevels and seven distinct textures, a colored ghost, crisp score/level/line
labels and next-piece preview. Line clears retain the existing timing and
pause/save behavior while using native-resolution flashes, center-out wipes,
sparks and a falling-stack animation. The OLED profile retains its original
renderers.

## Hidden Going Out mode

Hold A for 1.2 seconds in the library to enable Going Out mode. Its first home
page is a drink counter. Tap and release A to add one drink and play a liquid
pour animation; taps continue to work during the animation. Hold B for 1.2
seconds to reset the count. A short B press opens System. Use the D-pad to browse
the counter and all seven games. Hold A for 1.2 seconds on the counter to disable
the mode. Holding A during gameplay keeps its normal game function.

Both mode and count persist immediately after each change. With the mode enabled,
power-on opens the counter; saved games remain available through their cartridges.
Disabling the mode hides the counter and all its on-screen hints, preserving the
count for next time. Reset with B when starting a new count. Library A now opens
the selected game on release so a long hold does not accidentally launch a game.
The animation never blocks input or writes flash. A failed save leaves the last
committed count intact and shows SAVE FAILED on the counter; retry the action.

While Going Out mode is enabled, Settings has a fourth row, **Drink goal**.
Use Left/Right or A to set OFF (the default) or 1–99. The goal saves immediately
and appears beneath the counter. Reaching it with an A tap plays 2.2 seconds of
confetti with a GOAL REACHED banner; the pouring animation and input keep working.
It celebrates once per count, so going past the target or restarting at the
target does not replay it. Hold B to reset the count and allow it to trigger again.
The goal setting is hidden when Going Out mode is disabled, and its saved value
is retained. A failed goal save keeps the previous target and shows SAVE FAILED
in Settings; retry the edit.

### Going Out games

The hidden library now has ten pages: Counter, Reaction Duel, High or Low,
Pass the Bomb, then the seven arcade games. Use Left/Right or Up/Down to browse;
tap A to open a game. Each party game opens with instructions. **Left returns
to its library card at any time.** Holding A on a library card returns to the
counter; hold A on the counter to leave Going Out mode. The normal library
continues to show only its seven arcade games.

- **Reaction Duel:** two players take A and B. A readies the round, then wait
  for GO before pressing your button. The first player wins; an early press
  awards the opponent a point. Presses in the same input poll tie. The wait is
  randomly 1.5–5 seconds. Reaction time and a running score are shown; **Up**
  starts the next round. A/B presses after the result are ignored, so a slower
  player's press cannot accidentally restart it. A 10-second no-response
  timeout awards no point.
- **High or Low:** A deals a shuffled 52-card deck. A guesses higher, B guesses
  lower. Aces are high; equal ranks push without changing the streak. Correct
  guesses add one to the streak; misses reset it. Cards flip to reveal the
  result. A advances, or reshuffles when the deck is finished.
- **Pass the Bomb:** A starts a hidden random 15–35-second fuse. Pass the console
  around, tapping A on each pass. The fuse spark and bomb pulse accelerate toward
  the explosion; whoever holds it at BOOM loses. A starts another round.

Party games have visual feedback and short piezo cues. They never change
the drink counter, goal or saved arcade game. Rounds and scores last for the
current game session; rebooting opens the persistent counter as before.

## Pause, power off and resume

Pause the game and wait for **SAVED** on the OLED before switching off.
After power-on, the Continue screen offers **A: Resume**, **B: Library** or
**Left: New game**. For Tetris, turn the console right 90° before pressing A.
The saved cartridge also shows **SAVED GAME** in the library.

Snake, Tetris, Breakout, Invaders, Sokoban and Micro Motorways save their complete playable state, including Snake's queued
turns, Tetris' piece bag and line-animation phase, arcade ball/bullet positions,
Sokoban's current puzzle and its undo history, and Micro Motorways' city, cars, supplies and weekly reward. Game timers remain frozen while
off. The saved run also retains its difficulty; menu settings remain separate.

There is one checkpoint, for the most recently paused game. Pausing a different
game replaces it. Opening another game keeps the previous checkpoint until
that game is paused. Finishing or restarting the saved run removes its checkpoint.
Resuming keeps the last checkpoint until the next pause, so pause again before
switching off to save your latest position. A **SAVE FAILED** message means the
current pause was not saved; resume and pause again to retry.

The firmware writes one versioned, checksummed NVS blob on each pause and verifies
it before showing SAVED. It never saves every frame. Invalid or incompatible
checkpoints are ignored at boot, leaving the library usable. Existing settings,
high scores and completed Sokoban levels use their existing NVS keys.

Doom uses a separate native save slot. During live gameplay, A+B opens a frozen
pause screen and saves the full Doom level state. Wait for **SAVED** before
switching off. At boot, A on Continue restores that level, player inventory,
position, enemies and world state. Doom's cartridge also offers Continue;
Left on that screen deletes its checkpoint for a new game. Its save does not
replace the shared game checkpoint. After resuming, pause again to save newer
progress. Saves require a living player in an active level; title/demo screens
use the original Doom menu instead. Leaving Doom restarts the MCU and returns
directly to the library, freeing the engine's tasks and working memory.

Settings include three starting speeds, solid or wrapping walls, and 180-degree
screen rotation. Snake accelerates after every five foods. Best score and
settings persist in ESP32 NVS. Tetris has a separate saved best score, seven-piece
bags, a next-piece preview, a landing ghost, wall/floor kicks, 400 ms lock delay
and increasing gravity every ten lines. Speed settings apply to Snake and Tetris;
the wall setting applies to Snake. Breakout and Invaders use fixed 16 ms simulation
ticks so their speed does not depend on I²C transfer time. Each has a separate
saved high score, recorded at game over, exit or restart. Sokoban saves level
completion, and pausing saves an unfinished puzzle for resume. There is no battery indicator because no battery
sense pin was specified.

Tetris uses seven distinct monochrome tile textures for the I/O/T/S/Z/J/L pieces,
shown on falling pieces, the next-piece preview and the settled stack. Ghosts
remain outlines. The header alternates between total lines and current level.

Line-clear scoring follows the [original Game Boy instruction booklet](https://www.videogamemanual.com/gameboy/Tetris%20(USA).pdf):

| Lines cleared together | Base points |
| --- | --- |
| Single | 40 |
| Double | 100 |
| Triple | 300 |
| Tetris (four lines) | 1200 |

Multiply the base points by the current level + 1. Level begins at zero and
increases every ten lines. A Tetris therefore earns 7.5 times as much as four
separate singles. The footer briefly shows the clear type and awarded points.
Completed rows flash, wipe outwards from the center, then the stack slides down.
The animation lasts 420 ms for smaller clears and 650 ms for a Tetris. Gravity
and new-piece movement wait for it to finish; A can pause/resume the animation.
Soft drop still adds one point per cell and hard drop adds two per cell, retaining
the existing controls and modern hard-drop convenience. Classic scoring has its
own saved best-score record; the previous record remains stored separately.

The I²C bus and display driver both use 400 kHz, including after transfers.
The old 30 FPS cap has been removed: frames are requested at 60 FPS, with actual
refresh limited by full-frame I²C transfer. On this connected console, telemetry
measured 37 FPS and 25.1 ms per transfer. Serial telemetry measures actual
refresh and transfer times. This uses the
SSD1306's specified I²C timing rather than overclocking the bus.
