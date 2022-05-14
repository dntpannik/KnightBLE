//
//  PreviewGenerators.swift
//  KnightBLE
//
//  Created by Christopher Janover on 5/4/22.
//

import Foundation

func KnightWithAllAblities() -> Knight {
    let peripheralId = BluetoothIds.testUUID
    
    var knight = Knight(name: "Incarnate Defender", peripheralId: peripheralId)
    
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
    
    //Right Gun LEDs
    knight.abilities.append(
        BoolKnightAbility(
            characteristicId: BluetoothIds.rightGunLedCharacteristic,
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
            delay: 1345,
            noises: [NoiseMap.Horn, NoiseMap.Test1]))
    
    return knight
}
