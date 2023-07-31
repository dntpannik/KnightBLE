//
//  KnightAbilityView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/23/23.
//

import SwiftUI
import CoreBluetooth

struct KnightAbilityView: View {
    var peripheralId: UUID
    @ObservedObject var ability: KnightAbility
    
    var body: some View {
        VStack {
            ForEach(Array(ability.settings.values), id: \.id) { setting in
                SettingView(setting, ability.serviceId, peripheralId)}.id(UUID())
                .padding([.leading, .trailing], 20)
        }
    }
}

#Preview {
    KnightAbilityView(peripheralId: UUID(), ability: KnightWithAllAblities().abilities.values.first!)
}
