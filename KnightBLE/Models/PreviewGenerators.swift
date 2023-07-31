//
//  PreviewGenerators.swift
//  KnightBLE
//
//  Created by Christopher Janover on 5/4/22.
//

import Foundation
import CoreBluetooth

func UnknownKnight() -> Knight {
    let knight = Knight(name: "Unknown", peripheralId: UUID())
    return knight
}

func UnknownAbility() -> AbilitySetting {
    return ActionSetting(characteristicId: CBUUID())
}

func KnightWithAllAblities() -> Knight {
    let peripheralId = BluetoothIds.testUUID
    
    let knight = Knight(name: "Death Storm", peripheralId: peripheralId)
    knight.connected = true
    
    //Eyes
    let eyeAbility = KnightAbility(serviceId: CBUUID.init(string: "45359908-db96-4ac9-bfa9-77a853440bd2"), name: "Eyes")
    let setting = ToggleSetting(characteristicId: BluetoothIds.toggleCharacteristic, value: false, settingName: "Enabled")
    eyeAbility.settings[setting.characteristicId] = setting
    knight.abilities[eyeAbility.serviceId] = eyeAbility

    //Left Weapon
    let leftWeaponAbility = KnightAbility(serviceId: CBUUID.init(string: "469ab925-21f4-4c55-bed1-e34c436db5bd"), name: "Left Weapon")
    let setting1 = ToggleSetting(characteristicId: BluetoothIds.toggleCharacteristic, value: false, settingName: "Enabled")
    leftWeaponAbility.settings[setting1.characteristicId] = setting1
    knight.abilities[leftWeaponAbility.serviceId] = leftWeaponAbility
    
    return knight
}

func KnightArray() -> [UUID:Knight]
{
    var knightArray = [UUID:Knight]()
    
    let knight = KnightWithAllAblities()
    knightArray[knight.peripheralId] = knight;
    
    let knight2 = KnightWithAllAblities()
    knightArray[knight2.peripheralId] = knight2;
    
    return knightArray
}
