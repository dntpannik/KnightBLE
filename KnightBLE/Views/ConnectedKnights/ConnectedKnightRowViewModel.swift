//
//  ConnectedKnightRowViewModel.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import SwiftUI
import Combine
import CoreBluetooth

class ConnectedKnightRowViewModel : ObservableObject {
    @Published var knight: Knight
    @State var updateToggle: Bool = false
    
    private lazy var _cancellables: Set<AnyCancellable> = .init()
    
    init (knight: Knight) {
        self.knight = knight
    }
    
    deinit {
        _cancellables.cancel()
    }
    
    func Start() {
        bleManager.characteristicDiscoveredSubject
            .filter { [weak self] characteristic in
                (self?.knight.peripheralId == characteristic.service?.peripheral?.identifier &&
                 self?.knight.abilities.contains { ability in
                    ability.characteristicId == characteristic.uuid } == false )
            }
            .sink { [weak self] characteristic in
                let ability = KnightAbility(characteristicId: characteristic.uuid, currentValue: characteristic.value ?? Data([]))
                self?.knight.abilities.append(ability)
            }
            .store(in: &_cancellables)
        
        bleManager.characteristicValueUpdatedSubject
            .filter { [weak self] characteristic in
                (self?.knight.peripheralId == characteristic.service?.peripheral?.identifier &&
                 self?.knight.abilities.contains { ability in
                    ability.characteristicId == characteristic.uuid } == true )
            }
            .sink { [weak self] characteristic in
                let index = self?.knight.abilities.firstIndex { ability in
                    ability.characteristicId == characteristic.uuid }
                
                guard let i = index else {
                    return
                }
                
                guard let data = characteristic.value else {
                    return
                }
                
                self?.knight.abilities[i].currentValue = data
            }
            .store(in: &_cancellables)
    }
}
