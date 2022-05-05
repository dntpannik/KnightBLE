//
//  NoiseView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 5/3/22.
//

import SwiftUI

struct NoiseView: View {
    var knight: Knight
    var ability: SoundKnightAbility
    var knightNoise: KnightNoise
    
    @State private var delay: Double = 0
    
    var body: some View {
        HStack {
            Text(knightNoise.noise.description)
                .italic()
                .bold()
                .font(.callout)
            VStack {
                Text("Delay")
                Slider(
                    value: $delay,
                    in: 0...2000,
                    step: 20)
                Text("\(delay, specifier: "%.2f") ms")
            }
            Button("Play", action: {
                bleManager.WriteValue(
                    peripheralId: knight.peripheralId,
                    serviceId: BluetoothIds.soundService,
                    characteristicId: BluetoothIds.soundControlCharacteristic,
                    withValue: knightNoise.GetData(volume: ability.volume, delay: UInt16(delay)))
            })
            .buttonStyle(GrowingButton())
        }
        .onAppear {
            delay = Double(knightNoise.delayMS)
        }
    }
}

struct NoiseView_Previews: PreviewProvider {
    static var previews: some View {
        NoiseView(
            knight: Knight(name: "TestKnight", peripheralId: UUID()),
            ability: SoundKnightAbility(
                characteristicId: BluetoothIds.eyeLedCharacteristic,
                volume: 23,
                sounds: [0]),
            knightNoise: KnightNoise(noise: NoiseMap.Horn, delayMS: 15))
        .environmentObject(ModelData())
    }
}
