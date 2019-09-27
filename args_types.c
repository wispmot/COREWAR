/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args_types.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nwispmot <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/09/25 20:51:52 by nwispmot          #+#    #+#             */
/*   Updated: 2019/09/25 20:51:55 by nwispmot         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "asm.h"

void    valid_arg(t_parser *par, t_token *token)
{
    int				arg_type;
    int mask;
    unsigned b_start;

    b_start = g_bytes;
    mask = op[par->oper].args_types[par->arg];
    arg_type = token->type;
    if (token->type == DIRECT_LABEL)
        arg_type = T_DIR;
    else if (token->type == INDIRECT_LABEL)
        arg_type = T_IND;
    if (arg_type != (arg_type & mask))
        error_syntax(par->y, par->x);
    if (arg_type == T_REG)
        g_bytes++;
    else if (arg_type == T_IND)
    {
        g_bytes += IND_SIZE;
        token->bytes = b_start;
    }
    else if (arg_type == T_DIR)
    {
        g_bytes += op[par->oper].t_dir_size;
        token->bytes = b_start;
    }
}

void	is_registry(char *string, t_parser *par)
{
    int i;

    i = 1;
    if(!(ft_isdigit(string[0]) && (ft_isdigit(string[1]) || string[1] == ' '
                                   || string[1] == '\t' || string[1] == '\0')))
        error_lexical(par->y, par->x);
    while(string[i] != '\0' && string[++i])
        if (string[i] != '\t' && string[i] != ' ')
            error_lexical(par->y, par->x + i);
    add_token(par, REGISTER, string);
    par->tokens->data = ft_atoi(string); ///////////////////////////////////////////
}

void  search_label(t_parser *par, char *label)
{
    t_label *head;

    head = par->labels;

    while (head)
    {
        if (ft_strcmp(head->point->content, label) == 0)
            return ;
        head = head->next;
    }
    error_no_label(label);
}

void is_indir_dir_label(char *string, t_parser *par, t_type type)
{
    int i;

    i = -1;
    while(string[++i] && string[i] != '\t' && string[i] != ' ')
        if (ft_strchr(LABEL_CHARS, string[i]) == NULL)
            error_lexical(par->y, par->x + ++i);
    i--;
    while(string[++i])
        if (string[i] != ' ' && string[i] != '\t')
            error_lexical(par->y, par->x + i);
    i = 0;
    while(string[i] && string[i] != '\t' && string[i] != ' ')
        i++;
    string = ft_strsub(string, 0, i);  //freeeeeee
    //search_label(par, string);
    add_token(par, type, string);
}

void is_dir_or_indir(char *string, t_parser *par, t_type type)
{
    int i;

    i = -1;
    if(string[0] == '-')
        i = 0;
    if(string[0] == LABEL_CHAR && type == DIRECT)
        is_indir_dir_label(&string[1], par, DIRECT_LABEL);
    else
    {
        while (string[++i] && string[i] != ' ' && string[i] != '\t')
            if (ft_isdigit(string[i]) == 0)
                error_lexical(par->y, par->x + i);
        while (string[i] != '\0' && string[++i])
            if (string[i] != '\t' && string[i] != ' ')
                error_lexical(par->y, par->x + i);
        add_token(par, type, string);
        par->tokens->data = ft_atoi(string); ///////////////////////////////////////////vozmogno nugna obrabotka otricatelnih chisel
    }
}

void    check_args(char *string, t_parser *par)
{
    int i;

    i = 0;
    while ((string[i] == ' ' || string[i] == '\t') && par->file[par->y][i])
        i++;
    if (string[i] == REG_CHAR)
        is_registry(&string[++i], par);
    else if (string[i] == DIRECT_CHAR)
        is_dir_or_indir(&string[++i], par, DIRECT);
    else if (ft_isdigit(string[i]) || (string[i] == '-' && ft_isdigit(string[i + 1])))
        is_dir_or_indir(&string[i], par, INDIRECT);
    else if (string[i] == LABEL_CHAR)
        is_indir_dir_label(&string[++i], par, INDIRECT_LABEL);
    else
        error_lexical(par->y, par->x);
    valid_arg(par, par->tokens);
    par->arg++;
}
