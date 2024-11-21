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
/*
#include <stdio.h>
#include <unistd.h>

int main() {
    for (int i = 0; i <= 100; i += 10) {
        //printf("\33[2K\rProgress: %d%%", i);  // Обновляем прогресс
        printf("\33[2K\rProgress: %d%%", i);
        fflush(stdout);
        sleep(1);
    }
    printf("\nDone!\n");
    return 0;
}
*/
/*
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("This line will be cleared in 3 seconds.\n");
    sleep(3);

    printf("\33[2K\r");
    fflush(stdout);

    printf("New text in the same line.\n");
    return 0;
}
*/
/*
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Hello, World!\n");
    printf("\33[2K\rThis text replaces the previous line.\n");
    fflush(stdout);
    return 0;
}
*/
/*
#include <stdio.h>
#include <unistd.h>

int main() {
    // Выводим красный текст
    printf("\e[31mThis is red text.\e[0m\n");
    sleep(3);
    printf("This line will be cleared in 3 seconds.\n");
    sleep(3);
    
    // Перемещаем курсор в начало строки и очищаем её
    printf("HELLO \e[2K\rNew text in the same line.\n");
    sleep(3);
    
    return 0;
}
*/
#include "parser.h"

void signals(void)
{
    signal(SIGQUIT, SIG_IGN);  // Ignorer le signal SIGQUIT (Ctrl+\)
    signal(SIGINT, ft_signal_ctr_c); // Définir un gestionnaire pour Ctrl+C (SIGINT)
}

void ft_signal_ctr_c(int status)
{
    if (status == SIGINT) // Vérifier si le signal est SIGINT (Ctrl+C)
    {
        printf("\n");
        rl_on_new_line();  // Indiquer à readline qu'on est sur une nouvelle ligne
        rl_replace_line("", 0); // Remplacer la ligne actuelle par une ligne vide
        rl_redisplay();  // Redessiner la ligne de commande
        global_exit = 130;  // Mettre le code de sortie à 130, traditionnellement pour Ctrl+C
    }
}

void ft_signal_ctr_d(void)
{
    // Gestionnaire pour Ctrl+D (EOF)
    printf("logout\n");  // Afficher "logout"
    rl_clear_history();  // Effacer l'historique des commandes
    exit(0);             // Quitter le programme avec un code de sortie 0
}

void signals_heredoc(void)
{
    signal(SIGQUIT, SIG_IGN);  // Ignorer le signal SIGQUIT (Ctrl+\)
    signal(SIGINT, ft_signal_ctr_c_hd); // Définir un gestionnaire pour Ctrl+C dans heredoc
}

void ft_signal_ctr_c_hd(int status)
{
    if (status == SIGINT) // Vérifier si le signal est SIGINT (Ctrl+C)
    {
        printf("\e[2K\r");  // Effacer la ligne actuelle
        exit(global_exit);  // Quitter le processus avec le code de sortie actuel
    }
}

