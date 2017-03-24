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

    @IBOutlet weak var window: Window!
    @IBOutlet weak var tileTypesButton: NSButton!
    @IBOutlet      var textView: NSTextView!
    @IBOutlet weak var roomField: NSTextField!
    @IBOutlet weak var gameWrapper: NSView!
    
    var gameState : GameState!
    var episode   : Episode!
    var gameView  : GameView!

    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        let gameUrl = Bundle.main.url(forResource:   "Time Runners",
                                      withExtension: nil)!
        
        let episodeUrl  = gameUrl.appendingPathComponent("Ep. 1")
        
        var input     = Input(.still, .still, .nonFiring)

        gameState = GameState()
        episode   = Episode(url: episodeUrl)!
        gameView  = GameView(episode: episode)

        gameWrapper.addSubview(gameView)
        window.makeFirstResponder(gameView)

        
        window.inputDidChange = {
            input = $0
        }

        Timer.scheduledTimer(withTimeInterval: 1 / 20.0, repeats: true) { _ in
            self.gameState.tick(input: input, episode: self.episode)
            self.gameView.apply(state: self.gameState)
            
            //self.textView.string = "\(self.gameState)"
        }
    }
    
    @IBAction func room(_ sender: Any) {
        gameState.room = roomField.integerValue
    }
    
    @IBAction func tileTypesChecked(_ sender: Any) {
        gameView.roomView.showTypes = tileTypesButton.state == NSOnState
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }
}

