//
//  BoolKnightAbility.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/29/22.
//

import Foundation
import CoreBluetooth

class BoolKnightAbility: KnightAbility {
    @Published var value: Bool
    
    init(characteristicId: CBUUID, value: Bool) {
        self.value = value
        super.init(characteristicId: characteristicId)
    }
    
    init(characteristicId: CBUUID, value: Data) {
        self.value = ParseBool(value: value)
        super.init(characteristicId: characteristicId)
    }
    
    override func UpdateValue(data: Data) {
        guard data.count == 1 else {
            print("Unexpected number of bytes when updating value for BoolKnightAbility")
            return //unexpected number of bytes
        }
        
        self.value = data[0] != 0
        print("Value updated to \(self.value)")
    }
    
    override func GetData() -> Data {
        return Data([UInt8(self.value.intValue)])
    }
}
