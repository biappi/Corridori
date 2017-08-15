//
//  GameScene.swift
//  Simulmondo
//
//  Created by Antonio Malara on 15/08/2017.
//  Copyright © 2017 Antonio Malara. All rights reserved.
//

import SpriteKit
import GameplayKit

extension Bitmap {
    func toSKTexture(palette: Palette, baseColor: Int = 0) -> SKTexture {
        let void   : [UInt8] = [0, 0, 0, 0]
        let lut    = palette.colors.map { [$0.r, $0.g, $0.b, $0.a] }
        let bitmap = data.flatMap { $0.map { lut[Int($0) + baseColor] } ?? void }
        return SKTexture(data: Data(bitmap), size: self.size.cg, flipped: true)
    }
}

class GameScene: SKScene {
    var gameState : GameState!
    var episode   : Episode!
    
    var input     = Input(.still, .still, .nonFiring)
    
    var eleTextures   : [SKTexture]!
    var tileMapsNodes : [SKTileMapNode?]!
    
    var pupoNode : SKSpriteNode!
    
    override func sceneDidLoad() {
        let gameUrl = Bundle.main.url(forResource:   "Time Runners",
                                      withExtension: nil)!
        
        let episodeUrl  = gameUrl.appendingPathComponent("Ep. 1")
        
        gameState   = GameState()
        episode     = Episode(url: episodeUrl)!
        
        eleTextures = episode.eles.map { $0.toSKTexture(palette: episode.palette, baseColor: 225)}
        
        tileMapsNodes = episode.tilesets.map {
            $0.map {
                t -> SKTileMapNode in
                
                let tileset = SKTileSet(tileGroups: t.tiles.map {
                    let texture = $0.toSKTexture(palette: episode.palette)
                    
                    let identity = SKTileDefinition(texture: texture)
                    let flipped  = SKTileDefinition(texture: texture)
                    flipped.flipHorizontally = true
                    
                    return
                        SKTileGroup(rules: [SKTileGroupRule(adjacency: .adjacencyAll,
                                                            tileDefinitions: [identity, flipped])])
                })
                
                return SKTileMapNode(
                    tileSet:  tileset,
                    columns:  Room.tiles.width,
                    rows:     Room.tiles.height,
                    tileSize: Episode.TILE_SIZE.cg
                )
            }
        }
    }
    
    override func didMove(to view: SKView) {
        pupoNode = self.childNode(withName: "//pupo") as! SKSpriteNode
        
        for i in tileMapsNodes {
            if let i = i {
                self.addChild(i)
            }
        }
    }
    
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
        
        input = (
            left ? .left   : right      ? .right  : .still,
            top  ? .top    : bottom     ? .bottom : .still,
            fire ? .firing : .nonFiring
        )
    }
    
    override func keyUp(with event: NSEvent) {
        if event.keyCode ==  49 { fire   = false }
        if event.keyCode == 124 { right  = false }
        if event.keyCode == 123 { left   = false }
        if event.keyCode == 126 { top    = false }
        if event.keyCode == 125 { bottom = false }
        
        input = (
            left ? .left   : right      ? .right  : .still,
            top  ? .top    : bottom     ? .bottom : .still,
            fire ? .firing : .nonFiring
        )
    }
    
    var currentRoom = [Int](repeating: -1, count: Room.tiles.area)
    
    override func update(_ currentTime: TimeInterval) {
        // Called before each frame is rendered
        gameState.tick(input: input, episode: episode)
        
        for row in 0 ..< Room.tiles.height {
            for col in 0 ..< Room.tiles.width {
                let i = row * Room.tiles.height + col
                
                let tile   = gameState.theRoom.tiles[i]
                let tileId = tile.tileId(frame: gameState.roomFrame)
                
                if currentRoom[i] == (tileId << 12 + tile.tilesetId) {
                    continue
                }
                
                currentRoom[i] = (tileId << 12 + tile.tilesetId)
                
                for (i, node) in tileMapsNodes.lazy.enumerated() {
                    if i == tile.tilesetId, let tileGroup = node?.tileSet.tileGroups[tileId] {
                        node?.setTileGroup(
                            tileGroup,
                            andTileDefinition: tileGroup.rules[0].tileDefinitions[tile.flip ? 1 : 0],
                            forColumn: col,
                            row: Room.tiles.height - row - 1
                        )
                    }
                    else {
                        node?.setTileGroup(
                            nil,
                            forColumn: col,
                            row: Room.tiles.height - row - 1
                        )
                    }
                }
            }
        }
        
        let frame = episode.frames[gameState.pupoAni][gameState.pupoAniFrame]
        let ele   = episode.eles[frame.frame]
        
        pupoNode.texture     = eleTextures[frame.frame]
        pupoNode.size        = ele.size.cg
        pupoNode.xScale      = frame.flip ? -1 : 1
        pupoNode.anchorPoint = CGPoint(x: 0.5, y: 0)
        pupoNode.position    = CGPoint(
            x: (      gameState.pupoPos.x + frame.delta.x    ) - (320 / 2),
            y: (200 - gameState.pupoPos.y - frame.delta.y - 1) - (200 / 2)
        )
    }
}
