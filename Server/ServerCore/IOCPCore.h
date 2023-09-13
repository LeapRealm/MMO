#pragma once

/*-----------------
	IOCPObject
-----------------*/

class IOCPObject : public enable_shared_from_this<IOCPObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IOCPEvent* iocpEvent, int32 numOfBytes = 0) abstract;
};

/*---------------
	IOCPCore
---------------*/

class IOCPCore
{
public:
	IOCPCore();
	~IOCPCore();

	HANDLE	GetHandle() { return _iocpHandle; }

	bool	Register(IOCPObjectRef iocpObject);
	bool	Dispatch(uint32 timeoutMs = INFINITE);

private:
	HANDLE	_iocpHandle;
};
