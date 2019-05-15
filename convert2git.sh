#! /bin/bash
#Stephen 'Techimp' Fernandez
version=4.32
#Ease of use variables
comEmail="@mail.com" #common email suffix
stewardEmail="STEWARD@EMAIL"
baseSVN=""	#fill this with the common base web address of subversion repository 
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
#Default flags
clearCache=false
authExist=false
svnLocal=false
locFolder=false
batchConvert=false
#formatting
errRed="\033[31;40m"
success="\033[32;40m"
focus="\033[36;40m"
unusual="\033[33;40m"
clear="\033[0m"
function help {
	echo -e '\n Convert2git is used to ease the conversion process of a Subversion Or Mercurial \n repository to a Git repository. It is non-destructive copying process.'
	echo -e $unusual'\n The Mercurial half is still somewhat experimental. \n If you have issues, contact '$clear$focus$stewardEmail$clear
	echo -e $errRed'\n It can only convert repos that follow the standard subversion/mercurial \n format of branches and trunk. Nonstandard formats are not supported.'$clear
	echo -e '\n\tThe format the optional arguments of the svn2git command are: \n'
    echo -e $success"\t\tCommon"$clear
	echo -e '\t -h Help file (what you are viewing currently) '
    echo -e '\t -v returns the version number'
	echo -e '\t -x Skips authors.txt generation. Will use existing authors.txt file\n'
	echo -e $focus"\t\tUncommon conversion options"$clear
	echo -e '\t -d Triggers flat file decompression and creation of local SVN repo'
    echo -e '\t -f Skips decompression, works on local SVN repo folder in current\n\tdirectory'
	echo -e '\t -l triggers batch mode from file \n\t to convert all flat files listed\n'
	echo -e $unusual"\t\tDebug"$clear
	echo -e '\t -c (SVN ONLY) Clears cache, used primarily only for cleaning up 
	a failed but saved cached password issue. Use only to correct this issue'
	echo -e '\t -g (Mercurial ONLY) Clears .git folder and cleans up cache from a  
	mercurial repo\n'
	exit 0
}


#----------------------------------------------------------------------------------------
#------------------- Core conversions-----------------------------------------------------------
function svntogit {
	detectCommand java Java
	detectCommand git Git
	detectCommand svn Subversion
	createAuthors
}

function hgtogit {
	detectCommand git Git
	detectCommand hg Mercurial
	prepBranches
}

function convertSVN {
	projName=$1
	#echo "What:"$projName
	gitName=""
	if [ -z "$2" ]; then
		echo -e "\n"$focus"What would you like to call the folder for this new GIT repository?"$clear
		read gitName
		while [[ -z "$gitName" ]]; do
			read -p "GIT repository name required!" gitName
		done
	else
		gitName=$2-Git
	fi
	echo "Starting conversion process with:"
	echo -e "Subversion respository: \n \t $success $projName $clear"
	echo -e "Name of Git repository: \n \t $success $gitName $clear"

	echo -e "\n"$errRed"RUNNING:"$clear"\n" git svn clone --stdlayout --authors-file=authors.txt "\"$projName\"" "$gitName"
	if [ "$svnLocal" = true ];then
		eval git svn clone --stdlayout --authors-file=authors.txt "\"$projName\"" "$gitName"
	else
		git svn clone --stdlayout --authors-file=authors.txt $projName $gitName
	fi
	echo -e $success"Process complete."$clear
}

function convertHG {
	
	echo "Starting conversion process with:"
	echo $1
	echo -e "\n"$focus"This process will take a while...Please be patient..."$clear
	hg gexport -R $1 --debug
	working='cd "'$(echo $1 | sed 's:\\:\\\\:g')'" && git config --bool core.bare false'
	echo -e $focus'Setting up working directory...'$clear
	echo $working | bash
	
	resetHead='cd "'$(echo $1 | sed 's:\\:\\\\:g')'" && git reset HEAD -- .' 
	echo -e $focus'Creating index...'$clear
	echo $resetHead | bash
	
	
	echo -e $success"Process complete."$clear
	hgtogitIgnore $1
}
#---------------------------------------------------------------------------------
#----------------------Secondary functions----------------------------------------
function setupStruct {
	lrepo=""
	#echo $1
	if [ -z "$1" ]; then	
		echo -e "\n"$focus"Please enter the sbak file name. "$clear$errRed"\nDo not include the .sbak"$clear
		read lrepo
		while [[ -z "$lrepo" ]]; do
			read -p "File name required for setup" lrepo
		done
	else
		lrepo=$1
	fi
	#echo "test"$lrepo
	svnadmin create $lrepo-SVN
	explodeFile $lrepo-SVN $lrepo.sbak
	if [ $authExist = false ]; then
		createLocalAuthors $lrepo-SVN
	fi
	convertSVN "file://$DIR/$lrepo-SVN" $lrepo
}

function explodeFile {
	echo -e "\n"$focus"Loading local directory with data"$clear"\n"
	#echo "Location:"$1
	#echo "Argument file:"$2
	svnadmin load --bypass-prop-validation $1 < $2
	echo -e "\n"$success"Finished exploding local directory"$clear
	#convertSVN "\"file://$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )/$1\""
}

function runFolder {
	echo -e "\n"$focus"Please enter the directory path of the SVN folder location. "$clear
	echo -e "Your current location:"$unusual" $DIR "$clear"\n"
	read svnFolder
	while [[ -z "$svnFolder" ]]; do
		read -p "Directory path required for setup" repo
	done
	if [ $authExist = false ]; then
		createLocalAuthors $svnFolder
	fi
	convertSVN "file://$DIR/$svnFolder"
}

function createLocalAuthors {
	echo -e "\n"$focus"Creating authors file from local sources"$clear"\n"
	rm -f authors.txt
	touch authors.txt
	echo "location:"$1
	revStr="svnlook youngest "$1
	rm -f temp.txt
	touch temp.txt
	revMax="$($revStr)"
	i=1
	while [ "$i" -le "$revMax" ]; do	
		person="svnlook author -r "$i" "$1 
		echo $person
		echo "$($person) = $($person) <$($person)$comEmail>" >> temp.txt
		((i++))
	done
	echo "(no author) = no_author <no_author@no_author>" >> temp.txt
	sort -u -o authors.txt temp.txt
}


function createAuthors {

	echo -e "\n"$focus"Please enter the svn repo location:"$clear
	if [[ ! -z "$baseSVN" ]]; then
		echo -e "(This will follow a format similar to \n $baseSVN PROJNAME  \n where PROJNAME contains the branches and trunk of the repository)"
	else
		echo -e "Since baseSVN was not set in script, \nassuming the folder is in same directory as this script"
	fi
	read repo
	while [[ -z "$repo" ]]; do
		read -p "SVN repo location required!" repo
	done
	echo ""

	if [[ ! -z "$baseSVN" ]]; then
		if [[ $repo != *"$baseSVN"* && $repo != *"file://"* ]]; then
			echo -e $unusual"Shorthand detected"$clear
			repo="$baseSVN"$repo
			echo -e "Using $focus $repo $clear instead \n"
		fi
	fi

	svn info $repo
	#Ensure that the jar file that creates author file exists
	if [ "$authExist" = false ]; then
		if [ ! -f svn-migration-scripts.jar ];then
			echo -e $errRed"\n Required file svn-migration-scripts.jar missing:$clear  \n$focus Attempting to automatically re-download $clear"
			downloadResource https://bitbucket.org/atlassian/svn-migration-scripts/downloads/svn-migration-scripts.jar svn-migration-scripts.jar

			if [ ! -f svn-migration-scripts.jar ];then
				echo -e "\n"$errRed"svn-migration-scripts.jar is still missing!"$clear"\n"
				echo "Please go get it from: "
				echo -e $focus"https://bitbucket.org/atlassian/svn-migration-scripts/downloads/"$clear
				echo -e "and place it in $focus $(pwd) $clear and re-run $0 \n"
				exit
			fi
		fi
		java -jar svn-migration-scripts.jar authors $repo > authors.txt
		echo "(no author) = no_author <no_author@no_author>" >> authors.txt
		if [ "$comEmail" != "" ]; then
			sed -i 's/@mycompany.com/'$comEmail'/g' authors.txt
		fi
		echo -e "authors.txt was created in $(pwd)"
	else
		echo -e $unusual"Skipping authors.txt generation"$clear
	fi
	if [ "$svnLocal" = false ]; then 
		convertSVN $repo
	fi
}
function hgtogitIgnore {
	echo -e $focus"Generating rough copy of gitignore based on hgignore..."$clear
	cd $1 && rm .gitignore
	cd $1 && cp .hgignore .gitignore 
	echo ".hg" >> .gitignore
	echo ".hgignore" >> .gitignore
	echo -e $success" Complete "$clear
}
function prepBranches {
	echo -e "\n"$focus"What is the local path to the mercurial repository you wish to convert?"$clear
	read  -r location
	while [[ -z '$location' ]]; do
		read -pr "Mercurial repository local path required!" location
	done
	
	echo -e "\n"$focus"Starting branch conversion process..."$clear	

	hg branches -R $location -T "@{rev}:'hg-{branch}'\n" | sed -e 's/hg-default/master/g' | sed -e 's/ //g' | sed -e 's/:/ /' | sed -e 's/@/hg bookmark -R  "'$(echo "$location"  | sed 's:\\:\\\\:g')'" -r /'| bash -v
	
	
	echo -e "\n"$success"Done prepping branches"$clear
	
	convertHG $location
}
#--------------------------------------------AUX functions----------------------------------------------------
function batchProcess {
	echo -e "\n"$focus"Please enter the batch filename"$clear$errRed"\nEach entry must be on a new line and devoid of spaces"$clear
	read filename
	while [[ -z "$filename" ]]; do
		read -p "File name required for setup" filename
	done
	dos2unix $filename
	lines=`cat $filename`
	echo "==Starting run, saving to batch.end==" | tee -a batch.end
	date | tee -a batch.end
	for repo in $lines ; do
		echo "Attempting to work on:  "$repo | tee -a batch.end
		date +"%T" | tee -a batch.end
		setupStruct $repo
		date +"%T" | tee -a batch.end
		echo "----" | tee -a batch.end
	done
	echo "----Concluded----" | tee -a results.end
	date | tee -a batch.end

}
function detectCommand {
echo "Testing $1 to see if it exists as a command"
	which $1 &> /dev/null
	if [ $? == 0 ]; then
		echo -e $success"\t $1 EXISTS"$clear
	else
		echo -e $errRed"\t $1 does NOT exist"$clear
		echo -e $errRed"You need to install $2 to gain access to the $1 command"$clear
		exit 52
	fi
}
function downloadResource {
	type wget &> /dev/null
	if [ $? == 0 ];then
		echo -e $focus" Using wget to retrieve $2"$clear
		wget $1 &> /dev/null
		return
	fi
	type curl &> /dev/null
	if [ $? == 0 ];then
		echo -e $focus" Using curl to retrieve $2"$clear
		curl -L -o ${DIR}/$2 $1 &>/dev/null
		return
	fi
}

function removeCache {
	if [ "$clearCache" = true ]; then
		echo -e $errRed"Clearing SVN cache prior to operations"$clear
		rm -rf  ~/.subversion/auth
	fi
}

function cleanMercurialGit {
	detectCommand hg Mercurial
	echo -e "\n"$focus"What is the local path to the mercurial repository you wish to clean?"$clear
	read  -r location
	while [[ -z '$location' ]]; do
		read -pr "Mercurial repository local path required!" location
	done
	cd $location && rm -r .git
	hg gclear
	cd $1 && rm .gitignore
	echo "Process completed. Exiting"
	exit 0
}

function doWhichConvert {
	if [ "$svnLocal" = true ] && [ "$locFolder" = false ];then
		setupStruct
	elif [ "$svnLocal" = true ] && [ "$locFolder" = true ]; then
		runFolder
	elif [ "$batchConvert" = true ];then
		batchProcess
	else
		echo -e "\n"$focus"Convert from which?"$clear$errRed"\n m "$clear$focus" for Mercurial, "$clear$errRed" s "$clear$focus" for Subversion "$clear
		read  -r -n 1 option
		echo -e "\n"	
		case $option in
			m) hgtogit
			;;
			s) svntogit
			;;
			*) echo -e '\n'$option' is not a valid option.'
			doWhichConvert
			;;
		esac
	fi
}


#Start of program, note none of the arguments are required
while getopts hvcgxdfl option
do
	case $option in
		h) help
		;;
		v) echo -e "\nCurrent version is: "$focus$version$clear
		;;
		c) clearCache=true
		;;
		g) cleanMercurialGit
		;;
		x) authExist=true
		;;
		d) echo -e "\n"$focus"Converting a flat file to SVN repo "$clear
		svnLocal=true
		;;
		f) echo -e "\n"$focus"Converting a local SVN repo "$clear
		svnLocal=true
		locFolder=true
		;;
		l) echo -e "\n"$focus"Converting a bunch from a file list "$clear
		batchConvert=true
		;;
		'?') echo $OPTARG is not a valid option.
		help
		;;
	esac
done
echo -e ""



removeCache
doWhichConvert

echo -e $success"Press enter to exit"$clear
read stuff
