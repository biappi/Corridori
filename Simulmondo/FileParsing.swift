//
//  FileParsing.swift
//  Simulmondo
//
//  Created by Antonio Malara on 21/03/2017.
//  Copyright © 2017 Antonio Malara. All rights reserved.
//

import Foundation

extension IteratorProtocol where Element == UInt8 {
    mutating func parsePaletteColor() -> Color? {
        guard
            let r = self.next(),
            let g = self.next(),
            let b = self.next()
        else {
                return nil
        }
        
        return Color(r: r << 2,
                     g: g << 2,
                     b: b << 2,
                     a: 0xff)
    }
    
    mutating func parsePaletteFile() -> Palette? {
        guard
            let firstColor = self.byte(),
            let count      = self.byte(),
            let padding    = self.array(count: 3,
                                        { $0.byte() }),
            let colors     = self.array(count: count,
                                        { $0.parsePaletteColor()})
        else {
            return nil
        }
        
        _ = padding
        
        let startPadding = [Color](repeating: Color(r: 0, g: 0, b: 0, a: 255),
                                   count: firstColor)
        
        return Palette(colors: startPadding + colors)
    }
    
    mutating func parseBitmap(size: Size) -> Bitmap? {
        return self
            .array(count: size.area) {
                let x =  $0.next()
                return x
            }
            .flatMap {
                (x: [UInt8]?) in
                
                return x.map {
                    Bitmap(size: size,
                           data: $0)
                }
            }
    }
    
    
    mutating func parseTilesetFile(tileSize: Size) -> Tileset? {
        var tiles = [Bitmap]()
        
        while true {
            if let b = self.parseBitmap(size: tileSize) {
                tiles.append(b)
            }
            else {
                break
            }
        }
        
        return Tileset(tiles: tiles)
    }
    
    mutating func parseRoom() -> Room? {
        guard
            let header     = self.array(count: 4,               { i in i.byte() }),
            let tilesetsId = self.array(count: 0x10,            { i in i.be16() }),
            let tileIds    = self.array(count: Room.tiles.area, { i in i.be16() }),
            let padding    = self.array(count: 0x20,            { i in i.next() }),
            let tileTypes  = self.array(count: Room.tiles.area, { i in i.byte() }),
            header == [82, 79, 79, 77]
        else {
            return nil
        }
        
        _ = padding
        
        return Room(
            tiles: zip(tileIds, tileTypes).map {
                let (tileId, type) = $0
                
                return Room.Tile(
                    tileDesc:  tileId,
                    tilesetId: tilesetsId[(tileId & 0xf000) >> 12],
                    type:      type
                )
            }
        )
    }
    
    mutating func parseRoomFile() -> [Room]? {
        return self.arrayUntilValid { $0.parseRoom() }
    }
    
    mutating func parseExit() -> Exit? {
        guard
            let roomFrom = self.byte(),
            let roomTo   = self.byte(),
            let toY      = self.be16(),
            let fromY    = self.be16()
        else {
                return nil
        }
        
        return Exit(
            roomFrom: roomFrom,
            roomTo:   roomTo,
            toY:      toY,
            fromY:    fromY
        )
    }
    
    mutating func parseUscFile() -> [Exit]? {
        return self.arrayUntilValid { $0.parseExit() }
    }

    mutating func parseDoor() -> Door? {
        guard
            let roomFrom = self.byte(),
            let roomTo   = self.byte(),
            let fromY    = self.be16(),
            let toY      = self.be16(),
            let unk1     = self.be16(),
            let unk2     = self.be16()
        else {
            return nil
        }
        
        return Door(
            roomFrom: roomFrom,
            roomTo:   roomTo,
            fromY:    fromY,
            toY:      toY,
            unk1:     unk1,
            unk2:     unk2
        )
    }

    mutating func parsePrtFile() -> [Door]? {
        return self.arrayUntilValid { $0.parseDoor() }
    }
    
    mutating func parseEleItem() -> Bitmap? {
        guard
            let width  = self.le16(),
            let height = self.le16(),
            let pad    = self.byte(),
            let lines  = self.array(count: height, {
                (i : inout Self) in
                
                i.arrayUntil(0xff) {
                    (i: inout Self) in
                    
                    return i.byte()
                }
            })
        else {
            return nil
        }
        
        _ = pad
        
        var bitmap = Bitmap(size: Size(width: width, height: height))
    
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
                    
                    bitmap.put(color1, x: cur_x, y: cur_y) ; cur_x += 1
                    bitmap.put(color2, x: cur_x, y: cur_y) ; cur_x += 1
                }
                
                if count & 1 != 0 {
                    let colors = i.next()!
                    let color1 = UInt8(colors & 0x0f)
                    
                    bitmap.put(color1, x: cur_x, y: cur_y) ; cur_x += 1
                }
            } while true
        }
        
        return bitmap
    }
    
    mutating func parseEleFile() -> [Bitmap]? {
        let data = self.consume()
        var iter = data.makeIterator()
        
        
        guard
            let count   = iter.le16(),
            let offsets = iter.array(count: count, { $0.le32() })
        else {
            return nil
        }
        
        let eles = offsets.map { (i: Int) -> Bitmap? in
            var i2 = data[i + 2 ..< data.count].makeIterator()
            return i2.parseEleItem()
        }
        
        return eles as? [Bitmap]
    }
    
    mutating func parseFramesFile() -> [[Frame]]? {
        let data = self.consume()
        var iter = data.makeIterator()
        
        guard let offsets = iter.array(count: 106, { $0.be16() }) else {
            return nil
        }
        
        return offsets.flatMap {
            var i = data[$0 ..< data.count - 1].makeIterator()
            
            var frames = [Frame]()
            
            while true {
                guard
                    let frame = i.byte(),
                    let time  = i.byte()
                else {
                    return nil
                }
                
                if frame == 0 && time == 0 {
                    break
                }
                
                guard
                    let x = i.next(),
                    let y = i.next()
                else {
                    return nil
                }
                
                frames.append(
                    Frame(
                        frame: frame,
                        time:  time,
                        delta: Point(
                            x: Int(Int8(bitPattern: x)),
                            y: Int(Int8(bitPattern: y))
                        ),
                        flip: i.byte() != 0
                    )
                )
            }
            
            return frames
        }
    }

    mutating func parseAnimofsFile() -> Animofs? {
        let data = self.consume()
        var iter = data.makeIterator()
        
        guard let offsets = iter.arrayUntil(0xf0f0, { $0.be16() }) else {
            return nil
        }
        
        let lists = offsets.map {
            data[$0 ..< $0 + 106].map {
                Int(Int8(bitPattern: $0))
            }
        }
        
        guard
            let postX = lists[safe: 0],
            let postY = lists[safe: 1],
            let preX  = lists[safe: 2],
            let preY  = lists[safe: 3],
            postX.count == postY.count,
            preX.count  == preY.count
        else {
            return nil
        }
        
        return Animofs(
            pre:  zip(preX,  preY).map  { Point(x: $0.0, y: $0.1) },
            post: zip(postX, postY).map { Point(x: $0.0, y: $0.1) }
        )
    }

    mutating func parseAnimjoyFile() -> [Animjoy]? {
        let data = arrayUntilValid {
            (i: inout Self) -> [Int]? in
            return i.array(count: 18) {
                (i: inout Self) -> Int? in
                return i.byte()
            }
        }
            
        return data.map { $0.map { Animjoy(backingData: $0) } }
    }
    
    mutating func parseSostaniItem1() -> Sostani.Item1? {
        guard
            let oldAni = self.byte(),
            oldAni != 0xff
            else {
                return nil
        }
        
        guard
            let logitabIndex = self.byte(),
            let newAni       = self.byte()
        else {
            return nil
        }
        
        return Sostani.Item1(
            oldAni:       oldAni,
            newAni:       newAni,
            logitabIndex: logitabIndex
        )
    }
    
    mutating func parseSostaniItem2() -> Sostani.Item2? {
        guard
            let oldAni = self.byte(),
            oldAni != 0xff
        else {
            return nil
        }
        
        guard
            let newAni       = self.byte(),
            let logitabIndex = self.byte(),
            let xOffset      = self.next().map({Int(Int8(bitPattern: $0))})
        else {
            return nil
        }
        
        return Sostani.Item2(
            oldAni:       oldAni,
            newAni:       newAni,
            logitabIndex: logitabIndex,
            xOffset:      xOffset
        )
    }
    
    mutating func parseSostaniFile() -> Sostani? {
        let data = self.consume()
        
        var i = data.makeIterator()
        guard
            let off1 = i.be16(),
            let off2 = i.be16()
        else {
            return nil
        }
        
        var data1 = data[off1 ..< data.count].makeIterator()
        var data2 = data[off2 ..< data.count].makeIterator()
        
        guard
            let boh1 = data1.arrayUntilValid({ $0.parseSostaniItem1() }),
            let boh2 = data2.arrayUntilValid({ $0.parseSostaniItem2() })
        else {
            return nil
        }
        
        return Sostani(items1: boh1, items2: boh2)
    }
    
    mutating func parseLogitabFile() -> [[Int]]? {
        let data = self.consume()
        
        var it = data.makeIterator()
        guard let offsets = it.arrayUntil(0, { $0.be16() }) else {
            return nil
        }
        
        return offsets.map { (a: Int) -> [Int]? in
            var it = data[a ..< data.count].makeIterator()
            return it.arrayUntil(0xff) { $0.byte() }
        } as? [[Int]]
    }
    
    mutating func parseSwiTileIdsOverride() -> SwiItem.TileOverride? {
        if
            let xy     = self.be16(),
            let roomNr = self.be16(),
            let newId  = self.be16()
        {
            return SwiItem.TileOverride(xy: xy, roomNr: roomNr, newValue: newId)
        }
        else {
            return nil
        }
    }
    
    mutating func parseSwiTileTypesOverride() -> SwiItem.TileOverride? {
        if
            let xy     = self.be16(),
            let roomNr = self.byte(),
            let newId  = self.byte()
        {
            return SwiItem.TileOverride(xy: xy, roomNr: roomNr, newValue: newId)
        }
        else {
            return nil
        }
    }
    
    
    mutating func parseSwiFile() -> [SwiItem]? {
        return self.arrayUntilValid { it in
            guard let type  = it.be16() else { return nil }
            guard let count = it.be16() else { return nil }
            
            switch type {
            case 0x0008:
                if
                    let idsCount       = it.be16(),
                    let idsOverrides   = it.array(count: idsCount, { $0.parseSwiTileIdsOverride() }),
                    let typesCount     = it.be16(),
                    let typesOverrides = it.array(count: typesCount, { $0.parseSwiTileTypesOverride() })
                {
                    return SwiItem.Otto(idsOverride: idsOverrides, typesOverride: typesOverrides)
                }
                else {
                    return nil
                }
                
            case 0xffff:
                return nil
                
            default:
                return it.array(count: count - 4, { $0.next() }).map { data in
                    SwiItem.Unk(type: type, data: data)
                }
            }
            
        }
    }
    
    mutating func consume() -> [UInt8] {
        var me = [UInt8]()
        while true {
            if let n = next() {
                me.append(n)
            }
            else {
                return me
            }
        }
    }
}
