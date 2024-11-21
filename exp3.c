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

//#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Структура для хранения переменных окружения и последнего статуса выхода
typedef struct s_env {
    char **envp;          // Указатель на массив переменных окружения
    int last_result;      // Последний код выхода
} t_env;

// Утилита: проверка на пробел
static int is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

// Утилита: выделение подстроки (аналог ft_substr)
static char *ft_substr(const char *str, int start, int len) {
    char *substr = malloc(len + 1);
    if (!substr) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strncpy(substr, str + start, len);
    substr[len] = '\0';
    return substr;
}

// Получение значения переменной окружения из envp
char *get_env_value(const char *var_name, char **envp) {
    size_t var_len = strlen(var_name);
    for (int i = 0; envp[i] != NULL; i++) { // Перебираем массив envp
        if (strncmp(envp[i], var_name, var_len) == 0 && envp[i][var_len] == '=') {
            return &envp[i][var_len + 1]; // Возвращаем строку после '='
        }
    }
    return NULL; // Если переменная не найдена
}

// Основная функция для обработки переменной $<key>
static void handle_dollar(t_env *env, const char *str, int *i, char *result, int *j) {
    (*i)++; // Пропускаем символ '$'

    // Если сразу после '$' идет цифра
    if (isdigit(str[*i])) {
        (*i)++; // Пропускаем только первую цифру
        return; // Остальные символы копируются как есть
    }

    // Если сразу после '$' идет '?'
    if (str[*i] == '?') {
        char *status = malloc(12);
        snprintf(status, 12, "%d", env->last_result); // Преобразуем last_result в строку
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
    char *value = get_env_value(key, env->envp);
    if (value) {
        for (int k = 0; value[k]; k++) {
            result[(*j)++] = value[k];
        }
    }
    free(key);
}

// Основная функция обработки строки
char *process_str(t_env *env, const char *input) {
    char *result = malloc(strlen(input) * 2 + 1); // Запас для кавычек и замен
    if (!result) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    int i = 0, j = 0;
    char quote = '\0';

    while (input[i]) {
        // Если встречаем кавычку
        if (input[i] == '\'' || input[i] == '\"') {
            if (quote == '\0') {
                quote = input[i]; // Открываем кавычку
            } else if (quote == input[i]) {
                quote = '\0'; // Закрываем кавычку
            } else {
                result[j++] = input[i];
            }
            i++;
        }
        // Если встречаем знак '$'
        else if (input[i] == '$' && quote != '\'') {
            handle_dollar(env, input, &i, result, &j);
        }
        // Обычные символы
        else {
            result[j++] = input[i++];
        }
    }
    result[j] = '\0';
    return result;
}

// Тестовая функция
int main(int argc, char **argv, char **envp) {
    // Инициализация структуры окружения
    t_env env = { envp, 0 };

    // Пример строки для обработки
    char *input = "echo \"'$USER1'\" '$PATH' $12abc $? 'test' \"$?\"  $ADA12";
    char *output = process_str(&env, input);

    printf("Input:  %s\n", input);
    printf("Output: %s\n", output);

    free(output);
    return 0;
}
