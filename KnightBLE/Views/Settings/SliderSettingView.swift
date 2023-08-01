//
//  SliderSettingView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/24/23.
//

import SwiftUI
import CoreBluetooth

struct SliderSettingView: View {
    var peripheralId: UUID
    var serviceId: CBUUID
    @ObservedObject var setting: SliderSetting

    init(peripheralId: UUID, serviceId: CBUUID, setting: SliderSetting) {
        self.peripheralId = peripheralId
        self.serviceId = serviceId
        self.setting = setting
    }
    
    var body: some View {
        HStack {
            Text(setting.settingName)
            Slider(
                value: $setting.value,
                in: setting.minValue...setting.maxValue,
                step: setting.stepValue) { editing in
                    if (!editing) {
                        setting.value = setting.value
                        bleManager.WriteValue(
                            peripheralId: peripheralId,
                            serviceId: serviceId,
                            characteristicId: setting.characteristicId,
                            withValue: EncodeUInt16(value: UInt16(setting.value)))
                    }
                }
            Text("\(setting.value, specifier: "%.0f")")
            }
    }
}

#Preview {
    SliderSettingView(peripheralId: UUID(), serviceId: CBUUID(), setting: SliderSetting(characteristicId: CBUUID(), name: "Test Slider", value: 34, minValue: 0, maxValue: 112, stepValue: 5))
}
