//
//  KnightPeripheral.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import Foundation
import CoreBluetooth

class Knight : ObservableObject, Identifiable {
    
    var id = UUID()
    @Published var name: String = ""
    @Published var connected: Bool = false

    @Published var peripheralId: UUID
    @Published var abilities: [CBUUID : KnightAbility] = [:]
    
    init(name: String, peripheralId: UUID) {
        self.name = name
        self.peripheralId = peripheralId
    }
    
    init(name: String, peripheralId: UUID, abilities: [CBUUID:KnightAbility]) {
        self.name = name
        self.peripheralId = peripheralId
        self.abilities = abilities
    }
}
