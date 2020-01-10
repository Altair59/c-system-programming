### Git hooks!

A *git hook* is a program that runs at a specific time in the git workflow to perform specific checks about the action being run.
In this course, you'll be using a *pre-commit hook* to perform two checks every time you commit changes:

-   You may not add, delete, or modify top-level files or directories in your repository.
    Instead, all of your work should be done in assignment-specific subdirectories, like `lab0`.
-   When an assignment has required files, you'll receive a warning message if your repo is missing some of these files.
    If the assignment restricts your submission to *only* those required files,
    you won't be able to commit any other files inside that assignment's subdirectory.

**You must perform the following steps to set up these git hooks on your computer.**
Because these checks are also made by the MarkUs server when you push your code,
your submission may not be accepted if your changes fail to satisfy the above conditions.

1.  **OSX and Linux users only**: open the file `markus-hooks/pre-commit`.
    You'll need to modify the command inside the file to use `python3` (or `python3.6`, depending on your setup) rather than just plain `python`, which likely refers to a Python 2 interpreter.
2.  In your new cloned repository, copy the file `markus-hooks/pre-commit` into `.git/hooks`  ($ is the shell prompt):

    ```
    $ cp markus-hooks/pre-commit .git/hooks
    ```

    Note the period at the start of `.git`; this is a *hidden folder*, and may or may not show up in a graphical file explorer, depending on your settings.
    Running the above command in a terminal is the safest approach.

3.  Check your work: run `ls .git/hooks`.
    One of the files listed should be `pre-commit`.


### Submitting files

Now that the setup is complete, let's briefly review how to submit files using `git`.

1.  Create a new file `hello.txt` in your `lab0` folder.
    You can put in whatever text you like, or even leave the file empty.
2.  In the command line, run

    ```console
    $ git add lab0/hello.txt
    ```

    This causes git to track the new file, so that subsequent changes to this file will be noted.
3.  Then run

    ```console
    $ git commit -m "lab0: This is my commit message :)"
    ```

    This will commit your change (adding a new file) with the given message.
    At this point, the changes have been saved by git, but only on your computer.
    MarkUs (and your TAs) don't know anything about this file.

    *Git hook check*. When you commit, you should see some output that looks like the following:

    ```
    [MarkUs] Running pre-commit checks...
    [MarkUs] Checking whether top-level files/directories were created/modified...
    [MarkUs] Commit looks good!
    [master e0370c1] lab0: This is a commit message. :)
    1 file changed, 0 insertions(+), 0 deletions(-)
    create mode 100644 lab0/hello.txt
    ```

    The `[MarkUs]` lines are the output of the pre-commit hooks you installed.
    If you don't see them, you didn't install the hooks properly, and should go back to the previous section!

4.  Finally, run

    ```console
    $ git push
    ```

    This will copy over your new commit to the MarkUs server.

Now let's see what happens if you try to commit a file at the top-level of your repo.
*This assumes you have the git hooks installed properly.*

5.  In your top-level repoitory folder, create a new file with a different name (e.g., `top-level.txt`).
6.  Use `git add` to track this file.
7.  Now run a `git commit`. You should see output that looks like the following:

    ```
    [MarkUs] Running pre-commit checks...
    [MarkUs] Checking whether top-level files/directories were created/modified...
    [MarkUs] Error: Top-level change detected for "top-level.txt". (You should unstage this change.)
    ```

    If you then do a `git log`, you should see that your commit wasn't actually successful.

8.  To unstage this file, run `git reset HEAD -- top-level.txt`.
    You should probably delete this file as well, so that it doesn't clutter up your repository.


**Note**: if you accidentally did all these steps but didn't install the hook properly, your commit actually succeeded.
This isn't good -- you should go back to the previous section to make sure the hooks are properly installed.
Also, you should revert your previous commit by running `git reset HEAD~1`.

9.  **OSX and Linux users only**: commit your change to `markus-hooks/pre-commit`.
    This is safe (you can push the change to MarkUs), and will remove some clutter from your `git status` output.
