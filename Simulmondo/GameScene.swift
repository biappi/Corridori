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
    
    var eleTextures   : [SKTexture]!
    var tileMapsNodes : [SKTileMapNode?]!
    
    var playfield  : SKNode!
    var pupoNode   : SKSpriteNode!
    var tileLabels : [SKLabelNode] = []
    
    var input = KeyboardInput()
    
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
        playfield = self.childNode(withName: "playfield")
        pupoNode = self.childNode(withName: "//playfield/pupo") as! SKSpriteNode
        
        for row in 0 ..< Room.tiles.height {
            for col in 0 ..< Room.tiles.width {
                let label = SKLabelNode(text: "a")
                label.position = CGPoint(
                    x:  (CGFloat(col * Episode.TILE_SIZE.width ) - (320.0 / 2.0) + (CGFloat(Episode.TILE_SIZE.width)  * 0.5)) * playfield.xScale,
                    y: -(CGFloat(row * Episode.TILE_SIZE.height) - (200.0 / 2.0) + (CGFloat(Episode.TILE_SIZE.height) * 0.5)) * playfield.yScale
                )
                label.fontName = "Helvetica"
                label.fontSize = 20
                
                self.addChild(label)
                tileLabels.append(label)
            }
        }
        
        for i in tileMapsNodes {
            if let i = i {
                playfield.addChild(i)
            }
        }
    }
    
    override func keyDown(with event: NSEvent) {
        if let code = KeyboardInput.Keycodes(rawValue: event.keyCode),
           event.isARepeat == false
        {
            input.keyDown(code)
        }
    }
    
    override func keyUp(with event: NSEvent) {
        if let code = KeyboardInput.Keycodes(rawValue: event.keyCode) {
            input.keyUp(code)
        }
    }
    
    var currentRoom = [Int](repeating: -1, count: Room.tiles.area)
    
    override func update(_ currentTime: TimeInterval) {
        // Called before each frame is rendered
        gameState.tick(input: input.input, episode: episode)
        
        for row in 0 ..< Room.tiles.height {
            for col in 0 ..< Room.tiles.width {
                let i    = row * Room.tiles.height + col
                let tile = gameState.theRoom.tiles[i]
                
                tileLabels[i].text = tile.type == 0 ? nil : String(format: "%02x", arguments: [tile.type])
            }
        }
        
        for row in 0 ..< Room.tiles.height {
            for col in 0 ..< Room.tiles.width {
                let i = row * Room.tiles.height + col
                
                let tile   = gameState.theRoom.tiles[i]
                let tileId = tile.tileId(frame: gameState.roomFrame)
                
                let hash = (tileId << 24 + tile.tilesetId) * (tile.flip ? -1 : 1)
                
                if currentRoom[i] == hash {
                    continue
                }

                currentRoom[i] = hash
                
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
