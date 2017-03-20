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
    
    override func mouseDown(with event: NSEvent) {
        block()
    }
}

class RoomView : NSView {
    override var isFlipped: Bool { return true }
    
    let tilesetsImages : [[(NSImage, NSImage)]?]
    
    var tileViews  : [TileView] = []
    
    var next : ()->() = { }
    var prev : ()->() = { }
    
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
            
            if tileView.image !== image {
                tileView.image = image
            }
            
            tileView.block = { Swift.print(roomTile) }
        }
    }
    
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }
    
    override func keyDown(with event: NSEvent) {
        if event.keyCode == 124 { next() }
        if event.keyCode == 123 { prev() }
    }
    
    override var canBecomeKeyView: Bool {
        return true
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

