import Glib
import Foundation

public class Socket: SocketReader, SocketWriter {

	// MARK: Enums
	
	// MARK: -- AddressFamily
	
	///
	/// Socket Address Family Values
	///
	/// **Note:** Only the following are supported at this time:
	///			inet = AF_INET (IPV4)
	///			inet6 = AF_INET6 (IPV6)
	///
	public enum AddressFamily {
		
		/// AF_INET (IPV4)
		case inet
		
		/// AF_INET6 (IPV6)
		case inet6
		
		///
		/// Return enum equivalent of a raw value
		///
		/// - Parameter forValue: Value for which enum value is desired
		///
		/// - Returns: Optional contain enum value or nil
		///
		static func getFamily(forValue: Int32) -> AddressFamily? {
			
			switch forValue {
				
			case Int32(AF_INET):
				return .inet
			case Int32(AF_INET6):
				return .inet6
			default:
				return nil
			}
		}
		
	}


	// MARK: -- SocketType
	
	///
	/// Socket Type Values
	///
	/// **Note:** Only the following are supported at this time:
	///			stream = SOCK_STREAM (Provides sequenced, reliable, two-way, connection-based byte streams.)
	///			datagram = SOCK_DGRAM (Supports datagrams (connectionless, unreliable messages of a fixed maximum length).)
	///
	public enum SocketType {
		
		/// SOCK_STREAM (Provides sequenced, reliable, two-way, connection-based byte streams.)
		case stream
		
		/// SOCK_DGRAM (Supports datagrams (connectionless, unreliable messages of a fixed maximum length).)
		case datagram
		
		///
		/// Return enum equivalent of a raw value
		///
		/// - Parameter forValue: Value for which enum value is desired
		///
		/// - Returns: Optional contain enum value or nil
		///
		static func getType(forValue: Int32) -> SocketType? {
			
			switch forValue {
				
			case Int32(SOCK_STREAM.rawValue):
				return .stream
			case Int32(SOCK_DGRAM.rawValue):
				return .datagram
			default:
				return nil
			}
		}
	}

	// MARK: Class Methods
	
	///
	/// Create a configured Socket instance.
	/// **Note:** Calling with no passed parameters will create a default socket: IPV4, stream, TCP.
	///
	/// - Parameters:
	///		- family:	The family of the socket to create.
	///		- type:		The type of socket to create.
	///		- proto:	The protocool to use for the socket.
	///
	/// - Returns: New Socket instance
	///
	public class func create(family: ProtocolFamily = .inet, type: SocketType = .stream, proto: SocketProtocol = .tcp) throws -> Socket {
		
		if type == .datagram || proto == .udp {
			
			throw Error(code: Socket.SOCKET_ERR_NOT_SUPPORTED_YET, reason: "Full support for Datagrams and UDP not available yet.")
			
		}
		return try Socket(family: family, type: type, proto: proto)
	}

	// MARK: Lifecycle Methods
	
	// MARK: -- Public
	
	///
	/// Internal initializer to create a configured Socket instance.
	///
	/// - Parameters:
	///		- family:	The family of the socket to create.
	///		- type:		The type of socket to create.
	///		- proto:	The protocool to use for the socket.
	///
	/// - Returns: New Socket instance
	///
	public init(family: AddressFamily = .inet, type: SocketType = .stream) throws {
		
		// Initialize the read buffer...
		self.readBuffer.initialize(to: 0)
		
		// Create the socket...
		self.socketfd = Glibc.socket(family.value, type.value, 0)
		
		// If error, throw an appropriate exception...
		if self.socketfd < 0 {
			
			self.socketfd = Socket.SOCKET_INVALID_DESCRIPTOR
			throw Error(code: Socket.SOCKET_ERR_UNABLE_TO_CREATE_SOCKET, reason: self.lastError())
		}
		
		// Create the signature...
		try self.signature = Signature(
			protocolFamily: family.value,
			socketType: type.value,
			proto: proto.value,
			address: nil)
	}


}