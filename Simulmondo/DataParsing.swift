//
//  DataParsing.swift
//  Simulmondo
//
//  Created by Antonio Malara on 20/03/2017.
//  Copyright © 2017 Antonio Malara. All rights reserved.
//

import Foundation

extension Collection {
    /// Returns the element at the specified index iff it is within bounds, otherwise nil.
    subscript (safe index: Index) -> Iterator.Element? {
        return indices.contains(index) ? self[index] : nil
    }
}

extension IteratorProtocol where Element == UInt8 {
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
        
        while true {
            guard let transformed = transform(&self) else {
                return nil
            }
            
            if transformed == until {
                return result
            }
            
            result.append(transformed)
        }
    }
    
    mutating func arrayUntilValid
        <Result>
        (_ transform: (inout Self) -> (Optional<Result>)) -> [Result]?
    {
        var result : [Result]? = nil
        
        while true {
            if let transformed = transform(&self) {
                result = result ?? [Result]()
                result?.append(transformed)
            }
            else {
                return result
            }
            
        }
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

