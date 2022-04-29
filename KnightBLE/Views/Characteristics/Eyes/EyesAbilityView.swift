//
//  eyesCharacteristic.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import SwiftUI

struct EyesAbilityView: View {
    @ObservedObject var viewModel: EyesAbilityViewModel
    
    var body: some View {
        HStack {
            Toggle("Toggle Eye LEDs", isOn: $viewModel.enabled)
                .onChange(of: viewModel.enabled) { value in
                    viewModel.Write()
            }
        }
    }
}

struct EyesAbilityView_Previews: PreviewProvider {
    static var previews: some View {
        EyesAbilityView(viewModel: EyesAbilityViewModel(peripheralId: UUID.init(), ability: KnightAbility(characteristicId: BluetoothIds.eyeLedCharacteristic, currentValue: Data([UInt8(false.intValue)]))))
    }
}
