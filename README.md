# quinnportfolio
# Brandon Quinn		March 29, 2019		CS 201		University of Alabama
This is my portfolio project for CS 201.
The project gets movie records from IMDB and stores them in a red-black tree for searching. 
The red-black tree builds with relatively few rotations in O(nlgn) time and allows for searches in O(lgn) time.
The users can then create individual catalogs of movies, adding new movies, editing existing movies in the log, and deleting movies from the log.
If the user has already created a log file it will be retrieved for them when they login.
After cloning the repository, to get the movie records from IMDB run:
	make full
Then to compile the actual project and run the code do:
	make
The tree will build (this takes a few seconds) and then it will prompt with the main menu.
The user can:
	Login
	Delete the log file of a user
	Quit the program
After logging in, the user can add movies to their log, edit movie entries to change the media type or date it was added, delete movies, 
and display the movie records associated with the user either as a list of titles with years or the full information (title, genre, running time, year, media type, and date added).
The user must select to save to their log file for changes to become permanent. If a user logs out before saving, changes will not be reflected.
For entering movie titles to search, the user is limited to 200 characters (which was longer than all titles in the database).
For entering most other data the user is limited to 50 characters (which is more than enough).

Link to demonstration video:
	https://alabama.box.com/s/kt0zznhird99inin30w9rv38oq6vd2ee

*If the user is asked for a number and enters a valid number followed by a space and any other entry, the number is taken as input and the rest is ignored*
**Usernames should be entered with letters and numbers only. They are case sensitive**
***For storing and searching in the red-black tree of movie records, everything is converted to all capital letters and leading articles (a, an, the) are ignored)***

I would like to acknowledge David M. Rogers from the University of South Florida who created the red-black tree I used in this project.
His code is shown in rbtree.c and rbtree.h and is available under the GNU General Public License as published by the Free Software Foundation.