//
//  ModelData.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import Foundation
import CoreBluetooth
import Combine

final class ModelData : ObservableObject {
    @Published var knights: [Knight] = []

    private lazy var _cancellables: Set<AnyCancellable> = .init()
    
    init() {
        InitializeHandlers()
    }
    
    init(knights: [Knight]) {
        self.knights = knights
    }
}

extension ModelData {
    
    func InitializeHandlers() {
        bleManager.peripheralConnectedSubject
            .filter { [weak self] peripheral in
                self?.knights.contains { knight in
                    knight.peripheralId == peripheral.identifier
                } == false
            }
            .sink { [weak self] peripheral in
                let knight = Knight(name: peripheral.name ?? "Unknown Knight", peripheralId: peripheral.identifier)
                self?.knights.append(knight)
            }
            .store(in: &_cancellables)
        
        bleManager.peripheralUpdatedSubject
            .filter { [weak self] peripheral in
                self?.knights.contains { knight in
                    knight.peripheralId == peripheral.identifier
                } == true
            }
            .sink { [weak self] peripheral in
                guard let knightIndex = self?.knights.firstIndex(where: { knight in
                    return knight.peripheralId == peripheral.identifier
                }) else {
                    return
                }
                
                self?.knights[knightIndex].name = peripheral.name ?? "Unknown Knight"
            }
            .store(in: &_cancellables)
        
        bleManager.peripheralDisconnectedSubject
            .filter { [weak self] peripheral in
                self?.knights.contains { knight in
                    knight.peripheralId == peripheral.identifier
                } == true
            }
            .sink { [weak self] peripheral in
                guard let knightIndex = self?.knights.firstIndex(where: { knight in
                    return knight.peripheralId == peripheral.identifier
                }) else {
                    return
                }
                
                self?.knights.remove(at: knightIndex)
            }
            .store(in: &_cancellables)
        
        bleManager.characteristicDiscoveredSubject
            .filter { [weak self] (peripheral, characteristic) in
                guard let knightIndex = self?.knights.firstIndex(where: { knight in
                    return knight.peripheralId == peripheral.identifier
                }) else {
                    return false
                }
                
                return self?.knights[knightIndex].abilities.contains { ability in
                    ability.characteristicId == characteristic.uuid
                } == false
            }
            .sink { [weak self] (peripheral, characteristic) in
                guard let knightIndex = self?.knights.firstIndex(where: { knight in
                    return knight.peripheralId == peripheral.identifier
                }) else {
                    return
                }
                
                let ability = KnightAbility(characteristicId: characteristic.uuid, value: characteristic.value ?? Data([]))
                self?.knights[knightIndex].objectWillChange.send()
                self?.knights[knightIndex].abilities.append(ability)
            }
            .store(in: &_cancellables)
        
        bleManager.characteristicValueUpdatedSubject
            .filter { [weak self] (peripheral, characteristic) in
                guard let knightIndex = self?.knights.firstIndex(where: { knight in
                    return knight.peripheralId == peripheral.identifier
                }) else {
                    return false
                }
                
                return self?.knights[knightIndex].abilities.contains { ability in
                    ability.characteristicId == characteristic.uuid
                } == true
            }
            .sink { [weak self] (peripheral, characteristic) in
                guard let knightIndex = self?.knights.firstIndex(where: { knight in
                    return knight.peripheralId == peripheral.identifier
                }) else {
                    return
                }
            
                guard let abilityIndex = self?.knights[knightIndex].abilities.firstIndex(where: { ability in
                    return ability.characteristicId == characteristic.uuid
                }) else {
                    return
                }
                
                self?.knights[knightIndex].abilities[abilityIndex].currentValue = characteristic.value ?? Data()
            }
            .store(in: &_cancellables)
    }
}
