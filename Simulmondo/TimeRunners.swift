//
//  TimeRunners.swift
//  Simulmondo
//
//  Created by Antonio Malara on 19/03/2017.
//  Copyright © 2017 Antonio Malara. All rights reserved.
//

import Cocoa

struct Episode {
    static let TILE_SIZE = Size(width:  16, height:  10)
    
    let palette: Palette
    let tilesets : [Tileset?]
    let rooms : [Room]
    
    let eles: [Bitmap]
    
    let frames: [[Frame]]
    let animofs: Animofs
    let animjoy: [Animjoy]
    
    init?(url: URL) {
        let gameDirUrl  = url.appendingPathComponent("GAME_DIR")
        let ar1Url      = gameDirUrl.appendingPathComponent("AR1")
        let staUrl      = ar1Url.appendingPathComponent("STA")
        let mapUrl      = ar1Url.appendingPathComponent("MAP")
        let filUrl      = ar1Url.appendingPathComponent("FIL")
        let bgTiles1Url = staUrl.appendingPathComponent("BUFFER1.MAT")
        let bgTiles2Url = staUrl.appendingPathComponent("BUFFER2.MAT")
        let bgTiles3Url = staUrl.appendingPathComponent("BUFFER3.MAT")
        let bgTiles4Url = staUrl.appendingPathComponent("BUFFER4.MAT")
        let bgTiles5Url = staUrl.appendingPathComponent("BUFFER5.MAT")
        let bgTiles6Url = staUrl.appendingPathComponent("BUFFER6.MAT")
        let bgTilesAUrl = staUrl.appendingPathComponent("BUFFERA.MAT")
        let bgTilesBUrl = staUrl.appendingPathComponent("BUFFERB.MAT")
        let paletteUrl  = staUrl.appendingPathComponent("ARCADE.PAL")
        let roomRoeUrl  = mapUrl.appendingPathComponent("ROOM.ROE")
        let imgUrl      = ar1Url.appendingPathComponent("IMG")
        let keleUrl     = imgUrl.appendingPathComponent("K.ELE")
        let treleUrl    = imgUrl.appendingPathComponent("TR.ELE")
        let framesUrl   = filUrl.appendingPathComponent("FRAMES.TAB")
        let animofsUrl  = filUrl.appendingPathComponent("ANIMOFS.TAB")
        let animjoyUrl  = filUrl.appendingPathComponent("ANIMJOY.TAB")
        
        
        func TRTileset(url: URL) throws -> Tileset? {
            var bytes = try Data(contentsOf: url).makeIterator()
            return bytes.parseTilesetFile(tileSize: Episode.TILE_SIZE)
        }
        
        do {
            tilesets = [
                nil,
                try TRTileset(url: bgTiles1Url)!,
                try TRTileset(url: bgTiles2Url)!,
                try TRTileset(url: bgTiles3Url)!,
                try TRTileset(url: bgTiles4Url)!,
                try TRTileset(url: bgTiles5Url)!,
                try TRTileset(url: bgTiles6Url)!,
                nil,
                nil,
                nil,
                try TRTileset(url: bgTilesAUrl)!,
                try TRTileset(url: bgTilesBUrl)!,
            ]
            
            //
            
            var paletteData = try Data(contentsOf: paletteUrl).makeIterator()
            var roomroeData = try Data(contentsOf: roomRoeUrl).makeIterator()
            var treleData   = try Data(contentsOf: treleUrl).makeIterator()
            var framesData  = try Data(contentsOf: framesUrl).makeIterator()
            var animofsData = try Data(contentsOf: animofsUrl).makeIterator()
            var animjoyData = try Data(contentsOf: animjoyUrl).makeIterator()
            
            palette = paletteData.parsePaletteFile()!
            rooms   = roomroeData.parseRoomFile()!
            eles    = treleData.parseEleFile()!
            frames  = framesData.parseFramesFile()!
            animofs = animofsData.parseAnimofsFile()!
            animjoy = animjoyData.parseAnimjoyFile()!
        }
        catch {
            return nil
        }
    }
}

func load(window: NSWindow) {
    let gameUrl = Bundle.main.url(forResource:   "Time Runners",
                                  withExtension: nil)!
    
    let episodeUrl  = gameUrl.appendingPathComponent("Ep. 1")
    let episode = Episode(url: episodeUrl)!
    
    let trEleNSImages = episode.eles.map {
        (b: Bitmap) -> (NSImage, NSImage) in
        
        let i = b.createNSImage(palette: episode.palette, baseColor: 225)
        return (i, i.flippedCopy())
    }

    //
    
    let ROOM_TIMER = 4
    
    struct Tr {
        var pupoPos      = Point(x: 0xa8, y: 0xa0)
        
        var pupoAni      = 0x03
        var pupoAniFrame = 0
        var pupoAniTimer = 3
        
        var room         = 0
        var roomFrame    = 0
        var roomTimer    = 4
    }
    
    func tick(tr: inout Tr, input: inout Input) {
        if tr.roomTimer == 0 {
            tr.roomFrame = (tr.roomFrame + 1) % 4
            tr.roomTimer = ROOM_TIMER
        } else {
            tr.roomTimer -= 1
        }
        
        if tr.pupoAniTimer == 0 {
            tr.pupoAniFrame += 1
            tr.pupoAniTimer = (episode.frames[safe: tr.pupoAni]?[safe: tr.pupoAniFrame]?.time).map {
                ($0 + 3) / 4
            } ?? 3
        }
        else {
            tr.pupoAniTimer -= 1
        }
        
        if tr.pupoAniFrame >= episode.frames[tr.pupoAni].count {
            tr.pupoPos = tr.pupoPos.adding(by: episode.animofs.pre[tr.pupoAni])
            
            tr.pupoAniFrame = 0
            tr.pupoAni = episode.animjoy[tr.pupoAni].nextAni(direction: input)
            
            tr.pupoPos = tr.pupoPos.adding(by: episode.animofs.post[tr.pupoAni])
        }
    }

    
    var tr = Tr()
    var input = Input(.still, .still, .nonFiring)
    
    let v = RoomView(
        tiles: Room.tiles,
        tileSize: Episode.TILE_SIZE,
        tilesetsImages: createTilesetsNSImages(
            tilesets: episode.tilesets,
            palette: episode.palette
        )
    )
    
    v.frame = NSRect(x: 0, y: 0, width: 320 * 3, height: 200 * 3)
    
    window.setContentSize(v.frame.size)
    window.contentView = v
    window.makeFirstResponder(v)
    
    let t = NSImageView(frame: NSRect.zero)
    t.imageScaling = .scaleProportionallyUpOrDown
    v.addSubview(t)
   
    var canSetInput = true
    v.inputDidChange = {
        if canSetInput {
            input = $0
            canSetInput = false
        }
    }
    
    func apply(tr: inout Tr) {
        v.setRoom(room:  episode.rooms[tr.room],
                  frame: tr.roomFrame)
        
        let frame     = episode.frames[tr.pupoAni][tr.pupoAniFrame]
        let ele       = episode.eles[frame.frame]
        
        let pupoPos   = tr.pupoPos.adding(by: frame.delta).adding(by: Point(
            x: -(ele.size.width / 2),
            y: -(ele.size.height)
        ))
        
        let image = frame.flip ? trEleNSImages[frame.frame].1 : trEleNSImages[frame.frame].0
        
        if image !== t.image {
            t.image = image
        }
        
        t.frame = ele.size.multiplied(by: 3).nsrect(origin: pupoPos.multiplied(by: 3))
    }
    
    Timer.scheduledTimer(withTimeInterval: 1 / 20.0, repeats: true) { _ in
        tick(tr: &tr, input: &input)
        apply(tr: &tr)
        canSetInput = true
    }
}

