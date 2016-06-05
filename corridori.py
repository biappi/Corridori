GAME_RESOLUTION = (320, 200)
MULTIPLIER      = 3
WINDOW_SIZE     = [i * MULTIPLIER for i in GAME_RESOLUTION]
FPS             = 18
TILE_WIDTH      = 16
TILE_HEIGTH     = 10
TILE_SIZE       = (TILE_WIDTH, TILE_HEIGTH)
ROOM_TILEWIDTH  = 0x14
ROOM_TILEHEIGTH = 0x14

class ResourcesPaths(object):
    def __init__(self, episode_base_path):
        self.base = episode_base_path

    def gamedir(self, thing):
        return '%s/GAME_DIR/%s' % (self.base, thing)

    def arcade_palette(self):         return self.gamedir("AR1/STA/ARCADE.PAL")
    def background_tileset(self, nr): return self.gamedir("AR1/STA/BUFFER%X.MAT" % nr)
    def room_roe(self):               return self.gamedir("AR1/MAP/ROOM.ROE")
    def k_ele(self):                  return self.gamedir("AR1/IMG/K.ELE")
    def tr_ele(self):                 return self.gamedir("AR1/IMG/TR.ELE")
    def ucc_ele(self, nr):            return self.gamedir("AR1/UCC/UCCI%d.ELE" % nr)
    def animjoy(self):                return self.gamedir("AR1/FIL/ANIMJOY.TAB")
    def frames(self):                 return self.gamedir("AR1/FIL/FRAMES.TAB")
    def animofs(self):                return self.gamedir("AR1/FIL/ANIMOFS.TAB")

def load_file(path):
    return [ord(i) for i in open(path, 'rb').read()]

def skip(l, n):
    for _ in xrange(n): next(l)

def oneoff(l):
    next(l)
    return next(l)

def word(l):
    return next(l) + (next(l) << 8)

def wordbe(l):
    return (next(l) << 8) + next(l)

def dword(l):
    return word(l) + (word(l) << 16)

def load_palette(path):
    data        = load_file(path)
    first_color = data[0]
    count       = data[1]

    palette = [(255, 0, 255) for i in xrange(0x100)]
    for i in xrange(count):
        palette[i + first_color] = (
            data[5 + (3 * i    )] << 2,
            data[5 + (3 * i + 1)] << 2,
            data[5 + (3 * i + 2)] << 2,
        )

    return palette

class Renderer:
    def __init__(self, width, height): pass
    def set_at(self, pos, colori, transparent): pass
    def image(self): pass

def get_background_tiles(path, pal, renderer=Renderer):
    TILE_BYTE_SIZE = TILE_WIDTH * TILE_HEIGTH

    data  = load_file(path)
    tiles = []

    for tile_i in xrange((320 / TILE_WIDTH) * (200 / TILE_HEIGTH)):
        tile = renderer(TILE_SIZE)

        for y in xrange(TILE_HEIGTH):
            for x in xrange(TILE_WIDTH):
                color = data[TILE_BYTE_SIZE * tile_i + TILE_WIDTH * y + x]
                tile.set_at((x, y), pal[color])

        tiles.append(tile.image())

    return tiles

def load_room_description(path):
    data  = load_file(path)
    size  = 0x4f4
    base  = 0x24
    rooms = []

    for i in xrange(len(data) / size):
        room_file = iter(data[i * size:])

        skip(room_file, 4)

        tileset_ids = [oneoff(room_file) for _ in xrange(0x10)]
        tile_ids    = [[word(room_file) for _ in xrange(ROOM_TILEWIDTH)]
                            for _ in xrange(ROOM_TILEHEIGTH)]

        skip(room_file, 0x20)
        tile_types  = [[next(room_file) for _ in xrange(ROOM_TILEWIDTH)]
                            for _ in xrange(ROOM_TILEHEIGTH)]

        rooms.append((
            tileset_ids,
            tile_ids,
            tile_types,
        ))

    return rooms


def blit_room(room, tilesets, frame, show_grid, blitter):
    def adjust_tile_for_frame(tile_id, frame):
        if ((tile_id & 0xf0) >> 4) != 9:
            return tile_id

        frame_to_use = frame

        if tile_id & 8:
            frame_to_use = 3
        else:
            if (tile_id & 4) and (frame == 3):
                    tile_id = tile_id | 8

        return (tile_id & 0xfcff) | (frame_to_use <<  8)

    tileset_ids, tile_ids, tile_types = room

    for y in xrange(ROOM_TILEHEIGTH):
        for x in xrange(ROOM_TILEWIDTH):
            tile_id    = tile_ids[y][x]
            tile_id    = adjust_tile_for_frame(tile_id, frame)
            tile_nr    = ((tile_id & 1) << 8) + (tile_id >>8)
            tileset_id = (tile_id & 0xf0) >> 4
            flip       = tile_id & 2
            tileset_nr = tileset_ids[tileset_id]
            tileset    = tilesets[tileset_nr]
            bitmap     = tileset[tile_nr]
            topleft    = (x * TILE_WIDTH, y * TILE_HEIGTH)

            blitter(bitmap, topleft, flip)

def load_ele_file(path):
    data        = load_file(path)
    ele_file    = iter(data)
    count       = word(ele_file)
    relocs      = [dword(ele_file) for _ in xrange(count)]
    images_data = [data[i + 2:] for i in relocs]
    images      = []

    for i in images_data:
        ele_item = iter(i)
        width    = word(ele_item)
        heigth   = word(ele_item)

        skip(ele_item, 1)

        lines = []
        for _ in xrange(heigth):
            line = []
            lines.append(line)

            while True:
                thing = next(ele_item)
                line.append(thing)
                if thing == 0xff:
                    break

        lines = reduce(lambda a, b: a + b, lines) + [0xff, 0xff]

        images.append((width, heigth, lines))

    return images

def render_ele_item(item, palette, col=-63, renderer=Renderer):
    width, heigth, lines = item

    the_ele        = iter(lines)
    consecutive_ff = 0
    surface        = renderer((width, heigth))
    cur_x, cur_y   = 0, 0

    while True:
        skip = next(the_ele)

        if skip != 0xff:
            consecutive_ff = 0
            cur_x += skip

            count = next(the_ele)

            if count != 0xff:
                consecutive_ff = 0

                for _ in xrange(count / 2):
                    colors = next(the_ele)
                    color1 = (colors & 0x0f)      + col
                    color2 = (colors & 0xf0 >> 4) + col

                    surface.set_at((cur_x, cur_y), palette[color1])
                    cur_x += 1
                    surface.set_at((cur_x, cur_y), palette[color2])
                    cur_x += 1

                if count & 1:
                    color = next(the_ele)
                    color1 = (color & 0x0f) + col
                    surface.set_at((cur_x, cur_y), palette[color1])
                    cur_x += 1
            else:
                consecutive_ff += 1
                if consecutive_ff == 3:
                    return surface.image()
        else:
            cur_x = 0
            cur_y += 1

            consecutive_ff += 1
            if consecutive_ff == 3:
                return surface.image()

# animjoy is a list of arrays, the inner arrays are indexed based
# on user input using these indices.

STILL, LEFT, RIGHT, TOP, BOTTOM, NO_RUNNING, RUNNING = xrange(7)

DIRECTIONS_TO_ANIMJOY = {
    (STILL, STILL,  NO_RUNNING): 0,
    (STILL, TOP, NO_RUNNING): 1,
    (RIGHT, TOP, NO_RUNNING): 2,
    (RIGHT, STILL,  NO_RUNNING): 3,
    (RIGHT, BOTTOM,    NO_RUNNING): 4,
    (STILL, BOTTOM,    NO_RUNNING): 5,
    (LEFT,  BOTTOM,    NO_RUNNING): 6,
    (LEFT,  STILL,  NO_RUNNING): 7,
    (LEFT,  TOP, NO_RUNNING): 8,
    (STILL, STILL,  RUNNING   ): 9,
    (STILL, TOP, RUNNING   ): 10,
    (RIGHT, TOP, RUNNING   ): 11,
    (RIGHT, STILL,  RUNNING   ): 12,
    (RIGHT, BOTTOM,    RUNNING   ): 13,
    (STILL, BOTTOM,    RUNNING   ): 14,
    (LEFT,  BOTTOM,    RUNNING   ): 15,
    (LEFT,  STILL,  RUNNING   ): 16,
    (LEFT,  TOP, RUNNING   ): 17,
}

def load_animjoy(path):
    data    = load_file(path)
    sizeof  = 18
    howmany = len(data) // sizeof
    animjoy = iter(data)

    return [[next(animjoy) for _ in xrange(sizeof)] for _ in xrange(howmany)]

def clamp(thing, interval):
    low, high = interval
    x = max(low, thing)
    x = min(x, high)
    return x

def to_signed_byte(byte):
    return byte if byte < 128 else (256 - byte) * -1

class Frame():
    def __init__(self, frame, time, x_delta, y_delta, flip):
        self.time    = time
        self.frame   = frame
        self.x_delta = x_delta
        self.y_delta = y_delta
        self.flip    = flip

    def __repr__(self):
        return ', '.join('%s: %04x' % i for i in self.__dict__.iteritems())

def load_frames(path):
    data    = load_file(path)
    howmany = 106
    index   = iter(data)

    offsets = [wordbe(index) for i in xrange(howmany)]

    anims   = []

    for offset in offsets:
        f = iter(data[offset:])
        frames  = []
        anims.append(frames)

        frame_off = offset

        while True:
            _frame = next(f)
            _time = next(f)
            
            if _time == 0 and _frame == 0:
            	break

            frame = Frame(_frame,
            	      _time,
            	      to_signed_byte(next(f)),
            	      to_signed_byte(next(f)),
            	      next(f))
            frame.offset = frame_off
            frame_off += 5 # 5 = sizeof frame
            frames.append(frame)

    return anims

def load_animofs(path):
    data    = load_file(path)
    animofs = iter(data)

    offsets = []
    while True:
        off = wordbe(animofs)
        if off == 0xf0f0:
            break
        offsets.append(off)

    sorted_offs = sorted(offsets)
    contents    = { start: [to_signed_byte(i) for i in data[start:end] ]
                      for start, end in zip(sorted_offs[:-1],
                                            sorted_offs[1:]) }

    contents[sorted_offs[-1]] = data[sorted_offs[-1]:]

    return [contents[i] for i in offsets]

class TimeRunners:
    def __init__(self, renderer):
        self.renderer   = renderer

        resources  = ResourcesPaths("episodes/ep1/")

        self.palette    = load_palette(resources.arcade_palette())
        self.tilesets   = {i: get_background_tiles(resources.background_tileset(i), self.palette, self.renderer) for i in (1, 2, 3, 4, 5, 6, 10, 11)}
        self.rooms      = load_room_description(resources.room_roe())
        self.kele       = [render_ele_item(i, self.palette, renderer=self.renderer) for i in load_ele_file(resources.k_ele())]
        self.trele      = [render_ele_item(i, self.palette, renderer=self.renderer) for i in load_ele_file(resources.tr_ele())]
        self.all_ucc    = [load_ele_file(resources.ucc_ele(i)) for i in xrange(2)]
        self.all_ucc    = reduce(lambda a, b: a + b, self.all_ucc)
        self.uccele     = [render_ele_item(i, self.palette, renderer=self.renderer) for i in self.all_ucc]
        self.animjoy    = load_animjoy(resources.animjoy())
        self.frames     = load_frames(resources.frames())
        self.animofs    = load_animofs(resources.animofs())

        self.current_room             = 0
        self.current_background_frame = 0
        self.current_k_ele            = 0
        self.current_ucc_ele          = 0
        self.current_show_grid        = False

        # lol 1992
        self.background_frame_delay_init = 1
        self.background_frame_delay      = self.background_frame_delay_init

        self.current_ani_frame        = 0
        self.current_ani_counter      = 0

        # patches:
        # 1. remove check for tiles (mostly floor)
        # 2. remove check for tiles (mostly else)
        # 3. remove pupo_x changes when input
        # 4. no health for monster
        # 5. hide mouse ptr

        # iv 406c:3af8 pupo_bob_x
        # iv 406c:3b5c pupo_bob_y
        # iv 406c:2efb current_ani
        # iv 406c:2ef6 pupo_x
        # iv 406c:2ef8 pupo_y
        # iv 406c:2f04 vita
        # iv 406c:094f delta_x
        # iv 406c:0952 frame
        # iv 406c:094d pupo_tr_frame

        # sm 32a9:1823 eb 34
        # sm 32a9:1865 eb 3f
        # sm 32a9:1926 90 90 90 90
        # sm 32a9:19b7 90 90 90 90
        # sm 3aad:0211 90 90 90 90
        # sm 31b3:0626 90 90 90 90 90

        # bpm 406c:0952
        # bpm 406c:094f
        # bpm 406c:2efb
        # bpm 406c:3af8
        
        # 406c:42a6 -- left_pressed

        # taken from debugger

        self.current_ani_countdown    = 3
        self.current_ani              = 0x03
        self.pupo_x                   = 0xa8
        self.pupo_y                   = 0xa0

    def step(self, direction):
        self.background_frame_delay -= 1
        if self.background_frame_delay == 0:
            self.current_background_frame = (self.current_background_frame + 1) % 4
            self.background_frame_delay = self.background_frame_delay_init

        self.current_k_ele = (self.current_k_ele + 1) % len(self.kele)
        self.current_ucc_ele = (self.current_ucc_ele + 1) % len(self.uccele)

        if self.current_ani_countdown == 0:
                self.current_ani_frame += 1
                try:
                    c = self.frames[self.current_ani][self.current_ani_frame].time
                    self.current_ani_countdown = (c + 3) / 4
                except:
                    self.current_ani_countdown = 3
        else:
                self.current_ani_countdown -=1

        if self.current_ani_frame >= len(self.frames[self.current_ani]):
            # current ani done
            self.pupo_x += self.animofs[2][self.current_ani]
            self.pupo_y += self.animofs[3][self.current_ani]

            dove_muovere = DIRECTIONS_TO_ANIMJOY[direction]
            self.current_ani_frame = 0
            self.current_ani = self.animjoy[self.current_ani][dove_muovere]

            self.pupo_x += self.animofs[0][self.current_ani]
            self.pupo_y += self.animofs[1][self.current_ani]

        # print "ani: ", self.current_ani, "frame: ", self.current_ani_frame
        
    def blit(self, blitter):
        try:
            self.blit2(blitter)
        except Exception as e:
            import traceback
            traceback.print_exc()

    def blit2(self, blitter):
        blit_room(
            self.rooms[self.current_room],
            self.tilesets,
            self.current_background_frame,
            self.current_show_grid,
            blitter,
        )

        #blitter(self.kele[self.current_k_ele], (10, 10))
        #blitter(self.uccele[self.current_ucc_ele], (100, 10))

        f = self.frames[self.current_ani][self.current_ani_frame]

        TR  = self.trele[f.frame]
        POS = self.pupo_x + f.x_delta - TR.size()[0] // 2, \
              self.pupo_y + f.y_delta - TR.size()[1]

        blitter(
            TR,
            POS,
            bool(f.flip),
        )


if __name__ == '__main__':
    resources  = ResourcesPaths("episodes/ep1/")

    class Suca:
        def __init__(self, size): self.s = size
        def set_at(self, pos, color): pass
        def image(self): return self

    trele      = [render_ele_item(i, [None] * 0x300, renderer=Suca) for i in load_ele_file(resources.tr_ele())]
    frames     = load_frames(resources.frames())
    animofs    = load_animofs(resources.animofs())

    offs = [(animofs[0][i], animofs[1][i]) for i in xrange(0x38)]

    for i, f in enumerate(frames):
        for k in f:
            print i, k
    
    for i, s in enumerate(trele):
        print i, ','.join(map(hex, s.s))

    for i, o in enumerate(offs):
        print hex(i), ' -- ', ','.join(map(hex, o))

