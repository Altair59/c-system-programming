
#define MAXLINE 256

/* A line from a makefile is either a target line, an action line, 
or a comment or line to ignore
     "target" line
        - Starts with a target, followed by a colon followed by a 
        space-separated list of dependencies
        - A target is a word that follows the Rule of valid file names  
        (assume that only valid file names are used in testing)
        - The colon has a space on either side of it
        - assume the line will not exceed MAXLINE
        - The list of dependencies follow the same Rule as the target

     "action" line
        - Begins with a tab
        - Contains a line that can be executed. First word is the relative path
        to the executable, remaining tokens are argument to the executable.
        - relative path means that if the executable is in the current working 
        directory, the path begins with "./" (Do not assume "." is in the path)
     
     Comment or empty line
        - contains only spaces and/or tabs
        - contains 0 or more spaces or tabs followed by a '#'.  Any other 
          characters after the '#' are ignored.
 */

typedef struct action_node {
    char **args;  // an array of strings suitable to be passed to execvp
    struct action_node *next_act;
} Action;

typedef struct dep_node {
    struct rule_node *rule;
    struct dep_node *next_dep;
} Dependency;

typedef struct rule_node{
    char *target;
    Dependency *dependencies;
    Action *actions;
    struct rule_node *next_rule;
} Rule;

typedef struct rule_record{
    Rule *rule;
    struct rule_record *next_record;
} Record;

/* Print the list of rules to stdout in makefile format */
void print_rules(Rule *rules);

/* Read from the open file fp, and creat the linked data structure
   that represents the Makefile contained in the file.
 */
Rule *parse_file(FILE *fp);

/* Return an array of strings where each string is one word in line. The final
   element of the array will contain NULL.
 */
char **build_args(char *line);

/* Return 1 if line contains only space characters, and 0 otherwise */
int is_comment_or_empty(char *line);

/* Return 1 if line contains only space characters */
int is_only_space(char *line);

/* Return 1 if line contains an action */
int is_action(char *line);

/* free methods */
/* free records*/
void free_rec();

/* free rules */
void free_rule(Rule *rule);

/* free dependencies */
void free_dep(Dependency *dependency);

/* free actions */
void free_act(Action *action);

/* record new rules */
void record_rule(Record ** record_ptr, Rule * rule);

/* fetch or find existing rules */
Rule *fetch_rule(Record *in_record,char* target);

/* create new rules */
Rule *create_rule(char* target);

/* find rule in rules */
Rule *find_rule(Rule *rules, char *target);

/* evaluate rule */
void evaluate(Rule *operand, int pflag);

/* execute rule */
void execute(Rule *operand);



/* Concatenates args into a single string and store it in buffer, up to size.
   Return a pointer to buffer.
 */
char *args_to_string(char **args, char *buffer, int size);

/* Evaluate the rule in rules with the target "target"
   If target is NULL, evaluate the first rule in rules.
   If pflag is 0, then create a new process to evaluate each dependency. The
   parent process will wait until all child processes have terminated before
   checking dependecy modfied times to decide whether to execute the actions
 */
void run_make(char *target, Rule *rules, int pflag);

