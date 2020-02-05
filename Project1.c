//Justin Garrison, Giuliana Tosi

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>


//struct definition for the treenode including the PID, PPID, name, and size
struct treeNode {
	int processid;
	int parentid;
	char name[1000];
	long unsigned int size;

	struct treeNode *child;      //points to the first child of this node
	struct treeNode *next;       //ptr to next node at the same level
	int depth;					 //level of the tree it is on
};
typedef struct treeNode treeNode;

treeNode *head;		//global variable 

treeNode * findbyID(treeNode * root, int id);	//return a pointer to a treenode
treeNode * createNode(int processid, int parentid, char name[1000], long unsigned int size);


int main(void) {

	struct dirent *de; // ptr for directory entry
	long id; 
	char buffer[50];	//hold filepath to the stat of the given process



	DIR *dr = opendir("/proc");		//dr pointing to directory so you can search through the /proc filesystem
	
	//make sure /proc opens 
	if (dr == NULL) {
		printf("Could not open current directory");
		return 0;
	}

	//initializing the head of the tree node to be 0	
	head = ( treeNode*)malloc(sizeof( treeNode));
	head->processid=0;
	strcpy(head->name, "head");
	head->size=0;

	//read directory with dr pointer to check all of the numeric processes
	//grab the ID of each numeric process and pass it into the function proc
	while ((de = readdir(dr)) != NULL) {
		if(isdigit(*de->d_name)) {
			id = strtol(de->d_name, NULL, 10);
			sprintf(buffer, "/proc/%ld/stat", id);
			proc(buffer);
		}
	}
	closedir(dr);	//close dr



	printProcTree(head);	//prints tree to console

	
	return 0;
}

//proc function which opens the /stat file of the given process
int proc(char* path) {
	FILE *fp;
		fp = fopen(path, "r");
		if(fp == NULL) {	//if the file does not exist, return 0 
			perror("fopen");
			printf("Could not open file\n");
			return 0;
		} else {	//hold the values of the process after scanning from the file
			int pid;
			int ppid;
			char comm[1000]; 
			long unsigned int vsize;

			//read in the values from the file denoting the ones we do not need with a *
			fscanf(fp, "%d %s %*c %d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %*lu %*lu %*ld %*ld %*ld %*ld %*ld %*ld %*llu %lu", 
			&pid, comm, &ppid, &vsize);	

				
			//take scanned values and put them into the treenode 
			treeNode *node = createNode(pid, ppid, comm, vsize/1000);
			//puts the node in the correct place of the tree with function add_child
			add_child(findbyID(head, node->parentid), node);
			
			fclose(fp);	//close file
			
		}
}


 //initialized the tree node with process values
 treeNode * createNode(int processid, int parentid, char name[1000], long unsigned int size) {
	treeNode* new_tNode = (treeNode*)malloc(sizeof( treeNode)); // allocate memory for node

	//If the new treenode exist then initialize the value from the parameter
	if ( new_tNode != NULL) {
		new_tNode->processid=processid;
		new_tNode->parentid=parentid;
		strcpy(new_tNode->name, name);
		new_tNode->size=size;
		new_tNode->next = NULL;
		new_tNode->child = NULL;
	} else {
		printf("Couldn't allocate memory for node");
	}
	return new_tNode;
}

//function which adds a sibling 
int add_next(treeNode* first, treeNode* new) {
	if(first == NULL || new == NULL) return -1; 	//check to make sure both nodes exist
	if(first->next != NULL) {						//if the node already points to a right sibling
		while(first->next != NULL) {				//move pointer until we hit a node that does not have right sibling
			first = first->next;					//set next pointer to the new node
		}
		first->next = new;
		new->depth = first->depth;					//set depth of new node to the depth of its sibling
		return 0;
		
	} else {										//add sibling when first spot is open
		first->next = new;
		new->depth = first->depth;
		return 0;
	}
	return -1;
}

//add a child function
int add_child(treeNode* parent, treeNode* child) {
	if (parent == NULL || child == NULL) return -1;
	
	if(parent->child != NULL) {			//check to see if parent already has a child
		add_next(parent->child, child);		//add node as a slbling of a child
		return 0;
	} else {
		parent->child = child;		//parent points to child
		child->depth = parent->depth + 1;	//childs depth changes to parents depth + 1
		return 0;
	}
	return -1;
}

//recursive function that finds a node based on the PID
treeNode * findbyID(treeNode* root, int id) {
	for(; root != NULL; root = root->next) {	
		if(root->processid == id) return root;	//check to see if the PID are the same 
		
		if(root->child != NULL) {				//searches through the tree at level order
			treeNode *temp = findbyID(root->child, id);
			if(temp) return temp;				
		}
	}
	return NULL;
}

//prints the process information
void printProc(treeNode * node) {
	if(node->processid != 0)		//this is so 0th process is not printed out
		printf("(%d), %s, %lu kb\n", node->processid, node->name, node->size);

}

//wrapper function for the recursive printTree function and onlt accepts the head of the tree
void printProcTree(treeNode *head) {
	head = head->child;	//move the head pointer down one level to start on level 1
	printTree(head);
}

//preorder (depth first search) recursive function
void printTree(treeNode *root) {
	if(root == NULL) {
		printf("node does not exist");		
		return;
	}
	for(int i = 1; i < root->depth; i++) {		//this is for indention purposes which is based upon the nodes depth
		printf("    ");
	}

	printProc(root);

	if(root->child != NULL)	
		printTree(root->child);		//recusive call to grab the first child
	if(root->next != NULL)
		printTree(root->next);		//recursive call to grab the siblings
}












