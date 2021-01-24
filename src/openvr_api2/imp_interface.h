#pragma once

class ImpInterface
{
public:
	virtual void Init() = 0;
	virtual void Shutdown() = 0;
	virtual bool IsHmdPresent() = 0;
	virtual bool IsRuntimeInstalled() = 0;
	virtual const char *RuntimePath() = 0;
	virtual void** GetAPIInterfaceHandles() = 0;
};

