/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of Other Maemo Weather(omweather)
 *
 * Copyright (C) 2006-2011 Vlad Vasiliev
 * Copyright (C) 2010-2011 Tanya Makova
 * 	for the code
 *        
 * Copyright (C) 2008 Andrew Zhilin
 *		      az@pocketpcrussia.com 
 *	for default icon set (Glance)
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU weather-config.h General Public
 * License along with this software; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
*/

#include "meego-netbookUX-common.h"
void init_omweather_core(void);
Core::DataParser *current_data(std::string& str);
int update_weather_forecast(Core::Config *config);
ClutterTimeline *create_update_animation(ClutterActor *actor);
Core::Config *create_and_fill_config(void);
GHashTable *hash_table_create(void);
void change_path(ClutterPath *path, gint need_size);
void change_actor_size_and_position(ClutterActor *actor, gint need_size);
extern Core::Config *config;
//extern std::vector<Core::Station*> stationslist;

