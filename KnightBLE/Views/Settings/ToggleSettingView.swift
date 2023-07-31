//
//  ToggleAbilityView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/21/23.
//

import SwiftUI

struct ToggleAbilityView: View {
    @EnvironmentObject var modelData: ModelData
    @State var knight: Knight
    @State var setting: ToggleSetting
    @State var enabled: Bool = false

    var body: some View {
        HStack {
            Toggle("Enabled", isOn: $enabled)
                .onChange(of: enabled ) { value in
                    bleManager.WriteValue(peripheralId: knight.peripheralId, serviceId: BluetoothIds.modelService, characteristicId: BluetoothIds.toggleCharacteristic, withValue: EncodeBool(value: enabled))
            }
        }
        .onAppear {
            enabled = setting.value
        }
    }
}

struct ToggleAbilityView_Previews: PreviewProvider {
    static var previews: some View {
        ToggleAbilityView(knight: Knight(name: "TestKnight", peripheralId: UUID()), setting: ToggleSetting(characteristicId: BluetoothIds.toggleCharacteristic, value: false))
    }
}
