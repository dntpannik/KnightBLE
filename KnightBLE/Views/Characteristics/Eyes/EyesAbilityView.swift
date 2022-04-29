//
//  eyesCharacteristic.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import SwiftUI
import CoreBluetooth

struct EyesAbilityView: View {
    @EnvironmentObject var modelData: ModelData
    @State var knight: Knight
    var characteristicId: CBUUID
    
    private var enabled: Binding<Bool> { Binding (
        get: { ParseBool(value: ability.currentValue) },
        set: { EncodeBool(value: $0) }
        )
    }
    
    private var ability: KnightAbility {
        return knight.abilities.first(where: {$0.characteristicId == characteristicId}) ?? KnightAbility(characteristicId: CBUUID(), value: false)
    }
    
    var body: some View {
        HStack {
            Toggle("Toggle Eye LEDs", isOn: self.enabled)
                .onChange(of: ability.currentValue ) { value in
                    bleManager.WriteValue(peripheralId: knight.peripheralId, serviceId: BluetoothIds.ledService, characteristicId: BluetoothIds.eyeLedCharacteristic, withValue: EncodeBool(value: ParseBool(value: value)))
            }
        }
    }
}

struct EyesAbilityView_Previews: PreviewProvider {
    static var previews: some View {
        EyesAbilityView(knight: Knight(name: "TestKnight", peripheralId: UUID(), abilities:  [KnightAbility(characteristicId: BluetoothIds.eyeLedCharacteristic, value: false)]), characteristicId: BluetoothIds.eyeLedCharacteristic)
    }
}
