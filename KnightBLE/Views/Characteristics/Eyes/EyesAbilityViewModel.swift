//
//  eyesCharacteristicViewModel.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/28/22.
//

import Foundation

import SwiftUI
import Combine
import CoreBluetooth

class EyesAbilityViewModel : ObservableObject {
    var peripheralId: UUID
    var ability: KnightAbility
    @Published var enabled: Bool = false
    
    func Start() {
        enabled = parse(data: ability.currentValue)
    }
    
    init (peripheralId: UUID, ability: KnightAbility) {
        self.peripheralId = peripheralId
        self.ability = ability
    }
    
    private func parse(data: Data) -> Bool {
        return (String(data: data, encoding: .utf8).flatMap(Bool.init) != nil)
    }
    
    private func encode(value: Bool) -> Data {
        return Data([UInt8(value.intValue)])
    }
    
    func Write() {
        bleManager.WriteValue(peripheralId: peripheralId, serviceId: BluetoothIds.ledService, characteristicId: BluetoothIds.eyeLedCharacteristic, withValue: encode(value: enabled))
    }
}
