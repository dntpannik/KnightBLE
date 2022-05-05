//
//  ContentView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/24/22.
//

import SwiftUI
import CoreBluetooth

struct ContentView: View {
    var body: some View {
        ConnectedKnightsView()
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
            .environmentObject(ModelData(knights: [KnightWithAllAblities()]))
    }
}
