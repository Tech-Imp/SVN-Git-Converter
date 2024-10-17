//syscheck.cpp Class file
//Stephen Fernandez
//

#include "syscheck.hpp"

void syscheck::run(){
	
	logger("\n\n\n\t*****Initializing...******");
	
	logger("Version: "+version);
	logger(currentDateTime());
	logger("Running as: "+string(getenv("userprofile")));
	logger(type);
	if(type=="~hg" || type=="~svn" || type=="~auth-svn"){
		logger("\n !!!Prereq check skipped. Special command: "+type+" used!!! \n");
		cout<<"\n Prereq check skipped! \n"<<endl;
	}
	else{
		if(type=="hg"){
			needRestart=(checkExists("MERCURIAL","hg",hgWeb, hgDef) || needRestart);
		}
		else if(type=="svn"){
			needRestart=(checkExists("JAVA","java",javaWeb, javaDef) || needRestart);
			needRestart=(checkExists("SVN","svn",svnWeb, svnDef) || needRestart);
		}
		else{
			logger("\n\n!!Fatal: Improper repository type passed.!!");
			cout<<"An error occured. Please contact developer."<<endl;
			system("PAUSE");
			exit (EXIT_FAILURE);
		}
		needRestart=(checkExists("GIT","git",gitWeb, gitDef) || needRestart);
	}
	if(needRestart){
		//system("cls");
		cout<<"+++++++++++++++++++++"<<endl;
		
		cout<<"\nIF there are NO errors above, "<<endl;
		cout<<"You will need to run this application again to get changes to path variable.\n"<<endl;
		cout<<"\n(Errors are typically caused by not running this program as admin)\n"<<endl;
		logger("\n\n!!RESTART REQUIRED-Reason: Path variable!!");
		system("PAUSE");
		exit (EXIT_FAILURE);
	}
	logger("*****Pre-req check complete****** \n \n");
	system("cls");
	if(type=="hg" || type=="~hg"){
		hgConvert->execute();
	}
	else if(type=="svn" || type=="~svn"){
		svnConvert->execute();
	}
	else if(type=="~auth-svn"){
		svnConvert->shortAuth();
	}
	else{
		logger("\n\n!!Fatal: Improper repository type passed.!!");
		cout<<"An error occured. Please contact developer."<<endl;
		system("PAUSE");
		exit (EXIT_FAILURE);
	}
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void syscheck::logger(string info){
	ofstream fout( "debug.log", ios::out | ios::app);
	fout<<info<<endl;
}
//----------------------------------------------------------------
//----------------------------------------------------------------
bool syscheck::checkExists(nstring name, string command, nstring site, string defLoc){
	cout<<"Checking if "<<name<<" is available..."<<endl; 
	
	cout<<"Testing PATH variable...."<<endl;
	logger("\n Testing "+command+" PATH variable....");
	
	if ((!system(("where /Q "+command).c_str()))){
		logger("\n Command found, no checking of path necessary");
		cout<<name<<" OK"<<endl;
	}
	else if(checkPath(command)) cout<<name<<" OK"<<endl;
    else {
		cout<<name<<" not on path currently...checking default install locations..."<<endl;
		
		if(setPath(defLoc, command)){
			cout<<name<<" was not on path...Will require restarting this app to take effect."<<endl;
			return true;
		}
		else{
			system(("setx /M PATH \"%PATH%\""));
			logger(command+" Failed to be added to path. Returning PATH to original. Installer shown");
			cout<<"FAILURE: Either "<<name<<" isnt installed (using default options)"<<endl;
			cout<<"and therefore isnt able to be put on the PATH variable"<<endl;
			cout<<"OR"<<endl;
			cout<<name<<" is installed but doesnt use the default install location"<<endl;
			cout<<"and thus cant be added to the path automatically by this program"<<endl;
			cout<<"-------------------------------------"<<endl;
		
			puts ("FAILED, sending to retrieving installer...");
			printf("Preparing to web browser to recover installer.\n");
			printf("Please run the installer using default settings and then re-run this program again.\n");
			system("PAUSE");
			logger(site);
			system(site);
			exit (EXIT_FAILURE);
		}
	}
	return false;
}
//----------------------------------------------------------------
//----------------------------------------------------------------
bool syscheck::checkPath(string command){
	logger("Running checkPath on: "+command);
	bool found=false;
	string myPath=getenv("PATH");
	cout<<"---"<<endl; 
	logger("--PATH--");
	logger(pathStr);
	logger("--END--");
	char delims[] = ";";    
	size_t tokenPos=myPath.find(delims);
	while( tokenPos != string::npos ) {  
		string dir = myPath.substr(0, tokenPos);
		struct stat buf;
		//Will only return 0 in even found, -1 if not
		logger("Test against: \n\t"+string(dir));
		
		if(stat((string(dir)+"\\"+command+sysEnding).c_str(),  &buf)==0){
			cout<<"\t--Directory on PATH: "<<dir<<endl;
			logger("\t--Directory on PATH: "+dir);
			found=true;
		}
		//Remove section from string and then find next delim
		myPath.erase(0, tokenPos+1);
        tokenPos = myPath.find(delims);  
	}
	//check for last edge in case delim forgot one
	if(myPath.length()){
		struct stat buf;
		logger("End Test against: \n\t"+string(myPath));
		if(stat((string(myPath)+"\\"+command+sysEnding).c_str(),  &buf)==0){
			cout<<"\t Directory on PATH: "<<myPath<<endl;
			logger("\t Directory on PATH: "+myPath);
			found=true;
		}
	}
	logger("--End test against "+command+"--");
	return found;
	
}
//-----------------------------------------------------------------
//-----------------------------------------------------------------
bool syscheck::setPath(string defLoc, string command){
	
	if(defLoc!=""){
		if(defLoc.find("*\\") != std::string::npos){
			defLoc=wildCardIterator(defLoc, true);
		}
		else if (defLoc.find("*") != std::string::npos) {
			defLoc=wildCardIterator(defLoc, false);
		}
		
		if(testValidLoc((defLoc+"\\"+command+sysEnding).c_str())){
			//Place on actual path after taking into account previous items
			if(prevLocs!="")defLoc=defLoc+";"+prevLocs;
			
			logger("\n########-Setting Path-########\n");
			logger("Running: setx /M PATH \"%PATH%;"+defLoc+"\" \n");
			system(("setx /M PATH \"%PATH%"+defLoc+"\"").c_str());
			system(("set PATH=%PATH%;"+defLoc).c_str());
			
			logger(getenv("PATH"));
			prevLocs=defLoc;
				
			logger("\n\nManual UNDO (use last listed): \n setx /M PATH \"%path:"+defLoc+";=%\"\n");
			logger("\n########-End Successful Set-########\n");
			return true;
		}
		else{
			cout<<"The default location of \n"+defLoc+"\nisnt valid. "<<endl;
			logger("Default location of "+command+" isnt valid. Will need installer");
			logger("Used: "+defLoc);
			return false;
		} 
	}
	else{
		cout<<"**********"<<endl;
		cout<<command<<" is added to the path by the installer."<<endl; 
		cout<<"Since its in user variables, it cant be accessed in admin mode"<<endl;
		cout<<"Run this application WITHOUT BEING IN ADMIN MODE to access the "<<command<<" command"<<endl;
		cout<<"**********"<<endl;
		logger("!!! Command "+command+" handled external to this application but cannot be seen !!!");
		
		return true;
	}
	
	
	return true; 
}
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
string syscheck::wildCardIterator(string defLoc, bool userPath){
	logger("Wildcard detected in "+defLoc);
	string prev="";
	if(userPath){
		//This section is only to be used when there would be a singular wildcard in directory
		string myConfig=getenv("userprofile");
		
		string::size_type n = defLoc.find_last_of("*");
		
		if (n != std::string::npos) {  ++n;}
		string spec = defLoc.substr(n);
		prev=myConfig+spec;
	}
	else{
		//Separate into the base directory
		string::size_type n = defLoc.find_last_of("\\");
		string  base = defLoc.substr(0,n);
		//Grab the specific directory wanted (contains wildcard)
		if (n != std::string::npos) {  ++n;}
		string  spec = defLoc.substr(n);
		//Remove Wildcard
		spec.erase(std::remove(spec.begin(), spec.end(), '*'), spec.end());
		//Open base directory
		DIR *dir = opendir(base.c_str());
		struct dirent *entry = readdir(dir);
		//Find largest alphanumerical directory that partially matches the wildcard
		while (entry != NULL)
		{
			if (entry->d_type == DT_DIR && (string(entry->d_name).find(spec) != std::string::npos)){
				cout<<"Found version of "<<entry->d_name<<endl;
				if(prev.compare(entry->d_name)<0)prev=entry->d_name;
			}
			entry = readdir(dir);
		}
		//And then pass back the proper location of the assumed binary folder
		closedir(dir);
		prev=base+"\\"+prev+"\\bin";
		//cout<<prev<<endl;
	}
	logger("Returned actual:  "+prev);
	return prev;
	
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
bool syscheck::testValidLoc(nstring defLoc){
	if (FILE *file = fopen(defLoc, "r")) {
        fclose(file);
		logger("\n\t"+string(defLoc)+" exists on system.");
		return true;
	}
	else{
		logger("\n\t FAILED:"+string(defLoc)+" does not exists on system.");
		return false;
	}
}

