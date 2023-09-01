//
//  BulkActionsView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 8/21/23.
//

import SwiftUI

struct BulkActionsView: View {
    @ObservedObject var modelData: ModelData
    
    var body: some View {
        Button(action: {
            modelData.SoundOffBulkAction()
            }) {
                Text("Sound Off")
            }
            .padding()
            .background(.blue)
            .foregroundColor(.white)
            .cornerRadius(22)
            .buttonStyle(BorderlessButtonStyle())
    }
}

#Preview {
    BulkActionsView(modelData: ModelData(knights: KnightArray()))
}
