//
//  RandomActionSetting.swift
//  KnightBLE
//
//  Created by Christopher Janover on 10/14/23.
//

import Foundation
import CoreBluetooth

class RandomActionSetting: AbilitySetting {
    @Published var actions: [RandomActionItem]
    
    override init(characteristicId: CBUUID) {
        self.actions = []
        super.init(characteristicId: characteristicId)
    }
    
    init(characteristicId: CBUUID, actionNames: [String]) {
        var i = 0;
        var actions: [RandomActionItem] = []
        for actionName in actionNames {
            let actionItem = RandomActionItem(name: actionName, index: UInt16(i))
            actions.append(actionItem)
            
            i+=1;
        }
        self.actions = actions
        
        super.init(characteristicId: characteristicId)
    }
    
    override func UpdateValue(data: Data) {
    }
    
    override func GetData() -> Data {
        return Data()
    }
    
    override func ProcessDescriptor(descriptorId: CBUUID, data: Data) {
        if (descriptorId == BluetoothIds.nameDescriptor) {
            let actionNameString =  ParseString(value: data)
            let actionNames = actionNameString.components(separatedBy: "|")
            
            var i = 0;
            var actions: [RandomActionItem] = []
            for actionName in actionNames {
                let actionItem = RandomActionItem(name: actionName, index: UInt16(i))
                actions.append(actionItem)
                
                i+=1;
            }
            self.actions = actions
        }
        if (descriptorId == BluetoothIds.orderDescriptor) {
            self.order = data.uint16
        }
    }
}
