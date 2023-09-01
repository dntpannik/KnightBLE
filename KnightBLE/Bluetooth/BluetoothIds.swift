//
//  KnightPeripheral.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/24/22.
//

import UIKit
import CoreBluetooth

class BluetoothIds {
    public static let modelService = CBUUID.init(string: "585ff59a-3e23-4405-ad70-1d6d87f7ae6d")
    
    public static let nameCharacteristic = CBUUID.init(string: "5b7a52b5-af5b-4451-921b-4cf039193c58")
    public static let toggleCharacteristic = CBUUID.init(string: "9929eb1f-c8c6-42d3-99a2-61d1d6e157bb")
    public static let rgbCharacteristic = CBUUID.init(string: "44c5f429-9c6d-460a-a9ab-0fd36ca26cd7")
    public static let sliderCharacteristic = CBUUID.init(string: "9a84a830-23c4-4685-b526-3e42fe867b6b")
    public static let actionCharacteristic = CBUUID.init(string: "6955ecc8-f7be-4efa-b122-ed0fef08fc28")

    public static let nameDescriptor = CBUUID.init(string: "b936f62f-6127-4463-9abd-8f965602c737")
    public static let orderDescriptor = CBUUID.init(string: "af93f94e-248c-4076-930e-a66a55877bb2")
    public static let minValueDescriptor = CBUUID.init(string: "0e20fe55-86f5-400f-a95b-8d194416731b")
    public static let maxValueDescriptor = CBUUID.init(string: "89db4977-95c5-42f8-8990-ede5ef227c6d")
    public static let stepValueDescriptor = CBUUID.init(string: "ef7548a1-9942-4f00-a0cf-cf744b3d15da")
    
    public static let acceptedServies: [CBUUID] = [
        modelService
    ]
    
    public static let acceptedCharacteristics: [CBUUID] = [
        toggleCharacteristic, nameCharacteristic, rgbCharacteristic, sliderCharacteristic, actionCharacteristic
    ]
    
    public static let displayCharacteristics: [CBUUID] = [
        toggleCharacteristic, rgbCharacteristic, sliderCharacteristic, actionCharacteristic
    ]
    
    public static let acceptedDescriptors: [CBUUID] = [
        nameDescriptor, orderDescriptor, minValueDescriptor, maxValueDescriptor, stepValueDescriptor
    ]

    public static let testUUID: UUID = UUID()
}
