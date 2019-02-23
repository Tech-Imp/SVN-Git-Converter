//hgconvert.cpp Class file
//Stephen Fernandez
//

#include "hgconvert.hpp"

void hgconvert::execute(){
	string repo;
	cout<<"------------------------"<<endl;
	cout<<"--Beginning conversion--"<<endl;
	cout<<"------------------------"<<endl;
	logger("----Conversion at "+currentDateTime()+" ----");
	logger("Version: "+version);
	createConfig();
	repo=locRepo();
	testHg(repo);
	runConversion(repo);
	setupIgnore(repo);
	cout<<"------------------------"<<endl;
	cout<<"----Process Complete----"<<endl;
	cout<<"------------------------"<<endl;
	logger("----END at "+currentDateTime()+" ---- \n\n");
}
//----------------------------------------------------------------
//----------------------------------------------------------------
string hgconvert::locRepo(){
	string repo="";
	cout<<"------------------------"<<endl;
	cout<<"--Repository  Location--"<<endl;
	cout<<"------------------------"<<endl;
	cout<<"Please enter the current location on disk of the mercurial"<<endl;
	cout<<"repository OR here if the program is within the repository folder"<<endl;
	cout<<"------------------------"<<endl;
	char approve='n';
	while(approve!='y'){
		cout<<"Please enter the mercurial repository location:"<<endl;
		cin>>repo;
		if((repo.find("here") != string::npos)){
			cout<<"\nEvaluating on current directory"<<endl;
			break;
		}
		cout<<endl<<"You entered : \n"<<repo<<"\n --Is this correct? (y/n)";
		cin>>approve;
	}
	logger("Using repository location: "+repo);
	if(repo=="here"){repo="";}
	else{repo="-R "+repo;}
	return repo;
}


void hgconvert::testHg(string repo=""){
	//Checks mercuial branches and preps for conversion
	char test [100];
	string delim="::";
	string rev="";
	
	string branchTag="hg branches "+repo+" -T \"{branch}::{rev} \n\"";
	logger("Running command to return all branches \n\t "+branchTag);
	cout<<"\nBranches will be renamed for git conversion process."<<endl;
	cout<<"You will find any branches created in mercuial with the 'hg-' prefix"<<endl;
	cout<<"with the notable exception of master (previously default).\n"<<endl;
	cout<<"------------------------"<<endl;
	
	FILE *fbranches= (FILE*)popen(branchTag.c_str(), "r");
	if (fbranches == NULL){
		logger("!!An error occured while attempting to open the file stream for the branches.!!");
		cout<<"Issue attempting to run command to retrieve branches."<<endl;
		cout<<"(Are you sure this is a mercurial repo?)"<<endl;
		exit (EXIT_FAILURE);
	}
	else{
		string tag="";
		while ( ! feof (fbranches) ){
			
			fscanf(fbranches, "%s", test);
			
			//account for the fact that spaces may exist in some branches
			if (string(test).find("default") != std::string::npos){
				system(("hg bookmark "+repo+" -r default master").c_str());
				cout<<test<<" set to master"<<endl;
				logger("Found default, assigned to master");
			}
			else if (string(test).find(delim) != std::string::npos) {
				tag=tag+test;
				int startDelim = tag.find(delim);
				tag.erase(startDelim, delim.length());
				rev=tag.substr(startDelim, std::string::npos);
				tag.erase(startDelim, std::string::npos);
				string revise="hg bookmark "+repo+" -r "+rev+" hg-"+tag;
				logger(tag+" with rev "+rev);
				logger("Running "+revise);
				system(revise.c_str());
				cout<<tag<<" with rev "<<rev<<endl;
				tag="";
			}
			else{
				tag=tag+test;
			}
		}
	fclose (fbranches);
	}
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void hgconvert::createConfig(){
	//Checks config for needed attributes. Wordy due to I/o issues
	string myConfig=getenv("userprofile");
	myConfig=myConfig+"\\mercurial.ini";
	
	ifstream cFile;
	cFile.open(myConfig.c_str());
	
	string line;
	bool needUpdate=true;
	string token="hggit = ";
	string token2="hgext.bookmarks =";
	string token3="intree = True";
	if (cFile.good()){ 
		while (getline( cFile, line )){
			if (line.find(token) != string::npos){
				needUpdate=false;
			}
			if (line.find(token2) != string::npos){
				needUpdate=false;
			}
			if (line.find(token3) != string::npos){
				needUpdate=false;
			}
		}
		cFile.close();
		if(needUpdate){
			cout<<endl<<"-----------------------"<<endl;
			cout<<endl<<"-Altering config file-"<<endl;
			cout<<myConfig<<endl;
			cout<<endl<<"-----------------------"<<endl;
			logger("Altering config file \n\t"+myConfig);
 
			ofstream fout;
			fout.open( myConfig.c_str(), ios::out | ios::app);
			
			fout<<"[extensions]"<<endl;
			fout<<"hggit = "<<endl;
			fout<<"hgext.bookmarks ="<<endl;
			fout<<endl;
			fout<<"[git]"<<endl;
			fout<<"intree = True"<<endl;
			fout.close();
		}
    }
	/* 
	char approve='n';
	cout<<"exit?"<<endl;
	cin>>approve;
	if(approve=='y'){
		cout<<"\nProgram execution paused while file is open...";
		system("notepad %Userprofile%\\mercurial.ini");
		//logger("File was opened for additional editting.");
		cout<<" Resuming..."<<endl;
		exit (EXIT_FAILURE);
	} */
	
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void hgconvert::runConversion(string repo){
	string gitname;
	cout<<endl<<"-----------------------"<<endl;
	cout<<"-Mercurial -> GIT conversion-"<<endl;
	cout<<"-----------------------"<<endl;
	logger("\nActual Mercurial project: \n\t"+repo);
	cout<<"If all previous steps concluded without error, this process will take a while..."<<endl;
	cout<<"Please stand by...\n"<<endl;
	
	string genGit="hg gexport "+repo+" --debug";
	logger("Prepare to run following: \n\t "+genGit+" \n");
	system(genGit.c_str());
	
	string delim="-R ";
	if (repo.find(delim) != std::string::npos) {		//swap the argument from hg out for windows specific
		int startDelim = repo.find(delim);
		repo.erase(startDelim, delim.length());
		repo="cd "+repo+" & " ;
	}
	cout<<"-----------------------"<<endl;
	cout<<"Preparing bare repository to be working repository."<<endl;
	string workGit=repo+"git  config --bool core.bare false";
	logger("Prepare to run following: \n\t "+workGit+" \n");
	system(workGit.c_str());
	cout<<"Creating index..."<<endl;
	string setGit=repo+"git  reset HEAD -- .";
	logger("Prepare to run following: \n\t "+setGit+"\n");
	system(setGit.c_str());
	
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void hgconvert::setupIgnore(string repo=""){
	string delim="-R ";
	if (repo.find(delim) != std::string::npos) {		//swap the argument from hg out for windows specific
		int startDelim = repo.find(delim);
		repo.erase(startDelim, delim.length());
		repo=repo+"\\";
	}
	logger("Testing for hgignore at: \n\t"+repo);
	if (FILE *file = fopen((repo+".hgignore").c_str(), "r")) {
		fclose(file);
		
			//Create a copy of hgignore for ease
			cout<<"Creating a simple copy of .gitignore from .hgignore"<<endl;
			cout<<"NOTE: PLEASE make sure to manually edit it to be relevant"<<endl;
			string copyFile="copy "+repo+".hgignore "+repo+".gitignore";
			system((copyFile).c_str());
			logger("Prepare to run following: \n\t "+copyFile+"\n");
			ofstream fout( (repo+".gitignore").c_str(), ios::out | ios::app);
			fout<<".hgignore"<<endl;
			fout<<".hg"<<endl;
			
	}
}

void hgconvert::logger(string info){
	ofstream fout( "repo.log", ios::out | ios::app);
	fout<<info<<endl;
}