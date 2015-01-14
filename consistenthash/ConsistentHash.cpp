#include "ConsistentHash.h"
#include "ConsistentHashImpl.h"

// Just delegate to the implementation

ConsistentHash::ConsistentHash(
							   HashAlgorithm aHashAlgorithm,
							   int aPointsPerServer
							   )
	: _pimpl(new ConsistentHashImpl(aHashAlgorithm,aPointsPerServer))
{}

ConsistentHash::~ConsistentHash()
{}

void ConsistentHash::print()
{ _pimpl->printContinuum(); }

void ConsistentHash::addServer(const std::string& aServerName)
{ _pimpl->addServer(aServerName); }

void ConsistentHash::removeServer(int aServerIndex)
{ _pimpl->removeServer(aServerIndex); }

const std::string& ConsistentHash::getServerName(size_t index) const
{ return _pimpl->getServerName(index); }

size_t ConsistentHash::hash(const std::string& aKey, size_t aSize) const
{ return _pimpl->hash(aKey,aSize); }
