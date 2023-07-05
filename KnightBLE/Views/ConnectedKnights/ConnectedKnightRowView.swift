//
//  ConnectedKnightRowView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import SwiftUI

struct ConnectedKnightRowView: View {
    @EnvironmentObject var modelData: ModelData
    var peripheralId: UUID
    
    private var knight: Knight {
        return modelData.knights.first(where: {$0.peripheralId == peripheralId}) ?? Knight(name: "Uknown", peripheralId: UUID())
    }

    var body: some View {
        VStack {
            HStack {
                Text(knight.name)
                    .bold()
                    .underline()
                Spacer()
            }
            .contentShape(Rectangle())
            .padding()
            .font(.title)
            
            ForEach(knight.abilities, id: \.id) { ability in
                AbilityView(ability, knight)
            }.id(UUID())
        }
    }
}

struct ConnectedKnightRowView_Previews: PreviewProvider {
    static var previews: some View {
        ConnectedKnightRowView(peripheralId: BluetoothIds.testUUID)
            .environmentObject(ModelData(knights: [KnightWithAllAblities()]))
    }
}
