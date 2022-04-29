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
    @StateObject private var modelData = ModelData()
    
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(modelData)
        }
    }
}
