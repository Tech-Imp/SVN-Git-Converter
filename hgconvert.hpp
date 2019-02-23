//hgconvert.hpp Header file
//Stephen Fernandez
//


#pragma once
#include "tools.hpp"


class hgconvert
{
    private:
		
		string log;
		string version;
		//data members that can be accessed by all memebers of the class
	public:
		hgconvert(string verNum="0"){
			
			version=verNum;
			log="repo.log";
				
		}
		~hgconvert(){}
		//next are all the prototypes
		void execute();
		void testHg(string repo);
		string locRepo();
		void createConfig();		//creates config file
		void runConversion(string repo);
		void setupIgnore(string repo);
		void logger(string info);
};