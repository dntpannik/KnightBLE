//
//  KnightPeripheral.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import Foundation
import CoreBluetooth

struct Knight : Identifiable {
    
    var id = UUID()
    var name: String = ""
    var connected: Bool = false

    var peripheralId: UUID
    var abilities: [KnightAbility] = []
    
    init(name: String, peripheralId: UUID) {
        self.name = name
        self.peripheralId = peripheralId
    }
    
    init(name: String, peripheralId: UUID, abilities: [KnightAbility]) {
        self.name = name
        self.peripheralId = peripheralId
        self.abilities = abilities
    }
}
