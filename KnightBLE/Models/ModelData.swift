//
//  ModelData.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import Foundation
import CoreBluetooth

final class ModelData : ObservableObject {
    @Published var connectedKnights: [Knight] = []
}
