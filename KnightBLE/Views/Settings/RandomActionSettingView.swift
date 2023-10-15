//
//  ToggleActionSettingView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/25/23.
//

import SwiftUI
import CoreBluetooth

struct RandomActionSettingView: View {
    var peripheralId: UUID
    var serviceId: CBUUID
    @ObservedObject var setting: RandomActionSetting
    
    init(peripheralId: UUID, serviceId: CBUUID, setting: RandomActionSetting) {
        self.peripheralId = peripheralId
        self.serviceId = serviceId
        self.setting = setting
    }
    
    private let twoColumnGrid = [
        GridItem(.flexible(minimum: 100)),
        GridItem(.flexible(minimum: 100))
    ]
    
    var body: some View {
        LazyVGrid(columns: twoColumnGrid, alignment: .center) {
            ForEach($setting.actions, id: \.id) { $action in
                RandomActionSettingRowView(peripheralId: peripheralId,
                                     serviceId: serviceId,
                                     setting: setting,
                                     actionItem: action)
            }
            .id(UUID())
        }
        
    }
}

#Preview {
    RandomActionSettingView(peripheralId: UUID(),
                      serviceId: CBUUID(),
                      setting: RandomActionSetting(characteristicId: CBUUID(), actionNames: ["Action 1 Long name", "Action 2", "Action 3", "Action 4"]))
}
