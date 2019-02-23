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
echo "==Starting run, saving to results.end==" | tee results.end
for repo in $lines ; do
	command="svnadmin dump /mnt/source/srv/svn/repos/"$repo" > "$repo".sbak 2>>results.end"
	echo "Compressing:"$repo | tee -a results.end
	echo $command | bash 
done
echo "----Concluded----" | tee -a results.end