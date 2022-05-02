//
//  BoolKnightAbility.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/29/22.
//

import Foundation
import CoreBluetooth

enum NoiseMap : UInt8 {
    case Horn = 0
}

class SoundKnightAbility: KnightAbility {
    @Published var volume: UInt8
    @Published var noises: [NoiseMap]
    @Published var activeTrack: UInt8 = 0
    
    init(characteristicId: CBUUID, volume: UInt8, sounds: [UInt8]) {
        self.volume = volume
        
        var noises: [NoiseMap] = []
        for sound in sounds {
            if let noise = NoiseMap(rawValue: sound) {
                noises.append(noise)
            }
        }
        self.noises = noises

        super.init(characteristicId: characteristicId)
    }
    
    init(characteristicId: CBUUID, value: Data) {
        let reader = DataReader(value)
        self.volume = reader.readNext()
        
        let numSounds: UInt8 = reader.readNext()
        
        var noises: [NoiseMap] = []
        for _ in 1...numSounds {
            let soundValue: UInt8 = reader.readNext()
    
            if let noise = NoiseMap(rawValue: soundValue) {
                noises.append(noise)
            }
        }
        self.noises = noises
        
        self.activeTrack = reader.readNext()
        
        super.init(characteristicId: characteristicId)
    }
    
    override func UpdateValue(data: Data) {
        let reader = DataReader(data)
        self.volume = reader.readNext()
        
        let numSounds: UInt8 = reader.readNext()
        
        var noises: [NoiseMap] = []
        for _ in 1...numSounds {
            let soundValue: UInt8 = reader.readNext()
    
            if let noise = NoiseMap(rawValue: soundValue) {
                noises.append(noise)
            }
        }
        self.noises = noises
        
        self.activeTrack = reader.readNext()
    }
    
    override func GetData() -> Data {
        return Data([self.volume, self.activeTrack])
    }
}
