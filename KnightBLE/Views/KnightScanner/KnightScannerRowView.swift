//
//  KnightScannerRowView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import SwiftUI
import CoreBluetooth

struct KnightScannerRowView: View {
    @EnvironmentObject var modelData: ModelData
    var knight: Knight
  
    var body: some View {
        HStack {
            Text(knight.name)
            Spacer()
            Image(systemName: knight.connected ? "link.icloud.fill" : "link.icloud")
                .resizable()
                .scaledToFit()
                .frame(width: 40, height: 40)
                .foregroundColor(knight.connected ? .blue : .gray)
        }
        .swipeActions {
            if knight.connected {
                Button("Disconnect") {
                    bleManager.Disconnect(knight.peripheralId)
                }
                .tint(.red)
            } else {
                Button("Connect") {
                    bleManager.Connect(knight.peripheralId)
                }
                .tint(.blue)
            }
        }
    }
}

struct KnightScannerRowView_Previews: PreviewProvider {
    static var previews: some View {
        KnightScannerRowView(knight: Knight(name: "TestKnight", peripheralId: BluetoothIds.testUUID, abilities: [BoolKnightAbility(characteristicId: BluetoothIds.eyeLedCharacteristic, value: false)]))
    }
}
