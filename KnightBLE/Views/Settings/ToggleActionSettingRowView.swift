//
//  ToggleActionSettingRowView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/25/23.
//

import SwiftUI
import CoreBluetooth

struct ToggleActionSettingRowView: View {
    var peripheralId: UUID
    var serviceId: CBUUID
    @ObservedObject var setting: ToggleActionSetting
    @ObservedObject var actionItem: ToggleActionItem
    
    init(peripheralId: UUID, serviceId: CBUUID, setting: ToggleActionSetting, actionItem: ToggleActionItem) {
        self.peripheralId = peripheralId
        self.serviceId = serviceId
        self.setting = setting
        self.actionItem = actionItem
    }
    
    var body: some View {
        Toggle(actionItem.name, isOn: $actionItem.value)
            .onChange(of: actionItem.value ) { oldValue, value in
                bleManager.WriteValue(
                    peripheralId: peripheralId,
                    serviceId: serviceId,
                    characteristicId: setting.characteristicId,
                    withValue: EncodeUInt16(value: actionItem.index))
        }
    }
}

#Preview {
    ToggleActionSettingRowView(
        peripheralId: UUID(),
        serviceId: CBUUID(),
        setting: ToggleActionSetting(characteristicId: CBUUID()),
        actionItem: ToggleActionItem(name: "Action", index: 1))
}
