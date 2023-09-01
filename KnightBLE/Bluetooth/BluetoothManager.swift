//
//  BleManager.swift
//  KnightBLE
//
//  Created by Christopher Janover on 4/25/22.
//

import CoreBluetooth
import Combine

class BluetoothManager : NSObject {
   
    private var _centralManager: CBCentralManager!
    private var _timer: Timer!
    private var _countTime: Int = 0
    private var _discoveredPeripherals: [UUID:CBPeripheral] = [:]
    private var _connectedPeripherals: [UUID] = []
    private var _scanning : Bool = false
    private var _initialzied: Bool = false
    
    var stateCallback: (CBManagerState) -> Void
    var scanningCallback: (Bool) -> Void
    var peripheralDiscoveredCallback: (String, CBPeripheral) -> Void
    var peripheralUpdatedCallback: (String, CBPeripheral) -> Void
    var peripheralConnectedCallback: (CBPeripheral) -> Void
    var peripheralDisconnectedCallback: (CBPeripheral) -> Void
    var serviceDiscoveredCallback: (CBPeripheral, CBService) -> Void
    var characteristicDiscoveredCallback: (CBPeripheral, CBCharacteristic) -> Void
    var characteristicValueUpdatedCallback: (CBPeripheral, CBCharacteristic) -> Void
    var descriptorDiscoveredCallback: (CBCharacteristic, CBDescriptor) -> Void
    var descriptorValueUpdatedCallback: (CBPeripheral, CBDescriptor) -> Void
    
    override init () {
        func stateInit(_:CBManagerState) {}
        stateCallback = stateInit
        
        func scanningInit(_:Bool) {}
        scanningCallback = scanningInit
        
        func peripheralDiscoveredInit(_:String, _:CBPeripheral) {}
        peripheralDiscoveredCallback = peripheralDiscoveredInit
        
        func peripheralUpdatedInit(_:String, _:CBPeripheral) {}
        peripheralUpdatedCallback = peripheralUpdatedInit
        
        func peripheralConnectedInit(_:CBPeripheral) {}
        peripheralConnectedCallback = peripheralConnectedInit
        
        func peripheralDisconnectedInit(_:CBPeripheral) {}
        peripheralDisconnectedCallback = peripheralDisconnectedInit
        
        func serviceDiscoveredInit(_:CBPeripheral, _:CBService) {}
        serviceDiscoveredCallback = serviceDiscoveredInit
        
        func characteristicDiscoveredInit(_:CBPeripheral, _:CBCharacteristic) {}
        characteristicDiscoveredCallback = characteristicDiscoveredInit
        
        func characteristicValueUpdatedInit(_:CBPeripheral, _:CBCharacteristic) {}
        characteristicValueUpdatedCallback = characteristicValueUpdatedInit
        
        func descriptorDiscoveredInit(_:CBCharacteristic, _:CBDescriptor) {}
        descriptorDiscoveredCallback = descriptorDiscoveredInit
        
        func descriptorValueUpdatedInit(_:CBPeripheral, _:CBDescriptor) {}
        descriptorValueUpdatedCallback = descriptorValueUpdatedInit
    
        super.init()
    }
    
    func Start() {
        if !_initialzied {
            _centralManager = .init(delegate: self, queue: .main)
        }
        
        _initialzied = true
    }
    
    func Scan() {
        if _centralManager.state != .poweredOn {
            print ("Bluetooth not powered on")
            return
        }
                
        if (_scanning) {
            print("Already scanning")
            return
        }
        
        //Start timer and scanner
        print("Scanning...")
        _scanning = true
        scanningCallback(true)
        _timer = Timer.scheduledTimer(timeInterval: 1, target: self, selector: #selector(self.scanTic), userInfo: nil, repeats: true)
        _centralManager.scanForPeripherals(withServices: BluetoothIds.acceptedServies, options: [CBCentralManagerScanOptionAllowDuplicatesKey : true])
    }
        
    func Connect(_ peripheralId: UUID) {
        guard let peripheral = _discoveredPeripherals[peripheralId] else {
            print("Unable to find peripheral with id (\(peripheralId)) when attempting to connect ")
            return
        }
        
        if _scanning {
            StopScan()
        }
        
        peripheral.delegate = self
        _centralManager.connect(peripheral)
    }
    
    func Disconnect(_ peripheralId: UUID) {
        guard let peripheral = _discoveredPeripherals[peripheralId] else {
            print("Unable to find peripheral with id (\(peripheralId)) when attempting to connect ")
            return
        }
        
        if _scanning {
            StopScan()
        }
        
        _centralManager.cancelPeripheralConnection(peripheral)
    }
    
    func DiscoveredPeripherals() -> [CBPeripheral] {
        return Array(_discoveredPeripherals.values)
    }
    
    func IsConnected(peripheral: CBPeripheral) -> Bool {
        return _connectedPeripherals.contains { $0 == peripheral.identifier}
    }
    
    func State() -> CBManagerState {
        return _initialzied ? _centralManager.state : .unknown
    }
    
    func GetServiceCharactieristics(peripheralId: UUID, serviceId: CBUUID) -> [CBUUID] {
        guard let peripheral = _discoveredPeripherals[peripheralId] else {
            print("Attempted to get characteristics for a peripheral that didnt exist")
            return []
        }
        
        guard let service = peripheral.services?.first(where: { $0.uuid == serviceId }) else {
            print("Attempted to get characteristics for a service that didnt exist")
            return []
        }
        
        var characteristicIds: [CBUUID] = []
        
        //Loop over all characteristics in the service
        if let characteristics = service.characteristics {
            for characteristic in characteristics {
                characteristicIds.append(characteristic.uuid)
            }
        }
        
        return characteristicIds;
    }
    
    func GetCharacteristics(peripheralId: UUID) -> [CBUUID] {
        guard let peripheral = _discoveredPeripherals[peripheralId] else {
            print("Attempted to write data to a peripheral whose id doesn't exist")
            return []
        }
            
        var characteristicIds: [CBUUID] = []
            
        //Loop over all services in the peripheral
        if let services = peripheral.services {
            for service in services {
                if BluetoothIds.acceptedServies.contains(where: {$0 == service.uuid}) {
                        
                    //Loop over all characteristics in the service
                    if let characteristics = service.characteristics {
                        for characteristic in characteristics {
                            characteristicIds.append(characteristic.uuid)
                        }
                    }
                }
            }
        }
            
        return characteristicIds
    }
    
    func WriteValue(peripheralId: UUID, serviceId: CBUUID, characteristicId: CBUUID , withValue value: Data) {
            guard let peripheral = _discoveredPeripherals[peripheralId] else {
                print("Attempted to write data to a peripheral whose id doesn't exist")
                return
            }
        
            guard let service = peripheral.services?.first(where: {$0.uuid == serviceId}) else {
                print("Attempted to write data to a service whose id doesn't exist")
                return
            }
            guard let characteristic = service.characteristics?.first(where: {$0.uuid == characteristicId}) else {
                print("Attempted to write data to a characteristic whose id doesn't exist")
                return
            }
            
            // Check if it has the write property
            if characteristic.properties.contains(.write) {
                peripheral.writeValue(value, for: characteristic, type: .withResponse)
               // peripheral.readValue(for: characteristic)
            } else {
                print("Attempted to write data to a characteristic who doesnt support response")
                return
            }
        }
    
    @objc func scanTic(){
        if _countTime >= 10{
            StopScan()
        } else {
            _countTime += 1
        }
    }
    
    private func StopScan() {
        _centralManager.stopScan()
        _scanning = false
        _timer.invalidate()
        _timer = nil
        _countTime = 0
        print("Scan Complete")
        scanningCallback(false)
    }
}


extension BluetoothManager: CBCentralManagerDelegate {
    
    //Fires when the state of the central manager is udpated
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        stateCallback(central.state)
    }
    
    //Fires when a peripheral is discovered
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        
        //Check if peripheral is new or updated
        let peripheralUpdated = _discoveredPeripherals[peripheral.identifier] != nil
        _discoveredPeripherals[peripheral.identifier] = peripheral
        
        var name: String = peripheral.name ?? "Unknown Device"
        if advertisementData.contains(where: {(key, value) in
            key == "kCBAdvDataLocalName"
        }) {
            name = advertisementData["kCBAdvDataLocalName"] as! String
        }

        print("Peripheral Discovered \(peripheral.identifier)")
        //Send correct message based on whether the peripheral is new or not
        if peripheralUpdated {
            peripheralUpdatedCallback(name, peripheral)
        } else {
            peripheralDiscoveredCallback(name, peripheral)
        }
    }
    
    // Fires when a peripheral was connected
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        if (_discoveredPeripherals[peripheral.identifier] != nil) {
            print("Connected to device \(peripheral.identifier)")
            _connectedPeripherals.append(peripheral.identifier)
            peripheralConnectedCallback(peripheral)
            peripheral.discoverServices(nil)
        }
    }
    
    // Fires when a peripheral was disconnected
    func centralManager(_ central: CBCentralManager,
         didDisconnectPeripheral peripheral: CBPeripheral,
                        error: Error?) {
        if (_discoveredPeripherals[peripheral.identifier] != nil) {
            print("Disconnected from device \(peripheral.identifier)")
            
            if let errorString = error {
                print("Disconnection Error: \(errorString)")
                _discoveredPeripherals.removeValue(forKey: peripheral.identifier)
            }
            
            if let index = _connectedPeripherals.firstIndex(where: { $0 == peripheral.identifier }) {
                _connectedPeripherals.remove(at: index)
            }
   
            peripheralDisconnectedCallback(peripheral)
        }
    }
}

extension BluetoothManager: CBPeripheralDelegate {
    
    // Handles discovery event
     func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
         if let services = peripheral.services {
             for service in services {
                 print("Service found \(service.uuid)")
                 serviceDiscoveredCallback(peripheral, service)
                 
                 peripheral.discoverCharacteristics(BluetoothIds.acceptedCharacteristics, for: service)
             }
         }
     }
     
     // Handling discovery of characteristics
     func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
         if let characteristics = service.characteristics {
             for characteristic in characteristics {
                 if BluetoothIds.acceptedCharacteristics.contains(where: {$0 == characteristic.uuid}) {
                     print("Chracteristic found \(characteristic.uuid)")
                     peripheral.readValue(for: characteristic)
                     characteristicDiscoveredCallback(peripheral, characteristic)
                     peripheral.discoverDescriptors(for: characteristic)
                 }
             }
         }
     }
    
    // Handling discovery of descriptors
    func peripheral(_ peripheral: CBPeripheral, didDiscoverDescriptorsFor characteristic: CBCharacteristic, error: Error?) {
        if let descriptors = characteristic.descriptors {
            for descriptor in descriptors {
                if BluetoothIds.acceptedDescriptors.contains(where: {$0 == descriptor.uuid}) {
                    print("Descriptor found \(descriptor.uuid)")
                    //peripheral.setNotifyValue(true, for: descriptor)
                    peripheral.readValue(for: descriptor)
                    //characteristic.readValue(for: descriptor)
                    //characteristicDiscoveredSubject.send((peripheral, characteristic))
                    //peripheral.discoverDescriptors(for: characteristic)
                }
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        if (error == nil) {
            characteristicValueUpdatedCallback(peripheral, characteristic)
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor descriptor: CBDescriptor, error: Error?) {
        if (error == nil) {
            descriptorValueUpdatedCallback(peripheral, descriptor)
        }
    }
}
