#!/usr/bin/env python

from PIL import Image
from sys import stderr, argv
import fontforge, csv, tempfile, os

geometry = [
  {
    'name': 'Numeric',
    'left': 30, 'top': 20, 'width': 19, 'height': 24, 'fontwidth': 16, 'fontheight': 32,
    'rows': [
      0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
      0xb0, 0xd0,
      0x170, 0x230, 0x370, 0x390, 0x3a0, 0x3b0, 0x3c0,
      0x2000, 0x2010, 0x2070, 0x2080, 0x2090,
      0x2100, 0x2140, 0x2160, 0x21c0,
      0x2210, 0x2220, 0x2240, 0x2260, 0x2290,
      0x2390, 0x23a0,
      0x2420, 0x2460, 0x2470, 0x2480, 0x2490, 0x24a0, 0x24b0, 0x24c0, 0x24d0, 0x24e0,
    ]
  },
  {
    'name': 'Standard',
    'left': 390, 'top': 14, 'width': 15, 'height': 21, 'fontwidth': 14, 'fontheight': 20,
    'rows': [
      0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
      0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0,
      0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170,
      0x230, 0x370, 0x390, 0x3a0, 0x3b0, 0x3c0,
      0x2000, 0x2010, 0x2020, 0x2070, 0x2080, 0x2090, 0x20a0,
      0x2100, 0x2110, 0x2120, 0x2140, 0x2160, 0x2190, 0x21c0, 0x21e0,
      0x2200, 0x2210, 0x2220, 0x2230, 0x2240, 0x2250, 0x2260, 0x2290, 0x22a0, 0x22b0,
      0x2300, 0x2310, 0x2390, 0x23a0, 0x23f0,
      0x2420, 0x2430, 0x2460, 0x2470, 0x2480, 0x2490, 0x24a0, 0x24b0, 0x24c0, 0x24d0, 0x24e0,
      0x2710,
    ]
  },
]

def getcolor(pxdat):
  if pxdat[3] == 0:
    return -1
  else:
    return (4 if pxdat[0] >= 128 else 0) + (2 if pxdat[1] >= 128 else 0) + (1 if pxdat[2] >= 128 else 0)

if len(argv) < 2 + len(geometry):
  stderr.write("Usage: " + argv[0] + " Charsets_fa.png " + " ".join(list(map(lambda g: 'WP43_' + g['name'] + 'Font.ttf', geometry))) + "\n")
  exit(1)
pngpath = argv[1]
fontfiles = argv[2:]
for i in range(0, len(geometry)):
  geometry[i]['fontfile'] = fontfiles[i]

img = Image.open(pngpath)
px = img.load()

for g in geometry:
  sfd = tempfile.NamedTemporaryFile(mode='w+', delete=False)

  glyphnum = 0
  stderr.write(g['name'] + ":\n")
  ttf = fontforge.open(g['fontfile'])
  ascent = ttf.ascent
  descent = ttf.descent
  for glyph in ttf.glyphs():
    if glyph.isWorthOutputting():
      if glyph.glyphname == '.notdef' or glyph.glyphname == '.null' or glyph.glyphname == 'nonmarkingreturn' or glyph.glyphname == '.nochar':
        glyphnum = glyphnum + 1
      else:
        ttf.removeGlyph(glyph)
  ttf.save(sfd.name)
  ttf.close()
  sfd_head = sfd.readlines()
  sfd_head.pop()
  sfd_head.pop()
  sfd.close()
  os.unlink(sfd.name)
  del sfd

  lines = []
  yy = g['top']
  ycnt = 0
  x = g['left'] - (g['width'] - g['fontwidth'] + 1)
  for y in range(g['top'], img.size[1]):
    if getcolor(px[x, y]) == 7 or getcolor(px[x, y]) == 4:
      ycnt = ycnt + 1
      if ycnt == 1:
        yy = y
      elif ycnt == g['fontheight']:
        lines.append(yy)
        if len(lines) >= len(g['rows']):
          break;
    elif getcolor(px[x, y]) == 6:
      ycnt = 0

  sfd_body = ""
  dot_size = (ascent + descent) / g['fontheight']
  for row in range(0, len(lines)):
    for cell in range(0, 16):
      stderr.write("Processing: U+{0:04X}\r".format(g['rows'][row] + cell))
      glyphdat = [""] * g['fontheight']
      glyph_exists = True
      for x in range(0, g['fontwidth']):
        defined_pixels = 0
        for y in range(0, g['fontheight']):
          color_class = getcolor(px[g['left'] + g['width'] * cell + x, lines[row] + y])
          if color_class == 7 or color_class == 0:
            defined_pixels = defined_pixels + 1
        if defined_pixels >= g['fontheight'] / 2:
          for y in range(0, g['fontheight']):
            color_class = getcolor(px[g['left'] + g['width'] * cell + x, lines[row] + y])
            glyphdat[y] = glyphdat[y] + ("1" if color_class == 0 else "0")
        else:
          if x <= 0:
            glyph_exists = False
          break
      if glyph_exists:
        sfd_body = sfd_body + "\n"
        sfd_body = sfd_body + "StartChar: uni{0:04X}\n".format(g['rows'][row] + cell)
        sfd_body = sfd_body + "Encoding: {0} {0} {1}\n".format(g['rows'][row] + cell, glyphnum)
        sfd_body = sfd_body + "Width: {0}\n".format(int(dot_size * len(glyphdat[0])))
        sfd_body = sfd_body + "GlyphClass: 2\n"
        sfd_body = sfd_body + "Flags: W\n"
        sfd_body = sfd_body + "LayerCount: 2\n"
        sfd_body = sfd_body + "Fore\n"
        sfd_body = sfd_body + "SplineSet\n"
        dots = 0
        for y in range(0, g['fontheight']):
          for x in range(0, len(glyphdat[0])):
            if glyphdat[y][x] == "1":
              sfd_body = sfd_body + "{0} {1} m 1,{2},-1\n".format(  x      * dot_size, ascent - (y + 1) * dot_size, dots * 4    )
              sfd_body = sfd_body + " {0} {1} l 1,{2},-1\n".format( x      * dot_size, ascent -  y      * dot_size, dots * 4 + 1)
              sfd_body = sfd_body + " {0} {1} l 1,{2},-1\n".format((x + 1) * dot_size, ascent -  y      * dot_size, dots * 4 + 2)
              sfd_body = sfd_body + " {0} {1} l 1,{2},-1\n".format((x + 1) * dot_size, ascent - (y + 1) * dot_size, dots * 4 + 3)
              sfd_body = sfd_body + " {0} {1} l 1,{2},-1\n".format( x      * dot_size, ascent - (y + 1) * dot_size, dots * 4    )
              dots = dots + 1
        sfd_body = sfd_body + "EndSplineSet\n"
        sfd_body = sfd_body + "EndChar\n"
        glyphnum = glyphnum + 1

  stderr.write("Processing: Done  \n")
  with tempfile.NamedTemporaryFile(mode='w+', delete=False) as sfd:
    for l in sfd_head:
      if "BeginChars:" in l:
        tokens = l.split()
        sfd.write("BeginChars: {0} {1}\n".format(tokens[1], glyphnum))
      else:
        sfd.write(l)
    sfd.write(sfd_body)
    sfd.write("EndChars\n")
    sfd.write("EndSplineSet\n")

  font = fontforge.open(sfd.name)
  stderr.write("Writing to " + g['fontfile'] + "\n")
  try:
    os.rename(g['fontfile'], g['fontfile'] + "~")
  except FileExistsError:
    os.unlink(g['fontfile'] + "~")
    os.rename(g['fontfile'], g['fontfile'] + "~")
  font.generate(g['fontfile'])
  font.close()
  os.unlink(sfd.name)
  del sfd
