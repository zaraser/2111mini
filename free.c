/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zserobia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 12:01:56 by zserobia          #+#    #+#             */
/*   Updated: 2024/11/13 12:01:59 by zserobia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

char	*ft_strjoin_free(char *s1, char *s2)
{
	char	*result;

	result = ft_strjoin(s1, s2);
	free(s1);
	return (result);
}

void	ft_free_lex(t_shell *shell)
{
	if (shell->input_line)
	{
		free(shell->input_line);
		shell->input_line = NULL;
	}
	if (shell->lexer_list)
		free_lexer_list(&shell->lexer_list);
}

void	ft_free_pars(t_shell *shell)
{
	if (shell->pars)
		free_pars(shell->pars);
	if (shell->pars_mini)
	{
		free(shell->pars_mini);
		shell->pars_mini = NULL;
	}
}

void	free_pars(t_simple_cmds *pars)
{
	t_simple_cmds	*current;
	t_simple_cmds	*next;
	int				i;

	i = 0;
	current = pars;
	while (current != NULL)
	{
		next = current->next;
		if (current->str)
		{
			while (current->str[i] != NULL)
			{
				free(current->str[i]);
				i++;
			}
			free(current->str);
		}
		if (current->hd_file_name)
			free(current->hd_file_name);
		if (current->redirections)
			free_lexer_list(&current->redirections);
		free(current);
		current = next;
	}
}

void	free_lexer_list(t_lexer **lexer)
{
	t_lexer	*current;
	t_lexer	*next;

	current = *lexer;
	if (lexer == NULL || *lexer == NULL)
		return ;
	while (current)
	{
		next = current->next;
		if (current->str)
			free(current->str);
		free(current);
		current = next;
	}
	*lexer = NULL;
}

void	free_redirections(t_lexer *redirection)
{
	t_lexer	*current;
	t_lexer	*next;

	current = redirection;
	while (current)
	{
		next = current->next;
		free(current->str);
		free(current);
		current = next;
	}
}

void	free_simple_cmds_list(t_simple_cmds *commands)
{
	t_simple_cmds	*current;
	int				i;

	while (commands)
	{
		current = commands;
		commands = commands->next;

		if (current->str)
		{
			for (i = 0; current->str[i]; i++)
			{
				free(current->str[i]);
				current->str[i] = NULL;
			}
			free(current->str);
			current->str = NULL;
		}
		free_redirections(current->redirections);
		current->redirections = NULL;
		free(current);
	}
}

void	free_command(char **cmd)
{
	int	i;

	i = 0;
	if (!cmd)
		return ;
	while (cmd[i])
	{
		free(cmd[i]);
		i++;
	}
	free(cmd);
}
