#! /bin/bash
# Stephen Fernandez
# This is for batch conversion of repos to flatfile
# Run 'dos2unix FILENAME' on both this file and the source file
echo -e "\nPlease enter the batch filename \nEach entry must be on a new line and devoid of spaces"
read filename
while [[ -z "$filename" ]]; do
	read -p "File name required for setup" filename
done
lines=`cat $filename`
dos2unix $filename
echo "==Starting run, saving to failReport.txt==" | tee failReport.txt | tee lastCommits.txt
for repo in $lines ; do
	repoFolder=$repo-SVN
	revStr="svnlook youngest "$repoFolder
	revMax="$($revStr)"
	echo "**************************************************" | tee -a failReport.txt | tee -a lastCommits.txt
	echo "********Checking: "$repoFolder | tee -a failReport.txt | tee -a lastCommits.txt
	echo "Has -"$revMax"- revisions" | tee -a failReport.txt | tee -a lastCommits.txt
	echo | tee -a failReport.txt | tee -a lastCommits.txt
	echo "-------Last commit:" | tee -a failReport.txt | tee -a lastCommits.txt
	svnlook info -r $revMax $repoFolder | tee -a failReport.txt | tee -a lastCommits.txt
	echo | tee -a failReport.txt | tee -a lastCommits.txt
	echo "-------Tree structure:" | tee -a failReport.txt
	svnlook tree -r $revMax $repoFolder --show-ids | tee -a failReport.txt
	echo | tee -a failReport.txt
	echo "**************************************************" | tee -a failReport.txt | tee -a lastCommits.txt
done
echo "----Concluded----" | tee -a failReport.txt | tee -a lastCommits.txt
unix2dos failReport.txt
unix2dos lastCommits.txt