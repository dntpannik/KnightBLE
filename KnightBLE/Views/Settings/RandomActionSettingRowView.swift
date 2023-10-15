//
//  ToggleActionSettingRowView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/25/23.
//

import SwiftUI
import CoreBluetooth

struct RandomActionSettingRowView: View {
    var peripheralId: UUID
    var serviceId: CBUUID
    @ObservedObject var setting: RandomActionSetting
    @ObservedObject var actionItem: RandomActionItem
    
    init(peripheralId: UUID, serviceId: CBUUID, setting: RandomActionSetting, actionItem: RandomActionItem) {
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
    RandomActionSettingRowView(
        peripheralId: UUID(),
        serviceId: CBUUID(),
        setting: RandomActionSetting(characteristicId: CBUUID()),
        actionItem: RandomActionItem(name: "Action", index: 1))
}
