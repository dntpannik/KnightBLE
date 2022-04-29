//
//  KnightList.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import SwiftUI

struct ConnectedKnightsView: View {
    @StateObject private var _viewModel: ConnectedKnightsViewModel = .init()
    @State var selection: Int? = nil

    private var knights: [Knight] {
        _viewModel.knights.sorted { left, right in
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
                    Spacer()
                    VStack {
                    List {
                        ForEach(_viewModel.knights, id: \.id) {
                            knight in ConnectedKnightRowView(viewModel: ConnectedKnightRowViewModel(knight: knight))
                        }
                    }
                    }
                }
                .navigationTitle("Active Knights")
            }
        .onAppear {
            _viewModel.Start()
        }
    }
}

struct KnightList_Previews: PreviewProvider {
    static var previews: some View {
        ConnectedKnightsView()
    }
}
