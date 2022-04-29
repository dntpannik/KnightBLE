//
//  LoadingView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/26/22.
//

import SwiftUI

struct LoadingView: View {
    var body: some View {
        HStack {
            DotView() // 1.
            DotView(delay: 0.2) // 2.
            DotView(delay: 0.4) // 3.
        }
    }
}

struct LoadingView_Previews: PreviewProvider {
    static var previews: some View {
        LoadingView()
    }
}
