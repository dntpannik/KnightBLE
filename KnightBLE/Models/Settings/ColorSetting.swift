//
//  ToggleSetting.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/23/23.
//

import Foundation
import CoreBluetooth
import SwiftUI

class ColorSetting: AbilitySetting {
    @Published var color: Color
    @Published var settingName: String
    
    init(characteristicId: CBUUID, color: Color, settingName: String) {
        self.color = color
        self.settingName = settingName
        super.init(characteristicId: characteristicId)
    }
    
    init(characteristicId: CBUUID, value: Data) {
        self.color = ParseColor(value: value)
        self.settingName = "Color"
        super.init(characteristicId: characteristicId)
    }
    
    override func UpdateValue(data: Data) {
        guard data.count == 6 else {
           print("Unexpected number of bytes when updating value for ColorSetting")
           return //unexpected number of bytes
        }
        
        self.color = ParseColor(value: data)
        print("Color updated to \(self.color)")
    }
    
    override func GetData() -> Data {
        return EncodeColor(value: self.color)
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
