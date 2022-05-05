//
//  PreviewGenerators.swift
//  KnightBLE
//
//  Created by Christopher Janover on 5/4/22.
//

import Foundation

func KnightWithAllAblities() -> Knight {
    let peripheralId = BluetoothIds.testUUID
    
    var knight = Knight(name: "Test Knight 1", peripheralId: peripheralId)
    
    //Eye LEDs
    knight.abilities.append(
        BoolKnightAbility(
            characteristicId: BluetoothIds.eyeLedCharacteristic,
            value: false))
    
    //Left Gun LEDs
    knight.abilities.append(
        BoolKnightAbility(
            characteristicId: BluetoothIds.leftGunLedCharacteristic,
            value: true))
    
    //Smoke Stacks
    knight.abilities.append(
        BoolKnightAbility(
            characteristicId: BluetoothIds.smokeStackCharacteristic,
            value: false))
    
    //Audio
    knight.abilities.append(
        SoundKnightAbility(
            characteristicId: BluetoothIds.soundInfoCharacteristic,
            volume: 22,
            sounds: [0, 1]))
    
    return knight
}
