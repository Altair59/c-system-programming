#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>

#include "pmake.h"

/*run rules given target  */
void run_make(char *target, Rule *rules, int pflag)
{
	Rule *operand;
	if (target == NULL)
	{ // if target is NULL, execute the first rule
		operand = rules;
		evaluate(operand, pflag);
	}
	else
	{
		operand = find_rule(rules, target);
		if (operand == NULL)
		{ // error
			fprintf(stderr, "ERROR: target rule does not exist\n");
			exit(1);
		}
		else
		{
			evaluate(operand, pflag); // recursively evaluate rules
		}
	}
}

/*find rule in rules by target name */
Rule *find_rule(Rule *rules, char *target)
{
	Rule *cur = rules;
	while (cur != NULL)
	{
		if (strcmp(cur->target, target) == 0)
		{
			return cur;
		}
		cur = cur->next_rule;
	}
	return NULL;
}

/* evaluate rule*/
void evaluate(Rule *operand, int pflag)
{
	// update each dependency
	Dependency *cur_dep = operand->dependencies;
	if (pflag == 0)
	{ // linearly update dependencies
		while (cur_dep != NULL)
		{
			evaluate(cur_dep->rule, pflag);
			cur_dep = cur_dep->next_dep;
		}
	}
	else
	{ // parallelly update dependencies
		int num_child = 0;
		while (cur_dep != NULL)
		{
			int r = fork();
			num_child++;
			if (r == -1)
			{ // system error call
				perror("fork");
				exit(1);
			}
			else if (r == 0)
			{									// child process
				evaluate(cur_dep->rule, pflag); // execute action
				free_rec();						// free
				exit(0);
			}
			cur_dep = cur_dep->next_dep;
		}
		int status;
		while (num_child > 0)
		{ // parent process
			if (wait(&status) == -1)
			{
				perror("wait");
				exit(1);
			}
			else
			{
				if (WIFEXITED(status))
				{
					if (WEXITSTATUS(status) == 1)
					{
						exit(1);
					}
				}
				else if (WIFSIGNALED(status))
				{
					fprintf(stderr, "%s\n", "child process terminated due to signal");
				}
			}
			num_child--;
		}
	}
	// execute action
	struct stat filestat;
	// execute rule if 1. theres more recent dependency 2. rules does not exist
	if (stat(operand->target, &filestat) == 0)
	{ // if target exists
		time_t t1 = filestat.st_mtim.tv_sec;
		time_t t2 = filestat.st_mtim.tv_nsec;

		cur_dep = operand->dependencies;
		while (cur_dep != NULL)
		{
			struct stat filestat1;
			stat(cur_dep->rule->target, &filestat1);
			time_t t3 = filestat1.st_mtim.tv_sec;
			time_t t4 = filestat1.st_mtim.tv_nsec;
			if (t1 < t3)
			{
				execute(operand);
				break;
			}
			else if (t1 == t3 && t2 < t4)
			{
				execute(operand);
				break;
			}
			cur_dep = cur_dep->next_dep;
		}
	}
	else if (stat(operand->target, &filestat) == -1)
	{
		if (fopen(operand->target, "r") != NULL){ // if error happen while stat
			perror("stat");
			exit(1);
		} else {
			execute(operand); // if error happen due to loss of file
		}
	}
	else
	{
		execute(operand);
	}
}

void execute(Rule *operand)
{
	Action *cur_act = operand->actions;
	while (cur_act != NULL)
	{
		int r = fork();
		if (r == -1)
		{ // system error call
			perror("fork");
			exit(1);
		}
		else if (r == 0)
		{											 // child process
			execvp(cur_act->args[0], cur_act->args); // execute action using exec
			perror("exec");
			exit(1);
		}
		else if (r > 0)
		{ // parent process
			int status;
			if (wait(&status) == -1)
			{
				perror("wait");
				exit(1);
			}
			else
			{
				if (WIFEXITED(status))
				{
					if (WEXITSTATUS(status) == 1)
					{
						exit(1);
					}
				}
				else if (WIFSIGNALED(status))
				{
					fprintf(stderr, "%s\n", "child process terminated due to signal");
				}
			}
		}
		cur_act = cur_act->next_act;
	}
}
