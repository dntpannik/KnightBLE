//
//  BoolExtensions.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import Foundation

extension Bool {
    var intValue: Int {
        return self ? 1 : 0
    }
}

extension Int {
    var boolValue: Bool {
        return self != 0
    }
}
