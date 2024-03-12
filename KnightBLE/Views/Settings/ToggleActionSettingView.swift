//
//  ToggleActionSettingView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/25/23.
//

import SwiftUI
import CoreBluetooth

struct ToggleActionSettingView: View {
    var peripheralId: UUID
    var serviceId: CBUUID
    @ObservedObject var setting: ToggleActionSetting
    
    init(peripheralId: UUID, serviceId: CBUUID, setting: ToggleActionSetting) {
        self.peripheralId = peripheralId
        self.serviceId = serviceId
        self.setting = setting
    }
    
    var body: some View {
        ForEach($setting.actions, id: \.id) { $action in
            ToggleActionSettingRowView(peripheralId: peripheralId,
                                 serviceId: serviceId,
                                 setting: setting,
                                 actionItem: action)
        }
        .id(UUID())
        
    }
}

#Preview {
    ToggleActionSettingView(peripheralId: UUID(),
                      serviceId: CBUUID(),
                      setting: ToggleActionSetting(characteristicId: CBUUID(), actionNames: ["Action 1 Long name", "Action 2", "Action 3", "Action 4"]))
}
