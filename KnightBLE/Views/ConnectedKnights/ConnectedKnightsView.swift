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
    @State private var selected: Set<UUID> = []
    @State private var isSideBarOpened = false
    
    private var knights: [Knight] {
        modelData.knights.filter({$0.connected}).sorted { left, right in
            return left.name < right.name
        }
    }
    
    var body: some View {
        ZStack {
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
                .toolbar {
                    Button {
                            isSideBarOpened.toggle()
                    } label: {
                        Label("Toggle SideBar",
                        systemImage: "line.3.horizontal.circle.fill")
                    }
                }
                .navigationBarTitleDisplayMode(.inline)
            }
            SidebarView(isSidebarVisible: $isSideBarOpened)
        }
    }
    
}

struct KnightList_Previews: PreviewProvider {
    static var previews: some View {
        ConnectedKnightsView()
            .environmentObject(ModelData(knights: [KnightWithAllAblities()]))
    }
}
