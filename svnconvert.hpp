//svnconvert.hpp Header file
//Stephen Fernandez
//


#pragma once
#include "tools.hpp"


class svnconvert
{
    private:
		nstring name, dlLink;
		string log, baseSVN; 
		string commonEmail;
		string version;
		string bakEnd, locSVN, baseName;
		bool skipAuth;
		//data members that can be accessed by all memebers of the class
	public:
		svnconvert(string verNum="0"){
			
			version=verNum;
			log="repo.log";
			skipAuth=false;
			commonEmail="@eid.utexas.edu";
			name="svn-migration-scripts.jar";	
			dlLink="explorer https://bitbucket.org/atlassian/svn-migration-scripts/downloads/";
			baseSVN="https://basesite.CHANGEME";
			bakEnd=".sbak";
			locSVN="-svn";
			
		}
		~svnconvert(){}
		//next are all the prototypes
		void execute();
		void testJar();
		string getRepo();									//All preprocessing to find repo or flat file
		string flatTest(string repo, string backup);		//Final processing for flat before author generation
		void createAuthors(string repo, string flatAuth);		//creates authors file
		void runConversion(string repo);					//Conversion process from svn to git when other processes have done hard work
		void logger(string info);							//Logger specific to the conversion process: repo.log
		void setupStruct(string repo, string backup);		//creates the blank local SVN repo for other processes
		string explodeFile(string repo, string backup);		//reinflates the flat file into blank local repo
		string currentDir(string repo, bool isFile);		//Helper function to get proper formatting for files and folders for other commands
		void shortAuth();									//Only generates authors file
		
};