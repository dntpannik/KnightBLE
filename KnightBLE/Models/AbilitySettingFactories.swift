//
//  AbilityMethods.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/29/22.
//

import Foundation
import SwiftUI
import CoreBluetooth

func Setting(_ characteristic: CBCharacteristic, _ data: Data) -> AbilitySetting? {
    switch characteristic.uuid {
    case BluetoothIds.toggleCharacteristic:
        return ToggleSetting(characteristicId: characteristic.uuid, value: data)
    case BluetoothIds.rgbCharacteristic:
        return ColorSetting(characteristicId: characteristic.uuid, value: data)
    case BluetoothIds.sliderCharacteristic:
        return SliderSetting(characteristicId: characteristic.uuid, value: data)
    case BluetoothIds.actionCharacteristic:
        return ActionSetting(characteristicId: characteristic.uuid)
    default:
        print("Unable to convert characteristic into an ability setting")
        return nil
    }
}

func SettingView(_ setting: AbilitySetting, _ serviceId: CBUUID, _ peripheralId: UUID) -> AnyView {
    switch setting.characteristicId {
    case BluetoothIds.toggleCharacteristic:
        return AnyView(ToggleSettingView(peripheralId: peripheralId, serviceId: serviceId, setting: setting as! ToggleSetting))
    case BluetoothIds.rgbCharacteristic:
        return AnyView(ColorSettingView(peripheralId: peripheralId, serviceId: serviceId,  setting: setting as! ColorSetting))
    case BluetoothIds.sliderCharacteristic:
        return AnyView(SliderSettingView(peripheralId: peripheralId, serviceId: serviceId,  setting: setting as! SliderSetting))
    case BluetoothIds.actionCharacteristic:
        return AnyView(ActionSettingView(peripheralId: peripheralId, serviceId: serviceId,  setting: setting as! ActionSetting))
    default:
        print("Unable to convert characteristic into an ability setting view")
        return AnyView(EmptyView())
    }
}
