//
//  Encodings.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/28/22.
//

import Foundation

func ParseBool(value: Data) -> Bool {
    return (String(data: value, encoding: .utf8).flatMap(Bool.init) != nil)
}

func EncodeBool(value: Bool) -> Data {
    return Data([UInt8(value.intValue)])
}

