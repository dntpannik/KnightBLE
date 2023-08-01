//
//  KnightImageView.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/19/23.
//

import SwiftUI

struct KnightImageView: View {
    @ObservedObject var knight: Knight
    
    init(knight: Knight) {
        self.knight = knight
    }
    
    var body: some View {
        VStack {
            Text(knight.name)
                .bold()
                .underline()
                .font(.system(size: 15))
            Image(knight.name.replacingOccurrences(of: " ", with: ""))
                .resizable()
                .scaledToFit()
                .frame(width: 150, height: 150)
        }
    }
}

struct KnightImageView_Previews: PreviewProvider {
    static var previews: some View {
        KnightImageView(knight: KnightWithAllAblities())
    }
}
