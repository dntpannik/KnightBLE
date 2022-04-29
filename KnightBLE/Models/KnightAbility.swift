//
//  KnightAbility.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/28/22.
//

import CoreBluetooth

struct KnightAbility : Identifiable {
    let id = UUID()
    let characteristicId: CBUUID
    var currentValue: Data
}
