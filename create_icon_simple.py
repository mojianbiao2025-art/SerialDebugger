from PIL import Image, ImageDraw

def make_icon(size):
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    s = size / 256.0
    
    # Blue circle
    m = int(20 * s)
    draw.ellipse([m, m, size-m, size-m], fill=(52, 152, 219), outline=(41, 128, 185), width=max(1, int(6*s)))
    
    # White box
    bm = int(50 * s)
    draw.rounded_rectangle([bm, bm, size-bm, size-bm], radius=int(12*s), fill=(255, 255, 255), outline=(44, 62, 80), width=max(1, int(4*s)))
    
    # Pins
    c = size // 2
    pr = max(2, int(8*s))
    sp = int(25 * s)
    ty = int(c - 25*s)
    by = int(c + 5*s)
    
    for i in [-2, -1, 0, 1, 2]:
        x = c + i * sp
        draw.ellipse([x-pr, ty-pr, x+pr, ty+pr], fill=(44, 62, 80))
    
    for i in [-1.5, -0.5, 0.5, 1.5]:
        x = int(c + i * sp)
        draw.ellipse([x-pr, by-pr, x+pr, by+pr], fill=(44, 62, 80))
    
    # Wave
    wy = int(c + 40*s)
    wh = max(1, int(8*s))
    ww = max(2, int(4*s))
    lw = max(1, int(3*s))
    
    for i in range(0, int(100*s), ww):
        x1 = int(c - 50*s + i)
        x2 = min(x1 + ww, int(c + 50*s))
        y1 = wy - wh if (i // ww) % 2 == 0 else wy + wh
        y2 = wy + wh if (i // ww) % 2 == 0 else wy - wh
        draw.line([(x1, y1), (x2, y1)], fill=(46, 204, 113), width=lw)
        if x2 < int(c + 50*s):
            draw.line([(x2, y1), (x2, y2)], fill=(46, 204, 113), width=lw)
    
    return img

sizes = [16, 32, 48, 64, 128, 256]
icons = []

for sz in sizes:
    print(f"Creating {sz}x{sz}...")
    img = make_icon(sz)
    img.save(f'app_icon_{sz}.png')
    icons.append(img)

print("Saving ICO...")
icons[0].save('app_icon.ico', format='ICO', sizes=[(s, s) for s in sizes], append_images=icons[1:])
print("Done!")
