#include <stdlib.h>
#include <sys/types.h>
#include <string.h>

#include "plist.h"


static struct qel {
	pid_t pid;
	char *cmdLine;
	struct qel *next;
} *head;

void walkList(int (*callback) (pid_t, const char *)) {
	// TODO: implement me
	
	struct qel *lauf=head;

	while (lauf){
		if (callback (lauf->pid, lauf->cmdLine))
			break;
		lauf= lauf->next;
	}
	
}

int insertElement(pid_t pid, const char *cmdLine) {
	struct qel *lauf = head;
	struct qel *schlepp = NULL;

	while (lauf) {
		if (lauf->pid == pid) {
			return -1;
		}

		schlepp = lauf;
		lauf = lauf->next;
	}

	lauf = malloc(sizeof(struct qel));
	if (lauf == NULL) {
		return -2;
	}

	lauf->cmdLine = strdup(cmdLine);
	if (lauf->cmdLine == NULL) {
		free(lauf);
		return -2;
	}

	lauf->pid  = pid;
	lauf->next = NULL;

	/* Einhaengen des neuen Elements */
	if (schlepp == NULL) {
		head = lauf;
	} else {
		schlepp->next = lauf;
	}

	return pid;
}

int removeElement(pid_t pid, char *buf, size_t buflen) {
	if (head == NULL) {
		return -1;
	}

	struct qel *lauf = head;
	struct qel *schlepp = NULL;

	while (lauf) {
		if (lauf->pid == pid) {
			if (schlepp == NULL) {
				head = head->next;
			} else {
				schlepp->next = lauf->next;
			}

			strncpy(buf, lauf->cmdLine, buflen);
			if (buflen > 0) {
				buf[buflen-1]='\0';
			}
			int retVal = (int)strlen(lauf->cmdLine);

			/* Speicher freigeben */
			free(lauf->cmdLine);
			lauf->cmdLine = NULL;
			lauf->next = NULL;
			lauf->pid = 0;
			free(lauf);
			return retVal;
		}

		schlepp = lauf;
		lauf = lauf->next;
	}

	/* PID not found */
	return -1; 
}
