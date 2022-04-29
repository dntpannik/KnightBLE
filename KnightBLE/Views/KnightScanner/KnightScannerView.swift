//
//  KnightScanner.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import SwiftUI
import CoreBluetooth

struct KnightScannerView: View {
    @StateObject private var _viewModel: KnightScannerViewModel = .init()
    @State private var _didAppear = false
    
    private var peripherals: [CBPeripheral] {
        _viewModel.peripherals.sorted { left, right in
            guard let leftName = left.name else {
                return false
            }
            guard let rightName = right.name else {
                return true
            }
            return leftName < rightName
        }
    }
    
    var body: some View {
        VStack {
            Text("Discovered Knights")
                .font(.title)
            if (_viewModel.scanning) {
                HStack {
                    Spacer()
                    LoadingView()
                    Spacer()
                }
            }
            contentView
                .refreshable {
                    _viewModel.Scan()
                }
                .onAppear {
                    if !_didAppear {
                        _viewModel.Start()
                    }
                    _didAppear = true
                }
        }
    }
        
    @ViewBuilder
    private var contentView: some View {
        if (_viewModel.state == .poweredOn) {
            List(peripherals, id: \.identifier) { peripheral in
                KnightScannerRowView(peripheral: peripheral)
            }
        } else {
            Text("Please enable bluetooth to search devices")
        }
    }
        
}

struct KnightScannerView_Previews: PreviewProvider {
    static var previews: some View {
        KnightScannerView()
    }
}
