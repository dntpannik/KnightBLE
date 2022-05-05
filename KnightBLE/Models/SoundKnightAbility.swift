//
//  BoolKnightAbility.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/29/22.
//

import Foundation
import CoreBluetooth

enum NoiseMap : UInt8, CustomStringConvertible {
    case Horn = 0
    case Test1 = 1

    var description : String {
        switch self {
            case .Horn: return "Horn"
            case .Test1: return "Test1"
        }
    }
}

struct KnightNoise : Identifiable {
    var id = UUID()
    var noise: NoiseMap
    var delayMS: UInt16
    
    init(noise: NoiseMap, delayMS: UInt16) {
        self.noise = noise
        self.delayMS = delayMS
    }
    
    func GetData(volume: UInt8, delay: UInt16) -> Data {
        let delay8Bit = UInt8(delay / 5)
        return Data([noise.rawValue, volume, delay8Bit])
    }
}

class SoundKnightAbility: KnightAbility {
    @Published var volume: UInt8 = 15
    @Published var noises: [KnightNoise]
    
    init(characteristicId: CBUUID, volume: UInt8, sounds: [UInt8]) {
        self.volume = volume
        
        var noises: [KnightNoise] = []
        for sound in sounds {
            if let noise = NoiseMap(rawValue: sound) {
                noises.append(KnightNoise(noise: noise, delayMS: 0))
            }
        }
        self.noises = noises

        super.init(characteristicId: characteristicId)
    }
    
    init(characteristicId: CBUUID, value: Data) {
        let reader = DataReader(value)
        //self.volume = reader.readNext()
        
        //let numSounds: UInt8 = reader.readNext()
        
        var noises: [KnightNoise] = []
//        for _ in 1...numSounds {
//            let soundValue: UInt8 = reader.readNext()
//
//            if let noise = NoiseMap(rawValue: soundValue) {
//                noises.append(KnightNoise(noise: noise, delayMS: 0))
//            }
//        }
        self.volume = 10
        self.noises = noises
        
        super.init(characteristicId: characteristicId)
    }
    
    override func UpdateValue(data: Data) {
        let reader = DataReader(data)
        
        let numSounds = data.count / 4
        for _ in 1...numSounds {
            let soundValue32: UInt32 = reader.readNext()
            let soundValue: UInt8 = UInt8(soundValue32)
    
            if let noise = NoiseMap(rawValue: soundValue) {
                if noises.first(where: {$0.noise.rawValue == noise.rawValue}) == nil {
                    self.noises.append(KnightNoise(noise: noise, delayMS: 0))
                }
            }
        }
    }
    
    override func GetData() -> Data {
        return Data()
    }
}
