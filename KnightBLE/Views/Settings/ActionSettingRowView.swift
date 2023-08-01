//
//  ActionSettingRowView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/25/23.
//

import SwiftUI
import CoreBluetooth

struct ActionSettingRowView: View {
    var peripheralId: UUID
    var serviceId: CBUUID
    @ObservedObject var setting: ActionSetting
    @ObservedObject var actionItem: ActionItem
    
    init(peripheralId: UUID, serviceId: CBUUID, setting: ActionSetting, actionItem: ActionItem) {
        self.peripheralId = peripheralId
        self.serviceId = serviceId
        self.setting = setting
        self.actionItem = actionItem
    }
    
    var body: some View {
        Button(action: {
            bleManager.WriteValue(
                peripheralId: peripheralId,
                serviceId: serviceId,
                characteristicId: setting.characteristicId,
                withValue: EncodeUInt16(value: actionItem.index))
            }) {
                Text(actionItem.name)
            }
            .padding()
            .background(.blue)
            .foregroundColor(.white)
            .cornerRadius(22)
            .buttonStyle(BorderlessButtonStyle())
    }
}

#Preview {
    ActionSettingRowView(
        peripheralId: UUID(),
        serviceId: CBUUID(),
        setting: ActionSetting(characteristicId: CBUUID()),
        actionItem: ActionItem(name: "Action", index: 1))
}
