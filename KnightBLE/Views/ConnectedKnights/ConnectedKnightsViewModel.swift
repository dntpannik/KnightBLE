//
//  ConnectedKnightsViewModel.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import SwiftUI
import Combine
import CoreBluetooth

class ConnectedKnightsViewModel : ObservableObject {
    @Published var knights: [Knight] = []
    
    private lazy var _cancellables: Set<AnyCancellable> = .init()
    
    deinit {
        _cancellables.cancel()
    }
        
    func Start() {
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
                let index = self?.knights.firstIndex { knight in
                    return knight.peripheralId == peripheral.identifier
                }
                
                guard let i = index else {
                    return
                }
                self?.knights[i].name = peripheral.name ?? "Unknown Knight"
            }
            .store(in: &_cancellables)
        
        bleManager.peripheralDisconnectedSubject
            .filter { [weak self] peripheral in
                self?.knights.contains { knight in
                    knight.peripheralId == peripheral.identifier
                } == true
            }
            .sink { [weak self] peripheral in
                let index = self?.knights.firstIndex { knight in
                    return knight.peripheralId == peripheral.identifier
                }
                
                guard let i = index else {
                    return
                }
                self?.knights.remove(at: i)
                
            }
            .store(in: &_cancellables)
    }
}
