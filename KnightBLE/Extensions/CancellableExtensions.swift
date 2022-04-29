//
//  CancellableExtensions.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/27/22.
//

import Combine

extension Set where Element: Cancellable {

    func cancel() {
        forEach { $0.cancel() }
    }
}
