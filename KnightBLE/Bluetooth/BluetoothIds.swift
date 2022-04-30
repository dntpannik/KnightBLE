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
    
    public static let smokeStackService = CBUUID.init(string: "9cd95a69-ee40-41be-a858-b0647c2fb955")
    public static let smokeStackCharacteristic = CBUUID.init(string: "e9196461-d1ec-4b7d-a44a-fb76ad4b0795")
    
    public static let acceptedServies: [CBUUID] = [
        ledService, smokeStackService
    ]
    
    public static let acceptedCharacteristics: [CBUUID] = [
        eyeLedCharacteristic, leftGunLedCharacteristic, smokeStackCharacteristic
    ]
    
    public static let testUUID: UUID = UUID()
}
