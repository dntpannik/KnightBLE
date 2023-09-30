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
    
    @Published var type: KnightType = KnightType.Armiger
    
    init(name: String, peripheralId: UUID) {
        self.name = name
        self.peripheralId = peripheralId
        setKnightType()
    }
    
    init(name: String, peripheralId: UUID, type: KnightType, abilities: [CBUUID:KnightAbility]) {
        self.name = name
        self.type = type
        self.peripheralId = peripheralId
        self.abilities = abilities
    }
    
    func setKnightType() {
        if (name == "Canis Invictoria" || name == "Corruptors Purge" || name == "Death Storm" || name == "Doom Unending" || name == "Fiends Woe" || name == "Fist of Thought" || name == "Iron Dread" || name == "Rhadinthos" ) {
            type = KnightType.Armiger
        }
        else if (name == "Fury Unbridled" || name == "Aspanis" || name == "Glory of Iron") {
            type = KnightType.Questoris
        }
        else if (name == "Unbound Scion") {
            type = KnightType.Cerastus
        }
    }
}
