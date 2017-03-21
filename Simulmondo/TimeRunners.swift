//
//  TimeRunners.swift
//  Simulmondo
//
//  Created by Antonio Malara on 19/03/2017.
//  Copyright © 2017 Antonio Malara. All rights reserved.
//

import Cocoa

func load(window: NSWindow) {
    let gameUrl = Bundle.main.url(forResource:   "Time Runners",
                                  withExtension: nil)!
    
    let episodeUrl  = gameUrl.appendingPathComponent("Ep. 1")
    let gameDirUrl  = episodeUrl.appendingPathComponent("GAME_DIR")
    let ar1Url      = gameDirUrl.appendingPathComponent("AR1")
    let staUrl      = ar1Url.appendingPathComponent("STA")
    let mapUrl      = ar1Url.appendingPathComponent("MAP")
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
    
    //
    
    
    var paletteData = try! Data(contentsOf: paletteUrl).makeIterator()
    let palette     = paletteData.parsePaletteFile()!
    
    //
    
    let TILE_SIZE   = Size(width:  16, height:  10)
    
    func TRTileset(url: URL) -> Tileset? {
        var bytes = try! Data(contentsOf: url).makeIterator()
        return bytes.parseTilesetFile(tileSize: TILE_SIZE)
    }
    
    let tilesets =  [
        nil,
        TRTileset(url: bgTiles1Url)!,
        TRTileset(url: bgTiles2Url)!,
        TRTileset(url: bgTiles3Url)!,
        TRTileset(url: bgTiles4Url)!,
        TRTileset(url: bgTiles5Url)!,
        TRTileset(url: bgTiles6Url)!,
        nil,
        nil,
        nil,
        TRTileset(url: bgTilesAUrl)!,
        TRTileset(url: bgTilesBUrl)!,
    ]
    
    //
    
    var roomroeData = try! Data(contentsOf: roomRoeUrl).makeIterator()
    let rooms       = roomroeData.parseRoomFile()!
    
    //
    
    let treleData = try! Data(contentsOf: treleUrl)
    let eles      = Bitmap.fromEleFile(elefile: treleData)!
    
    let trEleNSImages = eles.map {
        $0.createNSImage(palette: palette, baseColor: 225)
    }

    //
    
    struct Tr {
        var pupoPos   = Point(x: 0xa8, y: 0xa0)
        var pupoFrame = 0
        var room      = 0
        var roomFrame = 0
    }
    
    var tr = Tr()
    
    let v = RoomView(tiles: Room.tiles, tileSize: TILE_SIZE, tilesetsImages: createTilesetsNSImages(tilesets: tilesets, palette: palette))
    v.frame = NSRect(x: 0, y: 0, width: 320 * 3, height: 200 * 3)
    
    window.setContentSize(v.frame.size)
    window.contentView = v
    window.makeFirstResponder(v)
    
    let t = NSImageView(frame: NSRect.zero)
    t.imageScaling = .scaleProportionallyUpOrDown
    v.addSubview(t)
   
    func changeFrame (_ delta : Int) {
        tr.pupoFrame = (tr.pupoFrame  + delta) % eles.count
        tr.pupoFrame = tr.pupoFrame < 0 ? eles.count - 1 : tr.pupoFrame
        apply(tr: tr)
    }
    
    v.next = { changeFrame(1)  }
    v.prev = { changeFrame(-1) }
    
    func apply(tr: Tr) {
        v.setRoom(room:  rooms[tr.room],
                  frame: tr.roomFrame)
        
        let ele       = eles[tr.pupoFrame]
        let eleOffset = Point(
            x: -(ele.size.width / 2),
            y: -(ele.size.height)
        )
        
        let image = trEleNSImages[tr.pupoFrame]
        if image !== t.image {
            t.image = image
        }
        
        t.frame = ele.size.multiplied(by: 3).nsrect(origin: tr.pupoPos.adding(by: eleOffset))
    }
    
    Timer.scheduledTimer(withTimeInterval: 1 / 2.0, repeats: true) { _ in
        tr.roomFrame = (tr.roomFrame + 1) % 4
        apply(tr: tr)
    }
}

