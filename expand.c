/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zserobia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:03:39 by zserobia          #+#    #+#             */
/*   Updated: 2024/11/19 15:03:41 by zserobia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

// Проверка на пробельные символы
static int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

char *get_env_value(const char *var_name, char **envp) {
    printf("Searching for env variable: %s\n", var_name); // Debug: вывод имени переменной
    size_t var_len = strlen(var_name);
    for (int i = 0; envp[i] != NULL; i++) {
        if (strncmp(envp[i], var_name, var_len) == 0 && envp[i][var_len] == '=') {
            printf("Found value for %s: %s\n", var_name, &envp[i][var_len + 1]); // Debug
            return &envp[i][var_len + 1];
        }
    }
    printf("Env variable not found: %s\n", var_name); // Debug
    return NULL;
}


// Функция подсчета длины переменной окружения
size_t ft_countlen_envp(char *str, char **envp) {
    size_t len = strlen(str);

    while (*envp) {
        if (strncmp(str, *envp, len) == 0 && (*envp)[len] == '=') {
            return strlen(*envp + len + 1);
        }
        envp++;
    }
    return 0;
}

static void handle_dollar(t_shell *shell, const char *str, int *i, char *result, int *j) {
    (*i)++; // Skip '$'

    if (str[*i] == '?') {
        char *status = malloc(12); // Ensure large enough for status
        snprintf(status, 12, "%d", shell->last_status);
        for (int k = 0; status[k]; k++) {
            result[(*j)++] = status[k];
        }
        free(status);
        (*i)++;
        return;
    }

    // Handle environment variables
    int start = *i;
    while (str[*i] && (isalnum(str[*i]) || str[*i] == '_')) {
        (*i)++;
    }

    if (start != *i) {
        char *key = ft_substr(str, start, *i - start);
        char *value = get_env_value(key, shell->envp);
        if (value) {
            for (int k = 0; value[k]; k++) {
                result[(*j)++] = value[k];
            }
        }
        free(key);
    }
}


/*
static void handle_dollar(t_shell *shell, const char *str, int *i, char *result, int *j) {
    (*i)++; // Пропускаем символ '$'

    printf("Handling $ at position %d\n", *i);

    // Если сразу после '$' идет цифра
    if (isdigit(str[*i])) {
        printf("Handling number after $ at position %d\n", *i);
        int num_len = 1;
        while (isdigit(str[*i + num_len])) num_len++; // Подсчитываем длину числа
        for (int k = 0; k < num_len; k++) {
            result[(*j)++] = str[*i + k];
        }
        *i += num_len; // Пропускаем цифры
        return;
    }

    // Если сразу после '$' идет '?'
    if (str[*i] == '?') {
        printf("Handling '?' after $ at position %d\n", *i);
        char *status = malloc(12);
        snprintf(status, 12, "%d", shell->last_status); // Преобразуем last_result в строку
        for (int k = 0; status[k]; k++) {
            result[(*j)++] = status[k];
        }
        free(status);
        (*i)++;
        return;
    }

    // Если сразу после '$' идет ключ из env
    int start = *i;
    while (str[*i] && (isalnum(str[*i]) || str[*i] == '_')) {
        (*i)++;
    }
    char *key = ft_substr(str, start, *i - start);
    printf("Processing env variable: %s\n", key);
    char *value = get_env_value(key, shell->envp);
    if (value) {
        printf("Found env value: %s\n", value);
        for (int k = 0; value[k]; k++) {
            result[(*j)++] = value[k];
        }
    } else {
        printf("Env variable not found: %s\n", key);
    }
    free(key);
}*/


// Функция для подсчета общей длины строки с учетом переменных окружения
/*int ft_trouve_len(const char *input, char **envp) {
    int i = 0;
    int len = 0;
    char quote = '\0';

    while (input[i]) {
        if (input[i] == '\'' || input[i] == '\"') {
            // Обработка кавычек
            if (quote == '\0') {
                quote = input[i];
            } else if (quote == input[i]) {
                quote = '\0';
            }
        } else if (input[i] == '$' && quote != '\'' && isdigit(input[i + 1])) {
            // Если после $ идет цифра
            len += 11; // Предположим максимальную длину числа
            i++; // Пропустить число
        } else if (input[i] == '$' && quote != '\'') {
            // Обработка переменной окружения
            i++;
            int start = i;
            while (input[i] && (isalnum(input[i]) || input[i] == '_')) {
                i++;
            }
            char *key = ft_substr(input, start, i - start);
            len += ft_countlen_envp(key, envp);
            free(key);
        }
        i++;
    }
    return len + i;
}*/

int ft_trouve_len(const char *input, char **envp) {
    int i = 0;
    int len = 0;
    char quote = '\0';

    while (input[i]) {
        if (input[i] == '\'' || input[i] == '\"') {
            if (quote == '\0') {
                quote = input[i]; // Opening quote
            } else if (quote == input[i]) {
                quote = '\0'; // Closing quote
            }
            i++;
        } else if (input[i] == '$' && quote != '\'') {
            i++; // Skip '$'
            if (input[i] == '?') {
                // Handle $?
                len += 11; // Fixed length for status
                i++;
            } else if (isalnum(input[i]) || input[i] == '_') {
                // Handle environment variable
                int start = i;
                while (isalnum(input[i]) || input[i] == '_') {
                    i++;
                }
                char *key = ft_substr(input, start, i - start);
                len += ft_countlen_envp(key, envp);
                free(key);
            }
        } else {
            len++;
            i++;
        }
    }
    return len + 1; // Include space for null terminator
}



char *process_str(const char *input, t_shell *shell) {
    int len = ft_trouve_len(input, shell->envp); 
    char *result = malloc(len + 1);  
    if (!result) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    printf("Processing string: %s\n", input); // Выводим строку, которую обрабатываем

    int i = 0, j = 0;
    char quote = '\0';

    while (input[i]) {
        printf("Current character: %c, i: %d\n", input[i], i); // Выводим текущий символ

        if (input[i] == '\'' || input[i] == '\"') {
            // Обработка кавычек
            if (quote == '\0') {
                quote = input[i]; // Открываем кавычку
                i++; // Пропускаем кавычку
            } else if (quote == input[i]) {
                quote = '\0'; // Закрываем кавычку
                i++; // Пропускаем закрывающую кавычку
            } else {
                result[j++] = input[i];
                i++; // Увеличиваем индекс
            }
        } else if (input[i] == '$' && quote != '\'') {
            // Обработка переменных окружения
            printf("Found $ at position %d\n", i); // Выводим информацию о встрече $
            handle_dollar(shell, input, &i, result, &j);
        } else {
            result[j++] = input[i++];
        }
    }
    printf("result - %s", result);
    result[j] = '\0'; // Завершаем строку
    return result;
}




void expand_part(t_shell *shell) {
    // Iterate through all commands in the linked list
    while (shell->commands) {
        t_simple_cmds *current_command = shell->commands;

        // Iterate through all strings in the `str` array for the current command
        char **str_array = current_command->str;  // Access the array of strings
        while (str_array && *str_array) {  // Process each string in the array
            char *str = *str_array;  // Get the current string
            printf("str - %s\n", str);

            if (str && *str) {
                // Process the string to replace environment variables
                char *new_str = process_str(str, shell);  // Get the processed string

                // Replace the old string if a new one is returned
                if (new_str) {
                    free(str);  // Free the old string
                    *str_array = new_str;  // Update the string in the array
                }
            }
            // Move to the next string in the array
            str_array++;
        }

        // Move to the next command in the linked list
        shell->commands = shell->commands->next;
    }
    printf("NEW\n");
    print_simple_cmds(shell->commands);
}



#include <string.h>



#include <stdio.h>
void	ft_bzero(void *s, size_t n)
{
	size_t	i;
	char	*ptr;

	ptr = (char *)s;
	i = 0;
	while (i < n)
	{
		ptr[i] = '\0' ;
		i++;
	}
}
void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*ptr;

	if ((nmemb >= 65535 && size >= 65535) || nmemb * size >= 65535)
		return (NULL);
	ptr = malloc(nmemb * size);
	if (!ptr)
		return (NULL);
	ft_bzero(ptr, size * nmemb);
	return (ptr);
}
/*
int	ft_size(long int n)
{
	int		i;

	i = 1;
	if (n < 0)
	{
		n *= -1;
		i++;
	}
	while (n >= 10)
	{
		n = n / 10;
		i++;
	}
	return (i);
}

char	*ft_itoa(int n)
{
	int			i;
	long int	nb;
	char		*str;

	nb = n;
	i = ft_size(nb);
	str = ft_calloc(sizeof(char), i + 1);
	if (!str)
		return (NULL);
	if (nb < 0)
		nb = -nb;
	while (i > 0)
	{
		i--;
		str[i] = nb % 10 + '0';
		nb = nb / 10;
	}
	if (n < 0)
		str[0] = '-';
	return (str);
}

int main(int argc, char **argv, char **envp) {
    size_t len;

    len = ft_countlen_envp("HOME", envp);
    printf("Length of PATH value: %zu\n", len);

    len = ft_countlen_envp("USER", envp);
    printf("Length of USER value: %zu\n", len);

    len = ft_countlen_envp("UNKNOWN", envp);
    printf("Length of UNKNOWN value: %zu\n", len);
    int a = 123568913;
    int b = strlen(ft_itoa(a));
    printf("len b = %d", b);
    return 0;
}
*/