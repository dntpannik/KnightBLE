//
//  KnightPeripheral.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/24/22.
//

import UIKit
import CoreBluetooth

class BluetoothIds {
    public static let ledService = CBUUID.init(string: "adb7bab2-df5c-4292-9f71-e2b6aa806c3b")
    public static let eyeLedCharacteristic = CBUUID.init(string: "d8eeaa08-db2f-48a0-99cd-aadd33194ffd")
    public static let leftGunLedCharacteristic = CBUUID.init(string: "9aa13f92-1bd9-4e59-90f6-e41dfa9a81a2")
    public static let rightGunLedCharacteristic = CBUUID.init(string: "9e2b8f55-26e4-40f0-843d-27a80203685e")
    
    public static let smokeStackService = CBUUID.init(string: "9cd95a69-ee40-41be-a858-b0647c2fb955")
    public static let smokeStackCharacteristic = CBUUID.init(string: "e9196461-d1ec-4b7d-a44a-fb76ad4b0795")
    
    public static let soundService = CBUUID.init(string: "ab936fb8-e5f7-4c43-b592-aab50be3c7da")
    public static let soundInfoCharacteristic = CBUUID.init(string: "607047f7-9dd2-477d-94fa-9a6333dba1d4")
    public static let soundControlCharacteristic = CBUUID.init(string: "8ff278a1-a01e-4c46-87e7-751db82bfe24")
    
    public static let acceptedServies: [CBUUID] = [
        ledService, smokeStackService, soundService
    ]
    
    public static let acceptedCharacteristics: [CBUUID] = [
        eyeLedCharacteristic, leftGunLedCharacteristic, rightGunLedCharacteristic, smokeStackCharacteristic, soundInfoCharacteristic, soundControlCharacteristic
    ]
    
    public static let displayCharacteristics: [CBUUID] = [
        eyeLedCharacteristic, leftGunLedCharacteristic, rightGunLedCharacteristic, smokeStackCharacteristic, soundInfoCharacteristic
    ]

    public static let testUUID: UUID = UUID()
}
