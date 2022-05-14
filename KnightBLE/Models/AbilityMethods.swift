//
//  AbilityMethods.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/29/22.
//

import Foundation
import SwiftUI
import CoreBluetooth

func Ability(_ characteristic: CBCharacteristic, _ data: Data) -> KnightAbility? {
    switch characteristic.uuid {
        case BluetoothIds.eyeLedCharacteristic:
            return BoolKnightAbility(characteristicId: characteristic.uuid, value: data)
        case BluetoothIds.leftGunLedCharacteristic:
            return BoolKnightAbility(characteristicId: characteristic.uuid, value: data)
        case BluetoothIds.rightGunLedCharacteristic:
            return BoolKnightAbility(characteristicId: characteristic.uuid, value: data)
        case BluetoothIds.smokeStackCharacteristic:
            return BoolKnightAbility(characteristicId: characteristic.uuid, value: data)
        case BluetoothIds.soundInfoCharacteristic:
            return SoundKnightAbility(characteristicId: characteristic.uuid, value: data)
        default:
            print("Unable to convert characteristic into a knight ability")
            return nil
    }
}

func AbilityView(_ ability: KnightAbility, _ knight: Knight) -> AnyView {
    switch ability.characteristicId {
        case BluetoothIds.eyeLedCharacteristic:
            return AnyView(EyesAbilityView(knight: knight, ability: ability as! BoolKnightAbility))
        case BluetoothIds.leftGunLedCharacteristic:
            return AnyView(LeftGunAbilityView(knight: knight, ability: ability as! BoolKnightAbility))
        case BluetoothIds.rightGunLedCharacteristic:
            return AnyView(RightGunAbilityView(knight: knight, ability: ability as! BoolKnightAbility))
        case BluetoothIds.smokeStackCharacteristic:
            return AnyView(SmokeStackAbilityView(knight: knight, ability: ability as! BoolKnightAbility))
        case BluetoothIds.soundInfoCharacteristic:
            return AnyView(SoundAbilityView(knight: knight, ability: ability as! SoundKnightAbility))
        default:
            print("Unable to convert characteristic into a knight ability")
            return AnyView(EmptyView())
    }
}
