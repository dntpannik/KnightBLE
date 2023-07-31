//
//  ToggleSetting.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/23/23.
//

import Foundation
import CoreBluetooth

class SliderSetting: AbilitySetting {
    @Published var settingName: String
    @Published var value: Double
    @Published var minValue: Double
    @Published var maxValue: Double
    @Published var stepValue: Double
    
    init(characteristicId: CBUUID, value: UInt16) {
        self.settingName = ""
        self.value = Double(value)
        self.minValue = 0
        self.maxValue = 100
        self.stepValue = 5
        super.init(characteristicId: characteristicId)
    }
    
    init(characteristicId: CBUUID, name: String, value: UInt16, minValue: UInt16, maxValue: UInt16, stepValue: UInt16) {
        self.settingName = name
        self.value = Double(value)
        self.minValue = Double( minValue)
        self.maxValue = Double(maxValue)
        self.stepValue = Double(stepValue)
        super.init(characteristicId: characteristicId)
    }
    
    init(characteristicId: CBUUID, value: Data) {
        self.settingName = ""
        
        if (value.count == 4) {
            self.value = Double(ParseUInt16(value: value))
        } else {
            self.value = 0
        }
        
        self.minValue = 0
        self.maxValue = 100
        self.stepValue = 5
        super.init(characteristicId: characteristicId)
    }
    
    override func UpdateValue(data: Data) {
        guard data.count == 2 else {
            print("Unexpected number of bytes when updating value for SliderSetting")
            return //unexpected number of bytes
        }
        
        self.value = Double(data.uint16)
        print("Value updated to \(self.value)")
    }
    
    override func GetData() -> Data {
        return EncodeUInt16(value: UInt16(self.value))
    }
    
    override func ProcessDescriptor(descriptorId: CBUUID, data: Data) {
        if (descriptorId == BluetoothIds.nameDescriptor) {
            self.settingName = ParseString(value: data)
            return
        }
//        if (descriptorId == BluetoothIds.minValueDescriptor) {
//            let reader = DataReader(data);
//            let test : UInt16 = reader.readNext();
//
//            return
//        }
//        if (descriptorId == BluetoothIds.maxValueDescriptor) {
//            let reader = DataReader(data);
//            self.maxValue = reader.readNext();
//            
//            return
//        }
//        if (descriptorId == BluetoothIds.stepValueDescriptor) {
//            let reader = DataReader(data);
//            self.stepValue = reader.readNext();
//            return
//        }
    }
}
