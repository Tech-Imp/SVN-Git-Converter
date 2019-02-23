//Stephen Fernandez								core.cpp
//techimp.program@gmail.com
//Core module for Mercurial-git conversion automation

#include "tools.hpp"
#include "syscheck.hpp"

int main(int argc, char* argv[]){
	const double version = 4.30;
	string srvOwnerEmail= "nobody@company.com";
	banner();
	
	cout<<endl;
	cout<<"Welcome to the Convert to Git program!"<<endl;
	cout <<"Version:" <<setprecision(2) << fixed << version <<endl;
	cout<<"This version can convert both Mercurial and Subversion repositories to Git."<<endl;
	cout<<"It also now handles flat SVN dump files"<<endl;
	cout<<"\n***********************************************************************"<<endl;
	cout<<"WARNING: This application MUST be run as Administrator in the event you do not "<<endl;
	cout<<"possess the appropriate packages or do not have them on the PATH variable!"<<endl;
	cout<<"Otherwise this application cannot properly perform certain actions."<<endl;
	cout<<"***********************************************************************\n"<<endl;
	cout<<"-----------------------------------------------------------------------"<<endl;
	cout<<"If there are issues still upon running this application..."<<endl;
	cout<<"Contact: "<<srvOwnerEmail<<endl;
	cout<<"And attach the repo.log and debug.log files to your email, "<<endl;
	cout<<"if they exist, to help with the troubleshooting process."<<endl;
	cout<<"Please be as descriptive as possible."<<endl;
	cout<<"-----------------------------------------------------------------------\n"<<endl;
	
	string type="";
	while(1){
		cout<<"\nType 'svn' for subversion conversion or 'hg' for mercurial conversion."<<endl;
		cout<<"Uncommon commands: '~svn' and '~hg' skips some verification checks for each"<<endl;
		cout<<"\t\t   '~auth-svn' generates svn authors file only"<<endl;
		cin>>type;
		if(type!="svn" && type!="hg" && type!="~svn" && type!="~hg" && type!="~auth-svn"){
			cout<<"Invalid selection: "<<type<<endl;
			continue;
		}
		break;
	}
	system("cls");
	cout<<"Running prerequiste tests..."<<endl;
	cout<<"-----------------------------"<<endl;
	
	std::ostringstream verNum;
	verNum << version;
	
	syscheck myCheck(verNum.str(), type);
	myCheck.run();
	
	system("PAUSE");
	return 0; 
}
