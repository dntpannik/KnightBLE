//
//  ToggleAbilityView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/21/23.
//

import SwiftUI
import CoreBluetooth

struct ColorSettingView: View {
    var peripheralId: UUID
    var serviceId: CBUUID
    @ObservedObject var setting: ColorSetting
    
    var body: some View {
        HStack {
            ColorPicker(setting.settingName, selection: $setting.color)
                .onChange(of: setting.color) { value in
                    bleManager.WriteValue(peripheralId: peripheralId,
                                          serviceId: serviceId,
                                          characteristicId: setting.characteristicId,
                                          withValue: EncodeColor(value: setting.color))
                }
            }
    }
}

struct ColorSettingView_Previews: PreviewProvider {
    static var previews: some View {
        ColorSettingView(peripheralId: UUID(), serviceId: CBUUID(), setting: ColorSetting(characteristicId: CBUUID(), color: Color.purple, settingName: "Test Setting"))
    }
}
