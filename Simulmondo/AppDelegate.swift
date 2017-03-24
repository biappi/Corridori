//
//  AppDelegate.swift
//  Simulmondo
//
//  Created by Antonio Malara on 19/03/2017.
//  Copyright © 2017 Antonio Malara. All rights reserved.
//

import Cocoa

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    @IBOutlet weak var window: NSWindow!
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        let gameUrl = Bundle.main.url(forResource:   "Time Runners",
                                      withExtension: nil)!
        
        let episodeUrl  = gameUrl.appendingPathComponent("Ep. 1")
        
        let episode   = Episode(url: episodeUrl)!
        var gameState = GameState()
        let gameView  = GameView(episode: episode)
        var input     = Input(.still, .still, .nonFiring)
        
        window.setContentSize(gameView.frame.size)
        window.contentView = gameView
        window.makeFirstResponder(gameView)

        gameView.inputDidChange = {
            input = $0
        }

        Timer.scheduledTimer(withTimeInterval: 1 / 20.0, repeats: true) { _ in
            gameState.tick(input: input, episode: episode)
            gameView.apply(state: gameState)
        }
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }
}

