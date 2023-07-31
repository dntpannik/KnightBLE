//
//  ToggleSetting.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/23/23.
//

import Foundation
import CoreBluetooth

class VolumeSetting: AbilitySetting {
    @Published var value: UInt16
    
    init(characteristicId: CBUUID, value: UInt16) {
        self.value = value
        super.init(characteristicId: characteristicId)
    }
    
    init(characteristicId: CBUUID, value: Data) {
        self.value = ParseBool(value: value)
        super.init(characteristicId: characteristicId)
    }
    
    override func UpdateValue(data: Data) {
        guard data.count == 1 else {
            print("Unexpected number of bytes when updating value for ToggleSetting")
            return //unexpected number of bytes
        }
        
        self.value = data[0] != 0
        print("Value updated to \(self.value)")
    }
    
    override func GetData() -> Data {
        return Data([UInt8(self.value.intValue)])
    }
}
