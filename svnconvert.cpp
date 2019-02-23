//svnconvert.cpp Class file
//Stephen Fernandez
//

#include "svnconvert.hpp"

void svnconvert::execute(){
	cout<<"------------------------"<<endl;
	cout<<"--Beginning conversion--"<<endl;
	cout<<"------------------------"<<endl;
	logger("\n----Conversion at "+currentDateTime()+" ----");
	logger("Version: "+version);
	testJar();
	string repo=getRepo();
	string backup=repo+bakEnd;
	repo=flatTest(repo, backup);
	runConversion(repo);
	cout << '\a';
	cout<<"------------------------"<<endl;
	cout<<"----Process Complete----"<<endl;
	cout<<"------------------------"<<endl;
	logger("----END at "+currentDateTime()+" ---- \n\n");
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void svnconvert::testJar(){
	//Checks for Jar responsible for author file creation
	cout<<"Checking for "<<name<<" file within the same folder \n as the executable..."<<endl;
	if (FILE *file = fopen(name, "r")) {
        fclose(file);
        cout<<"Found! Running Last verification check!"<<endl<<endl;
		system("java -jar svn-migration-scripts.jar verify");
		logger("Verify argument ran");
		cout<<endl<<"NOTE: Any mention of data loss on a case insensitive system would only"<<endl; 
		cout<<"occur in the event two files in the same directory were named EXACTLY"<<endl;
		cout<<"the same, EXCEPT for capitalization"<<endl;
		//return true;
    } else {
		cout<<"NOT Found!"<<endl;
		cout<<"Browser will now be opened to download "<<name<<endl;
		cout<<"When downloaded, please move the file into the same directory as this executable and re-run svn-git.exe"<<endl;
		system("PAUSE");
		logger("!!! File "+string(name)+" missing!!");
		logger(dlLink);
		system(dlLink);
		exit (EXIT_FAILURE);
        //return false;
    }   
}
//----------------------------------------------------------------
//----------------------------------------------------------------
string svnconvert::getRepo(){
	cout<<endl<<"-----------------------------------"<<endl;
	cout<<endl<<"-Name of Repository for conversion-"<<endl;
	cout<<endl<<"-----------------------------------"<<endl;
	string approve="n";
	string repo;
	cout<<"(This will follow a format similar to \n "<<baseSVN<<"PROJNAME  \n where PROJNAME contains the branches and trunk of the repository.)\n"<<endl;
	cout<<" NOTE: If you do not see branches, trunk, and tags at \n "<<baseSVN<<"PROJNAME \n then follow the folders down until you do. \n That will be what PROJNAME will be. \n"<<endl;	
	while(approve!="y"){
		cout<<"Please enter the svn repo address:"<<endl;
		cin>>repo;
		cout<<endl<<"You entered : \n"<<repo<<"\n --Is this correct? (y/n)";
		cin>>approve;
	}
	logger("Got for initial repo: \n\t"+repo);
	if((repo.find("file://") != string::npos)){		//Deal with case that user passed a file protocol
		cout<<"\nA filepath was passed. \nAuthors file generation will be skipped automatically."<<endl;
		cout<<"If you pass a filepath, program assumes you have a valid authors file already,"<<endl;
		cout<<"in same directory as this program."<<endl;
		logger("Filepath passed. Warning given. Gave: \n\t"+repo);
		skipAuth=true;
	}
	
	if(!(repo.find(baseSVN) != string::npos) && !(repo.find("file://") != string::npos) && (baseSVN != "")){
		approve="n";
		logger("Shorthand detected!! Gave: \n\t"+repo);
		cout<<"\nShorthand detected! Would you like to use the following instead: \n\t"<<baseSVN<<repo<<"\n"<<endl;
		cout<<"[Skipping authors file generation is NOT RECOMMENDED]"<<endl;
		cout<<"\t'y' for Yes \n\t'n' for No \n\t'~y' Yes BUT skip authors file creation \n\t'~n' No AND skip authors file creation)"<<endl;
		cin>>approve;
		//Cant use switchcase statment due to C++ not working well with string types in cases
		if(approve=="y"){
			repo=baseSVN+repo;
			cout<<"\nUsing: \n\t"<<repo<<"\nfor project URL instead"<<endl<<endl;
		}
		else if(approve=="~y"){
			repo=baseSVN+repo;
			logger("Generation of authors.txt skipped \n");
			cout<<"\nSkipping Author file generation"<<endl;
			skipAuth=true;
			cout<<"\nUsing: \n"<<repo<<"\n for project URL instead"<<endl<<endl;
		}
		else if(approve=="~n"){
			cout<<"\nSkipping Author file generation"<<endl;
			skipAuth=true; 
			logger("Generation of authors.txt skipped \n");
		}
	}
	return repo;
}
//----------------------------------------------------------------
//----------------------------------------------------------------
string svnconvert::flatTest(string repo, string backup){
	string flatAuth="";
	if (FILE *file = fopen(backup.c_str(), "r")) {
		fclose(file);
		cout << '\a';
		cout<<"\n---------------------------------------"<<endl;
		cout<<"--!!Local compressed copy detected!!--"<<endl;
		cout<<"---------------------------------------\n"<<endl;
		cout<<"\tFilename:  "<<backup<<endl<<endl;
		char approve='n';
		logger("\n!!!Local backup "+backup+" exists!!!\n");
		cout<<"Would you like to load and run this local compressed copy \n or just run on the expanded directory (if exists)?\n"<<endl;
		cout<<"\ty for yes, expand file and run on local copy \n\tr for run on expanded directory only \n\tn to skip this step and ignore local compressed copy"<<endl;
		cin>>approve;
		if(approve=='y'){					//reinflate
			setupStruct(repo, backup);
			cout<<"\nResuming..."<<endl;
			explodeFile(repo, backup);
			cout << '\a';
		}
		if(approve!='n'){
			flatAuth=repo+locSVN;			//run on directory
			repo=currentDir(repo, true);
			cout<<"Using local repository location instead:\n\t"+repo<<endl;
			logger("Using local repository location instead:\n\t"+repo+"\n");
		}
	}
	
	createAuthors(repo, flatAuth);
	return repo;
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void svnconvert::createAuthors(string repo="", string flatAuth=""){
	//Creates Author file needed for later processes
	if(!skipAuth){
		cout<<endl<<"-----------------------"<<endl;
		cout<<endl<<"-Creating Authors file-"<<endl;
		cout<<endl<<"-----------------------"<<endl;
		if(flatAuth.empty()){
			//Prettify the authors file with required extras and a default common email
			logger("Base repo to create authors file: \n\t "+repo);
			
			system(("java -jar svn-migration-scripts.jar authors "+repo+" > authors.txt").c_str());
			ofstream fout( "authors.txt", ios::out | ios::app);
			fout<<"(no author) = no_author <no_author@no_author>"<<endl;
			fout.close();
			if (!system("where /Q powershell")){
				logger("\nPowershell exists and was used to create default email domains");
				system(("powershell -Command \"(gc authors.txt) -replace \'@mycompany.com\', \'"+commonEmail+"\' | Out-File -encoding ASCII authors.txt\"").c_str());
			}
		}
		else{	//Handle flatfiles and author files
			ofstream fout( "authors.txt", ios::out);
			fout<<"(no author) = no_author <no_author@no_author>"<<endl;
			int revNumMax=atoi(sysResponse(("cd "+flatAuth+" && svnlook youngest .").c_str()).c_str()); //all commits incremental in SVN
			logger("\nRan due to flatfile author generation :\n");
			logger(("\tcd "+flatAuth+" && svnlook youngest .").c_str());
			vector<string> authors; 		//verified unique authors
			cout<<"Generating authors file via incremental revision check...please wait..."<<endl;
			cout<<"================================================================================"<<endl;
			int numAuth=0;
			for(int rev=1; rev<revNumMax;++rev){
				string person=sysResponse(("cd "+flatAuth+" && svnlook author -r "+to_string(rev)+" .").c_str());
				if( find(authors.begin(), authors.end(), person) != authors.end()){
					cout<<".";
				}
				else{
					authors.push_back(person);
					fout<<person+" = "+person+" <"+person+commonEmail+">"<<endl;
					cout<<"A";
					++numAuth;
				}
			}
			logger("\n--Out of " +to_string(revNumMax)+ " commits, "+to_string(numAuth)+" unique authors were found");
			fout.close();
		}
		cout<<endl<<"-----------------------"<<endl;
		cout<<"----NEW Author file----"<<endl;
		cout<<"-----------------------"<<endl;
		cout << '\a';
		cout<<"Please make sure to edit and save any changes needed to ensure an accurate \n authors.txt"<<endl;
		string approve="n";
		cout<<"Would you like to open that file now? (y/n)"<<endl;
		cin>>approve;
		if(approve=="y"){
			cout<<"\nProgram execution paused while file is open...";
			system("notepad authors.txt");
			logger("\nFile was opened for additional editting.");
			cout<<"\nResuming..."<<endl;
		}
	}
	
}
//----------------------------------------------------------------
//----------------------------------------------------------------
void svnconvert::runConversion(string repo){
	string gitname;
	cout<<endl<<"-----------------------"<<endl;
	cout<<"-SVN -> GIT conversion-"<<endl;
	cout<<"-----------------------"<<endl;
	
	logger("\n Actual SVN project: \n\t"+repo);
	char approve='n';
	while(approve!='y'){
		cout<<endl<<"What would you like to call the new GIT repository?"<<endl;
		cin>>gitname;
		cout<<endl<<"You entered : \n"<<gitname<<"\n --Is this correct? (y/n)";
		cin>>approve;
	}
	logger("\n New git project name: \n\t"+gitname);
	cout<<"If the repository URL is correct, this will take a little while."<<endl;
	cout<<"Please stand by..."<<endl;
	logger("Prepare to run following: \n\t git svn clone --stdlayout --authors-file=authors.txt "+repo+" "+gitname);
	system(("git svn clone --stdlayout --authors-file=authors.txt "+repo+" "+gitname).c_str());
	
}
//-------------------------------------------------------------------------------------
void svnconvert::logger(string info){
	ofstream fout( "repo.log", ios::out | ios::app);
	fout<<info<<endl;
}
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------

void svnconvert::setupStruct(string repo, string backup){
	string svnExp=repo+locSVN;
	cout<<"\nSetup of folder "+svnExp+" attempted...\n"<<endl;
	logger("\tLocal SVN folder created: "+svnExp);
	logger("Attempting to create \n\t"+svnExp+" \nvia Command:\n\tsvnadmin create "+svnExp);
	sysResponse(("svnadmin create "+svnExp).c_str());
	
}
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
string svnconvert::explodeFile(string repo, string backup){
	string svnFolder=repo+locSVN;
	cout<<"\n\t\t=========WARNING========="<<endl; 
	cout<<"The file expanding process is a long process that cannot be interrupted"<<endl;
	cout<<"Prematurely closing this application can cause issues which will "<<endl;
	cout<<"require you to delete the folder: \n\n\t"+svnFolder+"\n\nand start this process fresh\n"<<endl;
	cout<<"This process can run in the background without issue."<<endl;
	cout<<"Are you sure you wish to expand: \n\n\t"+backup+"\n\n into a local copy of the SVN repo at: \n\n\t"+svnFolder+"\n\n (y/n)? "<<endl;
	char approve='n';
	cin>>approve;
	if(approve=='y'){
		logger("Attempting to load "+svnFolder+" \n\t via Command: ");
		string filePath=currentDir(repo, false);
		logger("svnadmin load --bypass-prop-validation "+filePath+" < "+repo+bakEnd);
		system(("svnadmin load --bypass-prop-validation "+filePath+" < "+repo+bakEnd).c_str());
		return filePath;	
	}
	else{
		cout<<"\nUser terminated program. Please rerun when you are ready.\n"<<endl;
		logger("User terminated program. Chose to not proceed with reinflate.");
		system("PAUSE");
		exit (EXIT_FAILURE);
	}
	
}
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
string svnconvert::currentDir(string repo, bool isFile=false){
	string svnFolder=repo+locSVN;
	string path=sysResponse("cd"); //gets current working directory
	
	if(isFile){
		path=findReplace(path, "\\", "/");
		path=findReplace(path, ":", "");
		string filePath=path+"/"+svnFolder;
		return "\"file:///"+filePath+"\"";}
	else{return "\""+path+"\\"+svnFolder+"\"";}
}
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void svnconvert::shortAuth(){
	cout<<"Creation of authors file only selected."<<endl;
	logger("\n----Authors file only at "+currentDateTime()+" ----");
	logger("Version: "+version);
	string repo=getRepo();
	string backup=repo+bakEnd;
	repo=flatTest(repo, backup);
	cout<<"\n------------------------"<<endl;
	cout<<"---Authors file only----"<<endl;
	cout<<"----Process Complete----"<<endl;
	cout<<"------------------------"<<endl;
	logger("----END at "+currentDateTime()+" ---- \n\n");
}