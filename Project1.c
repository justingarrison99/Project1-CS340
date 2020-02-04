
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

struct treeNode {
	int processid;
	int parentid;
	char name[1000];
	long unsigned int size;

	struct treeNode *child;      //points to the first child of this node
	struct treeNode *next;       //ptr to next node at the same level
	int depth;
};
typedef struct treeNode treeNode;

	const char ROOT[] = "ROOT";
	struct treeNode *head = {0, -1, "ROOT", 0,
	NULL, NULL, 0 };


treeNode * findbyID(treeNode * root, int id);
treeNode * createNode(int processid, int parentid, char name[1000], long unsigned int size);


int main(void) {


	struct dirent *de; // ptr for directory entry
	long id; 
	char buffer[50];
	DIR *dr = opendir("/proc");
	
	if (dr == NULL) {
		printf("Could not open current directory");
		return 0;
	}

	while ((de = readdir(dr)) != NULL) {
		if(isdigit(*de->d_name)) {
			id = strtol(de->d_name, NULL, 10);
			sprintf(buffer, "/proc/%ld/stat", id);
			proc(buffer);
		}
	}
	closedir(dr);

	
	




	
	return 0;
}

int proc(char* path) {
	FILE *fp;
		fp = fopen(path, "r");
		if(fp == NULL) {
			perror("fopen");
			printf("Could not open file\n");
			return 0;
		} else {
			int pid;
			int ppid;
			char comm[1000]; 
			long unsigned int vsize;

			fscanf(fp, "%d %s %*c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %*u %*u %*d %*d %*d %*d %*d %*d %*lu %lu", 
			&pid, comm, &ppid, &vsize);

			//printf("(%d), ppid = %d,  %s, %lu kb\n", 
			//pid, ppid, comm, (vsize/1000));
			fclose(fp);
				

			treeNode *node = createNode(pid, ppid, comm, vsize/1000);
			add_child(findbyID(head, node->parentid), node);

			
		}
}



 treeNode * createNode(int processid, int parentid, char name[1000], long unsigned int size) {
	treeNode* new_tNode = ( treeNode*)malloc(sizeof( treeNode)); // allocate memory for node

	if ( new_tNode != NULL) {
		new_tNode->processid=processid;
		new_tNode->parentid=parentid;
		strcpy(new_tNode->name, name);
		new_tNode->size=size;
		new_tNode->next = NULL;
		new_tNode->child = NULL;
	}
	return new_tNode;
}

int add_next(treeNode * first, treeNode *new) {
	if(first == NULL || new == NULL) return -1;
	if(first->next != NULL) {
		while(first->next != NULL) {
			first = first->next;	
		}
		first->next = new;
		new->depth = first->depth;
		return 0;
		
	} else {
		first->next = new;
		new->depth = first->depth;
		return 0;
	}
	return -1;
}

int add_child(treeNode * parent, treeNode *child) {
	if (parent == NULL || child == NULL) return -1;
	
	if(parent->child != NULL) {
		add_next(parent->child, child);	
		return 0;
	} else {
		parent->child = child;	
		child->depth = parent->depth + 1;
		return 0;
	}
	return -1;
}

treeNode * findbyID(treeNode * root, int id) {
	for(; root != NULL; root = root->next) {
		if(root->processid == id) return root;
		
		if(root->child != NULL) {
			treeNode *temp = findbyID(root->child, id);
			if(temp) return temp;
		}
	}
	return NULL;
}

void printProc(treeNode * node) {
	printf("(%d), %s, %lu kb\n", node->processid, node->name, node->sizse);
}

void printTree(treeNode *root) {

	if(root == NULL) return;
	
	for(int i = 0; i < root->depth; i++) {
		printf("\t");
	}

	printTree(root->next
}












