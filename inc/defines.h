/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/10 13:10:40 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/16 18:37:36 by eseferi          ###   ########.fr       */
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