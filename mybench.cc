
//#define DEBUG
#define THR
// count throughtput

#ifdef THR
	//if mode 5~ this is second
	//for throughput
	#define PROCESS_NUM 10
	#define OP_NUM 60*30
	//def 1800 seconds
	#define SET_OR_GET 5
	// default: 3 
	// set only 1, get only 2, set and get 3 
	// throughput 5-

#else
	//for latency
	#define PROCESS_NUM 10
	#define OP_NUM 10*1000*1000
//	#define OP_NUM 1*1000*1000

	#define SET_OR_GET 1
	// default: 3 
	// set only 1, get only 2, set and get 3 
	// throughput 5-

#endif

#define VALUE_RANGE 1*1000
// OP/PROCESS
#define KEY_RANGE 1*1000*1000
// Def 1KB
//#define ITEM_SIZE 1000
#define ITEM_SIZE 2*1000
//For check
//#define KEY_RANGE 10000
//#define ITEM_SIZE 100

//#define PERCENT_OF_SET 50
// now not work
 
#define DBPATH "/testdb"


#include <sys/time.h>
#include <time.h>
#include <vector>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <sstream>
#include <string>
#include "leveldb/db.h"
#include "process.h"


struct pass_data{
	leveldb::DB* db;
	std::vector < timeval > stime;
	std::vector < timeval > gtime;
	std::vector < int >  thrtime;
	int set_or_get;
	int thread_num;
	std::vector< std::string > value_set;
};

//std::vector< std::string > data;
//std::vector< std::vector < timeval > > all_time;

leveldb::Status myget(leveldb::DB* db,std::string key,std::string* value){ 
	return db->Get(leveldb::ReadOptions(),key,value);
}

leveldb::Status myput(leveldb::DB* db,std::string key,std::string value){ 
	return db->Put(leveldb::WriteOptions(),key,value);
}

//std::string rand_string(std::string *stp){
std::string rand_string(){
	int count;
	std::string buf;
	for(count=0;count<(ITEM_SIZE/2);count++){
			buf+='a'+(rand()%26);
			buf+='a'+(count%26);
	}
	//*stp+=buf;
	return buf;
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
	srand((unsigned)time(NULL)+p->thread_num);

	//leveldb::Status status = leveldb::DB::Open(options,dbpath,&db);

	//assert(status.ok());
	switch(p->set_or_get){
		case 1://set only
			for(int count=0;count<OP_NUM;count++)
			{

				int ran = rand()%KEY_RANGE;
				std::ostringstream sstream;
				sstream << ran;
				key = sstream.str();
				ran = rand()%VALUE_RANGE;
				gettimeofday(&s, NULL);
				//status = myput(db,key,rand_string());
				status = myput(db,key,p->value_set[ran%VALUE_RANGE]);
				gettimeofday(&e, NULL);
				
				assert(status.ok());
				calc.tv_sec =  e.tv_sec - s.tv_sec;
				calc.tv_usec = e.tv_usec - s.tv_usec;
				p->stime.push_back(calc);
				//status = myput(db,key,pvalue);
				//assert(status.ok());
			}
			break;
		case 2://get only

			for(int count=0;count<OP_NUM;count++)
			{

				int ran = rand()%KEY_RANGE;	
				std::ostringstream gstream;
				gstream << ran;
				key = gstream.str();
				status = myget(db,key,&gvalue);
				assert(status.ok());
			}
			break;
		case 3://set&get
			for(int count=0;count<OP_NUM;count++)
			{
				//int ran = count%KEY_RANGE;
				int ran = rand()%KEY_RANGE;
				std::ostringstream sstream;
				sstream << ran;
				key = sstream.str();

				gettimeofday(&s, NULL);
				//status = myput(db,key,rand_string());
				//status = myput(db,key,data[ran]);
				status = myput(db,key,p->value_set[ran%VALUE_RANGE]);
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
		case 4:// preset data
			for(int count=0;count<KEY_RANGE;count++)
			{
				std::ostringstream sstream;
				sstream << count;
				key = sstream.str();
				//status = myput(db,key,rand_string());
				//status = myput(db,key,data[count]);
				status = myput(db,key,p->value_set[count%VALUE_RANGE]);
				assert(status.ok());
				//printf("now %d/%d\n",count,KEY_RANGE);
			}
			printf("put all key range\n");
			break;

		case 5://set and throughput
			time_t last,next;
			int thr;
			thr=0;
			last = time(0);
			for(int count=0;count<OP_NUM;)
			{

				int ran = rand()%KEY_RANGE;
				std::ostringstream sstream;
				sstream << ran;
				key = sstream.str();

				//status = myput(db,key,rand_string());
				//status = myput(db,key,data[ran]);
				status = myput(db,key,p->value_set[ran%VALUE_RANGE]);

				assert(status.ok());
				thr++;
				if (time(&next) != last){
					time_t buf=last;
					last = next;
					count+=(int)next-buf;
					p->thrtime.push_back(thr);
					thr=0;
				}
#ifdef DEBUG
				if(count%10==0)
					printf("now : %d",count);
#endif
				//status = myput(db,key,pvalue);
				//assert(status.ok());
			}
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
	for(int count=0;count<VALUE_RANGE;count++){
		for(int c=0;c<ITEM_SIZE;c++)
		{
			rstring+='a'+(rand()%26);
		}
		dataset->push_back(rstring);
	}
}
void calc_thr(std::vector<struct pass_data> vpd ,long double *thr,bool get)
{
	
	std::ofstream tofs("current_thr.txt", std::ios::out | std::ios::app );
	tofs << "OPERATION TIME : "<< OP_NUM  << "KEY RANGE  : " << KEY_RANGE << "PROCESS : " << PROCESS_NUM << "ITEMSIZE : " << ITEM_SIZE <<std::endl;
	for(int count=0;count<PROCESS_NUM;count++)
        {
		tofs << "PROCESS:" << count << std::endl;
		for(int ct=0;ct<OP_NUM;ct++)
                {
			tofs<< vpd[count].thrtime[ct]  <<", ";
		}
		tofs<< std::endl << std::flush;
	}


	std::ofstream aofs("current_sumthr.txt", std::ios::out | std::ios::app );
	aofs << "OPERATION TIME : "<< OP_NUM  << "KEY RANGE  : " << KEY_RANGE << "PROCESS : " << PROCESS_NUM << "ITEMSIZE : " << ITEM_SIZE <<std::endl;
        for(int ct1=0;ct1<OP_NUM;ct1++)
        {
		unsigned long int sum=0;//per sec sum
		for(int counter=0;counter<PROCESS_NUM;counter++)
		{
			sum+=vpd[counter].thrtime[ct1];
		}
		aofs<< sum <<", ";
        }
	aofs<< std::endl << std::flush;
}

void calc_data(std::vector<struct pass_data> vpd ,long double *ave,long double *var, long double *thr,bool get )
{
	int count;
	long double avstime=0.0,avgtime=0.0,varstime=0.0,vargtime=0.0;
	long double buf=0.0;
	if(get == true){
	std::ofstream gofs("current_get.txt", std::ios::out | std::ios::app );
	gofs << "OPERATIONS : "<< OP_NUM  << "KEY RANGE  : " << KEY_RANGE << "PROCESS : " << PROCESS_NUM << "ITEMSIZE : " << ITEM_SIZE <<std::endl;
	for(count=0;count<PROCESS_NUM;count++)
	{
		gofs << "Process : "<< count << std::endl;
		for(int ct=0;ct<OP_NUM;ct++)
		{
			avgtime+=vpd[count].gtime[ct].tv_sec + vpd[count].gtime[ct].tv_usec * 1.0E-6;
			gofs << vpd[count].gtime[ct].tv_sec + vpd[count].gtime[ct].tv_usec * 1.0E-6 << std::endl;
		}
		gofs << std::endl;
	}
	*ave=avgtime=avgtime/(PROCESS_NUM*OP_NUM);
	
	gofs.close();

	for(count=0;count<PROCESS_NUM;count++)
	{
		for(int ct=0;ct<OP_NUM;ct++)
		{
			buf = avgtime - (vpd[count].gtime[ct].tv_sec + vpd[count].gtime[ct].tv_usec * 1.0E-6);
			vargtime += buf * buf;
		}
	}
	*var=vargtime/(PROCESS_NUM*OP_NUM);
	
	} 
	else {
	std::ofstream sofs("current_set.txt", std::ios::out | std::ios::app );
	sofs << "OPERATIONS : "<< OP_NUM  << "KEY RANGE  : " << KEY_RANGE << "PROCESS : " << PROCESS_NUM << "ITEMSIZE : " << ITEM_SIZE <<std::endl;
	for(count=0;count<PROCESS_NUM;count++)
	{
		sofs << "Process : "<< count << std::endl;
		for(int ct=0;ct<OP_NUM;ct++)
		{
			avstime+=vpd[count].stime[ct].tv_sec + vpd[count].stime[ct].tv_usec * 1.0E-6;
			sofs << vpd[count].stime[ct].tv_sec + vpd[count].stime[ct].tv_usec * 1.0E-6 << std::endl;
		}
		sofs << std::endl;
	}
	*ave=avstime=avstime/(PROCESS_NUM*OP_NUM);
	
	sofs.close();	

	for(count=0;count<PROCESS_NUM;count++)
	{
		for(int ct=0;ct<OP_NUM;ct++)
		{
			buf = avstime - (vpd[count].stime[ct].tv_sec + vpd[count].stime[ct].tv_usec * 1.0E-6);
			varstime += buf * buf;
		}
	}
	*var=varstime/(PROCESS_NUM*OP_NUM);
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

	//create dataset(value)
	for(count=0;count<PROCESS_NUM;count++)
        {
		g_dataset(&(vpd[count].value_set));
	}
	//g_dataset(&data);

	// setdata
	////////////////////////////////////////////////////////
	
	for(count=0;count<1;count++)
	{
		vpd[count].db=db;
		vpd[count].set_or_get=4;
		vpd[count].thread_num=count;
		pthread_create(&plist[count] , NULL ,&bench ,(void *)&vpd[count]);
 		
	}
	printf("wait\n");
	for(count=0;count<1;count++)
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
		vpd[count].thread_num=count;
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


	delete db;
	//status = leveldb::DB::Close(options,dbpath,&db);
	//assert(status.ok());

	long double avstime=0.0,avgtime=0.0,varstime=0.0,vargtime=0.0,thrstime=0.0,thrgtime=0.0;
	long double allthr=0.0;
	long double buf=0.0;
	

	switch(set_or_get){
	case 1:
		calc_data(vpd,&avstime,&varstime,&thrstime,false);
		break;
	case 2:
		calc_data(vpd,&avgtime,&vargtime,&thrgtime,true);
		break;
	case 3:
		calc_data(vpd,&avstime,&varstime,&thrstime,false);
		calc_data(vpd,&avgtime,&vargtime,&thrgtime,true);
		break;	
	case 4:
		return 0; 
	case 5:
		calc_thr(vpd,&allthr,false);
		break;
	}

	
	//////////////////////////////////////////////////////////	
/*
	std::ofstream gofs("current_get.txt", std::ios::out | std::ios::app );
	std::ofstream sofs("current_set.txt", std::ios::out | std::ios::app );
	sofs << "OPERATIONS : "<< OP_NUM  << "KEY RANGE  : " << KEY_RANGE << "PROCESS : " << PROCESS_NUM << "ITEMSIZE : " << ITEM_SIZE <<std::endl;
	gofs << "OPERATIONS : "<< OP_NUM  << "KEY RANGE  : " << KEY_RANGE << "PROCESS : " << PROCESS_NUM << "ITEMSIZE : " << ITEM_SIZE <<std::endl;
	for(count=0;count<PROCESS_NUM;count++)
	{
		sofs << "Process : "<< count << std::endl;
		gofs << "Process : "<< count << std::endl;
		for(int ct=0;ct<OP_NUM;ct++)
		{
			avstime+=vpd[count].stime[ct].tv_sec + vpd[count].stime[ct].tv_usec * 1.0E-6;
			sofs << vpd[count].stime[ct].tv_sec + vpd[count].stime[ct].tv_usec * 1.0E-6 << std::endl;
			avgtime+=vpd[count].gtime[ct].tv_sec + vpd[count].gtime[ct].tv_usec * 1.0E-6;
			gofs << vpd[count].gtime[ct].tv_sec + vpd[count].gtime[ct].tv_usec * 1.0E-6 << std::endl;
		}
		sofs << std::endl;
		gofs << std::endl;
	}
	avstime=avstime/(PROCESS_NUM*OP_NUM);
	avgtime=avgtime/(PROCESS_NUM*OP_NUM);

	gofs.close();
	sofs.close();	

	for(count=0;count<PROCESS_NUM;count++)
	{
		for(int ct=0;ct<OP_NUM;ct++)
		{
			buf = avstime - (vpd[count].stime[ct].tv_sec + vpd[count].stime[ct].tv_usec * 1.0E-6);
			varstime += buf * buf;
			buf = avgtime - (vpd[count].gtime[ct].tv_sec + vpd[count].gtime[ct].tv_usec * 1.0E-6);
			vargtime += buf * buf;
		}
	}
	varstime=varstime/(PROCESS_NUM*OP_NUM);
	vargtime=vargtime/(PROCESS_NUM*OP_NUM);
*/
	printf("average set time: %llf\naverage get time: %llf \n",avstime,avgtime);
	printf("var set time: %llf\nvar get time: %llf \n",varstime,vargtime);
	printf("all throughput: %llf\n",allthr);
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
