/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exp5.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zserobia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 17:06:53 by zserobia          #+#    #+#             */
/*   Updated: 2024/11/19 17:06:55 by zserobia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct s_shell {
    char **envp;         // Переменные окружения
    int last_status;     // Статус последней команды
    struct s_command *commands;  // Список команд
} t_shell;

typedef struct s_command {
    char **str;  // Строки команды
} t_command;

char *process_str(const char *input, t_shell *shell);
int ft_trouve_len(const char *input, char **envp);
void expand_part(t_shell *shell);
char *ft_substr(const char *s, int start, int len);
char *get_env_value(const char *var_name, char **envp);
static void handle_dollar(t_shell *shell, const char *str, int *i, char *result, int *j);

void setup_test_environment(t_shell *shell) {
    // Пример переменных окружения
    shell->envp = (char **)malloc(3 * sizeof(char *));
    shell->envp[0] = strdup("USER=zserobia");
    shell->envp[1] = strdup("HOME=/home/zserobia");
    shell->envp[2] = NULL;

    shell->last_status = 0;  // Установим статус последней команды

    // Инициализация команды
    shell->commands = (t_command *)malloc(sizeof(t_command));
    shell->commands->str = (char **)malloc(sizeof(char *));
    *shell->commands->str = strdup("echo $USER, welcome to $HOME!");
}

void print_simple_cmds(t_command *cmd) {
    if (cmd && cmd->str) {
        printf("Processed command: %s\n", *cmd->str);
    }
}

void expand_part(t_shell *shell) {
    if (shell->commands == NULL || shell->commands->str == NULL) {
        printf("Error: commands or str is NULL!\n");
        return;
    }

    char *str = *shell->commands->str;  // Получаем оригинальную строку

    while (str && *str) {
        // Обработка строки с заменой переменных окружения
        char *new_str = process_str(str, shell);  // Получаем новую строку

        // Освобождаем старую строку (если она была выделена динамически)
        free(*shell->commands->str);
        
        // Присваиваем обработанную строку обратно в команду
        *shell->commands->str = new_str;

        // Перезаписываем str на новую строку для дальнейшей обработки
        str = *shell->commands->str;
    }

    // Выводим команду, которая была обработана
    print_simple_cmds(shell->commands);
}

int main() {
    t_shell shell;
    setup_test_environment(&shell);

    // Обрабатываем строку с помощью функции expand_part
    expand_part(&shell);

    // Освобождаем память после выполнения
    free(*shell.commands->str);
    free(shell.commands->str);
    free(shell.commands);
    for (int i = 0; shell.envp[i]; i++) {
        free(shell.envp[i]);
    }
    free(shell.envp);

    return 0;
}

char *get_env_value(const char *var_name, char **envp) {
    size_t var_len = strlen(var_name);
    for (int i = 0; envp[i] != NULL; i++) {
        if (strncmp(envp[i], var_name, var_len) == 0 && envp[i][var_len] == '=') {
            return &envp[i][var_len + 1]; // Возвращаем строку после '='
        }
    }
    return NULL; // Если переменная не найдена
}

char *process_str(const char *input, t_shell *shell) {
    int len = ft_trouve_len(input, shell->envp); 

    char *result = malloc(len + 1);  
    if (!result) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int i = 0, j = 0;
    char quote = '\0';

    while (input[i]) {
        if (input[i] == '\'' || input[i] == '\"') {
            if (quote == '\0') {
                quote = input[i]; // Открываем кавычку
            } else if (quote == input[i]) {
                quote = '\0'; // Закрываем кавычку
            } else {
                result[j++] = input[i];
            }
            i++;
        } else if (input[i] == '$' && quote != '\'') {
            handle_dollar(shell, input, &i, result, &j);
        } else {
            result[j++] = input[i++];
        }
    }

    result[j] = '\0'; // Завершаем строку
    return result;
}

static void handle_dollar(t_shell *shell, const char *str, int *i, char *result, int *j) {
    (*i)++; // Пропускаем символ '$'

    if (isdigit(str[*i])) {
        int num_len = 1;
        while (isdigit(str[*i + num_len])) num_len++;
        for (int k = 0; k < num_len; k++) {
            result[(*j)++] = str[*i + k];
        }
        *i += num_len;
        return;
    }

    if (str[*i] == '?') {
        char *status = malloc(12);
        snprintf(status, 12, "%d", shell->last_status);
        for (int k = 0; status[k]; k++) {
            result[(*j)++] = status[k];
        }
        free(status);
        (*i)++;
        return;
    }

    int start = *i;
    while (str[*i] && (isalnum(str[*i]) || str[*i] == '_')) {
        (*i)++;
    }
    char *key = ft_substr(str, start, *i - start);
    char *value = get_env_value(key, shell->envp);
    if (value) {
        for (int k = 0; value[k]; k++) {
            result[(*j)++] = value[k];
        }
    }
    free(key);
}

int ft_trouve_len(const char *input, char **envp) {
    int i = 0;
    int len = 0;
    char quote = '\0';

    while (input[i]) {
        if (input[i] == '\'' || input[i] == '\"') {
            if (quote == '\0') {
                quote = input[i];
            } else if (quote == input[i]) {
                quote = '\0';
            }
        } else if (input[i] == '$' && quote != '\'' && isdigit(input[i + 1])) {
            len += 11; // Максимальная длина числа
            i++;
        } else if (input[i] == '$' && quote != '\'') {
            i++;
            int start = i;
            while (input[i] && (isalnum(input[i]) || input[i] == '_')) {
                i++;
            }
            char *key = ft_substr(input, start, i - start);
            len += strlen(get_env_value(key, envp));
            free(key);
        }
        i++;
    }
    return len + i;
}

char *ft_substr(const char *s, int start, int len) {
    char *sub = malloc(len + 1);
    if (!sub) {
        return NULL;
    }
    strncpy(sub, s + start, len);
    sub[len] = '\0';
    return sub;
}
