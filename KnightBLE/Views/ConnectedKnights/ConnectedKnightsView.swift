//
//  KnightList.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import SwiftUI

struct ConnectedKnightsView: View {
    @EnvironmentObject var modelData: ModelData
    @State var selection: Int? = nil
    
    private var knights: [Knight] {
        modelData.knights.filter({$0.connected}).sorted { left, right in
            return left.name < right.name
        }
    }
    
    var body: some View {
        NavigationView {
            VStack {
                NavigationLink(destination: KnightScannerView(), tag: 1, selection: $selection) {
                    Button("Discover Knights") { self.selection = 1 }
                        .buttonStyle(GrowingButton())
                }
                List {
                    ForEach(knights, id: \.id) {
                        knight in ConnectedKnightRowView(peripheralId: knight.peripheralId)
                    }
                }.id(UUID())
                Spacer()
            }
            .navigationTitle("Active Knights")
        }
    }
}

struct KnightList_Previews: PreviewProvider {
    static var previews: some View {
        ConnectedKnightsView()
            .environmentObject(ModelData(knights: [KnightWithAllAblities()]))
    }
}
