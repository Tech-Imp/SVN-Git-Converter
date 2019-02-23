#! /bin/bash
# Stephen Fernandez
# This is for batch conversion of local repos to Github Enterprise
# Run 'dos2unix FILENAME' on both this file and the source file
# File name requested is a list of already prepped (by convert2git.sh) local github repositories
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
echo -e "\nPlease enter the batch filename \nEach entry must be on a new line and devoid of spaces"
read filename
while [[ -z "$filename" ]]; do
	read -p "File name required for setup" filename
done
echo -e "\nPlease enter username"
read user
while [[ -z "$user" ]]; do
	read -p "Username required" user
done
echo -e "\nPlease enter Personal Access token"
read pass
while [[ -z "$pass" ]]; do
	read -p "Personal access token required" pass
done

lines=`cat $filename`
dos2unix $filename
echo "==Starting run, saving to creationReport.txt==" | tee creationReport.txt 
for repo in $lines ; do
	
	repoFolder=$repo-Git
	echo "**************************************************" | tee -a creationReport.txt
	echo "********Project: "$repo | tee -a creationReport.txt
	echo "********Checking: "$repoFolder | tee -a creationReport.txt
	
	curl https://api.github.com/orgs/ORGNAME/repos -k -u "$user:$pass" --header "Content-Type: application/json" --request POST --data "{\"name\":\"$repo\", \"private\":\"true\"}" |& tee -a creationReport.txt
	cd $repoFolder && git remote add origin https://api.github.com/orgs/ORGNAME/$repo.git |& tee -a creationReport.txt
	git push -u origin master |& tee -a creationReport.txt
	cd $DIR
	
	echo "**************************************************" | tee -a creationReport.txt
done
echo "----Concluded----" | tee -a creationReport.txt
unix2dos creationReport.txt
