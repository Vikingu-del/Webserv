/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eseferi <eseferi@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/14 18:29:47 by eseferi           #+#    #+#             */
/*   Updated: 2024/05/14 18:59:00 by eseferi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

namespace utils
{
    std::vector<std::string> split(const std::string &s, char delimiter);
    std::string concat(const std::vector<std::string> &v, char delimiter);
}