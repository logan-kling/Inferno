To install wxWidgets and use this project in visual studio:

NOTE: We are using VS 2017, different versions have different steps! If you have VS2015 there are some guides online, 
earlier versions should have guides on the wxWiki.

wxWidgets can also be used with command line compilers and other IDEs.

Documentation for wxWidgets can be found here:
  http://docs.wxwidgets.org/3.0/

Example wxWidgets code for just about every possible wxWidgets item can be found in '<wxWidgets install path>\samples'

-----------------------------------------------------------
Contents
-----------------------------------------------------------
* Install - wxWidgets
* Troubleshooting
* Using the Installer Package
* File Guide

-----------------------------------------------------------
Install - wxWidgets
-----------------------------------------------------------

Start by going here: https://www.wxwidgets.org/downloads/ and downloading the 3.1.0 version of the wxWidgets installer.
I used the installer so thats what these instructions assume you used.

Run that installer and once it is done go into your environment variables and add the directory you 
installed wxWidgets in to your path ALSO add a new variable called "WXWIN" that points to the install 
directory, this is for our Visual Studio project so it can run with different install paths.
For example, on my desktop WXWIN points to E:\wxWidgets-3.1.0 but on my laptop it points to C:\wxWidgets-3.1.0

Next open up visual studio, click "Open Project" and go to $(WXWIN)\build\msw and select the "wx_vc14.sln" file
It will ask if you want to convert it, just say yes and let it do its thing, its from an older visual studio.
After that finishes, in the menu bar select: Build -> Batch build, in that window click "select all" and then build the whole thing.
Depending on your computer this can take a long time.
It took around 30 minutes on my laptop and 15 on my desktop so only do this when you have the time.

Once that is done you should be set to load our project. Just go to wherever your local github repo folder is and open up the .sln file
that is in the "CapstoneCode" folder. As long as the WXWIN variable is set you shouldn't need to change any of the project properties to
build and run the program.

-----------------------------------------------------------
Troubleshooting
-----------------------------------------------------------

Make sure the SDK is set to what version you have installed on your computer. Found under:
 Project->Properties
 -Configuration_Properties->General
 --Windows_SKD_Version
 
If when you try and compile the project you get an error about missing wxWidgets includes, this probably means the environment variables aren't set right.
To fix this go back into:
 Project->Properties
 -Configuration_Properties->VC++ Directories
 'Include Directories' should have: 
		<install-path>\wxWidgets-<version>\include
		<install-path>\wxWidgets-<version>\include\msvc
 'Library Directories' should have:
		<install-path>\wxWidgets-<version>\lib\vc_lib
		
The only other issue we have encountered comes from the source files for our project not getting included when you first clone from github.
This is a simple fix:
 In the Solution Explorer,
 -Right click 'CapstoneBase'->Add->Existing Item
And add ALL the .h and .cpp files in the Project directory!

-----------------------------------------------------------
Using the Installer Package
-----------------------------------------------------------

Since VS 2017 does not come standard with a way to make installers we had to use this one:
https://marketplace.visualstudio.com/items?itemName=VisualStudioProductTeam.MicrosoftVisualStudio2017InstallerProjects
This must be installed before you can update or use the 'Setup1' Project. 

This project can be used to create updated installers after modifying the project so it can run on PCs without any extra packages.
Since the project has already been created, once you install the installer package you shouldn't need to do any aditional setup.
Simply select the 'Setup1' project, right click and select 'Build'. Now you should have a new installer under <project-root>\Setup1\Debug<OR>Release.

-----------------------------------------------------------
Using the Google Maps Webpage
-----------------------------------------------------------

Thankfully this shouldn't require any extra setup. It is located under <project-root>\webviewCode\route.html
Running the file should simply open the webpage in your default browser!

**IMPORTANT**
Google Maps requires that you have a valid API key, this is how they keep track of how many requests your website makes and how much they should bill you if you pay for more than the basic package.
It's currently using my API key but Google is changing how these keys are handled (they will require you to set up billing) which means that eventually it will need to be updated.

-----------------------------------------------------------
File Guide
-----------------------------------------------------------
** FOLDERS **
/Debug - This is where the project as a whole gets built to.
/OpenWeatherMapAPI - This contains the Weather API. Un-used in the main program it can get temperature data and cloud coverage.
/Project2 - The code for the main Project is contained here. 
/Setup1 - The installer package creates its installers here.
/UnitTest1 - Some basic Unit Tests for the math code in Sim.h/.cpp
** FILES **
Project2.sln - The solution file for the whole project. Open this in Visual Studio to modify the program.
README.txt - You Are Here! This should contain general info about anything in the project.
