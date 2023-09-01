//
//  KnightDetailsView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/20/23.
//

import SwiftUI
import CoreBluetooth

struct KnightDetailsView: View {
    @ObservedObject var knight: Knight
    @Environment(\.presentationMode) var presentationMode

    init(knight: Knight) {
        self.knight = knight
    }
    
    var body: some View {
        VStack {
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
                    .frame(width: 250, height: 250)
                    .padding([.leading, .trailing], 20)
                
                List {
                    ForEach(Array(knight.abilities.keys).sorted { knight.abilities[$0]?.order ?? 0 < knight.abilities[$1]?.order ?? 0 }, id: \.self) { key in
                        Section(header: Text(knight.abilities[key]?.name ?? "Unknown")
                                .font(.subheadline)
                                .foregroundColor(.blue)) {
                                    KnightAbilityView(peripheralId: knight.peripheralId, ability: knight.abilities[key] ?? KnightAbility(serviceId: CBUUID()))
                            }
                        }.id(UUID())
                            .padding([.leading, .trailing], 20)
                }
                .listStyle(.insetGrouped)
            }
        .onChange(of: knight.connected) {
            self.presentationMode.wrappedValue.dismiss()
        }
    }
}

struct KnightDetailsView_Previews: PreviewProvider {
    static var previews: some View {
        KnightDetailsView(knight: KnightWithAllAblities())
    }
}
