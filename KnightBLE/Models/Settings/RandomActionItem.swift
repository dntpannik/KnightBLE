//
//  RandomActionOptionSetting.swift
//  KnightBLE
//
//  Created by Christopher Janover on 10/14/23.
//

import Foundation
import CoreBluetooth

class RandomActionItem: ObservableObject, Identifiable {
    let id = UUID()
    @Published var name: String
    @Published var value: Bool
    @Published var index: UInt16
    
    init(name: String, index: UInt16) {
        self.name = name
        self.value = true
        self.index = index
    }
}
