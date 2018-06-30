#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NAME_LENGTH 200 



typedef struct book {

	char name[NAME_LENGTH];
	int copies;
	struct book *next;
} Book;
Book* new_book(char* name, int copiesnum) //creating a new book, type struct
{
	Book *newbook = (Book*)malloc(sizeof(Book));

	if (newbook == NULL)
	{
		printf("Error: memory allocation failed\n");
		exit(1);
	}
	strcpy(newbook->name, name);

	newbook->copies = copiesnum;
	newbook->next = NULL;
	return newbook;

}
Book* add_book(Book *head, Book *to_add) // adding a new book to its rightful place
{
	Book *curr, *prev = NULL;
	if (head == NULL)
	{
		return to_add;
	}

	if (strcmp(to_add->name, head->name)<0)
	{
		to_add->next = head;
		return to_add;
	}
	curr = head;
	while (curr != NULL  &&  strcmp(to_add->name, curr->name)>0)
	{
		prev = curr;
		curr = curr->next;
	}
	prev->next = to_add;
	to_add->next = curr;
	return head;
}
Book* returnbook(Book *head, char* name, int copiesnum)
{
	// first we try to find if there is exsiting copies of the book.
	Book *newbook;
	Book *curr = head;
	while (curr != NULL)
	{
		if (strcmp(curr->name, name) == 0)
		{
			curr->copies += copiesnum;
			return head;

		}
		curr = curr->next;
	}
	// if we got this far, it means we need to add new node of new book.
	newbook = new_book(name, copiesnum);
	return(add_book(head, newbook));

}



void borrow(Book *head, char* name, int copiesnum)
{
	Book *curr = head;
	while (curr != NULL)
	{
		if (strcmp(curr->name, name) == 0)
		{
			if (curr->copies < copiesnum)
			{
				curr->copies = 0;
				return;
			}
			else
			{
				curr->copies -= copiesnum;
				return;
			}
		}
		curr = curr->next;
	}
	
}

Book* renamebook(Book *head, char* oldname, char* newname)
{
	int copiesnum;
	Book *curr = head, *previous = NULL, *newbook;
	if (curr == NULL)
	{
		return NULL; //empty list
	}
	if (strcmp(curr->name, oldname)==0) //first on the list
	{

		copiesnum = curr->copies;
		head = head->next;
		free(curr);
		newbook = new_book(newname, copiesnum);
		return add_book(head, newbook);
	}
	previous = curr;
	curr = curr->next;
	while (curr != NULL)
	{
		if (strcmp(curr->name, oldname)==0)
		{

			copiesnum = curr->copies;
			previous->next = curr->next;
			free(curr);
			newbook = new_book(newname, copiesnum);
			return add_book(head, newbook);

		}
		previous = curr;
		curr = curr->next;
	}
	return head;
}


void free_list(Book *head)
{
	if (head == NULL)
	{
		return;
	}
	free_list(head->next);
	free(head);
}
int main(int argc, char *argv[])
{
	Book  *curr, *head = NULL; //pointer to the head of the connected list
	int i, marker, copiesnum; //simple counter, and marker to mark the index of $
	char line[400], name[NAME_LENGTH], newname[NAME_LENGTH], num[NAME_LENGTH], cmd[4];//reading each line, and book name, and maybe new name(in the case of renaming)
	int name_len;
	if (argc != 4)
	{
		printf("Error: invalid number of arguments (<%d> instead of 4)\n", argc);
		return 1;
	}
	FILE *fr = NULL, *fw = NULL, *fo = NULL;
	fr = fopen(argv[1], "r"); // file to read
	fo = fopen(argv[2], "r"); //file of orders
	fw = fopen(argv[3], "w"); //file to write
	if (fr == NULL) {

		printf("Error: opening %s failed\n", argv[1]);
		return 1;
	}
	if (fo == NULL) {

		printf("Error: opening %s failed\n", argv[2]);
		return 1;
	}

	while (!feof(fo)) // now reading the order file
	{
		fgets(line, 400, fo);
		name_len = strlen(line);
		strncpy(cmd, line, 3);// finding the correct command, return, borrown or rename
		cmd[3] = '\0';
		if (strcmp(cmd, "Ret")==0)// return command
		{
			for (i = 11; i < name_len; i++)
			{

				if (line[i]== '$')
				{
					marker = i;
					break;
				}

			}
			strncpy(name, line + 11, marker - 12);
			name[marker - 12] = '\0';
			marker += 4;
			copiesnum = atoi(strncpy(num, line + marker, name_len - marker));
			head = returnbook(head, name, copiesnum);

		}
		else if (strcmp(cmd, "Bor")==0) // borrow command
		{
			for (i = 11; i < name_len; i++)
			{

				if (line[i]== '$')
				{
					marker = i;
					break;
				}

			}
			strncpy(name, line + 11, marker - 12);
			name[marker - 12] = '\0';
			marker += 4;
			copiesnum = atoi(strncpy(num, line + marker, name_len - marker));
			borrow(head, name, copiesnum);
		}
		else if (strcmp(cmd, "Ren") == 0) // rename command
		{
			for (i = 11; i < name_len; i++)
			{

				if (line[i]== '$')
				{
					marker = i;
					break;
				}

			}
			strncpy(name, line + 11, marker - 12);
			name[marker - 12] = '\0';
			marker += 4;
			for (i = marker; i < name_len; i++)
			{
				if (line[i] == '\n' )
				{
					break;
				}
			}
			strncpy(newname, line + marker, i-marker);
			newname[i-marker] = '\0';
			head = renamebook(head, name, newname);
		}
		else if (strcmp(cmd, "Ini")==0) //initiation
		{
			while (!feof(fr)) // first reading the books fils, making a connected list of all the books
			{
				fgets(line, 400, fr);
				name_len = strlen(line);
				for (i = 0; i < name_len; i++)
				{

					if (line[i]=='$')
					{
						marker = i;
						break;
					}

				}
				strncpy(name, line, marker - 1);
				name[marker-1] = '\0';
				marker += 4;
				copiesnum = atoi(strncpy(num, line + marker, name_len - marker));
				head = returnbook(head, name, copiesnum);
			}
		}
		else if (strcmp(cmd, "Fin") == 0)
		{
			curr = head;
			while (curr != NULL) // didnt reach the end of the list
			{
				fprintf(fw, "%s $$$ %d \n", curr->name, curr->copies);
				curr = curr->next;
			}
		}

	}
	// now we write all the data back to the updated list

	fclose(fr);
	fclose(fo);
	fclose(fw);
	free_list(head);
	return 0;
}



