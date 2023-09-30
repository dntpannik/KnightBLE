//
//  ModelData.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import Foundation
import CoreBluetooth
import Combine

final class ModelData : ObservableObject {
    @Published var knights: [UUID: Knight] = [:]
    @Published var scanning: Bool = false
    @Published var state: CBManagerState = .unknown
    
    private lazy var _cancellables: Set<AnyCancellable> = .init()
    
    init() {
        bleManager.Start()
        InitializeHandlers()
    }
    
    init(knights: [UUID: Knight]) {
        self.knights = knights
    }
}

extension ModelData {

    func SoundOffBulkAction() {
        //Questoris: (0.8s to 4s)
        //Armiger: (0.8s to 4s)
      
        //---   Cerastus   ---//
        let cerastusKnightKeys = knights.keys.filter { knights[$0]?.type == KnightType.Cerastus && knights[$0]?.connected == true }
        
        var cumulativeDelay = 0
        for key in cerastusKnightKeys {
            DispatchQueue.main.asyncAfter(deadline: .now() + .seconds(cumulativeDelay)) { // Change `2.0` to the desired number of seconds.
                guard let knight = self.knights[key] else {
                    return
                }
                
                for ability in knight.abilities.values {
                    if ability.settings.keys.contains(BluetoothIds.actionCharacteristic) {
                        bleManager.WriteValue(
                            peripheralId: knight.peripheralId,
                            serviceId: ability.serviceId,
                            characteristicId: BluetoothIds.actionCharacteristic,
                            withValue: EncodeUInt16(value: 0))
                        break
                    }
                }
            
            }
            
            cumulativeDelay += Int.random(in: 1...2)
        }
        
        //---   Questoris   ---//
        let questorisKnightKeys = knights.keys.filter { knights[$0]?.type == KnightType.Questoris && knights[$0]?.connected == true }
        
        cumulativeDelay = 0
        for key in questorisKnightKeys {
            DispatchQueue.main.asyncAfter(deadline: .now() + .seconds(cumulativeDelay)) { // Change `2.0` to the desired number of seconds.
                guard let knight = self.knights[key] else {
                    return
                }
                
                for ability in knight.abilities.values {
                    if ability.settings.keys.contains(BluetoothIds.actionCharacteristic) {
                        bleManager.WriteValue(
                            peripheralId: knight.peripheralId,
                            serviceId: ability.serviceId,
                            characteristicId: BluetoothIds.actionCharacteristic,
                            withValue: EncodeUInt16(value: 0))
                        break
                    }
                }
            
            }
            
            cumulativeDelay += Int.random(in: 1...2)
        }
        
        if cumulativeDelay < 3 {
            cumulativeDelay = 3
        }

        //---   Armigers   ---//
        let armigerKnightKeys = knights.keys.filter { knights[$0]?.type == KnightType.Armiger && knights[$0]?.connected == true  }
        for key in armigerKnightKeys {
            DispatchQueue.main.asyncAfter(deadline: .now() + .seconds(cumulativeDelay)) { // Change `2.0` to the desired number of seconds.
                guard let knight = self.knights[key] else {
                    return
                }
                
                for ability in knight.abilities.values {
                    if ability.settings.keys.contains(BluetoothIds.actionCharacteristic) {
                        bleManager.WriteValue(
                            peripheralId: knight.peripheralId,
                            serviceId: ability.serviceId,
                            characteristicId: BluetoothIds.actionCharacteristic,
                            withValue: EncodeUInt16(value: 0))
                        break
                    }
                }
            }
            
            cumulativeDelay += Int.random(in: 1...2)
        }
    }
    
    func StateUpdated(state: CBManagerState) {
        self.state = state
    }
    
    func ScanningUpdated(scanning: Bool) {
        self.scanning = scanning
    }
    
    func PeripheralDiscovered(name: String, peripheral: CBPeripheral) {
        let knight = Knight(name: name, peripheralId: peripheral.identifier)
        self.knights[knight.peripheralId] = knight
        self.objectWillChange.send()
        print("Created Knight for peripheralId: \(peripheral.identifier)")
    }
    
    func PeripheralConnected(peripheral: CBPeripheral) {
        guard let knight = self.knights[peripheral.identifier] else {
            print("PeripheralConnected: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
            return
        }
        
        knight.connected = true
        self.objectWillChange.send()
    }
    
    func PeripheralDisconnected(peripheral: CBPeripheral) {
        guard let knight = self.knights[peripheral.identifier] else {
            print("PeripheralDisconnectedSink: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
            return
        }
        
        knight.connected = false
        self.knights.removeValue(forKey: peripheral.identifier)
        
        self.objectWillChange.send()
    }
    
    func PeripheralUpdated(name: String, peripheral: CBPeripheral) {
        guard let knight = self.knights[peripheral.identifier] else {
            print("PeripheralUpdated: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
            return
        }
        
        knight.name = name
        knight.setKnightType()
        self.objectWillChange.send()
    }
    
    func ServiceDiscovered(peripheral: CBPeripheral, service: CBService) {
        guard let knight = self.knights[peripheral.identifier] else {
            print("ServiceDiscovered: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
            return
        }
        
        //If this is the models service then skip. That service is just used to identify a Knight bluetooth
        if (service.uuid == BluetoothIds.modelService) {
            print("Skipping ModelsService for peripheralId: \(peripheral.identifier)")
            return
        }
        
        let ability = KnightAbility(serviceId: service.uuid)
        knight.abilities[service.uuid] = ability
    }
    
    func CharacteristicDiscovered(peripheral: CBPeripheral, characteristic: CBCharacteristic) {
        guard let knight = self.knights[peripheral.identifier] else {
            print("CharacteristicDiscoveredSink: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
            return
        }
        
        guard let service = characteristic.service else {
            print("CharacteristicDiscoveredSink: No service attached to characteristic with id \(characteristic.uuid)")
            return;
        }
        
        guard let ability = knight.abilities[service.uuid] else {
            print("CharacteristicDiscoveredSink: Could not find service corresponding to serviceId \(service.uuid)")
            return
        }
        
        let data = characteristic.value ?? Data()

        //---   Parse Knight Ability   ---//
        if (characteristic.uuid != BluetoothIds.nameCharacteristic) { //Ignore if it's the name characteristic
            guard let setting = Setting(characteristic, data) else {
                return
            }
            
            ability.settings[characteristic.uuid] = setting
            
        } else {
            ability.name = ParseString(value: data)
        }
    }
    
    func CharacteristicValueUpdated(peripheral: CBPeripheral, characteristic: CBCharacteristic) {
        
        guard let knight = self.knights[peripheral.identifier] else {
            print("CharacteristicValueUpdatedSink: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
            return
        }

        guard let service = characteristic.service else {
            print("CharacteristicValueUpdatedSink: No service attached to characteristic with id \(characteristic.uuid)")
            return;
        }
        
        guard let ability = knight.abilities[service.uuid] else {
            print("CharacteristicValueUpdatedSink: Could not find service corresponding to serviceId \(service.uuid)")
            return
        }
        
        //Get the data
        guard let data = characteristic.value else {
            return
        }
        
        //If it's the name ability then set the name. otherwise update the value
        if (characteristic.uuid == BluetoothIds.nameCharacteristic) {
            ability.name = ParseString(value: data)
            knight.objectWillChange.send()
            return
        }
    
        guard let setting = ability.settings[characteristic.uuid] else {
            print("CharacteristicValueUpdatedSink: Could not find setting corresponding to characteristic \(characteristic.uuid)")
            return
        }
        
        setting.UpdateValue(data: data)
    }
    
    func DescriptorValueUpdated(peripheral: CBPeripheral, descriptor: CBDescriptor) {
        
        guard let knight = self.knights[peripheral.identifier] else {
            print("DescriptorValueUpdatedSink: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
            return
        }
        
        guard let characteristic = descriptor.characteristic else {
            print("DescriptorValueUpdatedSink: No characteristic attched to descriptor with id \(descriptor.uuid)")
            return
        }
        
        guard let service = characteristic.service else {
            print("DescriptorValueUpdatedSink: No service attached to characteristic with id \(characteristic.uuid)")
            return;
        }
        
        guard let ability = knight.abilities[service.uuid] else {
            print("DescriptorValueUpdatedSink: Could not find service corresponding to serviceId \(service.uuid)")
            return
        }
        
        //Get the data
        guard let anyData = descriptor.value else {
            return
        }
        let nsData = anyData as? NSData ?? NSData()
        let data = nsData as Data
        
        //If this is the order descriptor on the name characteristic then set the order for the ability. Otherwise process
        if (characteristic.uuid == BluetoothIds.nameCharacteristic && descriptor.uuid == BluetoothIds.orderDescriptor) {
            ability.order = data.uint16
            knight.objectWillChange.send()
            
        } else {
            guard let setting = ability.settings[characteristic.uuid] else {
                print("DescriptorValueUpdatedSink: Could not find setting corresponding to characteristic \(characteristic.uuid)")
                return
            }
            
            setting.ProcessDescriptor(descriptorId: descriptor.uuid, data: data)
        }
    
        
    }
    
    
    func InitializeHandlers() {
        bleManager.stateCallback = StateUpdated
        bleManager.scanningCallback = ScanningUpdated
        bleManager.peripheralConnectedCallback = PeripheralConnected
        bleManager.peripheralDiscoveredCallback = PeripheralDiscovered
        bleManager.peripheralUpdatedCallback = PeripheralUpdated
        bleManager.peripheralDisconnectedCallback = PeripheralDisconnected
        bleManager.serviceDiscoveredCallback = ServiceDiscovered
        bleManager.characteristicDiscoveredCallback = CharacteristicDiscovered
        bleManager.characteristicValueUpdatedCallback = CharacteristicValueUpdated
        bleManager.descriptorValueUpdatedCallback = DescriptorValueUpdated
    }
}
