#ifndef LIBVIRT_RUNNER_H
#define LIBVIRT_RUNNER_H

#include <QString>

class Virtual_Machine;

class LibvirtRunner
{
public:
	static bool available();
	static QString lastError();

	static bool defineDomain( const Virtual_Machine &vm );
	static bool start( Virtual_Machine &vm );
	static bool stop( Virtual_Machine &vm );
	static bool pause( Virtual_Machine &vm );
	static bool resume( Virtual_Machine &vm );
};

#endif
