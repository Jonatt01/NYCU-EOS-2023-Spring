# ifndef TRANSLATE_H
# define TRANSLATE_H

/**
* Translate stop to specific index
*/
int whichstop(char *stop);

/**
* Translate stop to specific index
*/
int whichage(char *target);

/**
* Remove spaces in front of the string
*/
void remove_spaces(char* s);

/**
* transfer the final report matrix into the message going to send to clinet
*/
char* from_status_2_report(int array[][3]);

# endif