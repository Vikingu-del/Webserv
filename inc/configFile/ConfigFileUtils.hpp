/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigFileUtils.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ipetruni <ipetruni@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/15 15:23:48 by ipetruni          #+#    #+#             */
/*   Updated: 2024/05/22 16:54:51 by ipetruni         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_FILE_URILS
#define CONFIG_FILE_URILS

#include "Webserv.hpp"

int			ft_stoi(std::string str);
std::string	statusCodeString(short statusCode);

#endif