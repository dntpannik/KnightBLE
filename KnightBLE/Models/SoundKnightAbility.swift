//
//  BoolKnightAbility.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/29/22.
//

import Foundation
import CoreBluetooth

enum NoiseMap : UInt8, CustomStringConvertible {
    case Horn = 1
    case VolcanoLance = 2
    case PlasmaDecimator = 3
    case Test3 = 4
    case Test4 = 5

    var description : String {
        switch self {
            case .Horn: return "Horn"
            case .VolcanoLance: return "Vol. Lance"
            case .PlasmaDecimator: return "Plas.Dec."
            case .Test3: return "Test3"
            case .Test4: return "Test4"
        }
    }
}

class SoundKnightAbility: KnightAbility {
    @Published var volume: UInt8 = 15
    @Published var delay: UInt16 = 0
    @Published var noises: [NoiseMap]
    @Published var activeNoise: NoiseMap? = nil
    
    init(characteristicId: CBUUID, volume: UInt8, delay: UInt16, noises: [NoiseMap]) {
        self.volume = volume
        self.delay = delay
        self.noises = noises

        super.init(characteristicId: characteristicId)
    }
    
    init(characteristicId: CBUUID, value: Data) {
        self.noises = []
        super.init(characteristicId: characteristicId)
    }
    
    override func UpdateValue(data: Data) {
        let reader = DataReader(data)
        
        let numSounds = data.count / 4
        for _ in 1...numSounds {
            let soundValue32: UInt32 = reader.readNext()
            let soundValue: UInt8 = UInt8(soundValue32)
    
            if let noise = NoiseMap(rawValue: soundValue) {
                if noises.first(where: {$0.rawValue == noise.rawValue}) == nil {
                    self.noises.append(noise)
                }
            }
        }
        
        //If there is at least 1 noise then set as active
        if noises.count > 0 {
            activeNoise = noises[0]
        }
    }
    
    override func GetData() -> Data {
        guard let noise = self.activeNoise else {
            return Data()
        }

        return Data([noise.rawValue, self.volume])
    }
    
    func Play(peripheralId: UUID) {
        DispatchQueue.main.asyncAfter(deadline: .now() + Double(delay / 1000)) {
            bleManager.WriteValue(
                                peripheralId: peripheralId,
                                serviceId: BluetoothIds.soundService,
                                characteristicId: BluetoothIds.soundControlCharacteristic,
                                withValue: self.GetData())
        }
    }
}
