//
//  ToggleSetting.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/23/23.
//

import Foundation
import CoreBluetooth

class ToggleSetting: AbilitySetting {
    @Published var value: Bool
    @Published var settingName: String
    
    init(characteristicId: CBUUID, value: Bool, settingName: String) {
        self.value = value
        self.settingName = settingName
        super.init(characteristicId: characteristicId)
    }
    
    init(characteristicId: CBUUID, value: Data) {
        self.value = false
        self.settingName = "Enabled"
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
    
    override func ProcessDescriptor(descriptorId: CBUUID, data: Data) {
        if (descriptorId == BluetoothIds.nameDescriptor) {
            self.settingName = ParseString(value: data)
        }
        if (descriptorId == BluetoothIds.orderDescriptor) {
            self.order = data.uint16
        }
    }
}
