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
    
    let doors:    [Door]
    let exits:    [Exit]
    let sostani:  Sostani
    let logitab:  [[Int]]
    let swi:      [SwiItem]
    
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
        let prtUrl      = filUrl.appendingPathComponent("PRT")
        let uscUrl      = filUrl.appendingPathComponent("USC")
        let sostaniUrl  = filUrl.appendingPathComponent("SOSTANI.TAB")
        let logitabUrl  = filUrl.appendingPathComponent("LOGITAB.TAB")
        let swiUrl      = filUrl.appendingPathComponent("SWI")
        
        
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
            var doorsData   = try Data(contentsOf: prtUrl).makeIterator()
            var exitsData   = try Data(contentsOf: uscUrl).makeIterator()
            var sostaniData = try Data(contentsOf: sostaniUrl).makeIterator()
            var logitabData = try Data(contentsOf: logitabUrl).makeIterator()
            var swiData     = try Data(contentsOf: swiUrl).makeIterator()
            
            palette = paletteData.parsePaletteFile()!
            rooms   = roomroeData.parseRoomFile()!
            eles    = treleData.parseEleFile()!
            frames  = framesData.parseFramesFile()!
            animofs = animofsData.parseAnimofsFile()!
            animjoy = animjoyData.parseAnimjoyFile()!
            doors   = doorsData.parsePrtFile()!
            exits   = exitsData.parseUscFile()!
            sostani = sostaniData.parseSostaniFile()!
            logitab = logitabData.parseLogitabFile()!
            swi     = swiData.parseSwiFile()!
            
            dump(swi)
        }
        catch {
            return nil
        }
    }
}

// cosa cho di fronte
func cosa(episode: Episode,
          room: Room,
          pupoAni: Int,
          currentPos: Point,
          tentativePos: Point) -> Int
{
    var returnAni: Int   = pupoAni
    
    var changed = false
    repeat {
        changed = false
        
        guard
            let s = episode.sostani.items2.filter( { $0.oldAni == returnAni }).first
        else {
            continue
        }
        
        var xoff = tentativePos.x - currentPos.x + s.xOffset
        xoff = xoff >  0x40 ? xoff + 0x10 : xoff
        xoff = xoff < -0x40 ? xoff - 0x10 : xoff
        
        for i in stride(from: xoff, to: 0, by: xoff < 0 ? 0x10 : -0x10) {
            let tileposx = (currentPos.x + i) / Episode.TILE_SIZE.width
            let tileposy = currentPos.y / Episode.TILE_SIZE.height
            
            let tile = room.tiles[tileposy * Room.tiles.width + tileposx]
            
            let logitabEntry = episode.logitab[s.logitabIndex]
            
            if logitabEntry.contains(tile.type) {
                print(s)
                print("  \(tileposx), \(tileposy) -- \(tile.type)")
                print("  " +
                    episode.logitab[s.logitabIndex]
                        .map { String(format: "%02x", arguments: [$0]) }
                        .joined(separator: ",")
                    
                )
                
                returnAni = s.newAni
                changed = true
                break
            }
        }
    } while changed == true
    
    return returnAni
}

func adjustRoomTilesFromSwi(swi: [SwiItem], swivars2: [Bool], roomNr: Int, room: inout Room) {
    let swiItems = swivars2
        .enumerated()
        .filter { $0.element }
        .map { swi[$0.offset] }
    
    for item in swiItems {
        switch item {
        case .Unk(let type, let data):
            print("Unk \(type) \(data)")
            
        case .Otto(let idsOverride, let typesOverride):
            for item in idsOverride.filter ({ $0.roomNr == roomNr }) {
                room.tiles[Room.tiles.height * item.y + item.x].tileDesc = item.newValue
            }
                
            for item in typesOverride.filter ({ $0.roomNr == roomNr }) {
                room.tiles[Room.tiles.height * item.y + item.x].type = item.newValue
            }
        }
    }
}



struct GameState {
    static let ROOM_TIMER = 3

//    var pupoPos      = Point(x:312, y:160) // Point(x: 0xa8, y: 0xa0)
//    var room         = 1
//    var pupoAni      = 56 // 0x03
//
    var pupoPos      = Point(x: 0xa8, y: 0xa0)
    var pupoAni      = 0x03
    
    var pupoAniFrame = 0
    var pupoAniTimer = 3
    
    var room         = 0
    var roomFrame    = 0
    var roomTimer    = GameState.ROOM_TIMER
    
    var swivars2     = [Bool](repeatElement(false, count: 0x40))
    var theRoom      = Room(tiles: []) // bogus
    
    mutating func tick(input: Input, episode: Episode) {
        theRoom = episode.rooms[room]
        
        adjustRoomTilesFromSwi(
            swi: episode.swi,
            swivars2: swivars2,
            roomNr: room,
            room: &theRoom
        )
        
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
            
            // --
            
            for s in episode.sostani.items1.filter( { $0.oldAni == pupoAni }) {
                let tileposx = pupoPos.x / Episode.TILE_SIZE.width
                let tileposy = (pupoPos.y) / Episode.TILE_SIZE.height
                
                let tile = theRoom.tiles[tileposy * Room.tiles.width + tileposx]
                
                let logitabEntry = episode.logitab[s.logitabIndex]
                
                if logitabEntry.contains(tile.type) {
                    print(s)
                    print("  \(tile.type)")
                    print("  " +
                        episode.logitab[s.logitabIndex]
                            .map { String(format: "%02x", arguments: [$0]) }
                            .joined(separator: ",")
                    )
                    
                    
                    pupoAni = s.newAni
                    continue
                }
            }
            
            //
            
            let x = Int(pupoPos.x / Episode.TILE_SIZE.width )
            let y = Int(pupoPos.y / Episode.TILE_SIZE.height)
            
            let type       = theRoom.tiles[Room.tiles.width * y + x].type
            let type_other = theRoom.tiles[Room.tiles.width * (y - 1) + x].type
            
            
            if type & 0xF0 == 0xD0 {
                let swivarIdx = type - 0xD0 + 0xC
                swivars2[swivarIdx] = true
                
                adjustRoomTilesFromSwi(
                    swi: episode.swi,
                    swivars2: swivars2,
                    roomNr: room,
                    room: &theRoom
                )
            }
            
            if pupoAni == 0x10 && episode.logitab[0x26].contains(type) {
                pupoAni = 0x1D
            }
            
            if pupoAni == 0x10 && episode.logitab[0x28].contains(type) {
                pupoAni = 0x1D
            }
            
            if pupoAni == 0x1f ||
               pupoAni == 0x20 ||
               pupoAni == 0x54
            {
                if episode.logitab[0x28].contains(type) {
                    print("boh")
                }
                else {
                    if type == 0xa8 {
                        pupoAni = 0x20
                    }
                    else {
                        print("boh2")
                    }
                }
            }
            
            if pupoAni == 0x55 {
                if episode.logitab[0x25].contains(type) ||
                   episode.logitab[0x27].contains(type)
                {
                    room = type_other
                    theRoom = episode.rooms[room]
                    
                    adjustRoomTilesFromSwi(
                        swi: episode.swi,
                        swivars2: swivars2,
                        roomNr: room,
                        room: &theRoom
                    )
                }
            }
            
            //
            
            // --
            // controlla ostacoli nel camminare
            // --
            
            var ani = pupoAni
            while true {
                let newAni = cosa(
                    episode: episode,
                    room: theRoom,
                    pupoAni: ani,
                    currentPos: pupoPos,
                    tentativePos: pupoPos.adding(by: episode.animofs.post[ani])
                )
                
                if newAni == ani {
                    break
                }
                
                ani = newAni
            }
            
            pupoPos = pupoPos.adding(by: episode.animofs.post[ani])
            pupoAni = ani
            
            
            if  (pupoPos.x <   0 && pupoAni < 0x35) ||
                (pupoPos.x > 320 && pupoAni > 0x35)
            {
                let dx = pupoPos.x > 320
                
                let x = Int(dx ? Room.tiles.width - 1 : 0)
                let y = Int(pupoPos.y / Episode.TILE_SIZE.height) - 1
                
                let oldRoom  = room
                room = theRoom.tiles[Room.tiles.width * y + x].type
                
                theRoom = episode.rooms[room]
                
                adjustRoomTilesFromSwi(
                    swi: episode.swi,
                    swivars2: swivars2,
                    roomNr: room,
                    room: &theRoom
                )
                
                episode
                    .exits
                    .filter {
                        $0.roomFrom == oldRoom &&
                        $0.roomTo   == room &&
                        $0.fromY    == pupoPos.y
                    }
                    .first
                    .map {
                        pupoPos.y = $0.toY
                    }
                
                pupoPos.x = pupoPos.x > 320 ? 8 : 320 - 8
            }
            else {
                if type == 0 {
                    let facingRight = pupoAni >= 0x35
                    pupoAni = facingRight ? 0x67 : 0x32
                }
            }
        }
    }
}

