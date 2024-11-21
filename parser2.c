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

#include "parser.h"  // Предполагаем, что ваши функции объявлены в parser.h

// Прототипы функций

/*
После $ идет цифра то она пропускается и записываюся все следующие данные
если после $ идет _  то все что идет после не печататется кроме если будет $
если после $ идет ? то печатается статус
если что-то другое то мы ищем в envp
ведь так ведет себя баш?
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo $123
23
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo $12
2
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo $1

zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo $_
echo
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo $_____

zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo $_____jfkg

zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo $_____jfkg123

zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ $_

zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ $USER
zserobia: command not found
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo $USER
zserobia
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo $USER1

zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo "'$USER'"
'zserobia'
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo "'$USERjk'"
''
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$ echo $kl

zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza$

*/

int ft_trouve_len(const char *input, char **envp)
{
    int i;
    int len;
    char quote;

    i = 0;
    len = 0;
    quote = '\0';

    while (input[i])
    {
        if (input[i] == '\'' || input[i] == '\"')
        {
            if (quote == '\0')
                quote = input[i];
            else if (quote == input[i])
                quote = '\0';
            else
                len++;
            i++;
        } 
        else if (input[i] == '$' && quote != '\'')
        {
            i++; // Пропускаем '$'
            if (input[i] == '\0' || ft_ifspace(input[i]) ||input[i] == '\'' )
                len++;
            else if (input[i] == '?')
            {
                len += 11; // Пространство для статуса завершения
                i++;
            } 
            else if (isdigit(input[i]))
            {
                i++;
                while (isdigit(input[i]))
                {
                    len++; // Добавляем длину для оставшихся цифр
                    i++;
                }
            } 
            /*else if (input[i] == '_') {
                while (isalnum(input[i]) || input[i] == '_') {
                    i++;
                }
            } */
            else if (isalpha(input[i]) || input[i] == '_' )
            {
                int start = i;
                while (isalnum(input[i]) || input[i] == '_')
                    i++;
                char *key = ft_substr(input, start, i - start);
                char *value = get_env_value(key, envp);
                if (value) {
                    len += strlen(value);
                    //printf("LEN = %d\n VALUE = %s\n", len, value);
                }
                free(key);
            } 
            else
                i++; // Пропускаем недопустимый символ
        } 
        else
        {
            len++;
            i++;
        }
    }
    return (len + 1); // Учитываем символ завершения строки
}

// Функция для подсчета длины значения переменной окружения
size_t ft_countlen_envp(char *str, char **envp)
{
    size_t len;

    len = strlen(str);
    while (*envp)
    {
        if (strncmp(str, *envp, len) == 0 && (*envp)[len] == '=')
            return (strlen(*envp + len + 1)); // Возвращаем длину значения переменной
        envp++;
    }
    return (0);
}

// Получение значения переменной окружения
char *get_env_value(const char *var_name, char **envp)
{
    size_t var_len;
    int i;

    var_len = strlen(var_name);
    i = 0;
    while (envp[i] != NULL)
    {
        if (strncmp(envp[i], var_name, var_len) == 0 && envp[i][var_len] == '=')
            return (&envp[i][var_len + 1]);
        i++;
    }
    return (NULL);
}

void handle_dollar(t_shell *shell, const char *str, int *i, char *result, int *j)
{
    (*i)++; // Пропускаем '$'
    if (str[*i] == '\0' || ft_ifspace(str[*i]))
        result[(*j)++] = '$';
    else if (str[*i] == '?')
    {
        char *a = ft_itoa(global_exit); // Преобразуем число в строку
        if (!a) // Проверяем успешность выделения памяти
         return; // Или обработка ошибки
        char *temp = a; // Сохраняем оригинальный указатель
        while (*a)
        result[(*j)++] = *a++;
        free(temp); // Освобождаем память, если itoa использует malloc
        (*i)++;
    }
    else if (isdigit(str[*i]))
    {
        (*i)++;
        while (isdigit(str[*i]))
            result[(*j)++] = str[(*i)++];
    } 
    /*else if (str[*i] == '_') {
        while (isalnum(str[*i]) || str[*i] == '_') {
            (*i)++;
        }
    } */
    else if (isalpha(str[*i]) || str[*i] == '_')
    {
        int start = *i;
        int k;

        k = 0;
        while (isalnum(str[*i]) || str[*i] == '_')
            (*i)++;
        char *key = ft_substr(str, start, *i - start);
        char *value = get_env_value(key, shell->envp);
        if (value)
        {
            while ( value[k])
            {
                result[(*j)++] = value[k];
                k++;
            }
        }
        free(key);
    } 
    else {
        (*i)++; // Пропускаем недопустимый символ
    }
}

// Главная функция для обработки строки с расширением переменных
char *process_str(const char *input, t_shell *shell)
{
    int len;
    char *result;

    len = ft_trouve_len(input, shell->envp) + 1;
    printf("len = %d\n", len);
    result = malloc(len);
    if (!result) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    int i = 0, j = 0;
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
        } else if (input[i] == '$' && quote != '\'' && input[i + 1] != '\0' && !ft_ifspace(input[i + 1]) && input[i + 1] != '\'' && input[i + 1] != '"') {
            // Обработка переменных окружения
            handle_dollar(shell, input, &i, result, &j);
        } else {
            // Обычный символ
            result[j++] = input[i++];
        }
    }
    // Завершаем строку
    result[j] = '\0';

    return result;
}

// Главная функция для расширения переменных в командах
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
        printf("NEWS\n");
        print_simple_cmds(shell->commands);  // Печать команд (если необходимо)
    }
}

