//
//  eyesCharacteristic.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import SwiftUI
import CoreBluetooth

struct SmokeStackAbilityView: View {
    @EnvironmentObject var modelData: ModelData
    @State var knight: Knight
    @State var ability: BoolKnightAbility
    @State var enabled: Bool = false

    var body: some View {
        HStack {
            Toggle("Toggle Smoke Stacks", isOn: $enabled)
                .onChange(of: enabled ) { value in
                    bleManager.WriteValue(peripheralId: knight.peripheralId, serviceId: BluetoothIds.smokeStackService, characteristicId: BluetoothIds.smokeStackCharacteristic, withValue: EncodeBool(value: enabled))
            }
        }
        .onAppear {
            enabled = ability.value
        }
    }
}

struct SmokeStackAbilityView_Previews: PreviewProvider {
    static var previews: some View {
        SmokeStackAbilityView(knight: Knight(name: "TestKnight", peripheralId: UUID()), ability: BoolKnightAbility(characteristicId: BluetoothIds.eyeLedCharacteristic, value: false))
    }
}
