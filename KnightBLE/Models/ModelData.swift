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
        
        //Uncomment to give default data while debugging
        //knights = [KnightWithAllAblities(), KnightWithAudioSmokeEyeAblities()]
    }
    
    init(knights: [UUID: Knight]) {
        self.knights = knights
    }
}

extension ModelData {

    func InitializeHandlers() {
        bleManager.stateSubject
            .sink { [weak self] state in
                self?.state = state
            }
            .store(in: &_cancellables)
        
        bleManager.scanningSubject
            .sink { [weak self] scanning in
                self?.scanning = scanning
            }
            .store(in: &_cancellables)
        
        bleManager.peripheralDiscoveredSubject
            .filter { [weak self] (name, peripheral) in
                self?.knights.keys.contains(peripheral.identifier) == false
            }
            .sink { [weak self] (name, peripheral) in
                let knight = Knight(name: name, peripheralId: peripheral.identifier)
                self?.knights[knight.peripheralId] = knight
                print("Created Knight for peripheralId: \(peripheral.identifier)")
            }
            .store(in: &_cancellables)
        
        bleManager.peripheralConnectedSubject
            .filter { [weak self] peripheral in
                self?.knights.keys.contains(peripheral.identifier) == true
            }
            .sink { [weak self] peripheral in
                guard let knight = self?.knights[peripheral.identifier] else {
                    print("PeripheralConnectedSink: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
                    return
                }
                
                knight.connected = true
            }
            .store(in: &_cancellables)
        
        bleManager.peripheralUpdatedSubject
            .filter { [weak self] (name, peripheral) in
                self?.knights.keys.contains(peripheral.identifier) == true
            }
            .sink { [weak self] (name, peripheral) in
                guard let knight = self?.knights[peripheral.identifier] else {
                    print("PeripheralUpdatedSink: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
                    return
                }
                
                knight.name = name
            }
            .store(in: &_cancellables)
        
        bleManager.peripheralDisconnectedSubject
            .filter { [weak self] peripheral in
                self?.knights.keys.contains(peripheral.identifier) == true
            }
            .sink { [weak self] peripheral in
                guard let knight = self?.knights[peripheral.identifier] else {
                    print("PeripheralDisconnectedSink: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
                    return
                }
                
                knight.connected = false
            }
            .store(in: &_cancellables)
        
        bleManager.serviceDiscoveredSubject
            .filter { [weak self] (peripheral, service) in
                guard let knight = self?.knights[peripheral.identifier] else {
                    print("ServiceDiscoveredFilter: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
                    return false
                }
                
                return knight.abilities.values.contains { ability in
                    ability.serviceId == service.uuid
                                } == false
            }
            .sink { [weak self] (peripheral, service) in
                guard let knight = self?.knights[peripheral.identifier] else {
                    print("ServiceDiscoveredSink: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
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
            .store(in: &_cancellables)
        
        bleManager.characteristicDiscoveredSubject
            .filter { [weak self] (peripheral, characteristic) in
                guard let knight = self?.knights[peripheral.identifier] else {
                    print("CharacteristicDiscoveredFilter: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
                    return false
                }
              
                guard let service = characteristic.service else {
                    print("CharacteristicDiscoveredFilter: No service attached to characteristic with id \(characteristic.uuid)")
                    return false;
                }
                
                guard let ability = knight.abilities[service.uuid] else {
                    print("CharacteristicDiscoveredFilter: Could not find service corresponding to serviceId \(service.uuid)")
                    return false
                }
                
               return ability.settings.keys.contains(characteristic.uuid) == false
            }
            .sink { [weak self] (peripheral, characteristic) in
                guard let knight = self?.knights[peripheral.identifier] else {
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
            .store(in: &_cancellables)
        
        bleManager.characteristicValueUpdatedSubject
            .filter { [weak self] (peripheral, characteristic) in
                guard let knight = self?.knights[peripheral.identifier] else {
                    print("CharacteristicValueUpdatedFilter: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
                    return false
                }
                
                guard let service = characteristic.service else {
                    print("CharacteristicValueUpdatedFilter: No service attached to characteristic with id \(characteristic.uuid)")
                    return false;
                }
                
                guard let ability = knight.abilities[service.uuid] else {
                    print("CharacteristicValueUpdatedFilter: Could not find ability corresponding to service \(service.uuid)")
                    return false
                }
                
                //If it's the name characteristic then return true. otherwise check that the setting exists
                if (characteristic.uuid == BluetoothIds.nameCharacteristic) {
                    return true
                }
                
                return ability.settings.keys.contains(characteristic.uuid) == true
            }
            .sink { [weak self] (peripheral, characteristic) in
                guard let knight = self?.knights[peripheral.identifier] else {
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
                    return
                }
            
                guard let setting = ability.settings[characteristic.uuid] else {
                    print("CharacteristicValueUpdatedSink: Could not find setting corresponding to characteristic \(characteristic.uuid)")
                    return
                }
                
                setting.UpdateValue(data: data)
            }
            .store(in: &_cancellables)
        
        bleManager.descriptorValueUpdatedSubject
            .filter { [weak self] (peripheral, descriptor) in
                guard let knight = self?.knights[peripheral.identifier] else {
                    print("DescriptorValueUpdatedFilter: Could not find knight corresponding to peripherialId \(peripheral.identifier)")
                    return false
                }
                
                guard let characteristic = descriptor.characteristic else {
                    print("DescriptorValueUpdatedFilter: No characteristic attched to descriptor with id \(descriptor.uuid)")
                    return false
                }
          
                guard let service = characteristic.service else {
                    print("DescriptorValueUpdatedFilter: No service attached to characteristic with id \(characteristic.uuid)")
                    return false;
                }
                
                guard let ability = knight.abilities[service.uuid] else {
                    print("DescriptorValueUpdatedFilter: Could not find ability corresponding to service \(service.uuid)")
                    return false
                }
                
                return ability.settings.keys.contains(characteristic.uuid) == true
            }
            .sink { [weak self] (peripheral, descriptor) in
                guard let knight = self?.knights[peripheral.identifier] else {
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
                
                guard let setting = ability.settings[characteristic.uuid] else {
                    print("DescriptorValueUpdatedSink: Could not find setting corresponding to characteristic \(characteristic.uuid)")
                    return
                }
                
            
                setting.ProcessDescriptor(descriptorId: descriptor.uuid, data: data)
            }
            .store(in: &_cancellables)
    }
}
