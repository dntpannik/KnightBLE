//
//  DataExtensions.swift
//  KnightBLE
//
//  Created by Christopher Janover on 5/3/22.
//

import Foundation

extension UInt16 {
    var data: Data {
        var int = self
        return Data(bytes: &int, count: MemoryLayout<UInt16>.size)
    }
}
