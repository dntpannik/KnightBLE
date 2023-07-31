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
    @Published var name: String

    var settings: [CBUUID:AbilitySetting] = [:]
    
    init(serviceId: CBUUID) {
        self.serviceId = serviceId
        self.name = ""
    }
    
    init(serviceId: CBUUID, name: String) {
        self.serviceId = serviceId
        self.name = name
    }
    
    init(serviceId: CBUUID, name: String, settings: [CBUUID:AbilitySetting])
    {
        self.serviceId = serviceId
        self.name = name
        self.settings = settings
    }
}
