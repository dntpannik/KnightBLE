//
//  KnightScanner.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import SwiftUI
import CoreBluetooth
import SwiftfulLoadingIndicators

struct KnightScannerView: View {
    @EnvironmentObject var modelData: ModelData

    private var knights: [Knight] {
        modelData.knights.sorted { left, right in
            return left.name < right.name
        }
    }
    
    var body: some View {
        VStack {
            Text("Discovered Knights")
                .font(.title)
            
            if (modelData.scanning) {
                LoadingIndicator(animation: .threeBalls)
            }
            
            if (modelData.state == .poweredOn) {
                List {
                    ForEach(knights, id: \.id) {
                        knight in KnightScannerRowView(knight: knight)
                    }
                }
                Spacer()
            } else {
                Text("Please enable bluetooth to search devices")
            }
        }
        .refreshable {
            bleManager.Scan()
        }
    }
}

struct KnightScannerView_Previews: PreviewProvider {
    static var previews: some View {
        KnightScannerView()
            .environmentObject(ModelData(knights: [Knight(name: "TestKnight", peripheralId: BluetoothIds.testUUID, abilities: [BoolKnightAbility(characteristicId: BluetoothIds.eyeLedCharacteristic, value: false)])]))
    }
}
