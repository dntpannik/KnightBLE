//
//  eyesCharacteristic.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import SwiftUI
import CoreBluetooth

struct LeftGunAbilityView: View {
    @EnvironmentObject var modelData: ModelData
    @State var knight: Knight
    @State var ability: BoolKnightAbility
    @State var enabled: Bool = false

    var body: some View {
        HStack {
            Toggle("Toggle Left Gun LEDs", isOn: $enabled)
                .onChange(of: enabled ) { value in
                    bleManager.WriteValue(peripheralId: knight.peripheralId, serviceId: BluetoothIds.ledService, characteristicId: BluetoothIds.leftGunLedCharacteristic, withValue: EncodeBool(value: enabled))
            }
        }
        .onAppear {
            enabled = ability.value
        }
    }
}

struct LeftGunAbilityView_Previews: PreviewProvider {
    static var previews: some View {
        LeftGunAbilityView(knight: Knight(name: "TestKnight", peripheralId: UUID()), ability: BoolKnightAbility(characteristicId: BluetoothIds.leftGunLedCharacteristic, value: false))
    }
}
