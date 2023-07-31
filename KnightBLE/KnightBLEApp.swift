//
//  KnightBLEApp.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/24/22.
//

import SwiftUI

let bleManager: BluetoothManager = BluetoothManager()

@main
struct KnightBLEApp: App {
    var body: some Scene {
        WindowGroup {
            ContentView()
        }
    }
}
