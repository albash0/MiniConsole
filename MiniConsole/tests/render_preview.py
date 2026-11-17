"""Render captured OLED framebuffers without third-party dependencies."""
from pathlib import Path
import struct
import sys
import zlib

source = Path(sys.argv[1])
destination = Path(sys.argv[2])
destination.mkdir(parents=True, exist_ok=True)

def load(name):
    words = (source / (name + '.pbm')).read_text().split()
    width, height = map(int, words[1:3])
    return width, height, list(map(int, words[3:]))

def chunk(kind, data):
    return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data) & 0xffffffff)

def grid(filename, names, columns=2, scale=4):
    images = [load(name) for name in names]
    cell_w = max(item[0] for item in images) * scale + 16
    cell_h = max(item[1] for item in images) * scale + 16
    width = columns * cell_w + 16
    height = ((len(images) + columns - 1) // columns) * cell_h + 16
    canvas = [bytearray([28] * width) for _ in range(height)]
    for i, (w, h, bits) in enumerate(images):
        left, top = 16 + (i % columns) * cell_w, 16 + (i // columns) * cell_h
        for y in range(h):
            for x in range(w):
                value = 240 if bits[y * w + x] else 0
                for sy in range(scale):
                    canvas[top + y * scale + sy][left + x * scale:left + (x + 1) * scale] = bytes([value]) * scale
    raw = b''.join(b'\x00' + bytes(row) for row in canvas)
    png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 0, 0, 0, 0))
    png += chunk(b'IDAT', zlib.compress(raw)) + chunk(b'IEND', b'')
    (destination / filename).write_bytes(png)

grid('five-game-library.png', ['SNAKE', 'breakout_play', 'invaders_play', 'sokoban_play'])
grid('library-cards.png', ['SNAKE', 'BLOCKS', 'BREAKOUT', 'INVADERS', 'SOKOBAN', 'DOOM', 'system'])
grid('game-screens.png', ['snake_ready', 'breakout_ready', 'invaders_ready', 'sokoban_ready', 'arcade_pause', 'arcade_over', 'sokoban_clear', 'sokoban_complete', 'library_complete'], columns=3)
grid('controls.png', ['controls_0', 'controls_1', 'controls_2', 'controls_3', 'controls_4', 'controls_5', 'controls_6'])
grid('blocks.png', ['blocks_ready', 'blocks_play'])
if (source / 'resume_0.pbm').exists():
    grid('pause-save.png', ['save_0', 'resume_0', 'save_4', 'resume_4'])
    grid('blocks-resume.png', ['save_1', 'resume_1'])
if (source / 'going_out_empty.pbm').exists():
    grid('going-out.png', ['going_out_empty', 'going_out_pour_early', 'going_out_pour_full', 'going_out_pour_drain', 'going_out_two', 'going_out_max'], columns=3)
if (source / 'drink_goal_settings.pbm').exists():
    grid('drink-goal.png', ['drink_goal_hidden_saved', 'drink_goal_settings', 'drink_goal_confetti_early', 'drink_goal_confetti'], columns=2)
if (source / 'party_card_0.pbm').exists():
    grid('party-library.png', ['party_card_0', 'party_card_1', 'party_card_2'], columns=3)
    grid('party-games.png', ['party_reaction_ready', 'party_reaction_go', 'party_reaction_result', 'party_cards_guess', 'party_cards_flip', 'party_cards_result', 'party_bomb_ready', 'party_bomb_running', 'party_bomb_exploded'], columns=3)
