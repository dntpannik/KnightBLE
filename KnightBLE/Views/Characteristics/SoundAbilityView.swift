//
//  SoundAbilityView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 5/1/22.
//

import SwiftUI

struct SoundAbilityView: View {
    @EnvironmentObject var modelData: ModelData
    @State var knight: Knight
    @State var ability: SoundKnightAbility
    
    @State private var volume: Double = 10.0

    var body: some View {
        VStack {
            HStack {
                Text("Volume")
                Slider(
                    value: $volume,
                    in: 0...30,
                    step: 1
                ) { editing in
                    if (!editing) {
                        ability.volume = UInt8(volume)
                    }
                }
            }
            ForEach(ability.noises, id: \.id) { noise in
                NoiseView(knight: knight, ability: ability, knightNoise: noise)
            }.id(UUID())
            Spacer()
        }
        .onAppear() {
            volume = Double(ability.volume)
        }
    }
}

struct SoundAbilityView_Previews: PreviewProvider {
    static var previews: some View {
        SoundAbilityView(
            knight: Knight(name: "TestKnight", peripheralId: UUID()),
            ability: SoundKnightAbility(
                characteristicId: BluetoothIds.eyeLedCharacteristic,
                volume: 23,
                sounds: [0, 0]))
        .environmentObject(ModelData())
    }
}
