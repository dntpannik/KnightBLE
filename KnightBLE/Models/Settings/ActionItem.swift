//
//  ActionItem.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/25/23.
//

import Foundation

class ActionItem: ObservableObject, Identifiable {
    let id = UUID()
    @Published var name: String
    @Published var index: UInt16
    
    init(name: String, index: UInt16) {
        self.name = name
        self.index = index
    }
}
