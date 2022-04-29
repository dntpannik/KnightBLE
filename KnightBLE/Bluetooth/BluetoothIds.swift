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
    
    public static let acceptedServies: [CBUUID] = [
        ledService
    ]
    
    public static let acceptedCharacteristics: [CBUUID] = [
        eyeLedCharacteristic
    ]
}
