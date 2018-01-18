To install wxWidgets and use this project in visual studio:

NOTE: We are using VS 2017, different versions have different steps! If you have VS2015 there are some guides online, 
earlier versions should have guides on the wxWiki.

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
Depending on your computer this can take a very long time, The reason for this is wxWidgets has 
different builds for different versions, Aka a release version is different than the debug version 
and it is going to build all of them. It is possible to only build the neccesary versions however 
I was having trouble running some of the sample programs and this fixed it.
It took around 30 minutes on my laptop and 15 on my desktop so only do this when you have the time.

Once that is done you should be set to load our project. Just go to wherever your local github repo folder is and open up the .sln file
that is in the "CapstoneCode" folder. As long as the WXWIN variable is set you shouldn't need to change any of the project properties to
build and run the program.