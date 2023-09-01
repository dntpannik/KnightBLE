//
//  KnightAbility.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/28/22.
//

import CoreBluetooth

class KnightAbility : ObservableObject, Identifiable {
    let id = UUID()
    var serviceId: CBUUID
    @Published var name: String {
        willSet {
                objectWillChange.send()
            }
    }

    @Published var order: UInt16 {
        willSet {
                objectWillChange.send()
            }
    }
    
    @Published var settings: [CBUUID:AbilitySetting] = [:]
    
    init(serviceId: CBUUID) {
        self.serviceId = serviceId
        self.name = ""
        self.order = 0
    }
    
    init(serviceId: CBUUID, name: String) {
        self.serviceId = serviceId
        self.name = name
        self.order = 0
    }
    
    init(serviceId: CBUUID, name: String, order: UInt16, settings: [CBUUID:AbilitySetting])
    {
        self.serviceId = serviceId
        self.name = name
        self.order = order
        self.settings = settings
    }
}
