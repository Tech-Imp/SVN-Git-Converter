# convert-to-git
Tool to aid with git migration (can handle subversion or mercurial)


### What this tool does:
Automate the initial creation of a local git repository from an existing subversion or mercurial repository


## Getting Started
 
**The subversion conversion this tool does is based on [Atlassian's git to svn migration tutorial](https://www.atlassian.com/git/tutorials/migrating-overview).**
  
**While not as comprehensive as Atlassian's tutorial, this should handle basic migrations very well.**

**The mercurial conversion follows the conversion process found here https://helgeklein.com/blog/2015/06/converting-mercurial-repositories-to-git-on-windows/ **
  
### Required software:
For subversion:
subversion, git, java

For mercurial:
mercurial, git


## HOW TO USE:

**Windows:** Run convert-to-git.exe as administrator and follow onscreen prompts

**Mac/Linux:** Run convert2git.sh and follow onscreen prompts. Use the -h parameter for additional options.



## Limitations:

**Both:** Will ONLY convert repositories that follow the standard format of trunk and branches. Nonstandard formats are not supported, and will fail with odd results.

**convert-to-git.exe** Must be run as administrator if you dont have the required software. This is needed so that that software is added to the PATH variable appropriately.

**convert2git.sh**  Has a small handful of options that can be seen by passing the '-h' parameter. Since this serves both Mac and various flavors of Linux, it will NOT automatically go out and get the appropriate required software packages. It will just halt upon finding that a required package does not exist. User is responsible for adding the requested missing package themselves to their system.


## Key differences:

The Windows executable can, since the environment is known, direct you to the proper places to download the required software and adjust your PATH variable so that the conversion process is as easy as possible. Complete with a logger to aid in debugging. Much more verbose to user.

The bash version is lightweight and behaves more as a utility script more than a program. It is easy to adjust since no recompliation is necessary to alter some parameters.


### Whats going on behind the scenes:

* Test to see if required command exists (java, svn, git, hg), depending on the source repository type, and if it doesnt, alert the user.
* Check if additional files are needed but not present and if not, download if possible or provide the user with their web browser opened to the appropriate download page.
* Run commands that are accessible on the command line with standard setup
 

# Troubleshooting:

**General:**
If it seems like SVN, Git or Java is missing even though they were recently installed, chances are the installer did not install the command line commands. Uninstall and then reinstall ensuring command line commands are selected for installation. Typically they appear as an option during the install process, and typically they are installed by default, but that is not always the case.

**Windows:**
Most issues with convert-to-git.exe stem from not running it as administrator. To do so, right click the application and select 'Run as Administrator'. A new prompt may pop up requesting you to log in as an administrator after which, the executable will run with the elevated privledges it needs.

Additionally, the windows executable creates 2 different logs it appends to for debugging purposes. Debug.log will capture any issues preventing the testing of whether commands are available for use. Repo.log captures the repo that the conversion was attempted on along with the full git svn command issued. **NOTE:** The bash script does not have this functionality.

**Linux/Mac:**

The most common error to see is 
> 'git: svn is not a git command'

This is due to the fact that the user is lacking git svn. Please search for the appropriate package for your system using your browser and install it. 

Another issue that may arise is the script finding characters that do not visibly exist preventing the script from being run. Example:
> ./convert2git.sh: line 17: $'\r': command not found
> ./convert2git.sh: line 18: $'\r': command not found
> ./convert2git.sh: line 19: syntax error near unexpected token `$'{\r''
> '/convert2git.sh: line 19: `function help {

To fix this, run: 
>dos2unix convert2git.sh

This will strip out additional non printing characters that may be causing the issue.

General issues:
>SSL handshake failed: SSL error: sslv3 alert handshake failure 

Means the version of subversion available to the command line is likely out of date and/or not tortoiseSVN (on windows). This can be rectified by contacting the service admin listed in the program or by manually fixing the path and installing the latest version of SVN yourself. 


If you run into a "Connection reset by peer..."  Error continually without being able to get the next commit, chances are you will not be able to follow the normal procedure. You will need a flat file dump generated by the following command:

>svnadmin dump /mnt/source/srv/svn/repos/NAMEOFREPO > NAMEOFREPO.sbak

It is important that the name of the backup file match the repo, otherwise additional steps involving using bypass commands will be needed for the program.

To obtain this flat file dump, contact the steward at dvcs@utlists.utexas.edu with the name of the repo this issue is occurring for and request the flat file dump. This flat file must be placed in the same directory as the executible in order for the program to detect it. After the authors file generation stage, the program will detect and prompt the user of the existence of the file and options for it, from expanding the file into a local subversion repository, to just run on the already existing folder (in the event you do not need to reinflate again) to ignoring the dump files presence entirely. Processing will then resume as normal with the program handling where to properly point rather than the user having to worry. 

To do this process manually run:

>svnadmin create REPOS (creates an empty local subversion repository named REPOS at location. If no path is given, assumed to be in current folder)
>svnadmin load --bypass-prop-validation /PATH/TO/DIRECTORY/REPO_MADE_IN_LAST_STEP < REPO_FLAT_FILE.sbak
This will create a local version of the expanded flat file.

To use this local copy using the program, when prompted for the svn repo address, pass in the following format:

>file:///drive/folder/path/to/REPO  
example:

>file:///c/SVN/MyRepo

Note that the file command needs three slashes and the drive part of the path does not possess a colon like it normally does. In order to run it in this way it is assumed that a valid authors file exists and it will not generate one. To force the program to just generate an authors file only, when prompted to "Type 'svn' for subversion conversion or 'hg' for mercurial conversion." type the following instead:

>~auth-svn 
This will lead you through the process of generating an authors file alone. It will terminate upon completing the creation.


###DEV Additional notes:

There is a makefile included in the source of this project in the event convert-to-git.exe needs to be regenerated. Run the 'make' command 

There are hidden commands in the windows version that bypass certain checks. ~svn or ~hg bypass checks to ensure items are on the path. ~auth-svn will switch to making an authors file only for SVN. ~y and ~n bypass the authors file generation when given in response to the question "Shorthand detected! Would you like to use...instead?"
