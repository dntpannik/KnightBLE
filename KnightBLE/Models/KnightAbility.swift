//
//  KnightAbility.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/28/22.
//

import CoreBluetooth

class KnightAbility : ObservableObject, Identifiable {
    let id = UUID()
    var characteristicId: CBUUID
    //var currentValue: Data
    
    @Published var boolValue: Bool?
    @Published var intValue: Int?
    
    init(characteristicId: CBUUID) {
        self.characteristicId = characteristicId
    }
    
    func UpdateValue(data: Data) {
        fatalError("Must Override")
    }
    
    func GetData() -> Data {
        fatalError("Must Override")
    }
}
