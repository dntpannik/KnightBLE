//
//  KnightDetailsView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/20/23.
//

import SwiftUI

struct KnightDetailsView: View {
    @EnvironmentObject var modelData: ModelData
    var peripheralId: UUID
    
    private var knight: Knight {
        return modelData.knights.first(where: {$0.peripheralId == peripheralId}) ?? Knight(name: "Uknown", peripheralId: UUID())
    }
    
    var body: some View {
        VStack {
            ScrollView {
                HStack {
                    Text(knight.name)
                        .bold()
                        .underline()
                }
                .contentShape(Rectangle())
                .padding()
                .font(.title)
                
                
                Image(knight.name.replacingOccurrences(of: " ", with: ""))
                    .renderingMode(.original)
                    .resizable()
                    .scaledToFill()
                    .padding([.leading, .trailing], 20)
                
                ForEach(knight.abilities, id: \.id) { ability in
                    AbilityView(ability, knight)
                }.id(UUID())
                    .padding([.leading, .trailing], 20)
            }
        }
    }
}

struct KnightDetailsView_Previews: PreviewProvider {
    static var previews: some View {
        KnightDetailsView(peripheralId: BluetoothIds.testUUID)
            .environmentObject(ModelData(knights: [KnightWithAllAblities()]))
    }
}
