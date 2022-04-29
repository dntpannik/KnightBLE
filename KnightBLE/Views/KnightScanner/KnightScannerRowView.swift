//
//  KnightScannerRowView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import SwiftUI
import CoreBluetooth

struct KnightScannerRowView: View {
    @StateObject private var _viewModel: KnightScannerRowViewModel = .init()
    @State private var _didAppear = false
    private let _peripheral: CBPeripheral?

    init(peripheral: CBPeripheral?) {
        _peripheral = peripheral
    }
    
    var body: some View {
            HStack {
                if let peripheralName = _viewModel.peripheral?.name {
                    Text(peripheralName)
                } else {
                    Text("Unknown")
                        .opacity(0.2)
                }
                Spacer()
                Image(  systemName: _viewModel.connected ? "link.icloud.fill" : "link.icloud")
                    .resizable()
                    .scaledToFit()
                    .frame(width: 40, height: 40)
                    .foregroundColor(_viewModel.connected ? .blue : .gray)
            
            }
            .swipeActions {
                if _viewModel.connected {
                    Button("Disconnect") {
                        _viewModel.Disconnect()
                    }
                    .tint(.red)
                } else {
                    Button("Connect") {
                        _viewModel.Connect()
                    }
                    .tint(.blue)
                }
            }
            .onAppear {
                _viewModel.Start(peripheral: _peripheral)
            }
    
    }
}

struct KnightScannerRowView_Previews: PreviewProvider {
    static var previews: some View {
        KnightScannerRowView(peripheral: nil)
    }
}
