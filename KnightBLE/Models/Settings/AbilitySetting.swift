//
//  AbilitySetting.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/23/23.
//

import Foundation
import CoreBluetooth

class AbilitySetting : ObservableObject, Identifiable {
    let id = UUID()
    var characteristicId: CBUUID
    @Published var order: UInt16

    init(characteristicId: CBUUID) {
        self.characteristicId = characteristicId
        self.order = 0
    }
    
    func UpdateValue(data: Data) {
        fatalError("Must Override")
    }
    
    func ProcessDescriptor(descriptorId: CBUUID, data: Data) {
        fatalError("Must Override")
    }
    
    func GetData() -> Data {
        fatalError("Must Override")
    }
}
