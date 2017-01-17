#include "B4RDefines.h"
namespace B4R {
	bool B4REthernet::InitializeDHCP (ArrayByte* MacAddress) {
		return Ethernet.begin((Byte*)MacAddress->data);
	}
	void B4REthernet::Initialize (ArrayByte* MacAddress, ArrayByte* IpAddress) {
		IPAddress ip((Byte*)IpAddress->data);
		Ethernet.begin((Byte*)MacAddress->data, ip);
	}
	B4RString* B4REthernet::getLocalIp() {
		IPAddress ip = Ethernet.localIP();
		return B4RString::PrintableToString(&ip);
	}
	Byte B4REthernet::MaintainDHCP() {
		return Ethernet.maintain();
	}
	EthernetSocket::EthernetSocket() {
		stream.wrappedStream = stream.wrappedClient = &client;
	}
	bool EthernetSocket::ConnectIP(ArrayByte* IP, UInt Port) {
		IPAddress ip((Byte*)IP->data);
		return client.connect(ip, Port);
	}
	bool EthernetSocket::ConnectHost(B4RString* Host, UInt Port) {
		return client.connect(Host->data, Port);
	}
	B4RStream* EthernetSocket::getStream() {
		return &stream;
	}
	bool EthernetSocket::getConnected() {
		return client.connected();
	}
	void EthernetSocket::Close() {
		client.stop();
	}
	void EthernetServerSocket::checkForClient(void* ess) {
		EthernetServerSocket* server = (EthernetServerSocket*)ess;
		EthernetClient c = server->server->available();
		if (c) {
			pollers.remove(&server->pnode);
			server->ethSocket.client = c;
			server->newConnection(&server->ethSocket);
		}
		
	}
	void EthernetServerSocket::Initialize(UInt Port, SubVoidEthernetSocket NewConnectionSub) {
		server = new (backend) EthernetServer(Port);
		this->newConnection = NewConnectionSub;
		FunctionUnion fu;
		fu.PollerFunction = checkForClient;
		pnode.functionUnion = fu;
		pnode.tag = this;
		pnode.next = NULL;

		server->begin();
	}
	
	void EthernetServerSocket::Listen() {
		if (pnode.next == NULL)
			pollers.add(&pnode);
	}
	EthernetSocket* EthernetServerSocket::getSocket() {
		return &ethSocket;
	}
		//static
	void B4REthernetUDP::checkForData(void* b) {
		B4REthernetUDP* me = (B4REthernetUDP*) b;
		int size = me->udp.parsePacket();
		if (size == 0)
			return;
		const UInt cp = B4R::StackMemory::cp;
		ArrayByte* arr = CreateStackMemoryObject(ArrayByte);
		arr->data = StackMemory::buffer + StackMemory::cp;
		arr->length = me->udp.read((Byte*)arr->data, size);
		B4R::StackMemory::cp += size;
		ArrayByte* ip = CreateStackMemoryObject(ArrayByte);
		ip->data = StackMemory::buffer + StackMemory::cp;
		ip->length = 4;
		B4R::StackMemory::cp += 4;
		IPAddress ipa = me->udp.remoteIP();
		for (byte i = 0;i < 4;i++) {
			((Byte*)ip->data)[i] = ipa[i];
		}
		me->PacketArrivedSub(arr, ip, me->udp.remotePort());
		B4R::StackMemory::cp = cp;
	}
	bool B4REthernetUDP::Initialize(UInt Port, SubPacketArrived PacketArrivedSub) {
		if (udp.begin(Port) == 0)
			return false;
		if (PacketArrivedSub != NULL) {
			this->PacketArrivedSub = PacketArrivedSub;
			FunctionUnion fu;
			fu.PollerFunction = checkForData;
			pnode.functionUnion = fu;
			pnode.tag = this;
			if (pnode.next == NULL) {
				pollers.add(&pnode);
			}
		}
		return true;
	}
	bool B4REthernetUDP::BeginPacket(ArrayByte* IP, UInt Port) {
		IPAddress ip((Byte*)IP->data);
		return udp.beginPacket(ip, Port);
	}
	Int B4REthernetUDP::Write(ArrayByte* Data) {
		return udp.write((Byte*)Data->data, Data->length);
	}
	bool B4REthernetUDP::SendPacket() {
		return udp.endPacket();
	}
	void B4REthernetUDP::Close() {
		return udp.stop();
	}
}