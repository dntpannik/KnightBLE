//
//  ConnectedKnightRowView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import SwiftUI

struct ConnectedKnightRowView: View {
    @ObservedObject var viewModel: ConnectedKnightRowViewModel

    var body: some View {
        VStack {
            Text(viewModel.knight.name)
                .bold()
                .underline()
            ForEach(viewModel.knight.abilities, id: \.id) { ability in
                if (ability.characteristicId == BluetoothIds.eyeLedCharacteristic) {
                    
                    EyesAbilityView(viewModel: EyesAbilityViewModel(peripheralId: viewModel.knight.peripheralId, ability: ability))
                }
            }
        }
        .onAppear {
            viewModel.Start()
        }
    }
}

struct ConnectedKnightRowView_Previews: PreviewProvider {
    static var previews: some View {
        ConnectedKnightRowView(viewModel: ConnectedKnightRowViewModel(knight: Knight(name: "TestKnight", peripheralId: UUID(), abilities: [KnightAbility(characteristicId: BluetoothIds.eyeLedCharacteristic, currentValue: Data([UInt8(false.intValue)]))])))
            .previewLayout(.fixed(width: 300, height: 120))
    }
}
