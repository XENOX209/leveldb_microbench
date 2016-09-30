#define PROCESS_NUM 10
#define OP_NUM 10000
// OP/PROCESS
#define KEY_RANGE 1000
#define ITEM_SIZE 100

#define SET_OR_GET 3 
// default: 3 
// set only 1, get only 2, set and get 3 
#define PERCENT_OF_SET 50

#define DBPATH "/tmp/testdb"

#include <sys/time.h>
#include <vector>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <assert.h>
#include <sstream>
#include <string>
#include "leveldb/db.h"
#include "process.h"

struct pass_data{
	leveldb::DB* db;
	std::vector < timeval > stime;
	std::vector < timeval > gtime;
	int set_or_get;
};

std::vector< std::string > data;
//std::vector< std::vector < timeval > > all_time;

leveldb::Status myget(leveldb::DB* db,std::string key,std::string* value){ 
	return db->Get(leveldb::ReadOptions(),key,value);
}

leveldb::Status myput(leveldb::DB* db,std::string key,std::string value){ 
	return db->Put(leveldb::WriteOptions(),key,value);
}

void* bench(void *vp)
{
	struct pass_data *p=(struct pass_data*)vp;
	//std::vector<timeval> time;
	timeval s,e,calc;

	std::string key("1");
	std::string pvalue("first");
	std::string gvalue;

	std::string dbpath(DBPATH);
	leveldb::DB* db=p->db;
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status;
	//leveldb::Status status = leveldb::DB::Open(options,dbpath,&db);

	//assert(status.ok());
	switch(p->set_or_get){
		case 1:
			for(int count=0;count<OP_NUM;count++)
			{
				status = myput(db,key,pvalue);
				assert(status.ok());
			}
			break;
		case 2:

			for(int count=0;count<OP_NUM;count++)
			{
				status = myget(db,key,&gvalue);
				assert(status.ok());
			}
			break;
		case 3:
			for(int count=0;count<OP_NUM;count++)
			{
				//int ran = count%KEY_RANGE;
				int ran = rand()%KEY_RANGE;
				std::ostringstream sstream;
				sstream << ran;
				key = sstream.str();

				gettimeofday(&s, NULL);
				status = myput(db,key,data[ran]);
				gettimeofday(&e, NULL);
				
				assert(status.ok());
				calc.tv_sec =  e.tv_sec - s.tv_sec;
				calc.tv_usec = e.tv_usec - s.tv_usec;
				p->stime.push_back(calc);

	
				//ran = (count+1)%KEY_RANGE;
				ran = rand()%KEY_RANGE;	
				std::ostringstream gstream;
				gstream << ran;
				key = gstream.str();

				gettimeofday(&s, NULL);
				status = myget(db,key,&gvalue);
				gettimeofday(&e, NULL);		

				assert(status.ok());
				calc.tv_sec =  e.tv_sec - s.tv_sec;
				calc.tv_usec = e.tv_usec - s.tv_usec;
				p->gtime.push_back(calc);
				//printf("now %d, %d\n",count,ran);
			}
			break;
		case 4:// set data
			for(int count=0;count<KEY_RANGE;count++)
			{
				std::ostringstream sstream;
				sstream << count;
				key = sstream.str();
				status = myput(db,key,data[count]);
				assert(status.ok());
				//printf("now %d/%d\n",count,KEY_RANGE);
			}
			printf("put all key range\n");
			break;
		default:
			printf("SET_OR_GET is error\n");
			exit(1);
	}
	printf("end child process\n");
	//std::cout << "key:" << key << "\nvalue" << gvalue <<std::endl;
	//all_time.push_back(time);
}


void g_dataset(void *vdataset)
{
	std::vector<std::string> *dataset=(std::vector<std::string>*)vdataset;
	std::string rstring;
	srand( (unsigned)time( NULL ) );
	for(int count=0;count<KEY_RANGE;count++){
		for(int c=0;c<ITEM_SIZE;c++)
		{
			rstring+='a'+(rand()%26);
		}
		dataset->push_back(rstring);
	}
}

int main(int argc,char *argv[])
{ 
	int set_or_get=SET_OR_GET;
	if(argc==2)
		set_or_get=atoi(argv[1]);	
	
	pthread_t plist[PROCESS_NUM];
	int count;
	std::vector<struct pass_data> vpd(PROCESS_NUM);	

	std::string dbpath(DBPATH);
	leveldb::DB* db;
	leveldb::Options options;
	options.create_if_missing = true;
	leveldb::Status status = leveldb::DB::Open(options,dbpath,&db);
	assert(status.ok());

	g_dataset(&data);

	// setdata
	////////////////////////////////////////////////////////
	
	for(count=0;count<PROCESS_NUM;count++)
	{
		vpd[count].db=db;
		vpd[count].set_or_get=4;
		pthread_create(&plist[count] , NULL ,&bench ,(void *)&vpd[count]);
 		
	}
	printf("wait\n");
	for(count=0;count<PROCESS_NUM;count++)
	{
		pthread_join(plist[count],NULL);
	}
	printf("init data set end\n");
	
	//////////////////////////////////////////////////////////
	// bench
	for(count=0;count<PROCESS_NUM;count++)
	{
		vpd[count].db=db;
		vpd[count].set_or_get=set_or_get;
		pthread_create(&plist[count] , NULL ,&bench ,(void *)&vpd[count]);
 		
	}
	printf("wait\n");
	for(count=0;count<PROCESS_NUM;count++)
	{
		pthread_join(plist[count],NULL);
	}
	printf("bench end\n");

	if(set_or_get == 4)
		return 0;
	
	//////////////////////////////////////////////////////////	
	long double avstime=0.0,avgtime=0.0,varstime=0.0,vargtime=0.0;
	long double buf=0.0;
	for(count=0;count<PROCESS_NUM;count++)
	{
		for(int ct=0;ct<OP_NUM;ct++)
		{
			avstime+=vpd[count].stime[ct].tv_sec + vpd[count].stime[ct].tv_usec * 1.0E-6;
			avgtime+=vpd[count].gtime[ct].tv_sec + vpd[count].gtime[ct].tv_usec * 1.0E-6;
		}
	}
	avstime=avstime/(PROCESS_NUM*OP_NUM);
	avgtime=avgtime/(PROCESS_NUM*OP_NUM);

	for(count=0;count<PROCESS_NUM;count++)
	{
		for(int ct=0;ct<OP_NUM;ct++)
		{
			buf = avstime - vpd[count].stime[ct].tv_sec + vpd[count].stime[ct].tv_usec * 1.0E-6;
			varstime += buf * buf;
			buf = avgtime - vpd[count].gtime[ct].tv_sec + vpd[count].gtime[ct].tv_usec * 1.0E-6;
			vargtime += buf * buf;
		}
	}
	varstime=varstime/(PROCESS_NUM*OP_NUM);
	vargtime=vargtime/(PROCESS_NUM*OP_NUM);

	printf("average set time: %llf\naverage get time: %llf \n",avstime,avgtime);
	printf("var set time: %llf\nvar get time: %llf \n",varstime,vargtime);
	return 0;

}

//Process version
//not work
/*
   int main(int argc,char *argv[])
   {
   Process* plist[PROCESS_NUM]; 
   int count;
   for(count=0;count<PROCESS_NUM;count++)
	{
		Process child(bench);
		plist[count] = &child;
	}
	printf("wait\n");
	for(count=0;count<PROCESS_NUM;count++)
	{
		plist[count]->wait();
	}
	printf("end\n");
	return 0;
}
*/
