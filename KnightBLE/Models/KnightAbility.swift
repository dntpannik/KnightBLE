//
//  KnightAbility.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/28/22.
//

import CoreBluetooth

struct KnightAbility : Identifiable {
    let id = UUID()
    var characteristicId: CBUUID
    var currentValue: Data
    
    init(characteristicId: CBUUID, value: Data) {
        self.characteristicId = characteristicId
        self.currentValue = value
    }
    
    init(characteristicId: CBUUID, value: Bool) {
        self.characteristicId = characteristicId
        self.currentValue = EncodeBool(value: value)
    }

}
