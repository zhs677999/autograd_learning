from pathlib import Path
from PIL import Image, ImageDraw, ImageFilter
import math
import random

OUT = Path(__file__).resolve().parent / "assets"
OUT.mkdir(parents=True, exist_ok=True)

W, H = 1280, 720

PALETTES = [
    ("#18BDF2", "#FF7A1A", "#7DE3FF"),
    ("#00D4FF", "#FFC857", "#8BF3D1"),
    ("#12A8E8", "#FF8A3D", "#6BE7FF"),
    ("#26C6FF", "#52D273", "#FF7A1A"),
]


def hex_rgba(value, alpha):
    value = value.lstrip("#")
    return tuple(int(value[i : i + 2], 16) for i in (0, 2, 4)) + (alpha,)


def bezier(p0, p1, p2, p3, steps=140):
    pts = []
    for i in range(steps + 1):
        t = i / steps
        x = (
            (1 - t) ** 3 * p0[0]
            + 3 * (1 - t) ** 2 * t * p1[0]
            + 3 * (1 - t) * t**2 * p2[0]
            + t**3 * p3[0]
        )
        y = (
            (1 - t) ** 3 * p0[1]
            + 3 * (1 - t) ** 2 * t * p1[1]
            + 3 * (1 - t) * t**2 * p2[1]
            + t**3 * p3[1]
        )
        pts.append((x, y))
    return pts


def draw_grid(draw, color):
    for x in range(-120, W + 180, 52):
        draw.line([(x, 0), (x + 260, H)], fill=color, width=1)
    for y in range(40, H, 72):
        draw.line([(0, y), (W, y + 20)], fill=color, width=1)


for idx in range(1, 13):
    random.seed(idx * 19)
    img = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    glow = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    g = ImageDraw.Draw(glow)
    cyan, orange, pale = PALETTES[idx % len(PALETTES)]

    draw_grid(d, hex_rgba("#7DE3FF", 16))

    base_y = 120 + (idx % 5) * 56
    for lane in range(4):
        offset = lane * 42
        p0 = (-160, base_y + offset + random.randint(-40, 40))
        p1 = (260 + random.randint(-80, 140), 20 + random.randint(0, 220))
        p2 = (820 + random.randint(-160, 100), 600 - random.randint(0, 260))
        p3 = (1440, 180 + offset + random.randint(-90, 120))
        pts = bezier(p0, p1, p2, p3, 160)
        color = [cyan, orange, pale, cyan][lane]
        g.line(pts, fill=hex_rgba(color, 72 if lane != 1 else 88), width=14 - lane * 2)
        d.line(pts, fill=hex_rgba(color, 170 if lane != 1 else 210), width=3)

        for j in range(10, len(pts), 30):
            x, y = pts[j]
            r = 2 + (j // 30) % 3
            d.ellipse((x - r, y - r, x + r, y + r), fill=hex_rgba(color, 210))

    for _ in range(48):
        x = random.randint(0, W)
        y = random.randint(0, H)
        r = random.choice([1, 1, 2])
        d.ellipse((x - r, y - r, x + r, y + r), fill=hex_rgba("#D9F7FF", random.randint(40, 110)))

    glow = glow.filter(ImageFilter.GaussianBlur(14))
    out = Image.alpha_composite(glow, img)
    out.save(OUT / f"flow_{idx:02d}.png")

print(OUT)
