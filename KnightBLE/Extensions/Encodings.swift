//
//  Encodings.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/28/22.
//

import Foundation
import SwiftUI

func ParseInt(value: Data) -> Int {
    return value.int
}

func EncodeInt(value: Int) -> Data {
    return value.data
}

func ParseUInt16(value: Data) -> UInt16 {
    if (value.count != 4) {
        return 0
    }
        
    return value.uint16
}

func EncodeUInt16(value: UInt16) -> Data {
    return value.data
}

func ParseString(value: Data) -> String {
    guard let string = (String(data: value, encoding: .utf8)) else {
        return "No string"
    }
    
    return string
}

func EncodeString(string: String) -> Data {
    guard let stringData = string.data(using: .ascii) else {
        return Data()
    }
    
    return stringData
}

func ParseBool(value: Data) -> Bool {
    return (String(data: value, encoding: .utf8).flatMap(Bool.init) != nil)
}

func EncodeBool(value: Bool) -> Data {
    return Data([UInt8(value.intValue)])
}

func ParseColor(value: Data) -> Color {
    guard let hexString = (String(data: value, encoding: .utf8)) else {
        return Color.white
    }
    
    return Color(hex: hexString)
}

func EncodeColor(value: Color) -> Data {
    guard let hexString = value.toHex() else {
        return Data()
    }
    
    guard let hexData = hexString.data(using: .ascii) else {
        return Data()
    }
    
    return hexData
}

