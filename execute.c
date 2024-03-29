#include "shell.h"

/**
 *  * is_path - checks if input command is part of directory PATH
 *   * @command: a command
 *    *
 *     * Return: 1 if path, 0 if no path
 *      */
int is_path(char *command)
{
		int i;

			i = 0;
				while (command[i] != '\0')
						{
									if (command[i] == '/')
													return (1);
											i++;
												}

						return (0);
}

/**
 *  * exec_builtins - custom function to execute builtin commands
 *   * @arginv: arguments inventory
 *    *
 *     * Return: 1 on success, 0 on failure
 *      */
int exec_builtins(arg_inventory_t *arginv)
{
		int i, retval;
			/* old_stdout */
			char *str, **commands;
				builtins_t builtins_list[] = {

							{"env", _env}, {"setenv", _setenv},
									{"unsetenv", _unsetenv}, {"history", _history}, {"cd", _cd},
											{"alias", _alias}, {"unalias", _unalias}, {"help", shell_help},
													{"exit", shell_exit},
															{NULL, NULL}
								};

					retval = EXT_FAILURE;
						commands = (char **)arginv->commands;

							/* old_stdout = redirect_output(arginv, 0); */

							for (i = 0; ((str = builtins_list[i].command) != NULL); i++)
									{
												if (_strcmp(str, commands[0]) == 0)
															{
																			retval = builtins_list[i].builtin_func(arginv);
																						break;
																								}
													}

								arginv->exit_status = retval;
									return (retval);
}

/**
 *  * exec_error_exit - frees all and exits if exec error
 *   * @msg: message to display
 *    * @command: command to free
 *     * @_environ: env double pointer to free
 *      * @arginv: arg inventory to free
 *       */
void exec_error_exit(char *msg, char *command, char **_environ,
				arg_inventory_t *arginv)
{
		delete_pipeline(&arginv->pipeline);
			delete_parser(&arginv->parser);
				delete_tokens(&arginv->tokens);
					free(command);
						free_paths(_environ);
							freeall(arginv);
								_perror(msg);
									exit(1);
}

/**
 *  * exec_path - custom function to execute from PATH
 *   * @command: command to execute
 *    * @arginv: arg inventory
 *     *
 *      * Return: pid of parent
 *       */
pid_t exec_path(char *command, arg_inventory_t *arginv)
{
		pid_t pid;
			char **_environ;

				pid = fork();
					if (pid < 0)
							{
										_perror("Critical error: unable to fork()!\n");
												exit(1);
													}

						if (pid == 0)
								{
											_environ = link_to_dpointer(arginv->envlist);

													if (execve(command, (char **)arginv->commands, _environ) < 0)
																	exec_error_exit("No Command\n", command, _environ, arginv);
														}
							free(command);
								return (pid);
}

/**
 *  * execute - completes execution of input commands
 *   * @arginv: arguments inventory
 *    *
 *     * Return: void
 *      */
pid_t execute(arg_inventory_t *arginv)
{
		env_t *envlist;
			char **commands;
				char *path, *command;
					char **paths;

						envlist = arginv->envlist;

							commands = (char **)arginv->commands;

								command = safe_malloc(sizeof(char) * BUFSIZE);
									command = _strcpy(command, *commands);

										if (exec_builtins(arginv) == EXT_FAILURE)
												{
															if (is_path(command))
																		{
																						return (exec_path(command, arginv));
																								}
																	else
																				{
																								path = safe_malloc(sizeof(char) * BUFSIZE);
																											locate_path(path, envlist);
																														paths = tokenize_path(path);
																																	cat_path(paths, command);
																																				free_paths(paths);
																																							free(path);
																																										return (exec_path(command, arginv));
																																												}
																		}
											free(command);
												return (-1);
}
