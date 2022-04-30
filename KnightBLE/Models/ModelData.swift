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
    @Published var scanning: Bool = false
    @Published var state: CBManagerState = .unknown
    
    private lazy var _cancellables: Set<AnyCancellable> = .init()
    
    init() {
        bleManager.Start()
        InitializeHandlers()
    }
    
    init(knights: [Knight]) {
        self.knights = knights
    }
}

extension ModelData {
    
    func InitializeHandlers() {
        bleManager.stateSubject
            .sink { [weak self] state in
                self?.state = state
            }
            .store(in: &_cancellables)
        
        bleManager.scanningSubject
            .sink { [weak self] scanning in
                self?.scanning = scanning
            }
            .store(in: &_cancellables)
        
        bleManager.peripheralDiscoveredSubject
            .filter { [weak self] (name, peripheral) in
                self?.knights.contains { knight in
                    knight.peripheralId == peripheral.identifier
                } == false
            }
            .sink { [weak self] (name, peripheral) in
                let knight = Knight(name: name, peripheralId: peripheral.identifier)
                self?.knights.append(knight)
            }
            .store(in: &_cancellables)
        
        bleManager.peripheralConnectedSubject
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
                
                self?.knights[knightIndex].connected = true
            }
            .store(in: &_cancellables)
        
        bleManager.peripheralUpdatedSubject
            .filter { [weak self] (name, peripheral) in
                self?.knights.contains { knight in
                    knight.peripheralId == peripheral.identifier
                } == true
            }
            .sink { [weak self] (name, peripheral) in
                guard let knightIndex = self?.knights.firstIndex(where: { knight in
                    return knight.peripheralId == peripheral.identifier
                }) else {
                    return
                }
                
                self?.knights[knightIndex].name = name
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
                
                self?.knights[knightIndex].connected = false
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
                
                let data = characteristic.value ?? Data()
                
                //---   Parse Knight Ability   ---//
                guard let ability = Ability(characteristic, data) else {
                    return
                }
                
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
                
                guard let data = characteristic.value else {
                    return
                }
                
                self?.knights[knightIndex].abilities[abilityIndex].UpdateValue(data: data)
            }
            .store(in: &_cancellables)
    }
}
