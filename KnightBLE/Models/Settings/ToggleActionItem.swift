//
//  ActionItem.swift
//  KnightBLE
//
//  Created by Christopher Janover on 7/25/23.
//

import Foundation

class ToggleActionItem: ObservableObject, Identifiable {
    let id = UUID()
    @Published var name: String
    @Published var value: Bool
    @Published var index: UInt16
    
    init(name: String, index: UInt16) {
        self.name = name
        self.value = false
        self.index = index
    }
}
