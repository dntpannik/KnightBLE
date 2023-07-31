//
//  ContentView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/24/22.
//

import SwiftUI
import CoreBluetooth

struct ContentView: View {
    @ObservedObject private var modelData = ModelData()

    var body: some View {
        TabView {
            ConnectedKnightsView(modelData: modelData)
                  .tabItem {
                      Label("Connected", systemImage: "point.3.filled.connected.trianglepath.dotted")
                  }.padding()
//            BulkActionsView()
//                .tabItem {
//                    Label("Bulk", systemImage: "person.3")
//                }
            KnightScannerView(modelData: modelData)
                .tabItem {
                    Label("Scanner", systemImage: "personalhotspot")
                }
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
            .environmentObject(ModelData(knights: KnightArray()))
    }
}
