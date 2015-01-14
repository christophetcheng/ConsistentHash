#ifndef _CONSISTENTHASH_H_
#define _CONSISTENTHASH_H_

#include <string>

class ConsistentHash
{
private:
	class ConsistentHashImpl* _pimpl;
public:
	enum HashAlgorithm { DEFAULT, FNV32, FNV64 /*for 64 bits, generate warnings in 32 bits*/ };
	ConsistentHash(HashAlgorithm aHashAlgorithm=DEFAULT, int aPointsPerServer=100);
    ~ConsistentHash();

	// Add and remove servers in the list
	void addServer(const std::string& aServerName);
	void removeServer(int aServerIndex);
	// Convenience method to get the the name of a server at an index
	const std::string& getServerName(size_t index) const;

	// Here we do the work: return the index of the server
	size_t hash(const std::string& aKey, size_t aSize) const;

	// Convenience method
	const std::string& operator[](const std::string& aKey) { return getServerName(hash(aKey,aKey.size())); }
	// Debug
	void print();
};

#endif // _CONSISTENTHASH_H_