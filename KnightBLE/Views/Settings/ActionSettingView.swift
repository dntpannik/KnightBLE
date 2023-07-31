//
//  ActionSettingView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/25/23.
//

import SwiftUI
import CoreBluetooth

struct ActionSettingView: View {
    var peripheralId: UUID
    var serviceId: CBUUID
    @ObservedObject var setting: ActionSetting
    
    private let twoColumnGrid = [
        GridItem(.flexible(minimum: 100)),
        GridItem(.flexible(minimum: 100))
    ]
    
    var body: some View {
        LazyVGrid(columns: twoColumnGrid, alignment: .center) {
            ForEach($setting.actions, id: \.id) { $action in
                ActionSettingRowView(peripheralId: peripheralId,
                                     serviceId: serviceId,
                                     setting: setting,
                                     actionItem: action)
            }
            .id(UUID())
        }
        
    }
}

#Preview {
    ActionSettingView(peripheralId: UUID(),
                      serviceId: CBUUID(),
                      setting: ActionSetting(characteristicId: CBUUID(), actionNames: ["Action 1 Long name", "Action 2", "Action 3", "Action 4"]))
}
