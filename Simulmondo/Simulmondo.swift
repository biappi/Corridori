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


