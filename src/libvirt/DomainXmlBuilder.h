#ifndef DOMAIN_XML_BUILDER_H
#define DOMAIN_XML_BUILDER_H

#include <QString>

class Virtual_Machine;

class DomainXmlBuilder
{
public:
	static QString build( const Virtual_Machine &vm );
};

#endif
