# AUX scripts
These are useful in migration of subversion to github in the event a subversion instance isnt running

## How to use
* Use forgeDump to get the flat files
* Download flat files
* Use convert2git to convert files in bulk locally (using the -l parameter)
* You can use oddInfo to evalutate whats going on in any repo that does not seem to convert properly on the previous step. Typically this is due to things not properly following trunk/branch notation (requires manual intervention and knowledge) OR containing multiple projects in one repository (requires only repo/subproject rather than repo as input)
* massPush can push a large quantity of local git repos created by convert2git

### forgedump.sh
This script takes in a new line delimited file and creates flatfiles for the projects listed. Its results go to screen and results.end

### oddInfo.sh
This script pulls up relevant data on a expanded flat file without a subversion instance. Useful for finding out whats going on and who is the last commiter of a repository if conversion seems to fail. It uses a file as an input list. Its results go to screen and failReport.txt (which is comprehensive) and lastCommits.txt (which is much shorter)

### massPush.sh
This script uses an input file to create remote repos and push local copies to githhub using the github API. Its results go to screen and creationReport.txt



