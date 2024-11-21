/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zserobia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 12:00:47 by zserobia          #+#    #+#             */
/*   Updated: 2024/11/13 12:00:49 by zserobia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

int global_exit = 0;

void	ft_init_shell(t_shell *shell, char **envp)
{
	shell->input_line = NULL;
	shell->lexer_list = NULL;
	shell->count_pipe = 0;
	shell->pars = NULL;
	shell->envp = envp;
	shell->commands = NULL;
	signals();
}

void	ft_start_loop(char **envp)
{
	t_shell	shell;

	while (1)
	{
		ft_init_shell(&shell, envp);
		shell.input_line = readline("Minishell$ ");
		if (shell.input_line == NULL)
			ft_signal_ctr_d();
		if (ft_strlen(shell.input_line) > 0)
		{
			add_history(shell.input_line);
			if (ft_minicheck(shell.input_line))
			{
				ft_free_lex(&shell);
				continue ;
			}
			if (!check_quotes(shell.input_line))
			{
				printf("-bash: Error: unmatched quotes found.\n");
				ft_free_lex(&shell);
				continue ;
			}
			ft_read_token(shell.input_line, &shell.lexer_list, &shell);
			//print_tokens(shell.lexer_list);
			if (ft_check_errors(shell.lexer_list))
			{
				ft_free_lex(&shell);
				continue ;
			}
			parser_part(shell.count_pipe, shell.lexer_list, &shell);
			expand_part(&shell);
			free_simple_cmds_list(shell.commands);

			//ft_free_lex(&shell);
			//ft_free_pars(&shell);
			if (shell.input_line)
			{
				free(shell.input_line);
				shell.input_line = NULL;
			}
		}
		else
			ft_free_lex(&shell);
	}
}

int main(int argc, char **argv, char **envp) {
    // Инициализация структуры окружения
	ft_start_loop(envp);
	rl_clear_history();
	return 0;
}
