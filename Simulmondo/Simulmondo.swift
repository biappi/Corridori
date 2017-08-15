//
//  Simulmondo.swift
//  Simulmondo
//
//  Created by Antonio Malara on 19/03/2017.
//  Copyright © 2017 Antonio Malara. All rights reserved.
//

import Foundation

struct Color {
    var r: UInt8
    var g: UInt8
    var b: UInt8
    var a: UInt8
}

struct Palette {
    let colors : [Color]
}

//

struct Point {
    var x : Int
    var y : Int
    
    func adding(by: Point) -> Point {
        return Point(x: self.x + by.x, y: self.y + by.y)
    }
    
    func multiplied(by: Int) -> Point {
        return Point(x: x * by, y: y * by)
    }
}

struct Size {
    let width  : Int
    let height : Int
    
    var ns : NSSize { return NSSize(width: width, height: height) }
    var area : Int { return width * height }
    
    func multiplied(by: Int) -> Size {
        return Size(width:  width  * by,
                    height: height * by)
    }
    
    func nsrect(origin: Point) -> NSRect {
        return NSRect(origin: NSPoint(x: origin.x, y: origin.y), size: self.ns)
    }
}

//

struct Bitmap {
    let size: Size
    var data: [UInt8?]
    
    init(size: Size, data: [UInt8?]? = nil) {
        self.size = size
        self.data = data ?? [UInt8?](repeating: nil,
                                     count: size.area)
    }
    
    mutating func put(_ color: UInt8, x: Int, y: Int) {
        data[y * size.width + x] = color
    }    
}

//

struct Tileset {
    let tiles : [Bitmap]
}

//

struct Room {
    struct Tile {
        var tileDesc  : Int
        var tilesetId : Int
        var type      : Int
        
        var flip      : Bool {
            return (tileDesc & 0x0200) != 0
        }
        
        func tileId(frame: Int) -> Int {
            var resultTile = tileDesc
            
            if resultTile & 0xf000 == 0x9000 {
                var frameToUse = frame
                
                if resultTile & 0x0800 != 0 {
                    frameToUse = 3
                }
                else {
                    if (resultTile & 0x0400 != 0) && (frameToUse == 3) {
                        resultTile = resultTile | 0x0800
                    }
                }
                
                resultTile = (resultTile & 0xfffc) | (frameToUse & 0x0003)
            }
            
            return resultTile & 0x01ff
        }
    }
    
    var tiles : [Tile]
    
    //
    
    static let tiles = Size(width: 0x14, height: 0x14)
}

struct Exit {
    let roomFrom: Int
    let roomTo:   Int
    let toY:      Int
    let fromY:    Int
}

struct Door {
    let roomFrom: Int
    let roomTo:   Int
    let fromY:    Int
    let toY:      Int
    let unk1:     Int
    let unk2:     Int
}


//

struct Frame {
    let frame : Int
    let time  : Int
    let delta : Point
    let flip  : Bool
}

struct Animofs {
    let pre:  [Point]
    let post: [Point]
}

//

enum Horizontal {
    case right
    case still
    case left
}

enum Vertical {
    case top
    case still
    case bottom
}

enum Firing {
    case nonFiring
    case firing
}

typealias Input = (
    horizontal: Horizontal,
    vertical:   Vertical,
    firing:     Firing
)

struct Animjoy {
    let backingData : [Int]
    
    func nextAni(direction: Input) -> Int {
        switch direction {
        case (.still, .still,  .nonFiring): return backingData[ 0]
        case (.still, .top,    .nonFiring): return backingData[ 1]
        case (.right, .top,    .nonFiring): return backingData[ 2]
        case (.right, .still,  .nonFiring): return backingData[ 3]
        case (.right, .bottom, .nonFiring): return backingData[ 4]
        case (.still, .bottom, .nonFiring): return backingData[ 5]
        case (.left,  .bottom, .nonFiring): return backingData[ 6]
        case (.left,  .still,  .nonFiring): return backingData[ 7]
        case (.left,  .top,    .nonFiring): return backingData[ 8]
        case (.still, .still,  .firing   ): return backingData[ 9]
        case (.still, .top,    .firing   ): return backingData[10]
        case (.right, .top,    .firing   ): return backingData[11]
        case (.right, .still,  .firing   ): return backingData[12]
        case (.right, .bottom, .firing   ): return backingData[13]
        case (.still, .bottom, .firing   ): return backingData[14]
        case (.left,  .bottom, .firing   ): return backingData[15]
        case (.left,  .still,  .firing   ): return backingData[16]
        case (.left,  .top,    .firing   ): return backingData[17]
        }
    }
}

//

struct Sostani {
    
    struct Item1 {
        let oldAni:       Int
        let newAni:       Int
        let logitabIndex: Int
    }
    
    struct Item2 {
        let oldAni:       Int
        let newAni:       Int
        let logitabIndex: Int
        let xOffset:      Int
    }
    
    let items1: [Item1]
    let items2: [Item2]
}

//

enum SwiItem {
    struct TileOverride {
        let x:        Int
        let y:        Int
        let roomNr:   Int
        let newValue: Int
        
        init(xy: Int, roomNr: Int, newValue: Int) {
            self.x = xy % 20
            self.y = xy / 20
            self.roomNr = roomNr
            self.newValue = newValue
        }
    }
    
    case Unk(type: Int, data:[UInt8])
    case Otto(idsOverride: [TileOverride], typesOverride: [TileOverride])
}
