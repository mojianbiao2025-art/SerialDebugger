#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Create sharp application icon for Serial Port Debugger
"""

try:
    from PIL import Image, ImageDraw
    
    def create_sharp_icon(size):
        """Create a sharp icon at specific size"""
        img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # Scale factor
        s = size / 256.0
        
        # Background circle
        margin = int(20 * s)
        draw.ellipse([margin, margin, size-margin, size-margin], 
                     fill=(52, 152, 219), 
                     outline=(41, 128, 185), 
                     width=max(1, int(6*s)))
        
        # White connector box
        box_margin = int(50 * s)
        box_radius = int(12 * s)
        draw.rounded_rectangle(
            [box_margin, box_margin, size-box_margin, size-box_margin],
            radius=box_radius,
            fill=(255, 255, 255),
            outline=(44, 62, 80),
            width=max(1, int(4*s))
        )
        
        # DB9 pins
        center = size // 2
        pin_r = max(2, int(8 * s))
        pin_inner = max(1, int(4 * s))
        spacing = int(25 * s)
        
        # Top 5 pins
        top_y = int(center - 25 * s)
        for i in range(-2, 3):
            x = center + i * spacing
            draw.ellipse([x-pin_r, top_y-pin_r, x+pin_r, top_y+pin_r], fill=(44, 62, 80))
            draw.ellipse([x-pin_inner, top_y-pin_inner, x+pin_inner, top_y+pin_inner], fill=(149, 165, 166))
        
        # Bottom 4 pins
        bottom_y = int(center + 5 * s)
        for i in range(-2, 2):
            x = center + (i + 0.5) * spacing
            draw.ellipse([x-pin_r, bottom_y-pin_r, x+pin_r, bottom_y+pin_r], fill=(44, 62, 80))
            draw.ellipse([x-pin_inner, bottom_y-pin_inner, x+pin_inner, bottom_y+pin_inner], fill=(149, 165, 166))
        
        # Signal wave
        wave_y = int(center + 40 * s)
        wave_h = int(8 * s)
        wave_w = int(4 * s)
        line_w = max(1, int(3 * s))
        
        x = int(center - 50 * s)
        end_x = int(center + 50 * s)
        y = wave_y
        high = True
        
        while x < end_x:
            next_x = min(x + wave_w, end_x)
            next_y = wave_y - wave_h if high else wave_y + wave_h
            draw.line([(x, y), (next_x, y)], fill=(46, 204, 113), width=line_w)
            draw.line([(next_x, y), (next_x, next_y)], fill=(46, 204, 113), width=line_w)
            x = next_x
            y = next_y
            high = not high
        
        return img
    
    # Generate all sizes
    sizes = [16, 32, 48, 64, 128, 256]
    icons = []
    
    print("Generating icons...")
    for size in sizes:
        img = create_sharp_icon(size)
        img.save(f'app_icon_{size}.png')
        icons.append(img)
        print(f"  Created {size}x{size}")
    
    # Save ICO
    icons[0].save('app_icon.ico', format='ICO', 
                  sizes=[(s, s) for s in sizes], 
                  append_images=icons[1:])
    
    print("\nSuccess! Created:")
    print("  - app_icon.ico")
    print("  - app_icon_16.png to app_icon_256.png")
    
except ImportError:
    print("Error: Pillow not installed")
    print("Run: pip install Pillow")
except Exception as e:
    print(f"Error: {e}")
    import traceback
    traceback.print_exc()
