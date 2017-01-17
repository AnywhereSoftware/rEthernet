#pragma once
#include "B4RDefines.h"
//~version: 1.11
//~dependson:<SPI.h>
//~dependson:<Ethernet.h>
namespace B4R {
	//~shortname: Ethernet
	class B4REthernet {
		public:
			//Initializes the ethernet connection.
			//Gets the local ip address from the DHCP service.
			//Returns true if the ip address was assigned successfully.
			//MacAddress - The shield mac address.
			//
			//Example:<code>
			//eth.InitializeDHCP(Array As Byte(0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED))</code>
			bool InitializeDHCP (ArrayByte* MacAddress);
			//Initializes the shield with a static ip address.
			void Initialize(ArrayByte* MacAddress, ArrayByte* LocalIp);
			//Returns the local ip address as a string.
			B4RString* getLocalIp();
			/**
			*Allows for the renewal of DHCP leases. This method should be called approximately 
			*every second when the the ethernet was initialized with InitializeDHCP.
			*/
			Byte MaintainDHCP();
	};
	//~shortname: EthernetSocket
	//A client socket implementation.
	class EthernetSocket {
		private:
			B4RStream stream;
		public:
			//~hide
			EthernetClient client;
			//~hide
			EthernetSocket();
			/**
			*Tries to connect to the server. Returns true if connection was successful.
			*IP - Server ip address as an array of bytes.
			*Port - Server port.
			*/
			bool ConnectIP(ArrayByte* IP, UInt Port);
			/**
			*Tries to connect to the server. Returns true if connection was successful.
			*Host - Host name.
			*Port - Server port.
			*/
			bool ConnectHost(B4RString* Host, UInt Port);
			//Gets the network stream. Can be used together with AsyncStreams.
			B4RStream* getStream();
			//Tests whether the client is connected.
			bool getConnected();
			//Closes the connection.
			void Close();
	};
	typedef void (*SubVoidEthernetSocket)(EthernetSocket*);
	//~shortname: EthernetServerSocket
	//~Event: NewConnection (NewSocket As EthernetSocket)
	//A server socket implementation.
	class EthernetServerSocket {
		private:
			Byte backend[sizeof(EthernetServer)];
			PollerNode pnode;
			EthernetSocket ethSocket;
			SubVoidEthernetSocket newConnection;
			static void checkForClient(void* ess);
		public:
			//~hide
			EthernetServer* server;
			//Initializes the server.
			//Port - The server port.
			//NewConnectionSub - The sub that will handle the NewConnection event.
			void Initialize(UInt Port, SubVoidEthernetSocket NewConnectionSub);
			//Starts listening for connections.
			//The NewConnection event will be raised when data from the new client is available.
			//This means that the client is expected to send at least one byte after the connection is established.
			//You should call Listen again after the connection has broken. AsyncStreams_Error is a good place for this call.
			void Listen();
			//Returns the last connected socket.
			EthernetSocket* getSocket();
			
	};
	typedef void (*SubPacketArrived)(ArrayByte*, ArrayByte*, UInt);
	//~shortname: EthernetUDP
	//~Event: PacketArrived (Data() As Byte, IP() As Byte, Port As UInt)
	//Allows receiving and sending UDP packets.
	class B4REthernetUDP {
		private:
			EthernetUDP udp;
			static void checkForData(void* b);
			SubPacketArrived PacketArrivedSub;
			PollerNode pnode;
		public:
			/**
			*Initializes the object. Returns True if successful.
			*Port - The UDP socket will be bound to this port.
			*PacketArrivedSub - The sub that handles the PacketArrived event.
			*/
			bool Initialize(UInt Port, SubPacketArrived PacketArrivedSub);
			/**
			*Starts sending a packet. The packet will be sent when SendPacket is called.
			*IP - Target ip address.
			*Port - Target port address.
			*/
			bool BeginPacket(ArrayByte* IP, UInt Port);
			//Writes data to the packet. Returns the number of bytes that were written successfully.
			//This method should only be called between a call to BeginPacket and SendPacket.
			//You can call Write multiple times.
			Int Write(ArrayByte* Data);
			//Sends the packet.
			bool SendPacket();
			//Closes the socket.
			void Close();
	};
}