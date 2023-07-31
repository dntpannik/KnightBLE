//
//  ToggleSetting.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/23/23.
//

import Foundation
import CoreBluetooth

class ActionSettings: AbilitySetting {
    @Published var actions: [ActionItem]
    
    override init(characteristicId: CBUUID) {
        self.actions = []
        super.init(characteristicId: characteristicId)
    }
    
    init(characteristicId: CBUUID, actionNames: [String]) {
        var i = 0;
        var actions: [ActionItem] = []
        for actionName in actionNames {
            let actionItem = ActionItem(name: actionName, index: UInt16(i))
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
    }
}
