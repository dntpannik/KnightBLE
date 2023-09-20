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
    eyeAbility.order = 1
    eyeAbility.settings[setting.characteristicId] = setting
    knight.abilities[eyeAbility.serviceId] = eyeAbility

    //Left Weapon
    let leftWeaponAbility = KnightAbility(serviceId: CBUUID.init(string: "469ab925-21f4-4c55-bed1-e34c436db5bd"), name: "Left Weapon")
    let setting1 = ToggleSetting(characteristicId: BluetoothIds.toggleCharacteristic, value: false, settingName: "Enabled")
    leftWeaponAbility.settings[setting1.characteristicId] = setting1
    leftWeaponAbility.order = 2
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

func LargeKnightArray() -> [UUID:Knight]
{
    var knightArray = [UUID:Knight]()
    
    let knight = KnightWithAllAblities()
    knightArray[knight.peripheralId] = knight;
    
    let knight2 = KnightWithAllAblities()
    knightArray[knight2.peripheralId] = knight2;
    
    let knight3 = KnightWithAllAblities()
    knightArray[knight3.peripheralId] = knight3;
    
    let knight4 = KnightWithAllAblities()
    knightArray[knight4.peripheralId] = knight4;
    
    let knight5 = KnightWithAllAblities()
    knightArray[knight5.peripheralId] = knight5;
    
    let knight6 = KnightWithAllAblities()
    knightArray[knight6.peripheralId] = knight6;
    
    let knight7 = KnightWithAllAblities()
    knightArray[knight7.peripheralId] = knight7;
    
    let knight8 = KnightWithAllAblities()
    knightArray[knight8.peripheralId] = knight8;
    
    return knightArray
}
