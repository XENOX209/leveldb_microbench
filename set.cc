#include "leveldb/db.h"
#include <iostream>
#include <assert.h>

int main(int argc, char *argv[])
{
	std::string key(argv[1]);
	std::string value(argv[2]);

	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options,"/tmp/testdb",&db);

	assert(status.ok());
	
	status = db->Put(leveldb::WriteOptions(),key,value);
	assert(status.ok());
	std::cout << "key:" << key << "\nvalue" << value <<std::endl;
}
