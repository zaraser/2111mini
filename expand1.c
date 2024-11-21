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
char *ft_calloc(size_t count, size_t size);
size_t ft_strlen(const char *str);
int ft_trouve_len(const char *input, char **envp);
size_t ft_countlen_envp(char *str, char **envp);
char *get_env_value(const char *var_name, char **envp);
void handle_dollar(t_shell *shell, const char *str, int *i, char *result, int *j, int *current_allocated_size);
char *ft_str_concat(char *s1, size_t new_size);  // Rename to reflect concatenation purpose

// String concatenation function (formerly ft_realloc)
char *ft_str_concat(char *s1, size_t new_size) {
    if (!s1) {
        return NULL;
    }

    // Allocate new memory for the concatenated string (size can be determined by new_size)
    char *new_str = realloc(s1, new_size);
    if (!new_str) {
        perror("realloc failed");
        exit(EXIT_FAILURE);
    }

    return new_str;
}

char *process_str(const char *input, t_shell *shell) {
    int len = ft_trouve_len(input, shell->envp) + 1;  // +1 for null terminator
    int current_allocated_size = len;  // Track allocated size
    char *result = malloc(current_allocated_size);
    if (!result) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int i = 0, j = 0;
    char quote = '\0';

    while (input[i]) {
        if (input[i] == '\'' || input[i] == '\"') {
            // Handle quotes
            if (quote == '\0') {
                quote = input[i++];
            } else if (quote == input[i]) {
                quote = '\0';
                i++;
            } else {
                result[j++] = input[i++];
            }
        } else if (input[i] == '$' && quote != '\'') {
            // Handle dollar expansion (environment variables)
            handle_dollar(shell, input, &i, result, &j, &current_allocated_size);
        } else {
            // Regular character
            result[j++] = input[i++];
        }

        // Resize the result buffer dynamically during the loop if it's near full
        if (j >= current_allocated_size - 1) {  // Reserve space for null terminator
            current_allocated_size *= 2;  // Double the size (or use another factor)
            result = ft_str_concat(result, current_allocated_size);  // Resize with the correct size
            if (!result) {
                perror("realloc failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Null-terminate the result string
    result[j] = '\0';

    return result;
}

// Calculate the length of the string accounting for environment variable expansion
int ft_trouve_len(const char *input, char **envp) {
    int i = 0;
    int len = 0;
    char quote = '\0';

    while (input[i]) {
        if (input[i] == '\'' || input[i] == '\"') {
            // Handle quotes
            if (quote == '\0') {
                quote = input[i];
            } else if (quote == input[i]) {
                quote = '\0';
            }
            i++;
        } else if (input[i] == '$' && quote != '\'') {
            // Handle environment variable expansion
            i++; // Skip '$'
            if (input[i] == '?') {
                len += 11; // Space for the status code (usually 11 chars)
                i++;
            } else if (isalnum(input[i]) || input[i] == '_') {
                int start = i;
                while (isalnum(input[i]) || input[i] == '_') {
                    i++;
                }
                char *key = ft_substr(input, start, i - start);
                len += ft_countlen_envp(key, envp); // Add the length of the expanded environment variable
                free(key);  // Free temporary key
            }
        } else {
            len++;  // Regular character, add to length
            i++;
        }
    }

    return len + 1; // Add 1 for null terminator
}

// Calculate the length of an environment variable's value
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

// Retrieve the value of an environment variable
char *get_env_value(const char *var_name, char **envp) {
    size_t var_len = strlen(var_name);
    for (int i = 0; envp[i] != NULL; i++) {
        if (strncmp(envp[i], var_name, var_len) == 0 && envp[i][var_len] == '=') {
            return &envp[i][var_len + 1];
        }
    }
    return NULL;
}

// Handle the expansion of variables in the input string
void handle_dollar(t_shell *shell, const char *str, int *i, char *result, int *j, int *current_allocated_size) {
    (*i)++; // Skip '$'
    if (str[*i] == '?') {
        char status[12];
        snprintf(status, sizeof(status), "%d", shell->last_status);
        for (int k = 0; status[k]; k++) {
            result[(*j)++] = status[k];
        }
        (*i)++;
    } else {
        int start = *i;
        while (isalnum(str[*i]) || str[*i] == '_') {
            (*i)++;
        }
        if (start != *i) {
            char *key = ft_substr(str, start, *i - start);
            char *value = get_env_value(key, shell->envp);
            if (value) {
                int len = strlen(value);
                // Resize result buffer if needed
                if (*j + len >= *current_allocated_size) {
                    *current_allocated_size = *j + len + 1;
                    result = ft_str_concat(result, *current_allocated_size); // Handle resizing
                    if (!result) {
                        perror("realloc failed");
                        exit(EXIT_FAILURE);
                    }
                }
                for (int k = 0; value[k]; k++) {
                    result[(*j)++] = value[k];
                }
            }
            free(key);
        }
    }
}

// Main function to expand environment variables in the commands
void expand_part(t_shell *shell) {
    t_simple_cmds *current_command = shell->commands;

    while (current_command) {
        char **str_array = current_command->str;
        while (str_array && *str_array) {
            char *str = *str_array;

            if (str && *str) {
                char *new_str = process_str(str, shell);
                if (new_str) {
                    free(str);
                    *str_array = new_str;
                } else {
                    fprintf(stderr, "Error: process_str failed for string: %s\n", str);
                }
            }
            str_array++;
        }
        current_command = current_command->next;
        printf("new\n");
        print_simple_cmds(shell->commands);
    }
}


#include "parser.h"

/*
char *ft_calloc(size_t count, size_t size);
size_t ft_strlen(const char *str);
int ft_trouve_len(const char *input, char **envp);
size_t ft_countlen_envp(char *str, char **envp);
char *get_env_value(const char *var_name, char **envp);
void handle_dollar(t_shell *shell, const char *str, int *i, char *result, int *j, int *current_allocated_size);
char *ft_str_concat(char *s1, size_t new_size);  // Rename to reflect concatenation purpose

// String concatenation function (manual resizing without realloc)
char *ft_str_concat(char *s1, size_t new_size) {
    if (!s1) {
        return NULL;
    }

    // Allocate new memory for the concatenated string (size can be determined by new_size)
    char *new_str = malloc(new_size);
    if (!new_str) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    // Copy the old string into the new one
    memcpy(new_str, s1, new_size - 1);
    new_str[new_size - 1] = '\0';  // Ensure null-termination

    free(s1);  // Free the old string

    return new_str;
}

char *process_str(const char *input, t_shell *shell) {
    int len = ft_trouve_len(input, shell->envp) + 1;  // +1 for null terminator
    int current_allocated_size = len;  // Track allocated size
    char *result = malloc(current_allocated_size);
    if (!result) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int i = 0, j = 0;
    char quote = '\0';

    while (input[i]) {
        if (input[i] == '\'' || input[i] == '\"') {
            // Handle quotes
            if (quote == '\0') {
                quote = input[i++];
            } else if (quote == input[i]) {
                quote = '\0';
                i++;
            } else {
                result[j++] = input[i++];
            }
        } else if (input[i] == '$' && quote != '\'') {
            // Handle dollar expansion (environment variables)
            handle_dollar(shell, input, &i, result, &j, &current_allocated_size);
        } else {
            // Regular character
            result[j++] = input[i++];
        }

        // Resize the result buffer dynamically during the loop if it's near full
        if (j >= current_allocated_size - 1) {  // Reserve space for null terminator
            current_allocated_size *= 2;  // Double the size (or use another factor)
            result = ft_str_concat(result, current_allocated_size);  // Resize with the correct size
            if (!result) {
                perror("malloc failed");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Null-terminate the result string
    result[j] = '\0';

    return result;
}

// Calculate the length of the string accounting for environment variable expansion
int ft_trouve_len(const char *input, char **envp) {
    int i = 0;
    int len = 0;
    char quote = '\0';

    while (input[i]) {
        if (input[i] == '\'' || input[i] == '\"') {
            // Handle quotes
            if (quote == '\0') {
                quote = input[i];
            } else if (quote == input[i]) {
                quote = '\0';
            }
            i++;
        } else if (input[i] == '$' && quote != '\'') {
            // Handle environment variable expansion
            i++; // Skip '$'
            if (input[i] == '?') {
                len += 11; // Space for the status code (usually 11 chars)
                i++;
            } else if (isalnum(input[i]) || input[i] == '_') {
                int start = i;
                while (isalnum(input[i]) || input[i] == '_') {
                    i++;
                }
                char *key = ft_substr(input, start, i - start);
                len += ft_countlen_envp(key, envp); // Add the length of the expanded environment variable
                free(key);  // Free temporary key
            }
        } else {
            len++;  // Regular character, add to length
            i++;
        }
    }

    return len + 1; // Add 1 for null terminator
}

// Calculate the length of an environment variable's value
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

// Retrieve the value of an environment variable
char *get_env_value(const char *var_name, char **envp) {
    size_t var_len = strlen(var_name);
    for (int i = 0; envp[i] != NULL; i++) {
        if (strncmp(envp[i], var_name, var_len) == 0 && envp[i][var_len] == '=') {
            return &envp[i][var_len + 1];
        }
    }
    return NULL;
}

// Handle the expansion of variables in the input string
void handle_dollar(t_shell *shell, const char *str, int *i, char *result, int *j, int *current_allocated_size) {
    (*i)++; // Skip '$'
    if (str[*i] == '?') {
        char status[12];
        snprintf(status, sizeof(status), "%d", shell->last_status);
        for (int k = 0; status[k]; k++) {
            result[(*j)++] = status[k];
        }
        (*i)++;
    } else {
        int start = *i;
        while (isalnum(str[*i]) || str[*i] == '_') {
            (*i)++;
        }
        if (start != *i) {
            char *key = ft_substr(str, start, *i - start);
            char *value = get_env_value(key, shell->envp);
            if (value) {
                int len = strlen(value);
                // Resize result buffer if needed
                if (*j + len >= *current_allocated_size) {
                    *current_allocated_size = *j + len + 1;
                    result = ft_str_concat(result, *current_allocated_size); // Handle resizing
                    if (!result) {
                        perror("malloc failed");
                        exit(EXIT_FAILURE);
                    }
                }
                for (int k = 0; value[k]; k++) {
                    result[(*j)++] = value[k];
                }
            }
            free(key);
        }
    }
}

// Main function to expand environment variables in the commands
void expand_part(t_shell *shell) {
    t_simple_cmds *current_command = shell->commands;

    while (current_command) {
        char **str_array = current_command->str;
        while (str_array && *str_array) {
            char *str = *str_array;

            if (str && *str) {
                char *new_str = process_str(str, shell);
                if (new_str) {
                    free(str);
                    *str_array = new_str;
                } else {
                    fprintf(stderr, "Error: process_str failed for string: %s\n", str);
                }
            }
            str_array++;
        }
        current_command = current_command->next;
        printf("new\n");
        print_simple_cmds(shell->commands);
    }
}
*/



/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

size_t ft_countlen_envp(const char *str, char **envp) {
    size_t len = strlen(str);
    while (*envp) {
        if (strncmp(str, *envp, len) == 0 && (*envp)[len] == '=') {
            return strlen(*envp + len + 1);
        }
        envp++;
    }
    return 0;
}

size_t ft_trouve_len(const char *input, char **envp) {
    size_t len = 0;
    int i = 0;
    char quote = '\0';

    while (input[i]) {
        if (input[i] == '\'' || input[i] == '\"') {
            // Обработка кавычек
            if (quote == '\0') {
                quote = input[i];
            } else if (quote == input[i]) {
                quote = '\0';
            }
            i++;
        } else if (input[i] == '$' && quote != '\'') {
            // Обработка переменной окружения
            i++; // Пропускаем '$'
            if (input[i] == '?') {
                // Статус выполнения команды (например, $? в bash)
                len += 11; // Обычно статус выполняется в 11 символах
                i++;
            } else if (isdigit(input[i])) {
                // Если после '$' идет цифра, считаем длину как число
                char num[12]; // Максимальная длина числа int
                int num_len = sprintf(num, "%d", atoi(&input[i]));
                len += num_len; // Добавляем длину числа
                i += num_len; // Пропускаем число
            } else if (isalnum(input[i]) || input[i] == '_') {
                // Обработка переменной окружения
                int start = i;
                while (isalnum(input[i]) || input[i] == '_') {
                    i++;
                }
                char *key = strndup(&input[start], i - start); // Получаем ключ переменной
                len += ft_countlen_envp(key, envp); // Добавляем длину значения переменной
                free(key);  // Освобождаем память
            }
        } else {
            len++;  // Обычный символ
            i++;
        }
    }
    return len + 1;  // +1 для завершающего нуля
}

char *process_str(const char *input, char **envp) {
    size_t len = ft_trouve_len(input, envp);
    printf("len = %ld\n", len);
    char *result = malloc(len);
    if (!result) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    size_t i = 0, j = 0;
    char quote = '\0';

    while (input[i]) {
        if (input[i] == '\'' || input[i] == '\"') {
            // Обработка кавычек
            if (quote == '\0') {
                quote = input[i++];
            } else if (quote == input[i]) {
                quote = '\0';
                i++;
            } else {
                result[j++] = input[i++];
            }
        } else if (input[i] == '$' && quote != '\'') {
            // Обработка переменной окружения
            i++;  // Пропускаем '$'
            if (input[i] == '?') {
                char status[12];
                snprintf(status, sizeof(status), "%d", 0); // Пример статуса, замените на ваш
                for (int k = 0; status[k]; k++) {
                    result[j++] = status[k];
                }
                i++;
            } else if (isdigit(input[i])) {
                // Если после '$' идет цифра, добавляем ее
                char num[12];
                int num_len = sprintf(num, "%d", atoi(&input[i]));
                for (int k = 0; k < num_len; k++) {
                    result[j++] = num[k];
                }
                i += num_len;
            } else if (isalnum(input[i]) || input[i] == '_') {
                // Обработка переменной окружения
                int start = i;
                while (isalnum(input[i]) || input[i] == '_') {
                    i++;
                }
                char *key = strndup(&input[start], i - start);
                char *value = getenv(key);  // Получаем значение переменной окружения
                if (value) {
                    size_t value_len = strlen(value);
                    memcpy(result + j, value, value_len);
                    j += value_len;
                }
                free(key);
            }
        } else {
            result[j++] = input[i++];
        }
    }

    result[j] = '\0';  // Завершающий нулевой символ
    return result;
}

int main() {
    char *input = "Hello $USER!";
    
    char *envp[] = {"USER=example", NULL};  // Пример envp
    char *output = process_str(input, envp);
    printf("Processed: %s\n", output);
    free(output);
    return 0;
}
*/