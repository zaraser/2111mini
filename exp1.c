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
/*
#include <stdio.h>
#include <string.h>

// Функция для получения значения переменной окружения
char *get_env_value(char *var_name, char **envp)
{
    size_t var_len = strlen(var_name);
    for (int i = 0; envp[i] != NULL; i++) // Перебираем массив envp
    {
        // Проверяем, совпадает ли имя переменной (плюс '=')
        if (strncmp(envp[i], var_name, var_len) == 0 && envp[i][var_len] == '=')
        {
            // Возвращаем строку после '='
            return &envp[i][var_len + 1];
        }
    }
    return NULL; // Если переменная не найдена, возвращаем NULL
}

int main(int argc, char **argv, char **envp)
{
    char *var_name = "HOME"; // Пример переменной
    char *value = get_env_value(var_name, envp);

    if (value)
        printf("Value of %s: %s\n", var_name, value);
    else
        printf("Variable %s not found.\n", var_name);

    return 0;
}*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Получение значения переменной окружения
char *get_env_value(char *var_name, char **envp) {
    size_t var_len = strlen(var_name);
    for (int i = 0; envp[i] != NULL; i++) { // Перебираем массив envp
        // Проверяем, совпадает ли имя переменной (плюс '=')
        if (strncmp(envp[i], var_name, var_len) == 0 && envp[i][var_len] == '=') {
            // Возвращаем строку после '='
            return &envp[i][var_len + 1];
        }
    }
    return NULL; // Если переменная не найдена, возвращаем NULL
}

// Проверка наличия символа '$' в строке
int contains_dollar_sign(const char *str) {
    return strchr(str, '$') != NULL;
}

// Проверка, находится ли символ $ внутри одинарных кавычек
int dollar_in_single_quotes(const char *str) {
    int in_single_quote = 0;
    for (size_t i = 0; str[i]; i++) {
        if (str[i] == '\'') {
            in_single_quote = !in_single_quote; // Входим/выходим из режима одинарных кавычек
        }
        if (str[i] == '$' && in_single_quote) {
            return 1; // Найден $ внутри одинарных кавычек
        }
    }
    return 0; // $ не внутри одинарных кавычек
}

// Обработка кавычек в строках без знака $
char *process_quotes_no_dollar(const char *str) {
    size_t len = strlen(str);
    char *result = malloc(len + 1);
    if (!result)
        return NULL;

    int in_single_quote = 0, in_double_quote = 0;
    size_t j = 0;

    for (size_t i = 0; i < len; i++) {
        char current_char = str[i];

        if (current_char == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote;
        } else if (current_char == '\"' && !in_single_quote) {
            in_double_quote = !in_double_quote;
        } else {
            result[j++] = current_char;
        }
    }
    result[j] = '\0';
    return result;
}

// Обработка строки со знаком $
char *process_quotes_with_dollar(const char *str, char **envp) {
    size_t len = strlen(str);
    char *result = malloc(len * 2 + 1); // Запас места для подстановок
    if (!result)
        return NULL;

    int in_single_quote = 0, in_double_quote = 0;
    size_t i = 0, j = 0;

    while (i < len) {
        char current_char = str[i];

        if (current_char == '\'' && !in_double_quote) {
            in_single_quote = !in_single_quote; // Входим/выходим из одинарных кавычек
            i++;
            continue;
        } else if (current_char == '\"' && !in_single_quote) {
            in_double_quote = !in_double_quote; // Входим/выходим из двойных кавычек
            i++;
            continue;
        } else if (current_char == '$' && !in_single_quote) {
            // Обработка переменной окружения
            i++;
            size_t start = i;
            while (i < len && (isalnum(str[i]) || str[i] == '_')) i++;
            if (i > start) {
                char var_name[256];
                strncpy(var_name, &str[start], i - start);
                var_name[i - start] = '\0';

                // Получаем значение переменной
                const char *value = get_env_value(var_name, envp);
                if (value) {
                    size_t value_len = strlen(value);
                    strncpy(&result[j], value, value_len);ы
                    j += value_len;
                }
                continue;
            } else {
                result[j++] = '$'; // Одиночный $ без имени переменной
                continue;
            }
        }

        // Копируем текущий символ
        result[j++] = current_char;
        i++;
    }

    result[j] = '\0';
    return result;
}

// Основная функция обработки строки
char *process_string(const char *str, char **envp) {
    if (contains_dollar_sign(str)) {
        return process_quotes_with_dollar(str, envp);
    } else {
        return process_quotes_no_dollar(str);
    }
}

// Пример использования
int main(int argc, char **argv, char **envp) {
    const char *test_str1 = "Hello 1$PATH!";                // Переменная вне кавычек
    const char *test_str2 = "Hello '$USER'!";              // $ внутри одинарных кавычек
    const char *test_str3 = "Hello \"$USER\"!";            // $ внутри двойных кавычек
    const char *test_str4 = "This is '$HOME and $USER'.";  // Смешанные случаи
    const char *test_str5 = "No variables here.";          // Без $

    char *result1 = process_string(test_str1, envp);
    char *result2 = process_string(test_str2, envp);
    char *result3 = process_string(test_str3, envp);
    char *result4 = process_string(test_str4, envp);
    char *result5 = process_string(test_str5, envp);

    printf("Result 1: %s\n", result1); // Ожидаем: Hello [значение $USER]!
    printf("Result 2: %s\n", result2); // Ожидаем: Hello $USER!
    printf("Result 3: %s\n", result3); // Ожидаем: Hello [значение $USER]!
    printf("Result 4: %s\n", result4); // Ожидаем: This is $HOME and $USER.
    printf("Result 5: %s\n", result5); // Ожидаем: No variables here.

    free(result1);
    free(result2);
    free(result3);
    free(result4);
    free(result5);

    return 0;
}

