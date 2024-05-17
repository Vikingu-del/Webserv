/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 13:10:40 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/17 13:12:30 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define LINE_END "\r\n"
#define MAX_CONTENT_LENGTH 3000000

//! Parsing Definitions

//? checkFileExistence return values
#define THIS_IS_FILE 1
#define THIS_IS_DIR 2
#define THIS_IS_LINK 3
#define THIS_FILE_DOESNT_EXIST 4

//? checkFilePermissons return values
#define READ_PERMISSION 4
#define WRITE_PERMISSION 2
#define EXECUTE_PERMISSION 1
#define THIS_FILE_HAS_PERMISSION 0
#define THIS_FILE_DOESNT_HAVE_PERMISSION -1

#define RED   "\033[31m"
#define GREEN  "\033[32m"
#define YELLOW  "\033[33m"
#define PURPLE "\033[35m"
#define CYAN  "\033[36m"
#define RESET "\033[0m"
#define BLUE "\033[0;34m"
#define PINK "\033[0;35m"
#define BLD "\033[1m"
#define RST "\033[0m"
