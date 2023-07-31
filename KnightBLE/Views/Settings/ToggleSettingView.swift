//
//  ToggleAbilityView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/21/23.
//

import SwiftUI
import CoreBluetooth

struct ToggleSettingView: View {
    var peripheralId: UUID
    var serviceId: CBUUID
    @ObservedObject var setting: ToggleSetting
    
    var body: some View {
        HStack {
            Toggle(setting.settingName, isOn: $setting.value)
                .onChange(of: setting.value ) { value in
                    bleManager.WriteValue(peripheralId: peripheralId, serviceId: serviceId, characteristicId: setting.characteristicId, withValue: EncodeBool(value: setting.value))
            }
        }
    }
}

struct ToggleSettingView_Previews: PreviewProvider {
    static var previews: some View {
        ToggleSettingView(peripheralId: UUID(), serviceId: CBUUID(), setting: ToggleSetting(characteristicId: CBUUID(), value: false, settingName: "Test Name"))
    }
}
