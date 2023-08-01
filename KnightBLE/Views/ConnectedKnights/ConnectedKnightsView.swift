//
//  KnightList.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import SwiftUI

struct ConnectedKnightsView: View {
    @ObservedObject var modelData: ModelData
    
    private let twoColumnGrid = [
        GridItem(.flexible(minimum: 40)),
        GridItem(.flexible(minimum: 40))
    ]
    
    var body: some View {
        
        NavigationView {
            VStack {
                LazyVGrid(columns: twoColumnGrid, alignment: .center) {
                    ForEach(Array(modelData.knights.keys).filter {modelData.knights[$0]?.connected == true}, id: \.self) { key in
                        GeometryReader { gr in
                            NavigationLink(destination: KnightDetailsView(knight: modelData.knights[key] ?? UnknownKnight()), label: {
                                VStack {
                                    Text(modelData.knights[key]?.name ?? "Unknown")
                                        .bold()
                                        .underline()
                                    Image(modelData.knights[key]?.name.replacingOccurrences(of: " ", with: "") ?? "Unknown")
                                        .renderingMode(.original)
                                        .resizable()
                                        .scaledToFill()
                                        .frame(height: gr.size.width)
                                }
                            })
                        }
                        .clipped()
                        .aspectRatio(1, contentMode: .fit)
                    }
                }
                Spacer()
            }
            .navigationTitle("Connected Knights")
            .navigationBarTitleDisplayMode(.inline)
        }
    }
}

struct KnightList_Previews: PreviewProvider {
    static var previews: some View {
        ConnectedKnightsView(modelData: ModelData(knights: KnightArray()))
    }
}
