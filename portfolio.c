#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "rbtree.h"

//Structure used to build the red-black tree of movie records
struct dirent;
struct dirent {
    char n[350];
	char title[300];
	char genre[50];
	char time[50];
	char year[50];
	char media[50];
	char date[50];
    unsigned char mark;
    struct dirent *L, *R;
} dex;

static struct dirent nil = {.mark=0};

//Comparison function for building the red-black tree
static int int_cmp(const void *ai, const void *bi) {
    const struct dirent *a = ai;
    const struct dirent *b = bi;
    return (strncmp(a->n,b->n,350));
}

//Define aspects of red-black tree operations
static rbop_t rbinf = {
    .cmp = int_cmp,
    .coff = (void *)&(dex.L) - (void *)&dex,
    .boff = (void *)&(dex.mark) - (void *)&dex,
    .nil = &nil,
    .mask = 1, 
};

//Struct for linked list of movies
struct LinkedList{
    char movieTitle[300];
	char movieGenre[50];
	char movieTime[50];
	char movieYear[50];
	char movieMedia[50];
	char movieDate[50];
    struct LinkedList *next;
 };

typedef struct LinkedList *node;	//Define node as pointer of data type struct LinkedList

//Struct for linked list of users
struct UserList{
    char user[50];
    struct UserList *next;
 };

typedef struct UserList *userNode;	//Define userNode as pointer of data type struct UserList

//Functions for using my red-black tree
char *to_uppercase(char *s);	//Convert a string to all uppercase characters
void searchMovie(struct dirent *a, char *b);	//Search the red black tree for any movies starting with a particular string
void makeTree();	//Create the red-black tree with the data from the movie_records file

//Functions for creating, retrieving, updating, and deleting log files
userNode createUserNode();	//Initialize node for linked list of users
node createNode();	//Initialize node for linked list of movies associated with a user
userNode addUserNode(userNode userHead, char *user);	//Add to a linked list of all users who have an active log file
node addNode(node head, char *movieTitle, char *movieGenre, char *movieTime, char *movieYear, char *movieMedia, char *movieDate);	//Add a movie with detail information to a linked list
void loadUsers();	//Load from users.txt the list of users who have a log file of movies
int mainInput();	//Main menu when you first enter the program. User can login, delete user logs, or quit the program
void login();	//Prompts the user for a username to login and checks if they already have a log file of movies
void getUser();	//Create a new user or retrieve the movie information for a returning user
void loadExistingMovies();	//Load movies from an existing user's log file
void deleteUser();	//Remove a user and no longer associate with their log file. If the user logs in again they will appear as a new user
int quitProgram();	//Exit the program
int commandInput();	//Show options for altering the current user's log file of movies
void addMovie();	//Add a movie for a user
void editMovie();	//Edit one of the existing movies for a user
void deleteMovie();	//Remove a movie for a user
void printCurrentList();	//Print the list of movie titles currently added to the user
void printFullCurrentList(); //Print the list of movies currently added to the user with all detailed information on the movies
void printUserList();	//Print the list of all usernames that currently have a log file of movies
void saveToLog();	//Save changes of the users movies to the user log file. No changes are permanent until they are saved
void saveUserList();	//Update users.txt with the current set of users with a log file of movies
int logout();	//Logout of this user and go back to the main menu

//Global variables
struct dirent *ent;	//Store the red-black tree of movies
void *tree;	//Pointer to the red-black tree of movies
char const* const fileName = "movie_records";	//Name of the file with movie entries from IMDB
char line[1000];	//String for reading in lines
int lineCount=0;	//Count of lines in movie_records
node head;	//Linked list for movie records matching a search
userNode userHead;	//Linked list for all current users
node currentList;	//Linked list for the movies associated with the current user
char user[50];	//String for storing the username of the current user

int main(int argc, char **argv) {
	//Create red-black tree from IMDB movie records
	tree = rbinf.nil;
	makeTree();
	//Initialize linked lists of movies and users
	userHead = createUserNode();
	head = createNode();
	currentList = createNode();
	//Store active usernames in userHead linked list
	loadUsers();
	//Continuously prompt with the menu and perform selected tasks until user selects to quit
	while (1) {
		//mainInput() returns 1 if user selects to quit, breaking out of the loop
		if (mainInput()) {
			break;
		}
	}
	//Free the space used for the red-black tree of movie records
    free(ent);
    return 0;
}

//Convert a string to all uppercase characters
char    *to_uppercase(char *s) {
    int i = 0;
    char    *str = strdup(s);

    while (str[i])
    {
        if (str[i] >= 97 && str[i] <= 122)
            str[i] -= 32;
        i++;
    }
    return (str);
}

////Search the red black tree for any movies starting with a particular string
void searchMovie(struct dirent *a, char *b) {
	//Compare the beginning of the movie title to the search string
	int d = strncmp(a->n,b,strlen(b));
	//If the strings match add the movie to the linked list of matches and traverse search both of its children
	if (d==0) {
		head = addNode(head,a->title, a->genre,  a->time,  a->year,  a->media,  a->date);
		if(a->R!=&nil) {
			searchMovie(a->R,b);
		}
		if(a->L!=&nil) {
			searchMovie(a->L,b);
		}
	}
	//If strcmp is negative search right childe
	else if(d<0) {
		if(a->R!=&nil) {
			searchMovie(a->R,b);
		}
	}
	//If strcmp is positive search the left child
	else if(d>0) {
		if(a->L!=&nil) {
			searchMovie(a->L,b);
		}
	}
}

//Create the red-black tree with the data from the movie_records file
void makeTree() {
	//Open the movie_records file from IMDB
	FILE* file = fopen(fileName, "r");
	//Count all the lines to know how much to allocate for the tree
	while (fgets(line, sizeof(line), file)) {
		lineCount++;
	}
	fclose(file);
	//Allocate space for the red-black tree of movie records
    if( (ent = malloc(lineCount*sizeof(struct dirent))) == NULL) {
        perror("malloc");
    }
	int lineCount2 = 0;
	FILE* file2 = fopen(fileName, "r");
	//Read in all the movies from the file
	while (fgets(line, sizeof(line), file2)) {
		char *token;
		const char s[2] = "	";
		token = strtok(line,s);
		int i = 0;
		char id[50];
		while (token!=NULL) {
			//Store the id
			if (i==0) {
				strncpy(id,token,50);
			}
			//Store the full title of the movie
			else if (i==2) {
				strncpy(ent[lineCount2].title,token,300);
			}
			//Store the year the movie was released
			else if (i==5) {
				strncpy(ent[lineCount2].year,token,50);
				//Make the key (n) of the movie entry the title in all caps with the year and movie id number concatenated to the end
				char str[50];
				char str2[300];
				strncpy(str,token,50);
				strncpy(str2,ent[lineCount2].title,300);
				strcat(str,id);
				strncpy(ent[lineCount2].n,to_uppercase(strcat(str2,str)),350);
			}
			//Store the running time of the movie in minutes
			else if (i==7) {
				strncpy(ent[lineCount2].time,token,50);
			}
			//Store the genre(s) of the movie
			else if (i==8) {
				char tempGenre[50];
				strncpy(tempGenre,token,50);
				//Remove newline characters from the end of the genre
				char *p;
				if((p=strchr(tempGenre, '\n'))){	//check exist newline
					*p = 0;
				}
				strncpy(ent[lineCount2].genre,tempGenre,50);
				//Remove leading articles from the key (n)
				char str3[300];
				strncpy(str3,ent[lineCount2].n,350);
				char strOriginal[300];
				strncpy(strOriginal,str3,300);
				char *strBeginning = strtok(str3," ");
				char *strRest = strtok(NULL,"");
				if(!strncmp(strBeginning,"THE",50) || !strncmp(strBeginning,"A",50) || !strncmp(strBeginning,"AN",50)) {
					strncpy(ent[lineCount2].n,strRest,350);
				}
				else {
					strncpy(ent[lineCount2].n,strOriginal,350);
				}
			}
			token = strtok (NULL, s);
			i++;
		}
		lineCount2++;
	}
	fclose(file2);
	//Build the red-black tree of movie records that were read in
	int ab = 0;
	for (ab = 0; ab<lineCount2; ab++) {
		if(add_node(&tree, ent+ab, &rbinf) != rbinf.nil) {
			//If a record fails to add to the tree print an error message
			printf("Error for %s\n",ent[ab].n);
		}
	}
    printf("Finished building the tree phase.\n");
	printf("\n\n\n\n");
}

//Initialize node for linked list of users
userNode createUserNode(){
    userNode temp; // declare a node
    temp = (userNode)malloc(sizeof(struct UserList)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new node
}

//Initialize node for linked list of movies associated with a user
node createNode(){
    node temp;	//declare a node
    temp = (node)malloc(sizeof(struct LinkedList));	// allocate memory using malloc()
    temp->next = NULL;	//make next point to NULL
    return temp;	//return the new node
}

//Add to a linked list of all users who have an active log file
userNode addUserNode(userNode userHead, char *user){
    userNode temp,p;	// declare two nodes temp and p
    temp = createUserNode();	//createNode will return a new node with data = value and next pointing to NULL.
    strncpy(temp->user,user,50);	//add element's value to data part of node
    if(userHead == NULL){
        userHead = temp;	//when linked list is empty
    }
    else{
        p  = userHead;	//assign head to p 
        while(p->next != NULL){
            p = p->next;	//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;	//Point the previous last node to the new node created.
    }
    return userHead;
}

//Add a movie with detail information to a linked list
node addNode(node head, char *movieTitle, char *movieGenre, char *movieTime, char *movieYear, char *movieMedia, char *movieDate){
    node temp,p;	//declare two nodes temp and p
    temp = createNode();	//createNode will return a new node with data = value and next pointing to NULL.
    //Store the title, genre, running time, year, media type (initialized as none), and acquired date (today)
	strncpy(temp->movieTitle,movieTitle,300);
	strncpy(temp->movieGenre,movieGenre,50);
	strncpy(temp->movieTime,movieTime,50);
	strncpy(temp->movieYear,movieYear,50);
	strncpy(temp->movieMedia,"none",50);
	//Get the current date
	int month, day, year;
	time_t now;
	time(&now);
	struct tm *local = localtime(&now);
	day = local->tm_mday;
	month = local->tm_mon+1;
	year = local->tm_year+1900;
	char date[50];
	//Convert the date to mm/dd/yyyy format
	if (month<10 && day<10) {
		sprintf(date, "0%d/0%d/%d",month,day,year);
	}
	else if (month>=10 && day<10) {
		sprintf(date, "%d/0%d/%d",month,day,year);
	}
	else if (month<10 && day>=10) {
		sprintf(date, "0%d/%d/%d",month,day,year);
	}
	else {
		sprintf(date, "%d/%d/%d",month,day,year);
	}
	strncpy(temp->movieDate,date,50);
    if(head == NULL){
        head = temp;	//when linked list is empty
    }
    else{
        p  = head;	//assign head to p 
        while(p->next != NULL){
            p = p->next;	//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;	//Point the previous last node to the new node created.
    }
    return head;
}

//Load from users.txt the list of users who have a log file of movies
void loadUsers() {
	FILE* userFile = fopen("users.txt", "r");
	char userLine[60];
	//Store usernames in the userHead linked list
	while (fgets(userLine, sizeof(userLine), userFile)) {
		char *p;
		if((p=strchr(userLine, '\n'))){	//check exist newline
			*p = 0;
		}
		addUserNode(userHead,userLine);
	}
	fclose(userFile);
}

//Main menu when you first enter the program. User can login, delete user logs, or quit the program. Return 1 when quitting the program.
int mainInput() {
	//Print main menu and prompt for input
	printf("Main options:\n0:\tLogin\n1:\tDelete a User\n2:\tQuit\nSelect an option number: ");
	int mainOptionInt = -1;
	scanf("%d", &mainOptionInt);
	int c;
	int optionCount = 3;
	//Flush buffer
	while ((c = fgetc(stdin)) != '\n' && c != EOF);
	//Notify of invalid input
	if (mainOptionInt>=optionCount || mainOptionInt<0) {
		printf("That is not a valid option number\n");
	}
	else {
		//Login to manipulate a user log file of movies
		if (mainOptionInt==0) {
			login();
			while (1) {
				if (commandInput()) {
					break;
				}
			}
			return 0;
		}
		//Delete an active user's movie log
		else if (mainOptionInt==1) {
			deleteUser();
			return 0;
		}
		//Quit the program
		else if (mainOptionInt==2) {
			return quitProgram();
		}
	}
	return 0;
}

//Prompts the user for a username to login and checks if they already have a log file of movies
void login() {
	//Get username and check if it exists
	getUser();
}

//Create a new user or retrieve the movie information for a returning user
void getUser() {
//Label for ensuring valid input
enterUserName:
	//Prompt for a username
	printf("Enter a username: ");
	char userName[50];
	fgets(userName, 50,stdin);
	char *p;
	//Remove new line characters and flush if more than 50 characters were entered
	if((p=strchr(userName, '\n'))){	//check exist newline
		*p = 0;
	} else {	//clear upto newline
		scanf("%*[^\n]");
		scanf("%*c");
		//If username was over 50 characters, prompt for new input
		printf("Username too long (max 50 characters)\n");
		goto enterUserName;
	}
	strncpy(user,userName,50);
	userNode place;
	place = userHead;
	//Use flag to tell if the user is new (0) or returning (1)
	int flag = 0;
	while(place != NULL){
		place = place->next;
		if (place != NULL) {
			char userFill[50];
			//Search linked list of users for the username that was entered
			strncpy(userFill,place->user,50);
			//Fill is the value of the string comparison between usernames
			int fill = strncmp(user,userFill,strlen(user));
			if (fill==0) {
				//If username exists, welcome the user back and load their movies
				printf("Welcome back %s!\n",user);
				loadExistingMovies();
				flag = 1;
				break;
			}
		}
	}
	if (flag==0) {
		//If user was not in the list of active users, add the user to the user list (not yet saved to the users.txt file since they have no movie records saved in their log)
		printf("I see you are new. Creating an account for %s...\n",user);
		addUserNode(userHead,user);
	}
}

//Load movies from an existing user's log file
void loadExistingMovies() {
	//Retrieve in the movie records from the user's log file
	char logFileName[60];
	sprintf(logFileName, "%s.log",user);
	FILE* logFile = fopen(logFileName, "r");
	char userMovieTitle[300];
	char userMovieGenre[300];
	char userMovieTime[300];
	char userMovieYear[300];
	char userMovieMedia[300];
	char userMovieDate[300];
	while (fgets(userMovieTitle, sizeof(userMovieTitle), logFile) && fgets(userMovieGenre, sizeof(userMovieGenre), logFile) && fgets(userMovieTime, sizeof(userMovieTime), logFile) && fgets(userMovieYear, sizeof(userMovieYear), logFile) && fgets(userMovieMedia, sizeof(userMovieMedia), logFile) && fgets(userMovieDate, sizeof(userMovieDate), logFile)) {
		//Remove all newline characters
		char *p;
		if((p=strchr(userMovieTitle, '\n'))){	//check exist newline
			*p = 0;
		}
		if((p=strchr(userMovieGenre, '\n'))){	//check exist newline
			*p = 0;
		}
		if((p=strchr(userMovieTime, '\n'))){	//check exist newline
			*p = 0;
		}
		if((p=strchr(userMovieYear, '\n'))){	//check exist newline
			*p = 0;
		}
		if((p=strchr(userMovieMedia, '\n'))){	//check exist newline
			*p = 0;
		}
		if((p=strchr(userMovieDate, '\n'))){	//check exist newline
			*p = 0;
		}
		//Add the movie records to the linked list of movies associated with the active user
		currentList = addNode(currentList,userMovieTitle, userMovieGenre,  userMovieTime,  userMovieYear,  userMovieMedia,  userMovieDate);
	}
	fclose(logFile);
}

//Remove a user and no longer associate with their log file. If the user logs in again they will appear as a new user
void deleteUser() {
	userNode place;
	place = userHead;
	char deleteUser[50];
	//If there are users in the list find and delete the chosen user
	if (place->next!=NULL) {
		if (place->next!=NULL) {
		//Label to ensure a valid user number is selected from the list
		enterDeleteUser:
			//Prompt for which user to delete
			place = userHead;
			printUserList();
			printf("Enter the number of the user you want to delete: ");
			int readingInt = -1;
			scanf("%d", &readingInt);
			int c;
			int userCount = -1;
			//Count the number of users in the linked list
			while(place != NULL){
				place = place->next;
				userCount++;
			}
			//flush buffer
			while ((c = fgetc(stdin)) != '\n' && c != EOF);
			//Validate the entered user number
			if (readingInt>=(userCount) || readingInt<0) {
				printf("That is not a valid user number\n");
				goto enterDeleteUser;
			}
			//Delete the user from the list
			else {
				printf("Deleting...\n");
				userNode place;
				place = userHead;
				int userCounter = 0;
				while(place != NULL){
					if (userCounter == readingInt) {
						strncpy(deleteUser,place->next->user,50);
						place->next=place->next->next;
					}
					place = place->next;
					userCounter++;
				}
			}
		}
		printf("Deleted %s\n",deleteUser);
		//Update the user list and display it
		saveUserList();
		printUserList();
	}
	//If user list is empty there are none to delete...move along
	else {
		printf("There are no users to delete.\n");
	}
}

//Exit the program returning 1 means exit
int quitProgram() {
//Label to ensure a valid selection of whether or not to quit
enterQuitting:
	//Confirm the user wants to quit
	printf("Are you sure you want to quit?\n0:\tYes\n1:\tNo\nSelect an option number: ");
	int quittingInt = -1;
	scanf("%d", &quittingInt);
	int c;
	//Flush buffer
	while ((c = fgetc(stdin)) != '\n' && c != EOF);
	//Validate the inputted number
	if (quittingInt>=2 || quittingInt<0) {
		printf("That is not a valid option number\n");
		goto enterQuitting;
	}
	//Exit the program
	if (quittingInt==0) {
		printf("Goodbye\n");
		return 1;
	}
	else {
		return 0;
	}
	
}

//Show options for altering the current user's log file of movies
int commandInput() {
	//Show list of options for editing a log file of movies and prompt for a selection
	printf("Command options:\n0:\tShow Current Movies\n1:\tAdd a Movie\n2:\tEdit a Movie Entry\n3:\tPrint Full Information of Movie List\n4:\tDelete a Movie\n5:\tSave to Log\n6:\tLogout\n\nSelect a command number: ");
	int commandInt = -1;
	scanf("%d", &commandInt);
	int c;
	int commandCount = 7;
	//Flush buffer
	while ((c = fgetc(stdin)) != '\n' && c != EOF);
	//Validate the input number
	if (commandInt>=commandCount || commandInt<0) {
		printf("That is not a valid command number\n");
	}
	else {
		//Print the list of movie titles associated with the user
		if (commandInt==0) {
			printCurrentList();
		}
		//Add a movie to the user
		else if (commandInt==1) {
			addMovie();
		}
		//Edit one of the user's movies for media type or date it was acquired
		else if (commandInt==2) {
			editMovie();
		}
		//Print the detailed list of movie record information
		else if (commandInt==3) {
			printFullCurrentList();
		}
		//Delete a movie from the user's log of movies
		else if (commandInt==4) {
			deleteMovie();
		}
		//Save movie changes to the log file for this user
		else if (commandInt==5) {
			saveToLog();
		}
		//Logout back to main menu
		else if (commandInt==6) {
			if (logout()) {
				userHead = createUserNode();
				loadUsers();
				currentList = createNode();
				return 1;
			}
			else {
				return 0;
			}
		}
	}
	return 0;
}

//Add a movie for a user
void addMovie() {
//Label to ensure a movie title less than 200 characters is entered.
enterMovieName:
	//Prompt for a movie title
	printf("Enter a movie: ");
	char readingStr[200];
	fgets(readingStr, 200,stdin);
	//Remove leading articles and convert to all caps to search
	strncpy(readingStr,to_uppercase(readingStr),200);
	char str3[200];
	strncpy(str3,readingStr,200);
	char strOriginal[200];
	strncpy(strOriginal,str3,200);
	char *strBeginning = strtok(str3," ");
	char *strRest = strtok(NULL,"");
	if(!strncmp(strBeginning,"THE",50) || !strncmp(strBeginning,"A",50) || !strncmp(strBeginning,"AN",50)) {
		strncpy(readingStr,strRest,200);
	}
	else {
		strncpy(readingStr,strOriginal,200);
	}
	//Remove newline character
	char *p;
	if((p=strchr(readingStr, '\n'))){	//check exist newline
		*p = 0;
	} 
	//Flush buffer and prompt for new input if movie name entered is too long
	else {	//clear upto newline
		scanf("%*[^\n]");
		scanf("%*c");
		printf("Name is too long (limited to 200 characters)\n");
		goto enterMovieName;
	}
	//Find all movies that start with the search term
	searchMovie(tree, readingStr);
	node place;
	place = head;
	int movieCount = 0;
	//Display the movies returned from the search and ask which one to add to the log
	if (place->next!=NULL) {
		while(place != NULL){
			place = place->next;
			if (place != NULL) {
				printf("%d:\t%s (%s)\n",movieCount,place->movieTitle,place->movieYear);
			}
			movieCount++;
		}
	//Label to ensure that a valid movie number is selected
	enterMovieNumber:
		printf("Enter the number of the movie you want to add: ");
		int readingInt = -1;
		scanf("%d", &readingInt);
		int c;
		int movieCounter = 0;
		//Flush buffer
		while ((c = fgetc(stdin)) != '\n' && c != EOF);
		//Validate input number
		if (readingInt>=(movieCount-1) || readingInt<0) {
			printf("That is not a valid movie number\n");
			goto enterMovieNumber;
		}
		//Add the selected movie to the user's list of movies
		else {
			printf("\nAdding...\n");
			place = head;
			while(place != NULL){
				place = place->next;
				if (movieCounter == readingInt) {
					currentList = addNode(currentList,place->movieTitle, place->movieGenre,  place->movieTime,  place->movieYear,  place->movieMedia,  place->movieDate);
				}
				movieCounter++;
			}
		}
		//Clear the list of movies searched
		head = createNode();
	}
	else {
		printf("No movies found for that search.\n");
	}
}

//Edit one of the existing movies for a user. Can change media type and date acquired
void editMovie() {
	//Show current movies and ask which one to edit
	printCurrentList();
	node place;
	place = currentList;
	if (place->next!=NULL) {
	//Label to ensure valid movie number is selected
	enterEditMovie:
		place = currentList;
		printf("Enter the number of the movie you want to edit: ");
		int readingInt = -1;
		scanf("%d", &readingInt);
		int c;
		//Validate movie number selected
		int movieCount = -1;
		while(place != NULL){
			place = place->next;
			movieCount++;
		}
		//Flush buffer
		while ((c = fgetc(stdin)) != '\n' && c != EOF);
		//Notify of invalid input
		if (readingInt>=(movieCount) || readingInt<0) {
			printf("That is not a valid movie number\n");
			goto enterEditMovie;
		}
		else {
		//Label to ensure valid editing option is selected
		enterOptionNumber:
			//User can change media type or the date the movie was added to the log
			printf("Editing Options:\n0:\tEdit Media Type\n1:\tEdit Date Added\n\nSelect an editing option: "); 
			int optionInt = -1;
			scanf("%d", &optionInt);
			int c;
			//Flush buffer
			while ((c = fgetc(stdin)) != '\n' && c != EOF);
			//Notify of invalid input
			if (optionInt>=2 || optionInt<0) {
				printf("That is not a valid option number\n");
				goto enterOptionNumber;
			}
			//Change Media Type
			else if (optionInt==0) {
			//Label to ensure valid media type number is selected
			enterMediaNumber:
				//Media type options are DVD, Bluray, and Digital
				printf("Media Options:\n0:\tDVD\n1:\tBluray\n2:\tDigital\n\nSelect the media type to set: ");
				int mediaInt = -1;
				scanf("%d", &mediaInt);
				int c;
				//Flush buffer
				while ((c = fgetc(stdin)) != '\n' && c != EOF);
				//Notify of invalid input
				if (mediaInt>=3 || mediaInt<0) {
					printf("That is not a valid media type number\n");
					goto enterMediaNumber;
				}
				//Update media type as selected
				else if (mediaInt == 0) {
					node place;
					place = currentList;
					int movieCounter = 0;
					while(place != NULL){
						place = place->next;
						if (movieCounter == readingInt) {
							strncpy(place->movieMedia,"DVD",50);
						}
						movieCounter++;
					}
				}
				else if (mediaInt == 1) {
					node place;
					place = currentList;
					int movieCounter = 0;
					while(place != NULL){
						place = place->next;
						if (movieCounter == readingInt) {
							strncpy(place->movieMedia,"Bluray",50);
						}
						movieCounter++;
					}
				}
				else if (mediaInt == 2) {
					node place;
					place = currentList;
					int movieCounter = 0;
					while(place != NULL){
						place = place->next;
						if (movieCounter == readingInt) {
							strncpy(place->movieMedia,"Digital",50);
						}
						movieCounter++;
					}
				}
			}
			//Change date the movie was added
			else if (optionInt==1) {
			//Label to ensure valid month number is entered (1-12)
			enterMonthNumber:
				printf("Enter the month number (1-12): ");
				int monthInt = -1;
				scanf("%d", &monthInt);
				int c;
				//Flush buffer
				while ((c = fgetc(stdin)) != '\n' && c != EOF);
				//Notify user of invalid input
				if (monthInt>12 || monthInt<1) {
					printf("That is not a valid month number\n");
					goto enterMonthNumber;
				}
				else {
				//Label to ensure valid day number is entered (1-31) (does not check if the month actually has those days, if a user wants to claim a movie was added Feb 30 they can)
				enterDayNumber:
					printf("Enter the day number (1-31): ");
					int dayInt = -1;
					scanf("%d", &dayInt);
					int c;
					//Flush buffer
					while ((c = fgetc(stdin)) != '\n' && c != EOF);
					//Notify user of invalid input
					if (dayInt>31 || dayInt<1) {
						printf("That is not a valid day number\n");
						goto enterDayNumber;
					}
					else {
					//Label to ensure valid year number is entered (1900-3000)
					enterYearNumber:
						printf("Enter the year number (1900-3000): ");
						int yearInt = -1;
						scanf("%d", &yearInt);
						int c;
						//Flush buffer
						while ((c = fgetc(stdin)) != '\n' && c != EOF);
						//Notify user of invalid input
						if (yearInt>3000 || yearInt<1900) {
							printf("That is not a valid year number\n");
							goto enterYearNumber;
						}
						else {
							//Convert date to mm/dd/yyyy format
							char date[50];
							if (monthInt<10 && dayInt<10) {
								sprintf(date, "0%d/0%d/%d",monthInt,dayInt,yearInt);
							}
							else if (monthInt>=10 && dayInt<10) {
								sprintf(date, "%d/0%d/%d",monthInt,dayInt,yearInt);
							}
							else if (monthInt<10 && dayInt>=10) {
								sprintf(date, "0%d/%d/%d",monthInt,dayInt,yearInt);
							}
							else {
								sprintf(date, "%d/%d/%d",monthInt,dayInt,yearInt);
							}
							node place;
							place = currentList;
							int movieCounter = 0;
							//Update the date
							while(place != NULL){
								place = place->next;
								if (movieCounter == readingInt) {
									strncpy(place->movieDate,date,50);
								}
								movieCounter++;
							}
						}
					}
				}
			}
		}
	}
}

//Remove a movie for a user
void deleteMovie() {
	//Display movies currently associated with the user
	printCurrentList();
	node place;
	place = currentList;
	if (place->next!=NULL) {
	//Label to ensure a valid movie number is entered to delete
	enterDeleteNumber:
		place = currentList;
		printf("Enter the number of the movie you want to delete: ");
		int readingInt = -1;
		scanf("%d", &readingInt);
		int c;
		//Validate the movie number
		int movieCount = -1;
		while(place != NULL){
			place = place->next;
			movieCount++;
		}
		//Flush buffer
		while ((c = fgetc(stdin)) != '\n' && c != EOF);
		//Notify user of invalid input
		if (readingInt>=(movieCount) || readingInt<0) {
			printf("That is not a valid movie number\n");
			goto enterDeleteNumber;
		}
		//Delete the movie from the user's linked list of movies
		else {
			printf("Deleting...\n");
			node place;
			place = currentList;
			int movieCounter = 0;
			while(place != NULL){
				if (movieCounter == readingInt) {
					place->next=place->next->next;
				}
				place = place->next;
				movieCounter++;
			}
		}
	}
}

//Print the list of movie titles currently added to the user
void printCurrentList() {
	node place;
	place = currentList;
	int movieCounts = 0;
	if (place->next==NULL) {
		printf("There are currently no movies for this user.\n");
	}
	else {
		printf("The movies currently added for this user are:\n");
	}
	while(place != NULL){
		place = place->next;
		if (place != NULL) {
			printf("%d:\t%s\n",movieCounts,place->movieTitle);
		}
		movieCounts++;
	}
}

//Print the list of movies currently added to the user with all detailed information on the movies
void printFullCurrentList() {
	node place;
	place = currentList;
	int movieCounts = 0;
	if (place->next==NULL) {
		printf("There are currently no movies for this user.\n");
	}
	else {
		printf("The movies currently added for this user are:\n");
	}
	while(place != NULL){
		place = place->next;
		if (place != NULL) {
			printf("%d:\n\tTitle\t\t%s\n\tGenre\t\t%s\n\tRunning Time\t%s minutes\n\tYear\t\t%s\n\tMedia Type\t%s\n\tDate Added\t%s (mm/dd/yyyy)\n",movieCounts,place->movieTitle, place->movieGenre,  place->movieTime,  place->movieYear,  place->movieMedia,  place->movieDate);
		}
		movieCounts++;
	}
}

//Print the list of all usernames that currently have a log file of movies
void printUserList() {
	userNode place;
	place = userHead;
	int userCounts = 0;
	if (place->next==NULL) {
		printf("There are currently no users.\n");
	}
	else {
		printf("The users currently existing are:\n");
	}
	while(place != NULL){
		place = place->next;
		if (place != NULL) {
			printf("%d:\t%s\n",userCounts,place->user);
		}
		userCounts++;
	}
}

//Save changes of the users movies to the user log file. No changes are permanent until they are saved
void saveToLog() {
	//Show the list of active users that have a movie log
	printUserList();
	node place;
	place = currentList;
	//If there are no movies in the user's list remove the user from the active user list
	if (place->next==NULL) {
		printf("There are currently no movies for this user. Nothing to save.\n");
		printf("Removing the user from the user list...\n");
		userNode place;
		place = userHead;
		while(place != NULL){ 
			if (place->next!=NULL) {
				char userFill[50];
				strncpy(userFill,place->next->user,50);
				int fill = strncmp(user,userFill,strlen(user));
				if (fill == 0) {
					place->next=place->next->next;
					saveUserList();
					addUserNode(userHead,user);
					break;
				}
			}
			place = place->next;
		}
	}
	//If the user has movies save the records to their log and update the list of users in users.txt
	else {
		saveUserList();
		char logFileName[60];
		sprintf(logFileName, "%s.log",user);
		FILE* logFile = fopen(logFileName, "w");
		while(place != NULL){
			place = place->next;
			if (place != NULL) {
				fprintf(logFile,"%s\n%s\n%s\n%s\n%s\n%s\n",place->movieTitle, place->movieGenre,  place->movieTime,  place->movieYear,  place->movieMedia,  place->movieDate);
			}
		}
		fclose(logFile);
	}
}

//Update users.txt with the current set of users with a log file of movies
void saveUserList() {
	FILE* userFile = fopen("users.txt", "w");
	userNode place;
	place = userHead;
	if (place->next==NULL) {
		printf("There are no users.\n");
	}
	while(place != NULL){
		place = place->next;
		if (place != NULL) {
			fprintf(userFile,"%s\n",place->user);
		}
	}
	fclose(userFile);
}

//Logout of this user and go back to the main menu, return 1 if logging out
int logout() {
enterLogout:
	printf("Are you sure you want to logout without saving?\n0:\tYes\n1:\tNo\nSelect an option number: ");
	int quittingInt = -1;
	scanf("%d", &quittingInt);
	int c;
	while ((c = fgetc(stdin)) != '\n' && c != EOF);
	if (quittingInt>=2 || quittingInt<0) {
		printf("That is not a valid option number\n");
		goto enterLogout;
	}
	if (quittingInt==0) {
		printf("Logging out\n");
		return 1;
	}
	else {
		return 0;
	}
}
