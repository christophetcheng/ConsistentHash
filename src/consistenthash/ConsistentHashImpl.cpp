#include "ConsistentHashImpl.h"
#include <algorithm>
using namespace std;
#include <stdio.h>
#include <stdlib.h>

ConsistentHashImpl::ConsistentHashImpl(ConsistentHash::HashAlgorithm aHashAlgorithm,int aPointsPerServer)
: kPOINTS_PER_SERVER(aPointsPerServer),kHASH_ALGORITHM(aHashAlgorithm)
{
}

ConsistentHashImpl::~ConsistentHashImpl(void)
{
}

void ConsistentHashImpl::addServer(const string& aServerName)
{
	_serverList.push_back(Server(aServerName));
	updateContinuum();
}

void ConsistentHashImpl::removeServer(int aServerIndex)
{
	_serverList.erase(_serverList.begin()+aServerIndex);
	updateContinuum();
}

// Return the index of the server
size_t ConsistentHashImpl::hash(const std::string& aKey, size_t aSize) const
{
	size_t hash = GenerateHash(aKey,aSize,kHASH_ALGORITHM);
	// Find the element in the continuum, using standard operator< from ContinuumItem
	// The continuum is sorted, we need to find the lower bound
	ContinuumItem v;
	v.value = hash;
	ContinuumList::const_iterator i = lower_bound(_continuumList.begin(),_continuumList.end(),v);
	// This is a circle, if we are beyond the last one, we take the first one
	if(i==_continuumList.end()) i=_continuumList.begin();
	return i->serverIndex;
}

const string& ConsistentHashImpl::getServerName(size_t index) const
{
	return _serverList[index].name;
}

// All from memcached
size_t ConsistentHashImpl::GenerateHash(const std::string& aKey, size_t aSize, ConsistentHash::HashAlgorithm aHashAlgo)
{
	size_t value = 0;
	switch(aHashAlgo)
	{
	case ConsistentHash::FNV64: // in 64 bits
		value = 0xcbf29ce484222325LL; 
		for(size_t i=0;i<aSize;++i)
		{
			value ^= aKey[i];
			value *= 0x100000001b3LL;
		}
		break;
	case ConsistentHash::FNV32:
		value = 2166136261UL; // 0xcbf29ce484222325LL in 64 bits
		for(size_t i=0;i<aSize;++i)
		{
			value ^= aKey[i];
			value *= 16777619; // 0x100000001b3LL in 64bits
		}
		break;

	case ConsistentHash::DEFAULT:
	default:
		const char *ptr= &aKey[0];
		++aSize; // bug in memcached ?
		while (--aSize) 
		{
			value += *ptr++;
			value += (value << 10);
			value ^= (value >> 6);
		}
		value += (value << 3);
		value ^= (value >> 11);
		value += (value << 15); 
		break;
	}
	return value == 0 ? 1 : value;
}

void ConsistentHashImpl::printContinuum(void) const
{
	// Compute the ratio of coverage of each server
	double kTOTAL = ((size_t)(1 << 31))*2.0;
	size_t ratio[10];
	size_t cur=0;
	for(ContinuumList::const_iterator i = _continuumList.begin(); i != _continuumList.end() ; ++i )
	{
		if(i->value < cur)
			printf("-------> Error in continuum\n");
		ratio[i->serverIndex] += i->value - cur;
		cur = i->value;
	}
	// the last one brings to the first one
	ratio[_continuumList.begin()->serverIndex] += (1<<32) - cur;

	printf("Continuum\n----------\n");
	for(size_t i=0; i<_serverList.size();++i)
	{
		printf("%s,%d=%2.4f\n", getServerName(i).c_str(),i,((double)ratio[i])/kTOTAL);
	}
}

void ConsistentHashImpl::updateContinuum(void)
{
	// resize the continuum list if need be
	size_t new_continuum_size = _serverList.size()*kPOINTS_PER_SERVER;
	if (_continuumList.size() != new_continuum_size)
	{
		_continuumList.resize(new_continuum_size);
	}

	// build the continuum list with "servername-index"
	int continuum_index=0;
	for(size_t s = 0 ; s < _serverList.size(); ++s)
	{
		char aServerKey[200];
		size_t aServerHash = 0;
		for(int p= 1; p <= kPOINTS_PER_SERVER; ++p) 
		{
			sprintf(aServerKey,"%s-%d",_serverList[s].name.c_str(),p);
			aServerHash = GenerateHash(aServerKey,strlen(aServerKey),kHASH_ALGORITHM);
			_continuumList[continuum_index].serverIndex = s;
			_continuumList[continuum_index].value = aServerHash;
			++continuum_index;
		}
	}

	// sort the continuum list by value
	sort(_continuumList.begin(),_continuumList.end());
}

