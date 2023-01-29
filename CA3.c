#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE_LEN 15

#define TRUE 1
#define FALSE 0

// define switch
#define NOTHING 0
#define SIGNUP 1
#define LOGGIN 2
#define POST 3
#define LOGOUT 4
#define DELETE 5
#define INFO 6
#define FIND_USER 7
#define NO_LOGGIN 8

// define dynamic_memory function
#define SUCCESSFUL 1
#define NOT_SUCCESFUL -1

// define search function
#define FIND 1
#define NOT_FIND 0

typedef struct SNuser
{
	char *username;
	char *password;
	int user_id;
	int last_post_id;
	struct SNuser *next;

} SNuser;

typedef struct SNmsg
{
	char *content;
	char *user_name;
	int post_id;
	struct SNmsg *next;
} SNmsg;

void menu(SNuser *loggin);
int get_mode_word(SNuser *loggin);
int signup(SNuser *head, SNuser **loggin);
int dynamic_memory_1(char **in_out_put);
int dynamic_memory_2(char **in_out_put);
int search_name(SNuser *head, SNuser **destination, char *name);
int login(SNuser *head, SNuser **loggin);
int post(SNuser *loggin, SNmsg *head);
int search_post(SNmsg *head, SNmsg **destination, char *username, int post_id);
void logout(SNuser **loggin);
int delete(SNmsg *head, SNuser *loggin);
int info(SNuser *user, SNmsg *head);
int find_user(SNuser *UserHead, SNmsg *PostHead);

void menu(SNuser *loggin)
{
	printf("Things you can do:\n");

	if (loggin == NULL)
	{
		printf("{{{\n");
		printf("\tsignup <name> <password>\n");
		printf("\tloggin <name> <password>\n");
		printf("}}}\n");
	}
	else
	{
		printf("{{{\n");
		printf("\tpost <txt>\n");
		printf("\tlogout\n");
		printf("\tdelete <post_id>\n");
		printf("\tinfo\n");
		printf("\tfind_user <username>\n");
		printf("}}}\n");
	}
}

int get_mode_word(SNuser *loggin)
{
	int counter;
	char ch;
	char array[MAX_SIZE_LEN];

	// '\n' for logout,info and space for others in getchar
	for (counter = 0; (ch = getchar()) != ' ' && ch != '\n'; counter++)
	{
		array[counter] = ch;
	}

	// last char is NULL
	array[counter] = '\0';

	int mode_word = 0;

	if (strcmp(array, "signup") == 0)
		mode_word = SIGNUP;

	else if (strcmp(array, "loggin") == 0)
		mode_word = LOGGIN;

	else if (strcmp(array, "post") == 0)
		mode_word = POST;

	else if (strcmp(array, "logout") == 0)
		mode_word = LOGOUT;

	else if (strcmp(array, "delete") == 0)
		mode_word = DELETE;

	else if (strcmp(array, "info") == 0)
		mode_word = INFO;

	else if (strcmp(array, "find_user") == 0)
		mode_word = FIND_USER;

	if (( mode_word == POST || mode_word == DELETE || mode_word == FIND_USER || mode_word == INFO) && loggin == NULL)
	{
		mode_word = NO_LOGGIN;
	}

	return mode_word;
}

int signup(SNuser *head, SNuser **loggin)
{
	if (*loggin != NULL)
	{
		printf("You are already logged in\n");
		return 0;
	}

	char *name = NULL;
	char *password = NULL;

	int check1 = dynamic_memory_1(&name);
	if (check1 != SUCCESSFUL)
	{
		printf("error dynamic allocat for name\n");
		return -1;
	}

	int check2 = dynamic_memory_2(&password);
	if (check2 != SUCCESSFUL)
	{
		printf("error dynamic allocat for password\n");
		return -1;
	}

	// add to linke list and check it by search name
	SNuser *last_user = NULL;
	if (search_name(head, &last_user, name) == FIND)
	{
		printf("choose another name\n");
		free(name);
		free(password);
		return -2;
	}

	SNuser *new_user = (SNuser *)malloc(sizeof(SNuser));
	if (new_user == NULL)
	{
		printf("error for memory signup\n");
		free(name);
		free(password);
		return -3;
	}
	static int user_id = 1;
	new_user->user_id = user_id;
	new_user->username = name;
	new_user->password = password;
	new_user->last_post_id = 1;

	last_user->next = new_user;
	new_user->next = NULL;

	user_id++;

	*loggin = new_user;

	printf("thank you for registering\n\n");
	printf("this is your information:\n");
	printf("{{{\n");
	printf("id: %d\n", new_user->user_id);
	printf("name: %s\n", new_user->username);
	printf("password: %s\n", new_user->password);
	printf("}}}\n\n");

	return 1;
}

// end with space or '\n'
int dynamic_memory_1(char **in_out_put)
{
	char *str;
	char ch;
	int size = 0;
	str = (char *)malloc((sizeof(char)));
	for (size = 0; (ch = getchar()) != ' ' && (ch != '\n'); size++)
	{
		str = (char *)realloc(str, (size + 1) * sizeof(char));
		str[size] = ch;
	}

	size++;
	// one more memory for NULL
	str = (char *)realloc(str, (size) * sizeof(char));
	if (str == NULL)
	{
		printf("error for memory\n");
		free(str);
		return SUCCESSFUL;
	}

	// last char in NULL
	str[size - 1] = '\0';

	*in_out_put = str;

	return SUCCESSFUL;
}

// end with \n
int dynamic_memory_2(char **in_out_put)

{
	char *str;
	char ch;
	int size = 0;
	str = (char *)malloc((sizeof(char)));
	for (size = 0; (ch = getchar()) != '\n'; size++)
	{
		str = (char *)realloc(str, (size + 1) * sizeof(char));
		str[size] = ch;
	}

	// size++;
	// one more memory for NULL
	str = (char *)realloc(str, (size) * sizeof(char));
	if (str == NULL)
	{
		printf("error for memory\n");
		free(str);
		return SUCCESSFUL;
	}

	// last char in NULL
	str[size] = '\0';

	*in_out_put = str;

	return SUCCESSFUL;
}

// destination will be our desired name (if FIND)  or will be the last name (if NOT_FIND)
int search_name(SNuser *head, SNuser **destination, char *name)
{
	SNuser *cur = head;
	SNuser *pre = NULL;

	while (cur->next != NULL)
	{
		if (strcmp(cur->next->username, name) == 0)
		{
			*destination = cur->next;
			return FIND;
		}
		pre = cur;
		cur = cur->next;
	}
	*destination = cur;
	return NOT_FIND;
}

int login(SNuser *head, SNuser **loggin)
{
	char *name = NULL;
	char *password = NULL;
	SNuser *account = NULL;

	int check1 = dynamic_memory_1(&name);

	if (check1 != SUCCESSFUL)
		return -1;

	int check2 = dynamic_memory_2(&password);
	if (check2 != SUCCESSFUL)
		return -1;

	if (*loggin != NULL)
	{
		printf("You are already in an account.\n");
		free(name);
		free(password);
		return -2;
	}

	if (search_name(head, &account, name) == NOT_FIND)
	{
		printf("User not found\nTry again\n");
		free(name);
		free(password);
		return -3;
	}

	if (strcmp(account->password, password) != 0)
	{
		printf("The password is incorrect\nMake sure The password is correct\n");
		free(name);
		free(password);
		return -4;
	}

	*loggin = account;

	printf("welcome back\n");
	printf("this is your information:\n");
	printf("{{{\n");
	printf("id: %d\n", (*loggin)->user_id);
	printf("name: %s\n", (*loggin)->username);
	printf("password: %s\n", (*loggin)->password);
	printf("}}}\n");

	free(name);
	free(password);
	return 1;
}

int post(SNuser *loggin, SNmsg *head)
{
	char *content = NULL;
	int check = dynamic_memory_2(&content);
	if (check != SUCCESSFUL)
	{
		
		printf("error\n can't get your text\n");
		return -1;
	}

	SNmsg *new_post = (SNmsg *)malloc(sizeof(SNmsg));
	if (new_post == NULL)
	{
		printf("error for memory\n");
		free(content);
		return -2;
	}

	new_post->post_id = loggin->last_post_id++;
	new_post->user_name = loggin->username;
	new_post->content = content;
	new_post->next = NULL;

	SNmsg *last_post = NULL;
	search_post(head, &last_post, new_post->user_name, new_post->post_id);
	// after search last_post will be the last

	// add to link list
	last_post->next = new_post;

	new_post->next = NULL;

	printf("{\n");
	printf("user: %s\n", new_post->user_name);
	printf("post_id: %d\n", new_post->post_id);
	printf("post: %s\n", new_post->content);
	printf("}\n");

	return 1;
}

// this function return pre_destination (if find post) or last (if not find post)
int search_post(SNmsg *head, SNmsg **destination, char *username, int post_id)
{
	SNmsg *cur = head;
	SNmsg *pre = NULL;
	while (cur != NULL)
	{
		if ((strcmp(cur->user_name, username)) == 0 && cur->post_id == post_id)
		{
			*destination = pre;

			return FIND;
		}
		pre = cur;
		cur = cur->next;
	}
	// if not find , destination will be the last
	*destination = pre;

	return NOT_FIND;
}

void logout(SNuser **loggin)
{
	if (*loggin != NULL)
	{
		*loggin = NULL;
		printf("you have succesfully loggin out of your account\nbye\n");
	}
	else
	{
		printf("you are not logged into any account\n\n");
	}
}

int delete(SNmsg *head, SNuser *loggin)
{
	char *post_id = NULL;
	int flag = dynamic_memory_2(&post_id);
	if (flag != SUCCESSFUL)
	{
		printf("can't get post id.\nTry again.\n");
		free(post_id);
		return -1;
	}

	SNmsg *cur = NULL;
	flag = search_post(head, &cur, loggin->username, atoi(post_id));
	if (flag != FIND)
	{
		printf("Post not found.\n");
		free(post_id);
		return -2;
	}

	// perform the delete operation
	// we want to delete cur->next

	SNmsg *pre;
	SNmsg *next;
	pre = cur;
	next = pre->next->next;
	pre->next = next;

	printf("The deletion was successful");

	free(cur->content);
	free(cur);
	free(post_id);

	return 1;
}

int info(SNuser *user, SNmsg *head)
{
	printf("{{{\n");
	printf("id: %d\n", user->user_id);
	printf("username: %s\n", user->username);
	printf("password: %s\n", user->password);
	for (int i = 1; i <= user->last_post_id; i++)
	{
		SNmsg *cur = NULL;
		int check = search_post(head, &cur, user->username, i);
		if (check == FIND)
		{
			printf("\tpost%i: %s\n ", cur->next->post_id, cur->next->content);
		}
	}
	printf("}}}\n");
	return 1;
}

int find_user(SNuser *UserHead, SNmsg *PostHead)
{
	char *username = NULL;
	int flag = dynamic_memory_2(&username);

	if (flag != SUCCESSFUL)
	{
		printf("error for memory.\n");
		free(username);
		return -1;
	}

	SNuser *user = NULL;
	flag = search_name(UserHead, &user, username);
	if (flag == NOT_FIND)
	{
		printf("User was not found.\n");
		free(username);
		return -2;
	}

	info(user, PostHead);
	free(username);
	return 1;
}

void main(void)
{

	SNuser *loggin = NULL;

	SNuser *UserHead = (SNuser *)malloc(sizeof(SNuser));

	SNmsg *PostHead = (SNmsg *)malloc(sizeof(SNmsg));

	printf("Welcome to our website\n");
	while (TRUE)
	{
		menu(loggin);
		switch (get_mode_word(loggin))
		{
		case SIGNUP:
			signup(UserHead, &loggin);
			break;
		case LOGGIN:
			login(UserHead, &loggin);
			break;
		case POST:
			post(loggin, PostHead);
			break;
		case LOGOUT:
			logout(&loggin);
			break;
		case DELETE:
			delete (PostHead, loggin);
			break;
		case INFO:
			info(loggin, PostHead);
			break;
		case FIND_USER:
			find_user(UserHead, PostHead);
			break;
		case NO_LOGGIN:
			printf("first loggin to your account then try again.\n");
			break;
		default:
			printf("Error\n");
			printf("I don't now what do you want to do\n");
			break;
		}
	}
}