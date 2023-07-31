//
//  AbilitySetting.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/23/23.
//

import Foundation

class AbilitySetting : ObservableObject, Identifiable {
    let id = UUID()
    var characteristicId: CBUUID
    var name: String

    init(characteristicId: CBUUID, name: String) {
        self.characteristicId = characteristicId
        self.name = name
    }
    
    func UpdateValue(data: Data) {
        fatalError("Must Override")
    }
    
    func GetData() -> Data {
        fatalError("Must Override")
    }
}
