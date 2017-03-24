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
    
    let palette:  Palette
    let tilesets: [Tileset?]
    let rooms:    [Room]
    
    let eles:     [Bitmap]
    
    let frames:   [[Frame]]
    let animofs:  Animofs
    let animjoy:  [Animjoy]
    
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

struct GameState {
    static let ROOM_TIMER = 4

    var pupoPos      = Point(x: 0xa8, y: 0xa0)
    
    var pupoAni      = 0x03
    var pupoAniFrame = 0
    var pupoAniTimer = 3
    
    var room         = 0
    var roomFrame    = 0
    var roomTimer    = 4
    
    mutating func tick(input: Input, episode: Episode) {
        if roomTimer == 0 {
            roomFrame = (roomFrame + 1) % 4
            roomTimer = GameState.ROOM_TIMER
        } else {
            roomTimer -= 1
        }
        
        if pupoAniTimer == 0 {
            pupoAniFrame += 1
            pupoAniTimer = (episode.frames[safe: pupoAni]?[safe: pupoAniFrame]?.time).map {
                ($0 + 3) / 4
                } ?? 3
        }
        else {
            pupoAniTimer -= 1
        }
        
        if pupoAniFrame >= episode.frames[pupoAni].count {
            pupoPos = pupoPos.adding(by: episode.animofs.pre[pupoAni])
            
            pupoAniFrame = 0
            pupoAni = episode.animjoy[pupoAni].nextAni(direction: input)
            
            pupoPos = pupoPos.adding(by: episode.animofs.post[pupoAni])
        }
    }
}
