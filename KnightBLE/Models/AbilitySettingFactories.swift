//
//  AbilityMethods.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/29/22.
//

import Foundation
import SwiftUI
import CoreBluetooth

func Setting(_ characteristic: CBCharacteristic, _ data: Data, _ name: String) -> AbilitySetting? {
    switch characteristic.uuid {
    case BluetoothIds.toggleCharacteristic:
            return ToggleSetting(characteristicId: characteristic.uuid, value: data, name: name)
       // case BluetoothIds.leftGunLedCharacteristic:
       //     return BoolKnightAbility(characteristicId: characteristic.uuid, value: data, name: name)
       // case BluetoothIds.rightGunLedCharacteristic:
       //     return BoolKnightAbility(characteristicId: characteristic.uuid, value: data, name: name)
       // case BluetoothIds.smokeStackCharacteristic:
       //     return BoolKnightAbility(characteristicId: characteristic.uuid, value: data, name: name)
       // case BluetoothIds.soundInfoCharacteristic:
       //     return SoundKnightAbility(characteristicId: characteristic.uuid, value: data, name: name)
        default:
            print("Unable to convert characteristic into an ability setting")
            return nil
    }
}

func SettingView(_ setting: AbilitySetting, _ ability: KnightAbility, _ knight: Knight) -> AnyView {
    switch setting.characteristicId {
    case BluetoothIds.toggleCharacteristic:
        return AnyView(ToggleAbilityView(knight: knight, ability: ability, setting: setting as! ToggleSetting))
    //    case BluetoothIds.leftGunLedCharacteristic:
     //       return AnyView(LeftGunAbilityView(knight: knight, ability: ability as! BoolKnightAbility))
    //    case BluetoothIds.rightGunLedCharacteristic:
     //       return AnyView(RightGunAbilityView(knight: knight, ability: ability as! BoolKnightAbility))
     //   case BluetoothIds.smokeStackCharacteristic:
     //       return AnyView(SmokeStackAbilityView(knight: knight, ability: ability as! BoolKnightAbility))
     //   case BluetoothIds.soundInfoCharacteristic:
     //       return AnyView(SoundAbilityView(knight: knight, ability: ability as! SoundKnightAbility))
        default:
            print("Unable to convert characteristic into an ability setting view")
            return AnyView(EmptyView())
    }
}
