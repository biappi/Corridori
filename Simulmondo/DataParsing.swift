//
//  DataParsing.swift
//  Simulmondo
//
//  Created by Antonio Malara on 20/03/2017.
//  Copyright © 2017 Antonio Malara. All rights reserved.
//

import Foundation

extension Collection where Indices.Iterator.Element == Index {
    /// Returns the element at the specified index iff it is within bounds, otherwise nil.
    subscript (safe index: Index) -> Generator.Element? {
        return indices.contains(index) ? self[index] : nil
    }
}

extension IteratorProtocol where Element == UInt8 {
    mutating func pluck <Result> (_ transform: (inout Self) -> (Result?)) -> Result? {
        return transform(&self)
    }
    
    mutating func array
        <Result>
        (count: Int, _ transform: (inout Self) -> (Optional<Result>)) -> [Result]?
    {
        var result = [Result]()
        
        for _ in 0 ..< count {
            guard let transformed = transform(&self) else {
                return nil
            }
            
            result.append(transformed)
        }
        
        return result
    }
    
    mutating func arrayUntil
        <Result: Equatable>
        (_ until: Result, _ transform: (inout Self) -> (Optional<Result>)) -> [Result]?
    {
        var result = [Result]()
        
        repeat {
            guard let transformed = transform(&self) else {
                return nil
            }
            
            if transformed == until {
                return result
            }
            
            result.append(transformed)
        } while true
    }
    
    mutating func arrayUntilValid
        <Result>
        (_ transform: (inout Self) -> (Optional<Result>)) -> [Result]
    {
        var result = [Result]()
        
        repeat {
            if let transformed = transform(&self) {
                result.append(transformed)
            }
            else {
                return result
            }
        } while true
    }
    
    mutating func byte() -> Int? {
        return next().map { Int($0) }
    }
    
    mutating func be16() -> Int? {
        guard
            let a = byte(),
            let b = byte()
            else {
                return nil
        }
        
        return
            a << 8 + b
    }
    
    mutating func le16() -> Int? {
        guard
            let a = byte(),
            let b = byte()
            else {
                return nil
        }
        
        return
            b << 8 + a
    }
    
    mutating func le32() -> Int? {
        guard
            let a = next(),
            let b = next(),
            let c = next(),
            let d = next()
            else {
                return nil
        }
        
        return
            Int(bitPattern:
                UInt(d) << 24 +
                    UInt(c) << 16 +
                    UInt(b) <<  8 +
                    UInt(a)
        )
    }
}

