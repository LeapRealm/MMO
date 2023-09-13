#pragma once

class Session;

enum class EventType : uint8
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send
};

/*---------------
	IOCPEvent
---------------*/

class IOCPEvent : public OVERLAPPED
{
public:
	IOCPEvent(EventType type);

	void Init();

public:
	EventType		eventType;
	IOCPObjectRef	owner;
};

/*-----------------
	ConnectEvent
-----------------*/

class ConnectEvent : public IOCPEvent
{
public:
	ConnectEvent() : IOCPEvent(EventType::Connect) { }
};

/*----------------------
	DisconnectEvent
----------------------*/

class DisconnectEvent : public IOCPEvent
{
public:
	DisconnectEvent() : IOCPEvent(EventType::Disconnect) { }
};

/*-----------------
	AcceptEvent
-----------------*/

class AcceptEvent : public IOCPEvent
{
public:
	AcceptEvent() : IOCPEvent(EventType::Accept) { }

public:
	SessionRef session = nullptr;
};

/*-----------------
	RecvEvent
-----------------*/

class RecvEvent : public IOCPEvent
{
public:
	RecvEvent() : IOCPEvent(EventType::Recv) { }
};

/*-----------------
	SendEvent
-----------------*/

class SendEvent : public IOCPEvent
{
public:
	SendEvent() : IOCPEvent(EventType::Send) { }

	vector<SendBufferRef> sendBuffers;
};
