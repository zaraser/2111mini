/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exp4.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zserobia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 15:37:45 by zserobia          #+#    #+#             */
/*   Updated: 2024/11/19 15:37:47 by zserobia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *get_env_value(char *name, char **envp) {
    while (*envp) {
        if (strncmp(*envp, name, strlen(name)) == 0 && (*envp)[strlen(name)] == '=') {
            return *envp + strlen(name) + 1;
        }
        envp++;
    }
    return ""; // Если переменная не найдена
}

int get_var_length(const char *str, char **envp) {
    char var_name[256];
    int i = 0;
    
    // Считываем имя переменной или число
    if (isdigit(str[i])) { 
        while (isdigit(str[i])) i++;
        return i; // Возвращаем длину числа
    } else if (str[i] == '?') {
        return snprintf(NULL, 0, "%d", 0); // Код завершения, например, 0
    } else {
        while (isalnum(str[i]) || str[i] == '_') {
            var_name[i] = str[i];
            i++;
        }
        var_name[i] = '\0';
        return strlen(get_env_value(var_name, envp));
    }
}

int calculate_length(const char *input, char **envp) {
    int length = 0;
    int i = 0;
    int in_single_quotes = 0;
    int in_double_quotes = 0;

    while (input[i]) {
        if (input[i] == '\'' && !in_double_quotes) {
            in_single_quotes = !in_single_quotes;
            i++;
        } else if (input[i] == '"' && !in_single_quotes) {
            in_double_quotes = !in_double_quotes;
            i++;
        } else if (input[i] == '$' && !in_single_quotes) {
            i++;
            length += get_var_length(&input[i], envp);
            while (isalnum(input[i]) || input[i] == '_' || input[i] == '?' || isdigit(input[i]))
                i++;
        } else {
            length++;
            i++;
        }
    }
    return length;
}

int main(int argc, char **argv, char **envp) {
    const char *test = "echo \"'$USER1'\" '$PATH' $12abc $? 'test' \"$?\"  $ADA12";
    printf("Length: %d\n", calculate_length(test, envp));
    return 0;
}
