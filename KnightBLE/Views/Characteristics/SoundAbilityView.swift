//
//  SoundAbilityView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 5/1/22.
//

import SwiftUI

struct SoundAbilityView: View {
    @EnvironmentObject var modelData: ModelData
    var knight: Knight
    var ability: SoundKnightAbility
    
    @State private var volume: Double = 0
    @State private var delay: Double = 0
    @State private var activeNoise: NoiseMap? = nil
    
    let columns = [GridItem(.adaptive(minimum: 120, maximum: 260))]
    
    var body: some View {
        VStack {
            Button("Play Sound")
            {
                DispatchQueue.main.asyncAfter(deadline: .now() + Double(ability.delay / 1000)) {
                    bleManager.WriteValue(
                                        peripheralId: knight.peripheralId,
                                        serviceId: BluetoothIds.soundService,
                                        characteristicId: BluetoothIds.soundControlCharacteristic,
                                        withValue: ability.GetData())
                }
            }
            .disabled(activeNoise == nil)
            .buttonStyle(GrowingButton())
            HStack {
                Text("Volume")
                Slider(
                    value: $volume,
                    in: 0...30,
                    step: 1) { editing in
                    if (!editing) {
                        ability.volume = UInt8(volume)
                    }
                }
                Text("\(volume, specifier: "%.0f")")
            }
            HStack {
                Text("Delay")
                Slider(
                    value: $delay,
                    in: 0...2000,
                    step: 10) { editing in
                        if (!editing) {
                            ability.delay = UInt16(delay)
                        }
                    }
                Text("\(delay, specifier: "%.2f") ms")
            }
            
            LazyVGrid(columns: columns,
                      alignment: .leading,
                      spacing: 30) {
                ForEach(ability.noises, id: \.self) { noise in
                    
                    let isActive = Binding<Bool>(get: {
                        guard let actNoise = activeNoise else {
                            return false
                        }
                        return actNoise == noise
                    }, set: { _ in
                        ability.activeNoise = noise
                        activeNoise = noise
                    })
                    
                    Toggle(isOn: isActive) {
                        if (activeNoise == noise) {
                            Text(noise.description)
                                .bold()
                        } else {
                            Text(noise.description)
                        }
                    }
                    
                }
            }
        }
        .onAppear() {
            volume = Double(ability.volume)
            delay = Double(ability.delay)
            activeNoise = ability.activeNoise
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
                delay: 2145,
                noises: [NoiseMap.Horn, NoiseMap.VolcanoLance, NoiseMap.PlasmaDecimator, NoiseMap.Test3, NoiseMap.Test4]))
        .environmentObject(ModelData())
    }
}
