#include "leveldb/db.h"
#include <iostream>
#include <assert.h>

int main(int argc, char *argv[])
{
	std::string key(argv[1]);

	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options,"/tmp/testdb",&db);

	assert(status.ok());
	
	status = db->Delete(leveldb::WriteOptions(),key);
	assert(status.ok());
	std::cout << "Delete key:" << key << "\n" <<std::endl;
}
