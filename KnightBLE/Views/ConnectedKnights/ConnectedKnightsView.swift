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
            ScrollView {
                VStack {
                    if (modelData.knights.values.contains { (value) -> Bool in
                        let knight = value as Knight
                        return knight.type == KnightType.Armiger
                    }) {
                        Text("Armiger")
                            .font(.title2)
                        LazyVGrid(columns: twoColumnGrid, alignment: .center) {
                            ForEach(Array(modelData.knights.keys).filter {modelData.knights[$0]?.connected == true && modelData.knights[$0]?.type == KnightType.Armiger}, id: \.self) { key in
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
                    
                    if (modelData.knights.values.contains { (value) -> Bool in
                        let knight = value as Knight
                        return knight.type == KnightType.Questoris
                    }) {
                        Text("Questoris")
                            .font(.title2)
                        
                        LazyVGrid(columns: twoColumnGrid, alignment: .center) {
                            ForEach(Array(modelData.knights.keys).filter {modelData.knights[$0]?.connected == true && modelData.knights[$0]?.type == KnightType.Questoris}, id: \.self) { key in
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
                    
                    if (modelData.knights.values.contains { (value) -> Bool in
                        let knight = value as Knight
                        return knight.type == KnightType.Dominus
                    }) {
                        Text("Dominus")
                            .font(.title2)
                        LazyVGrid(columns: twoColumnGrid, alignment: .center) {
                            ForEach(Array(modelData.knights.keys).filter {modelData.knights[$0]?.connected == true && modelData.knights[$0]?.type == KnightType.Dominus
                            }, id: \.self) { key in
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
                    
                    if (modelData.knights.values.contains { (value) -> Bool in
                        let knight = value as Knight
                        return knight.type == KnightType.Cerastus
                    }) {
                        Text("Cerastus")
                            .font(.title2)
                        LazyVGrid(columns: twoColumnGrid, alignment: .center) {
                            ForEach(Array(modelData.knights.keys).filter {modelData.knights[$0]?.connected == true && modelData.knights[$0]?.type == KnightType.Cerastus}, id: \.self) { key in
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
                    
                    if (modelData.knights.values.contains { (value) -> Bool in
                        let knight = value as Knight
                        return knight.type == KnightType.Acastus
                    }) {
                        Text("Acastus")
                            .font(.title2)
                        LazyVGrid(columns: twoColumnGrid, alignment: .center) {
                            ForEach(Array(modelData.knights.keys).filter {modelData.knights[$0]?.connected == true && modelData.knights[$0]?.type == KnightType.Acastus}, id: \.self) { key in
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
                }
                .navigationTitle("Connected Knights")
                .navigationBarTitleDisplayMode(.inline)
            }
        }
    }
}

struct KnightList_Previews: PreviewProvider {
    static var previews: some View {
        ConnectedKnightsView(modelData: ModelData(knights: LargeKnightArray()))
    }
}
