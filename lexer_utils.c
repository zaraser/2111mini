/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zserobia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/13 12:01:04 by zserobia          #+#    #+#             */
/*   Updated: 2024/11/13 12:01:07 by zserobia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void	print_tokens(t_lexer *list)
{
	t_lexer	*tmp;

	tmp = list;
	while (tmp)
	{
		printf("Index: %d, Type: %d, Value: %s\n", tmp->index, tmp->token, tmp->str);
		tmp = tmp->next;
	}
}

void	print_simple_cmds(t_simple_cmds *cmds)
{
	printf("Start Simple Commands:\n");
	while (cmds)
	{
		printf("Command: ");
		for (int i = 0; cmds->str && cmds->str[i]; i++)
		{
			printf("%s ", cmds->str[i]);
		}
		printf("\n");
		printf("Number of redirections: %d\n", cmds->num_redirections);
		t_lexer *redir = cmds->redirections;
		while (redir)
		{
			printf("Redirection: Token: %d, Str: %s\n", redir->token, redir->str);
			redir = redir->next;
		}
		cmds = cmds->next;
	}
	printf("End of Simple Commands.\n");
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*s3;
	int		i;
	int		j;

	i = 0;
	j = 0;
	if (!s1 && !s2)
		return (ft_strdup(""));
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	s3 = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));
	if (!s3)
		return (NULL);
	while (s1[i])
	{
		s3[i] = s1[i];
		i++;
	}
	while (s2[j])
		s3[i++] = s2[j++];
	s3[i] = '\0';
	return (s3);
}

int	ft_ifspace(char line)
{
	return (line == ' ' || (line >= 9 && line <= 13));
}

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
	{
		i++;
	}
	return (i);
}

char	*ft_substr(const char *str, size_t start, size_t len)
{
	size_t	str_len;
	char	*sub;

	str_len = strlen(str);
	if (start >= str_len)
		return (NULL);
	if (start + len > str_len)
		len = str_len - start;
	sub = (char *)malloc(len + 1);
	if (!sub)
		return (NULL);
	strncpy(sub, str + start, len);
	sub[len] = '\0';
	return (sub);
}

char	*ft_strdup(const char *s)
{
	size_t	len;
	char	*copy;

	len = strlen(s);
	copy = malloc(len + 1);
	if (!copy)
		return (NULL);
	memcpy(copy, s, len + 1);
	return (copy);
}
