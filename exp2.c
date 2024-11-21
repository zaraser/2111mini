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

#include "parser.h"

// Функция для проверки, является ли символ пробелом
static inline int	is_whitespace(char c)
{
	return (c == ' ' || c == '\n' || c == '\t'); // Возвращает true, если символ - пробел, новая строка или табуляция
}
int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	while ((s1[i] || s2[i]) && i < n)
	{
		if ((unsigned char) s1[i] != (unsigned char) s2[i])
			return ((unsigned char) s1[i] - (unsigned char) s2[i]);
		i++;
	}
	return (0);
}

char	*get_env(t_env *env, char *value)
{
	int			i;
	size_t		tmp;

	i = 0;
	// Перебираем каждую строку в массиве переменных окружения env->envp
	while (env->envp[i])
	{
		// Находим позицию символа '=' в текущей строке переменной окружения
		tmp = ft_strchrn(env->envp[i], '=');
		// Если имя переменной в env->envp совпадает с value и длина совпадает
		if (ft_strncmp(env->envp[i], value, ft_strlen(value)) == 0
			&& tmp == ft_strlen(value))
			// Возвращаем значение переменной (после символа '=')
			return (ft_strdup(env->envp[i] + tmp + 1));
		i++;
	}
	// Если переменная не найдена, возвращаем NULL
	return (NULL);
}

// Функция для расширения переменной окружения в строке ввода
static void	expand_env(t_env *env, char **input, char **result)
{
	int		i; // Индекс для обхода строки ввода
	int		j; // Индекс для копирования значения переменной в результат
	char	*name; // Имя переменной окружения
	char	*value; // Значение переменной окружения

	i = env->x + 1; // Начинаем с символа после '$'
	// Ищем конец имени переменной окружения
	while (is_env_char((*input)[i]) && !is_quote((*input)[i]) && (*input)[i])
		i++;
	// Извлекаем имя переменной окружения
	name = ft_substr(*input, env->x + 1, i - env->x - 1);
	// Проверяем, не является ли имя переменной символом '?', который возвращает последний статус
	if (ft_strncmp(name, "?", 1) == 0)
		value = ft_itoa(env->last_result); // Преобразуем статус в строку
	else
		value = get_env(env, name); // Получаем значение переменной окружения по имени
	j = 0;
	// Копируем значение переменной в результат
	if (value)
		while (value[j])
			(*result)[env->y++] = value[j++]; // Добавляем символы значения к результату
	// Обновляем индекс для следующего обхода
	env->x += ft_strlen(name) + 1;
	free(name); // Освобождаем память, выделенную для имени
	free(value); // Освобождаем память, выделенную для значения
}

// Обрабатывает ввод, заменяя переменные окружения на их значения
static void	handle_envp(t_env *env, char **input, char **result)
{
	env->x = 0; // Сбрасываем индекс для обхода строки ввода
	env->y = 0; // Сбрасываем индекс для записи результата
	while ((*input)[env->x]) // Пока не достигнут конец строки ввода
	{
		// Обработка одиночных кавычек
		if ((*input)[env->x] == '\'' && !check_unclosed_quotes(*input, env->x))
		{
			(*result)[env->y++] = (*input)[env->x++]; // Копируем открывающую кавычку
			// Копируем содержимое внутри кавычек до закрывающей
			while ((*input)[env->x] && (*input)[env->x] != '\'')
				(*result)[env->y++] = (*input)[env->x++];
			if ((*input)[env->x] == '\'') // Копируем закрывающую кавычку
				(*result)[env->y++] = (*input)[env->x++];
		}
		// Если встречаем знак доллара, начинаем расширение переменной окружения
		else if ((*input)[env->x] == '$' && (*input)[env->x + 1] != ' ')
			expand_env(env, input, result); // Вызываем функцию для расширения переменной
		else
			(*result)[env->y++] = (*input)[env->x++]; // Копируем обычный символ в результат
	}
	(*result)[env->y] = '\0'; // Завершаем строку результата нулевым символом
}



static void	toggle_quote_state(char **input, int i, char *quote)
{
	// Если нет открытых кавычек и текущий символ - кавычка, открываем кавычку
	if (*quote == '\0' && is_quote((*input)[i]))
		*quote = (*input)[i];
	// Если текущий символ - это закрывающая кавычка, закрываем кавычку
	else if (*quote == (*input)[i])
		*quote = '\0';
}


// Функция для обрезки пробелов в строке, учитывая состояние кавычек
void	trim_spaces(char **input, char *quote, char	**result)
{
	int		i = 0, j = 0; // Индексы для строк

	*quote = '\0'; // Инициализируем состояние кавычек как "закрытое"
	// Пропускаем начальные пробелы
	//while ((*input)[i] && is_whitespace((*input)[i]))
	//	i++;
	// Основной цикл по входной строке
	while ((*input)[i])
	{
		// Копируем символы в результирующую строку до пробела или конца строки
		while ((*input)[i])
		{
			toggle_quote_state(input, i, quote); // Переключаем состояние кавычек
			(*result)[j] = (*input)[i]; // Копируем текущий символ в результат
			j++;
			i++;
		}
	(*result)[j] = '\0'; // Завершаем результирующую строку нулевым символом
}

// Основная функция для обработки строки и расширения переменных окружения
void	expand_envp(char **envp, char **cmd)
{
	int	i = 0;
	char *result;

	// Для каждой строки в cmd->str (каждая строка - команда или её часть)
	while (*cmd)
	{
		result = safe_malloc(ft_strlen(str[i]) + 1);
		trim_spaces(&(str[i]), &quote, &result); // Убираем лишние пробелы
		handle_envp(envp, &(str[i]), &result); // Обрабатываем переменные окружения
		free(str[i]);
		str[i] = result; // Обновляем строку с результатом
		i++;
		cmd++;
	}
}

void	*safe_malloc(size_t size)
{
	void	*ptr;  // Указатель для хранения адреса выделенной памяти

	// Выделение памяти с помощью malloc
	ptr = malloc(size);  
	
	// Проверка, успешно ли выделена память
	if (!ptr)  
		// Если выделение памяти не удалось, завершить программу с сообщением об ошибке
		exit_program(NULL, "malloc failed", 1);  
	
	// Возвращает указатель на выделенную память
	return (ptr);  
}


static void trim_input(char **input)
{
	char	*result; // Указатель на результат
	char	quote; // Переменная для хранения типа кавычек

	result = safe_malloc(strlen(*input) + 1); // Выделяем память под результат
	trim_spaces(input, &quote, &result); // Удаляем пробелы и запоминаем тип кавычек
	free(*input); // Освобождаем исходный ввод
	if (quote != '\0') // Если кавычки не закрыты
	{
		free(result); // Освобождаем результат, так как ввод некорректен
		*input = NULL; // Устанавливаем указатель на NULL
	}
	else
		*input = result; // Устанавливаем указатель на очищенный ввод
}

// Пример вызова функции в parse_input
char	**parse_input(char **envp, char **input)
{
	trim_input(input); // Удаляем лишние пробелы
	if (!*input || !**input) // Если ввод пустой
		return (NULL); // Возвращаем NULL
	expand_envp(envp, cmd); // Расширяем переменные окружения в каждой команде
	if (!cmd) // Проверка на корректность
		exit_program(env, "Invalid input", 1);
	return ((input)); // Инициализация структуры t_hell
}

int main() {
    

    // Пример входных данных
    const char *cmd1[] = {"echo", "$USER", NULL};
    const char *cmd2[] = {"ls", "-l", NULL};

    cmd1 = parse_input(envp, cmd1);
    cmd2 = parse_input(envp, cmd2);
    return (0);
}



я делаю проект минишел. 
В итоге у меня получилась такая структура
typedef struct s_simple_cmds  // Use the struct name as s_simple_cmds
{
	char					**str;
	int						num_redirections;
	char					*hd_file_name;
	t_lexer					*redirections;
	struct s_simple_cmds	*next;
	struct s_simple_cmds	*prev;
}	t_simple_cmds;
Minishell$ ls " <hj" > hj | "jkjk" 'hjhk' - при написании этой команды
я получаю -
Start Simple Commands:
Command: ls " <hj"
Number of redirections: 1
Redirection: Token: 4, Str: hj
Command: "jkjk" 'hjhk'
Number of redirections: 0

и теперь мне нужно обработать строку char **str.
А именно как в bash.
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo "'
dfd'"
'dfd'
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo '"dfd"'
"dfd"
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo ""
dfd""
dfd
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo "d
fd"
dfd
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo 'd
fd'
dfd
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ "$PATH"
-bash: /usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/usr/lib/wsl/lib:/mnt/c/Program Files (x86)/Common Files/Intel/Shared Files/cpp/bin/Intel64:/mnt/c/WINDOWS/system32:/mnt/c/WINDOWS:/mnt/c/WINDOWS/System32/Wbem:/mnt/c/WINDOWS/System32/WindowsPowerShell/v1.0/:/mnt/c/WINDOWS/System32/OpenSSH/:/mnt/c/Program Files/SafeNet/Authentication/SAC/x64:/mnt/c/Program Files/SafeNet/Authentication/SAC/x32:/mnt/c/Program Files (x86)/dotnet/:/mnt/c/Program Files (x86)/Windows Kits/10/Windows Performance Toolkit/:/mnt/c/Program Files/Go/bin:/mnt/c/Program Files/Git/cmd:/mnt/c/Program Files/PowerShell/7-preview/preview:/mnt/c/Program Files/PowerShell/7/:/mnt/c/Users/sruza/AppData/Local/Programs/Python/Python38-32/Scripts/:/mnt/c/Users/sruza/AppData/Local/Programs/Python/Python38-32/:/mnt/c/Users/sruza/AppData/Local/Microsoft/WindowsApps:/mnt/c/Program Files/JetBrains/IntelliJ IDEA Community Edition 2022.3/bin:/mnt/c/Users/sruza/AppData/Local/Programs/Microsoft VS Code/bin:/mnt/c/Program Files/JetBrains/PyCharm 2023.1.2/bin:/mnt/c/Program Files/JetBrains/PyCharm Community Edition 2021.1.1/bin:/mnt/c/Users/sruza/go/bin:/snap/bin: No such file or directory
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ '$PATH'
$PATH: command not found
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo '$
PATH'
$PATH
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo $1PATH
PATH
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo $pPATH

zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo $_PATH

zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo $
$
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo $1
23
23
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo $?
0
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo $1?
?
zserobia@DESKTOP-EIPBO1L:/mnt/c/Users/sruza/OneDrive/Desktop/проекты/мойличныйпроектмини/1711$ echo $?
1
01

это все случаи.
Изначально если мы входим в одинарные кавычки, то удаляем первые одинарные кавычки и копируем все до тех пор пока не встретим вторые кавычки которые тоже удаляем. Знак доллара мы никак не отрабатываемю Лишь удаляем.
Если мы входим в двойные кавычки то все что внутри копируется и если мы встречаем знак $ то обрабатываем особым способом.
если после $ следует цифра то удаляется доллар и первая цифра а все остальное копируется до пробела.
если после $ следует ? то оставь этот момент пока, он должен вернуть предыдущий статус выхода. удаляем доллар и вопросительный знак.
если после $ следует _ то тоже оставь там возвращается предыдущая команда. и доллар и все что следует далее до пробела удаляется
если если после $ следует ключи из envp, то мы возвращаем значение envp и удаляем доллар и ключ.
Понятно как мне нужно правильно заменить строку str?
