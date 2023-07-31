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
    @ObservedObject var modelData: ModelData

    var body: some View {
        VStack {
            Text("Discovered Knights")
                .font(.title)
            
            if (modelData.scanning) {
                LoadingIndicator(animation: .threeBalls)
            }
            
            if (modelData.state == .poweredOn) {
                List {
                    ForEach(Array(modelData.knights.keys), id: \.self) {
                        key in KnightScannerRowView(knight: modelData.knights[key] ?? UnknownKnight())
                    }
                }
                .refreshable {
                    bleManager.Scan()
                }
                Spacer()
            } else {
                Text("Please enable bluetooth to search devices")
            }
        }
    }
}

struct KnightScannerView_Previews: PreviewProvider {
    static var previews: some View {
        KnightScannerView(modelData: ModelData(knights: KnightArray()))
    }
}
