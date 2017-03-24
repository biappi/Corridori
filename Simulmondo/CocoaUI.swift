//
//  CocoaUI.swift
//  Simulmondo
//
//  Created by Antonio Malara on 20/03/2017.
//  Copyright © 2017 Antonio Malara. All rights reserved.
//

import Cocoa

extension Bitmap {
    func createNSImage(palette: Palette, baseColor: Int = 0) -> NSImage {
        let imageData = self.data.map {
            paletteIdx in
            
            return
                paletteIdx
                    .map { Int($0) + baseColor }
                    .flatMap { palette.colors[$0] }
                    ?? Color(r: 0, g: 0, b: 0, a: 0)
        }
        
        let bitmapInfo = CGBitmapInfo(rawValue: CGImageAlphaInfo.premultipliedLast.rawValue)
        
        let provider = CGDataProvider(
            data: NSData(
                bytes:  imageData,
                length: imageData.count * MemoryLayout<Color>.size
            )
        )!
        
        return
            NSImage(
                cgImage: CGImage(
                    width:             size.width,
                    height:            size.height,
                    bitsPerComponent:  8,
                    bitsPerPixel:      32,
                    bytesPerRow:       size.width * MemoryLayout<Color>.size,
                    space:             CGColorSpaceCreateDeviceRGB(),
                    bitmapInfo:        bitmapInfo,
                    provider:          provider,
                    decode:            nil,
                    shouldInterpolate: true,
                    intent:            .defaultIntent
                    )!,
                size: size.ns
        )
    }
}

class TileView : NSImageView {
    var block : ()->() = { }
    var label : NSTextField?
    
    override func mouseDown(with event: NSEvent) {
        block()
    }
}

class RoomView : NSView {
    override var isFlipped: Bool { return true }
    
    let tilesetsImages : [[(NSImage, NSImage)]?]
    
    var tileViews  : [TileView] = []
    
    var showTypes = true {
        didSet {
            for t in tileViews {
                t.label?.isHidden = !showTypes
            }
        }
    }

    init(tiles: Size, tileSize: Size, tilesetsImages: [[(NSImage, NSImage)]?]) {
        self.tilesetsImages = tilesetsImages
        
        super.init(frame: NSRect(
            x: 0,
            y: 0,
            width:  tiles.width  * tileSize.width,
            height: tiles.height * tileSize.height
        ))
        
        for tileY in 0 ..< tiles.height {
            for tileX in 0 ..< tiles.width {
                let imageView = TileView(
                    frame: NSRect(
                        x:      tileX * tileSize.width,
                        y:      tileY * tileSize.height,
                        width:  tileSize.width,
                        height: tileSize.height
                    )
                )
                
                imageView.imageScaling = .scaleProportionallyUpOrDown
                
                imageView.autoresizingMask = [
                    .viewWidthSizable,
                    .viewHeightSizable,
                    .viewMinXMargin,
                    .viewMaxXMargin,
                    .viewMinYMargin,
                    .viewMaxYMargin,
                ]
                
                addSubview(imageView)
                
                imageView.label = NSTextField(string: "")
                imageView.label?.frame = imageView.bounds
                imageView.label?.textColor = NSColor.white
                imageView.label?.backgroundColor = NSColor(white: 1, alpha: 0.4)
                imageView.label?.font = NSFont.systemFont(ofSize: 15)
                imageView.label?.drawsBackground = false
                imageView.label?.isBezeled = false
                imageView.label?.isEditable = false
                imageView.label?.alignment = .center
                
                imageView.addSubview(imageView.label!)
                
                tileViews.append(imageView)
            }
        }
    }
    
    func setRoom(room: Room, frame: Int) {
        for (i, roomTile) in room.tiles.enumerated() {
            let images =
                tilesetsImages[roomTile.tilesetId]?[safe: roomTile.tileId(frame: frame)]
            let image = roomTile.flip ? images?.1 : images?.0
            
            let tileView = tileViews[i]
            
            tileView.label?.stringValue = roomTile.type == 0 ? "" : "\(roomTile.type)"
            tileView.label?.frame = tileView.bounds
            
            if tileView.image !== image {
                tileView.image = image
            }
            
            tileView.block = { Swift.print(roomTile) }
        }
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
}

class GameView : NSView {
    override var isFlipped: Bool { return true }

    let episode    : Episode
    let roomView   : RoomView
    let pupoView   : NSImageView
    let pupoImages : [(NSImage, NSImage)]
    
    init(episode: Episode) {
        self.episode = episode
        
        self.pupoImages = episode.eles.map {
            (b: Bitmap) -> (NSImage, NSImage) in
            
            let i = b.createNSImage(palette: episode.palette, baseColor: 225)
            return (i, i.flippedCopy())
        }
        
        
        self.roomView = RoomView(
            tiles: Room.tiles,
            tileSize: Episode.TILE_SIZE,
            tilesetsImages: createTilesetsNSImages(
                tilesets: episode.tilesets,
                palette:  episode.palette
            )
        )
        
        self.roomView.frame = NSRect(x: 0,
                                     y: 0,
                                     width: 320 * 3,
                                     height: 200 * 3)
        
        self.pupoView = NSImageView(frame: NSRect.zero)
        pupoView.imageScaling = .scaleProportionallyUpOrDown
        
        super.init(frame: self.roomView.frame)
        
        self.addSubview(roomView)
        self.addSubview(pupoView)
    }
    
    func apply(state: GameState) {
        roomView.setRoom(room:  episode.rooms[state.room],
                         frame: state.roomFrame)
        
        let frame     = episode.frames[state.pupoAni][state.pupoAniFrame]
        let ele       = episode.eles[frame.frame]
        
        let pupoPos   = state.pupoPos.adding(by: frame.delta).adding(by: Point(
            x: -(ele.size.width / 2),
            y: -(ele.size.height)
        ))
        
        let image = frame.flip
                        ? pupoImages[frame.frame].1
                        : pupoImages[frame.frame].0
        
        if image !== pupoView.image {
            pupoView.image = image
        }
        
        pupoView.frame = ele.size.multiplied(by: 3).nsrect(origin: pupoPos.multiplied(by: 3))
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

}
class Window : NSWindow {
    var inputDidChange : (Input) -> Void = { (_) in  }

    var left   = false
    var right  = false
    var top    = false
    var bottom = false
    var fire   = false
    
    override func keyDown(with event: NSEvent) {
        guard event.isARepeat == false else {
            return
        }
        
        if event.keyCode ==  49 { fire   = true }
        
        if event.keyCode == 124 { right  = true }
        if event.keyCode == 123 { left   = true }

        if event.keyCode == 126 { top    = true }
        if event.keyCode == 125 { bottom = true }

        inputDidChange((
            left ? .left   : right      ? .right  : .still,
            top  ? .top    : bottom     ? .bottom : .still,
            fire ? .firing : .nonFiring
        ))
    }

    override func keyUp(with event: NSEvent) {
        if event.keyCode ==  49 { fire   = false }
        
        if event.keyCode == 124 { right  = false }
        if event.keyCode == 123 { left   = false }

        if event.keyCode == 126 { top    = false }
        if event.keyCode == 125 { bottom = false }

        inputDidChange((
            left ? .left   : right      ? .right  : .still,
            top  ? .top    : bottom     ? .bottom : .still,
            fire ? .firing : .nonFiring
        ))
    }
    
    override var acceptsFirstResponder: Bool {
        return true
    }
}

//

func createTilesetsNSImages(tilesets: [Tileset?], palette: Palette, baseColor: Int = 0) -> [[(NSImage, NSImage)]?] {
    return
        tilesets.map {
            (tileset: Tileset?) in
            
            tileset.map {
                (tileset: Tileset) in
                
                tileset.tiles.map {
                    (tiles: Bitmap) -> (NSImage, NSImage) in
                    
                    let ns = tiles.createNSImage(palette: palette, baseColor: baseColor)
                    return (ns, ns.flippedCopy())
                }
            }
    }
}

//

extension NSImage {
    func flippedCopy() -> NSImage {
        let image = self.copy() as! NSImage
        image.lockFocus()
        
        NSColor.blue.setFill()
        
        NSBezierPath(rect: NSRect(origin: NSPoint.zero,
                                  size: image.size)).fill()
        
        let t = NSAffineTransform()
        t.translateX(by: image.size.width, yBy: 0)
        t.scaleX(by: -1, yBy: 1)
        t.concat()
        
        image.draw(at: NSPoint.zero,
                   from: NSRect(origin: NSPoint.zero,
                                size: image.size),
                   operation: .copy,
                   fraction: 1)
        
        image.unlockFocus()
        
        return image
    }
}

