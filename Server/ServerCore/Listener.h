#pragma once
#include "IOCPCore.h"
#include "NetAddress.h"

class AcceptEvent;
class ServerService;

/*---------------
	Listener
---------------*/

class Listener : public IOCPObject
{
public:
	Listener() = default;
	~Listener();

public:
	bool StartAccept(ServerServiceRef service);
	void CloseSocket();

public:
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IOCPEvent* iocpEvent, int32 numOfBytes = 0) override;

private:
	void RegisterAccept(AcceptEvent* acceptEvent);
	void ProcessAccept(AcceptEvent* acceptEvent);

protected:
	SOCKET					_socket = INVALID_SOCKET;
	vector<AcceptEvent*>	_acceptEvents;
	ServerServiceRef		_service;
};
