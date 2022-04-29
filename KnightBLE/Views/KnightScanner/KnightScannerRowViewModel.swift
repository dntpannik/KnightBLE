//
//  KnightScannerRowViewModel.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import SwiftUI
import Combine
import CoreBluetooth

class KnightScannerRowViewModel : ObservableObject {
    @Published var connected: Bool = false
    @Published var peripheral : CBPeripheral? = nil
    private lazy var _cancellables: Set<AnyCancellable> = .init()
    
    init() {
    }
    
    deinit {
        _cancellables.cancel()
    }
    
    func Start(peripheral: CBPeripheral?) {
        self.peripheral = peripheral
        if let per = peripheral {
            self.connected = bleManager.IsConnected(peripheral: per)
        }
        
        bleManager.peripheralConnectedSubject
            .filter { [weak self] in $0.identifier ==  self?.peripheral?.identifier }
            .sink {_ in self.connected = true}
            .store(in: &_cancellables)
        
        bleManager.peripheralDisconnectedSubject
            .filter { [weak self] in $0.identifier ==  self?.peripheral?.identifier }
            .sink {_ in self.connected = false}
            .store(in: &_cancellables)
    }
    
    func Connect() {
        guard let per = peripheral else {
            return
        }
        bleManager.Connect(per)
    }
    
    func Disconnect() {
        guard let per = peripheral else {
            return
        }
        bleManager.Disconnect(per)
    }
    
}
