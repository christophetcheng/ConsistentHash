#ifndef _CONSISTENTHASHIMPL_H_
#define _CONSISTENTHASHIMPL_H_
#include "ConsistentHash.h"
#include <string>
#include <vector>
class ConsistentHashImpl
{
public:
	ConsistentHashImpl(ConsistentHash::HashAlgorithm aHashAlgorithm,int aPointsPerServer);
    ~ConsistentHashImpl();

	// Add and remove servers (will update the continuum).
	void addServer(const std::string& aServerName);
	void removeServer(int aServerIndex);
	// Convenience method
	const std::string& getServerName(size_t index) const;

	// Return the index of the server for the specified key
	size_t hash(const std::string& aKey, size_t aSize) const;

	void printContinuum() const;
private:
	const int kPOINTS_PER_SERVER;
	const ConsistentHash::HashAlgorithm kHASH_ALGORITHM;
	// A list of SERVERS
	struct Server {
		Server(const std::string& n) : name(n) {}
		std::string name;
	};
	typedef std::vector<Server> ServerList;
	ServerList _serverList;
	// A list of continuum ITEMS
	struct ContinuumItem {
		size_t serverIndex;
		size_t value;
		bool operator< (const ContinuumItem& o) const { return value<o.value; }
	};
	typedef std::vector<ContinuumItem> ContinuumList;
	ContinuumList _continuumList;
	// Update the continuum after an addition or removal of a server
	void updateContinuum(void);
	// static method to generate a hash value based on a key
	static size_t GenerateHash(const std::string& aKey, size_t aSize, ConsistentHash::HashAlgorithm aHashAlgo);
};

#endif //_CONSISTENTHASHIMPL_H_