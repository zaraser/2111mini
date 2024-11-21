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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Обработка кавычек в строках со знаком $
char *process_quotes_with_dollar(const char *str) {
    if (dollar_in_single_quotes(str)) {
        // Если $ внутри одинарных кавычек, обрабатываем строку как обычную
        return process_quotes_no_dollar(str);
    }

    // Если $ вне одинарных кавычек, добавьте свою логику для интерпретации $:
    // Например, заменить $VAR на значение переменной окружения.
    return strdup(str); // Пока просто возвращаем копию строки
}

// Основная функция обработки строки
char *process_string(const char *str) {
    if (contains_dollar_sign(str)) {
        return process_quotes_with_dollar(str);
    } else {
        return process_quotes_no_dollar(str);
    }
}

int main() {
    const char *test_str1 = "h'j'";             // Без $
    const char *test_str2 = "h\"j\"";           // Без $
    const char *test_str3 = "h\"'j'\"";         // Без $
    const char *test_str4 = "some $var here";   // Со $
    const char *test_str5 = "'some $var here'"; // $ внутри одинарных кавычек
    const char *test_str6 = "\"some $var here\""; // $ внутри двойных кавычек

    char *processed_str1 = process_string(test_str1);
    char *processed_str2 = process_string(test_str2);
    char *processed_str3 = process_string(test_str3);
    char *processed_str4 = process_string(test_str4);
    char *processed_str5 = process_string(test_str5);
    char *processed_str6 = process_string(test_str6);

    printf("Processed 1: %s\n", processed_str1); // Ожидаем: hj
    printf("Processed 2: %s\n", processed_str2); // Ожидаем: hj
    printf("Processed 3: %s\n", processed_str3); // Ожидаем: h'j'
    printf("Processed 4: %s\n", processed_str4); // Ожидаем: some $var here (или результат обработки $)
    printf("Processed 5: %s\n", processed_str5); // Ожидаем: some $var here
    printf("Processed 6: %s\n", processed_str6); // Ожидаем: some $var here (или результат обработки $)

    free(processed_str1);
    free(processed_str2);
    free(processed_str3);
    free(processed_str4);
    free(processed_str5);
    free(processed_str6);

    return 0;
}

