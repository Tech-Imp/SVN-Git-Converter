//syscheck.hpp Header file
//Stephen Fernandez
//


#pragma once
#include "hgconvert.hpp"
#include "svnconvert.hpp"
#include "tools.hpp"


class syscheck
{
    private:
	    nstring javaWeb, svnWeb, hgWeb, gitWeb;
		string  javaDef, svnDef, gitDef, hgDef;
		hgconvert* hgConvert;
		svnconvert* svnConvert;
		nstring pathStr;
		bool needRestart;
		string version;
		string sysEnding;
		string prevLocs;
		string type;
		//data members that can be accessed by all memebers of the class
	public:
		syscheck(string verNum="0", string repoType="svn"){	
			version=verNum;
			type=repoType;
			//Only fill in a def var if the program does not normally add itself to user PATH
			//Web locations
			gitWeb="explorer https://git-scm.com/download/win";
			hgWeb="explorer https://www.mercurial-scm.org/downloads";
			javaWeb="explorer https://www.java.com/en/download/manual.jsp";
			svnWeb="explorer https://tortoisesvn.net/downloads.html";
			//local system locations
			javaDef="C:\\Program Files\\Java\\jre*";
			svnDef="C:\\Program Files\\TortoiseSVN\\bin";
			hgDef="C:\\Program Files\\TortoiseHg\\";
			//Git installs to path by default
			//gitDef="C:\\Users\\*\\AppData\\Local\\Programs\\Git\\cmd";
			gitDef="";
			
			needRestart=false;
			pathStr =getenv("PATH");
			prevLocs="";
			sysEnding=".exe";
			hgConvert=new hgconvert(version);
			svnConvert=new svnconvert(version);
		}
		~syscheck(){}
		//next are all the prototypes
		void run();
		void logger(string info);		//Logs items to file

		bool checkExists(nstring name, string command, nstring site, string defLoc);
		bool checkPath(string command);
		bool setPath(string defLoc, string command);
		string wildCardIterator(string defLoc, bool userPath);
		bool testValidLoc(nstring defLoc);
		
};