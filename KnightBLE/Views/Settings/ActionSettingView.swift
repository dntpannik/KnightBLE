//
//  ActionSettingView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/25/23.
//

import SwiftUI

struct ActionSettingsView: View {
    @EnvironmentObject var modelData: ModelData
    @State var knight: Knight
    @State var ability: KnightAbility
    @State var setting: ActionSetting

    private let threeColumnGrid = [
        GridItem(.flexible(minimum: 40)),
        GridItem(.flexible(minimum: 40)),
        GridItem(.flexible(minimum: 40))
    ]
    
    var body: some View {
        LazyVGrid(columns: threeColumnGrid, alignment: .center) {
            ForEach($setting.actionNames, id: \.id) { action in
                Button(action: {
                      print("Round Action")
                    }) {
                      Text( action)
                    }
                    .padding()
                    .background(.green)
                    .foregroundColor(.white)
                    .cornerRadius(22)
                
                
//                GeometryReader { gr in
//                    NavigationLink(destination: KnightDetailsView(peripheralId: knight.peripheralId), label: {
//                        VStack {
//                            Text(knight.name)
//                                .bold()
//                                .underline()
//                            Image(knight.name.replacingOccurrences(of: " ", with: ""))
//                                .renderingMode(.original)
//                                .resizable()
//                                .scaledToFill()
//                                .frame(height: gr.size.width)
//                        }
//                    })
//                }
//                .clipped()
//                .aspectRatio(1, contentMode: .fit)
            }
        }
        .onAppear {
        }
    }
}

#Preview {
    ActionSettingView()
}
