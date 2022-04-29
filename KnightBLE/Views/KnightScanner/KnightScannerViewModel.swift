//
//  KnightScannerViewModel.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import SwiftUI
import Combine
import CoreBluetooth

class KnightScannerViewModel : ObservableObject {
    @Published var state: CBManagerState = .unknown
    @Published var peripherals: [CBPeripheral] = []
    @Published var scanning: Bool = false
    
    private lazy var _cancellables: Set<AnyCancellable> = .init()
    
    deinit {
        _cancellables.cancel()
    }
    
    init() {
        self.peripherals = bleManager.DiscoveredPeripherals()
        self.state = bleManager.State()
    }
        
    func Start() {
        bleManager.stateSubject
            .sink { [weak self] state in
                self?.state = state
            }
            .store(in: &_cancellables)
        
        bleManager.peripheralDiscoveredSubject
            .filter { [weak self] in self?.peripherals.contains($0) == false }
            .sink { [weak self] in self?.peripherals.append($0) }
            .store(in: &_cancellables)
        
        bleManager.peripheralUpdatedSubject
            .filter { [weak self] updatedPeripheral in self?.peripherals.contains { peripheral in
                return peripheral.identifier == updatedPeripheral.identifier
            } == true}
            .sink { [weak self] updatedPeripheral in
                let index = self?.peripherals.firstIndex { peripheral in
                    return peripheral.identifier == updatedPeripheral.identifier
                }
                
                guard let i = index else {
                    return
                }
                self?.peripherals[i] = updatedPeripheral
            }
            .store(in: &_cancellables)
        
        bleManager.scanningSubject
            .sink { [weak self] scanning in
                self?.scanning = scanning
            }
            .store(in: &_cancellables)
        
        bleManager.Start()
    }
    
    func Scan() {
        bleManager.Scan()
    }
}
