/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_create.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zserobia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 12:06:27 by zserobia          #+#    #+#             */
/*   Updated: 2024/11/13 12:06:30 by zserobia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

// Crée un nouveau nœud lexer avec une valeur, un type de token et un index spécifié
t_lexer	*lexer_create(char *value, t_tokens token, int index)
{
	t_lexer	*new;

	new = (t_lexer *)malloc(sizeof(t_lexer));
	if (!new)
		return (NULL);
	new->str = value;
	new->token = token;
	new->index = index;
	new->next = NULL;
	new->prev = NULL;
	return (new);
}

// Ajoute un nouveau nœud lexer à la fin d'une liste chaînée
void	lexer_add_back(t_lexer **list, t_lexer *new_token)
{
	t_lexer	*temp;

	if (!*list)
	{
		*list = new_token;
		return ;
	}
	temp = *list;
	while (temp->next)
		temp = temp->next;
	temp->next = new_token;
	new_token->prev = temp;
}

// Crée et ajoute un nœud lexer à la liste
void	ft_create_lexer_list(char *value, t_tokens type, int index,
	t_lexer **list)
{
	t_lexer	*new_token;

	new_token = lexer_create(value, type, index);
	if (new_token)
		lexer_add_back(list, new_token);
}

// Lit un mot dans une ligne jusqu'à un espace ou un caractère spécial et retourne sa longueur
int	ft_read_word(char *line)
{
	int	i;

	i = 0;
	while (line[i] && !ft_ifspace(line[i]) && line[i] != '|' && line[i] != '>'
		&& line[i] != '<' && line[i] != '\'' && line[i] != '"')
		i++;
	return (i);
}

// Lit un mot entouré de guillemets et retourne sa longueur, y compris les guillemets fermants
int	ft_read_word_quote(char *line, char quote)
{
	int	i;

	i = 1;
	while (line[i] && line[i] != quote)
		i++;
	if (line[i] == quote)
		i++;
	return (i);
}
static int ft_process1(char **line)
{
	char	quote;
	int	len;

	len = 0;
	if (**line == '\'' || **line == '"')
	{
		quote = **line;
		len = ft_read_word_quote(*line, quote);
	}
	else
		len = ft_read_word(*line);
	return (len);
}
static char *ft_process2(char *result, char *temp)
{
	char *old_result;

	old_result = result;
	result = ft_strjoin(result, temp);
	free(temp);
	free(old_result);
	return (result);
}

static char *process_quoted_or_unquoted(char **line, int *total_len)
{
	char *temp;
	char *result;
	int len;
	temp = NULL;
	result = NULL;
	while (**line && !ft_ifspace(**line) && **line != '|' 
		&& **line != '<' && **line != '>')
	{
		len = ft_process1(line);
		temp = ft_substr(*line, 0, len);
		if (result)
			result = ft_process2(result, temp);
		else
			result = temp;
		*total_len += len;
		*line += len;
		if (!result && temp)
			free(temp);
	}
	return result;
}
/*static char *process_quoted_or_unquoted(char **line, int *total_len)
{
    char *temp ;
    char *result;
    char quote;
    int len;

    temp = NULL;
    result = NULL;
    while (**line && !ft_ifspace(**line) && **line != '|' 
           && **line != '<' && **line != '>')
    {
        len = 0;
        if (**line == '\'' || **line == '"')
        {
            quote = **line;
            len = ft_read_word_quote(*line, quote);
        }
        else
            len = ft_read_word(*line);
		len = cint(line);
		temp = ft_substr(*line, 0, len);
        if (result)
        {
            char *old_result = result;
            result = ft_strjoin(result, temp);
            free(temp);         // Free temp here after appending.
            free(old_result);   // Free the old result.
        }
        else
            result = temp; // temp is now managed by result.
        *total_len += len;
        *line += len;
        if (!result && temp)
            free(temp);
    }
    return result;
}*/

int	ft_read_token_1(char *line, t_lexer **list, int index)
{
	int		len;
	int		total_len;
	char	*word;
	char	*temp;

	total_len = 0;
	word = NULL;
	while (*line && !ft_ifspace(*line) && *line != '|' 
		&& *line != '<' && *line != '>')
	{
		temp = process_quoted_or_unquoted(&line, &total_len);
		word = ft_strjoin_free(word, temp);
		free(temp);
	}
	if (word)
		ft_create_lexer_list(word, WORD, index, list);
	return (total_len);
}

// Fonction principale pour lire tous les tokens d'une ligne et les ajouter à la liste lexer
void	ft_read_token(char *line, t_lexer **list, t_shell *shell)
{
	int	index;
	int	len;

	index = 0;
	while (*line)
	{
		while (ft_ifspace(*line))
			line++;
		len = 0;
		if (*line == '|')
		{
			ft_create_lexer_list(strdup("|"), PIPE, index++, list);
			shell->count_pipe++;
			len = 1;
		}
		else if (*line == '>' && *(line + 1) == '>')
		{
			ft_create_lexer_list(strdup(">>"), APPEND, index++, list);
			len = 2;
		}
		else if (*line == '>')
		{
			ft_create_lexer_list(strdup(">"), OUT, index++, list);
			len = 1;
		}
		else if (*line == '<' && *(line + 1) == '<')
		{
			ft_create_lexer_list(strdup("<<"), HEREDOC, index++, list);
			len = 2;
		}
		else if (*line == '<')
		{
			ft_create_lexer_list(strdup("<"), IN, index++, list);
			len = 1;
		}
		else
			len = ft_read_token_1(line, list, index++);
		line += len;
	}
}
