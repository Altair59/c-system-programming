#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include "pmake.h"

// set up recorder
Record *record = NULL;

/* Print the list of actions */
void print_actions(Action *act)
{
    while (act != NULL)
    {
        if (act->args == NULL)
        {
            fprintf(stderr, "ERROR: action with NULL args\n");
            act = act->next_act;
            continue;
        }
        printf("\t");

        int i = 0;
        while (act->args[i] != NULL)
        {
            printf("%s ", act->args[i]);
            i++;
        }
        printf("\n");
        act = act->next_act;
    }
}

/* Print the list of rules to stdout in makefile format. If the output
   of print_rules is saved to a file, it should be possible to use it to 
   run make correctly.
 */
void print_rules(Rule *rules)
{
    Rule *cur = rules;

    while (cur != NULL)
    {
        if (cur->dependencies || cur->actions)
        {
            // Print target
            printf("%s : ", cur->target);

            // Print dependencies
            Dependency *dep = cur->dependencies;
            while (dep != NULL)
            {
                if (dep->rule->target == NULL)
                {
                    fprintf(stderr, "ERROR: dependency with NULL rule\n");
                }
                printf("%s ", dep->rule->target);
                dep = dep->next_dep;
            }
            printf("\n");

            // Print actions
            print_actions(cur->actions);
        }
        cur = cur->next_rule;
    }
}

/* create rule by target name
*/
Rule *create_rule(char *target)
{
    Rule *rule = malloc(sizeof(Rule));
    rule->target = malloc(strlen(target) + 1);
    strncpy(rule->target, target, strlen(target) + 1);
    rule->dependencies = NULL;
    rule->actions = NULL;
    rule->next_rule = NULL;

    return rule;
}

/* fetch or find rule by target name
*/
Rule *fetch_rule(Record *in_record, char *target)
{
    Record *record = in_record;
    while (record != NULL && record->rule != NULL)
    {
        if (strcmp(record->rule->target, target) == 0)
        {
            return record->rule;
        }
        record = record->next_record;
    }
    return NULL;
}

/* record rule to recorder
*/
void record_rule(Record **record_ptr, Rule *rule)
{
    if (*record_ptr == NULL)
    {
        (*record_ptr) = malloc(sizeof(Record));
        (*record_ptr)->rule = rule;
        (*record_ptr)->next_record = NULL;
    }
    else
    {
        Record *cur_rec = *record_ptr;
        Record *new_record = malloc(sizeof(Record));
        new_record->rule = rule;
        new_record->next_record = NULL;
        while (cur_rec->next_record != NULL)
        {
            cur_rec = cur_rec->next_record;
        }
        cur_rec->next_record = new_record;
    }
}

/* free methods
*/
void free_rec()
{
    Record *temp;
    while (record != NULL)
    {
        free_rule(record->rule);
        temp = record;
        record = record->next_record;
        free(temp);
    }
}

void free_rule(Rule *rule)
{
    free(rule->target);
    if (rule->dependencies != NULL)
    {
        free_dep(rule->dependencies);
    }
    if (rule->actions != NULL)
    {
        free_act(rule->actions);
    }
    free(rule);
}

void free_dep(Dependency *dependency)
{
    Dependency *temp;
    while (dependency != NULL)
    {
        temp = dependency;
        dependency = dependency->next_dep;
        free(temp);
    }
}

void free_act(Action *action)
{
    Action *temp;
    while (action != NULL)
    {
        int i = 0;
        while ((action->args)[i] != NULL)
        {
            free((action->args)[i]);
            i++;
        }
        free(action->args);
        temp = action;
        action = action->next_act;
        free(temp);
    }
}

/* Create the rules data structure and return it.
   Figure out what to do with each line from the open file fp
     - If a line starts with a tab it is an action line for the current rule
     - If a line starts with a word character it is a target line, and we will
       create a new rule
     - If a line starts with a '#' or '\n' it is a comment or a blank line 
       and should be ignored. 
     - If a line only has space characters ('', '\t', '\n') in it, it should be
       ignored.
 */
Rule *parse_file(FILE *fp)
{
    // set a dummy head for rules linked list
    Rule *rules = malloc(sizeof(Rule));
    rules->target = NULL;
    rules->dependencies = NULL;
    rules->actions = NULL;
    rules->next_rule = NULL;
    // set up cursor
    Rule *cur = rules;
    // set up container for reading makefiles
    char str[MAXLINE];
    // read line from makefile and judge what to do with rules linked list
    while (fgets(str, MAXLINE, fp) != NULL)
    {
        if (is_only_space(str) == 1)
        { // if line contains only spaces
            continue;
        }
        else if (is_comment_or_empty(str) == 1)
        { // if line is comment or empty
            continue;
        }
        else if (is_action(str) == 1)
        { // if line contains an action
            if (cur->actions == NULL)
            { // it's the first action
                cur->actions = malloc(sizeof(Action));
                cur->actions->args = build_args(str);
                cur->actions->next_act = NULL;
            }
            else
            { // it's not the first action
                Action *cur_act = cur->actions;
                while (cur_act->next_act != NULL)
                {
                    cur_act = cur_act->next_act;
                }
                cur_act->next_act = malloc(sizeof(Action));
                cur_act->next_act->args = build_args(str);
                cur_act->next_act->next_act = NULL;
                cur_act = cur_act->next_act;
            }
        }
        else
        { // if line contains a rule
            // get target
            char *token;
            token = strtok(str, ": \n");
            if ((cur->next_rule = fetch_rule(record, token)) == NULL)
            {                                          // if rule with this target already exists
                cur->next_rule = malloc(sizeof(Rule)); // if not
                // record this rule
                record_rule(&record, cur->next_rule);
                // set target
                cur->next_rule->target = malloc(strlen(token) + 1);
                strncpy(cur->next_rule->target, token, strlen(token) + 1);
            }
            // set actions
            cur->next_rule->actions = NULL;
            // set dependencies
            token = strtok(NULL, ": \n");
            if (token == NULL)
            { // if the rule has no dependencies
                cur->next_rule->dependencies = NULL;
            }
            else
            { // if the rule has at least one dependency
                cur->next_rule->dependencies = malloc(sizeof(Dependency));
                cur->next_rule->dependencies->rule = NULL;
                cur->next_rule->dependencies->next_dep = NULL;
                Dependency *cur_dependency = cur->next_rule->dependencies;
                while (token != NULL)
                {
                    if ((cur_dependency->rule = fetch_rule(record, token)) == NULL)
                    {                                              // if rule already exists
                        cur_dependency->rule = create_rule(token); // if not, create an orphan with target name
                        record_rule(&record, cur_dependency->rule);
                    }
                    token = strtok(NULL, ": \n");
                    if (token == NULL)
                    { // if no dependencies
                        cur_dependency->next_dep = NULL;
                    }
                    else
                    { // if still have dependencies
                        cur_dependency->next_dep = malloc(sizeof(Dependency));
                        cur_dependency->next_dep->rule = NULL;
                        cur_dependency->next_dep->next_dep = NULL;
                        cur_dependency = cur_dependency->next_dep;
                    }
                }
            }
            // set next_rule
            cur->next_rule->next_rule = NULL;
            // forward cursor
            cur = cur->next_rule;
        }
    }
    // get rules
    Rule *ret = rules->next_rule;
    // adjust all rules' next_rule according to record sequence
    Record *cur_rec = record;
    while (cur_rec != NULL && cur_rec->next_record != NULL)
    {
        cur_rec->rule->next_rule = cur_rec->next_record->rule;
        cur_rec = cur_rec->next_record;
    }
    // clean dummy
    free(rules);
    return ret;
}
