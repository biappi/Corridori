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
    
    init?(bytesIterator: inout Data.Iterator)
    {
        guard
            let firstColor = bytesIterator.byte(),
            let count      = bytesIterator.byte(),
            let padding    = bytesIterator.array(count: 3, { (i : inout Data.Iterator) -> Int? in i.byte() }),
            let colors     = bytesIterator.array(count: count, {
                (it: inout Data.Iterator) -> Color? in
                
                guard
                    let r = it.next(),
                    let g = it.next(),
                    let b = it.next()
                else {
                    return nil
                }
                
                return Color(r: r << 2, g: g << 2, b: b << 2, a: 255)
            })
        else {
            return nil
        }
        
        _ = padding
        
        self.colors = [Color](repeating: Color(r: 0, g: 0, b: 0, a: 255), count: firstColor) + colors
    }
}

//

struct Point {
    let x : Int
    let y : Int
    
    func adding(by: Point) -> Point {
        return Point(x: self.x + by.x, y: self.y + by.y)
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
    
    init(size: Size) {
        self.size = size
        
        data = [UInt8?](repeating: nil,
                        count: size.area)
    }
    
    init?(bytesIterator: inout Data.Iterator, size: Size) {
        self.size = size
        
        if let data = bytesIterator.array(count: size.area, { $0.next() }) {
            self.data = data
        }
        else {
            return nil
        }
    }
    
    mutating func put(_ color: UInt8, x: Int, y: Int) {
        data[y * size.width + x] = color
    }    
}

//

struct Tileset {
    let tiles : [Bitmap]
    
    init?(bytesIterator: inout Data.Iterator, tileSize: Size) {
        tiles = bytesIterator.arrayUntilValid {
            Bitmap(bytesIterator: &$0,
                   size: tileSize)
        }
    }
}

//

struct Room {
    struct Tile {
        let tileDesc  : Int
        let tilesetId : Int
        let type      : Int
        
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
    
    let tiles : [Tile]
    
    //
    
    static let tiles = Size(width: 0x14, height: 0x14)
    
    //
    
    init?<I:IteratorProtocol>(bytesIterator i: inout I)
        where I.Element == UInt8
    {
        guard
            let header     = i.array(count: 4,               { i in i.byte() }),
            let tilesetsId = i.array(count: 0x10,            { i in i.be16() }),
            let tileIds    = i.array(count: Room.tiles.area, { i in i.be16() }),
            let padding    = i.array(count: 0x20,            { i in i.next() }),
            let tileTypes  = i.array(count: Room.tiles.area, { i in i.byte() }),
            header == [82, 79, 79, 77]
        else {
            return nil
        }
        
        _ = padding
        
        tiles = zip(tileIds, tileTypes).map {
            tileId, type in
            
            Tile(
                tileDesc:  tileId,
                tilesetId: tilesetsId[(tileId & 0xf000) >> 12],
                type:      type
            )
        }
    }
    
    init?(data: Data) {
        var iterator = data.makeIterator()
        self.init(bytesIterator: &iterator)
    }
    
    static func fromFile(data: Data) throws -> [Room] {
        var it = data.makeIterator()
        return it.arrayUntilValid { Room(bytesIterator: &$0) }
    }
}

//

extension Bitmap {
    init?<I:IteratorProtocol>(ele iterator: inout I)
        where I.Element == UInt8
    {
        guard
            let width  = iterator.le16(),
            let height = iterator.le16(),
            let pad    = iterator.byte(),
            let lines  = iterator.array(count: height, {
                (i : inout I) in
                
                i.arrayUntil(0xff) {
                    (i: inout I) in
                    
                    return i.byte()
                }
            })
        else {
            return nil
        }

        _ = pad

        size = Size(width: width, height: height)
        data = [UInt8?](repeating: nil,
                        count: size.area)

        for (cur_y, line) in lines.enumerated() {
            var i = line.makeIterator()
            var cur_x = 0
            
            repeat {
                guard
                    let skip  = i.next(),
                    let count = i.next()
                else {
                    break
                }
                
                cur_x += skip
                
                for _ in 0 ..< count / 2 {
                    let colors = i.next()!
                    
                    let color1 = UInt8( colors & 0x0f      )
                    let color2 = UInt8((colors & 0xf0) >> 4)
                    
                    self.put(color1, x: cur_x, y: cur_y) ; cur_x += 1
                    self.put(color2, x: cur_x, y: cur_y) ; cur_x += 1
                }
                
                if count & 1 != 0 {
                    let colors = i.next()!
                    let color1 = UInt8(colors & 0x0f)
                    
                    self.put(color1, x: cur_x, y: cur_y) ; cur_x += 1
                }
            } while true
        }
    }
    
    static func fromEleFile(elefile: Data) -> [Bitmap]?
    {
        var iterator = elefile.makeIterator()
        
        guard
            let count   = iterator.le16(),
            let offsets = iterator.array(count: count, { $0.le32() })
        else {
            return nil
        }
        
        let eles = offsets.map { (i: Int) -> Bitmap? in
            var i2 = elefile[i + 2 ..< elefile.count].makeIterator()
            return Bitmap(ele: &i2)
        }
        
        return eles as? [Bitmap]
    }
}


