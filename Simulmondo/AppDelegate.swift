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

    @IBOutlet weak var state: NSTextField!
    
    func applicationDidFinishLaunching(_ aNotification: Notification) {
        let gameUrl = Bundle.main.url(forResource:   "Time Runners",
                                      withExtension: nil)!
        
        let episodeUrl  = gameUrl.appendingPathComponent("Ep. 1")
        
        var input = Input(.still, .still, .nonFiring)

        gameState = initialState
        episode   = Episode(url: episodeUrl)!
        gameView  = GameView(episode: episode)

//        setupEmulation(episodeURL: episodeUrl)
        
        gameWrapper.addSubview(gameView)
        window.makeFirstResponder(gameView)
        
        window.inputDidChange = {
            input = $0
//            print("x")
        }

        var previousAni = -10
        
        Timer.scheduledTimer(withTimeInterval: 1 / 30.0, repeats: true) { _ in
            self.gameState.tick(input: input, episode: self.episode)
            self.gameView.apply(state: self.gameState)
            
            for (i, view) in self.gameView.roomView.tileViews.enumerated() {
                let vc = self.episode.logitab[0x25].contains(self.episode.rooms[self.gameState.room].tiles[i].type)
                if vc {
                    view.label?.stringValue = "\(view.label?.stringValue ?? "") - 25"
                }

                let vo = self.episode.logitab[0x28].contains(self.episode.rooms[self.gameState.room].tiles[i].type)
                if vo {
                    view.label?.stringValue = "\(view.label?.stringValue ?? "") - 28"
                }

            }
            
            if self.gameState.pupoAni != previousAni {
                previousAni = self.gameState.pupoAni
                
                let a = String(format: "%02x", previousAni)
                self.textView.string = "\(self.textView.string)\nchanged to \(a)"
                self.textView.scrollToEndOfDocument(nil)
            }
            
            self.state.stringValue = "\(self.gameState)"
            // let a = String(format: "%02x", self.gameState.pupoAni)
            // self.textView.string = "ani: \(a)"

            // self.textView.string = "\(self.gameState)"
        }
    }
    
    @IBAction func room(_ sender: Any) {
        //gameState.room = roomField.integerValue
        gameState.pupoAni = roomField.integerValue
    }
    
    @IBAction func tileTypesChecked(_ sender: Any) {
        gameView.roomView.showTypes = tileTypesButton.state == NSControl.StateValue.onState
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }
}

