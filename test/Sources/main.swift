import Foundation
import Dispatch

#if os(Linux)
    import Glibc
#endif

import Socket
//import SocketManager

let port: Int32 = 2222
let host: String = "127.0.0.1"
let family: Socket.ProtocolFamily = .inet
var keepRunning: Bool = true
private let numberOfEpollTasks = 2

public class SocketManagerTest {
    
    private let numberOfEpollTasks = 2
    
    private let queues:[DispatchQueue]
    
    var socket: Socket?
    
    var listener : Socket?
    
    public init() {
        
        do {
            listener = try Socket.create(family: family)
            try listener!.listen(on: Int(port), maxBacklogSize: 10)
            print("Listening on port: \(port)")
        } catch let error as NSError {
            print("error: \(error)")
        }
        
        var queueRef = [DispatchQueue]()
        for i in 0 ..< numberOfEpollTasks {
            queueRef += [DispatchQueue(label: "socketManager\(i)")]
        }
        queues = queueRef
        
        for i in 0 ..< numberOfEpollTasks {
            queues[i].async() { [unowned self] in self.handle() }
        }
    }
    
    public func handle() {
        
        do {
            socket = try listener!.acceptClientConnection()
        } catch let error as NSError {
            print("error: \(error)")
        }
        
        do {
            try socket!.write(from: "Hello, type 'QUIT' to end session\n")
        } catch let error as NSError {
            print("error: \(error)")
        }
        
        var bytesRead = 0
        
        
        
        print("start")
        
        repeat {
            
            var readData = Data()
            
            do {
                bytesRead = try socket!.read(into: &readData)
            } catch let error as NSError {
                print("error: \(error)")
            }
            
            if bytesRead > 0 {
                
                guard let response = NSString(data: readData, encoding: String.Encoding.utf8.rawValue) else {
                    print("Error decoding response...")
                    readData.count = 0
                    break
                }
                
                let concurrentQueue = DispatchQueue(label: "queuename", attributes: .concurrent)
                
                if response.hasPrefix("QUIT") {
                    keepRunning = false
                }
                
                let reply = "Server response: \n\(response)\n"
                do {
                    try socket!.write(from: reply)
                } catch let error as NSError {
                    print("error: \(error)")
                }
                
            }
            
            if bytesRead == 0 {
                break
            }
        } while keepRunning
        
        socket!.close()
        print("close")
    }
    
}

var socketMan = SocketManagerTest()



