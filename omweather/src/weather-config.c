/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of Other Maemo Weather(omweather)
 *
 * Copyright (C) 2006-2010 Vlad Vasiliev
 * Copyright (C) 2006-2010 Pavel Fialko
 * Copyright (C) 2008      Andrew Olmsted
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
/*******************************************************************************/
#include "weather-config.h"
#include "weather-stations.h"
#include "weather-utils.h"
#include "weather-event.h"
#include "weather-alerts.h"
#include "weather-home.h"
#if defined(OS2008) || defined(DEBUGTEMP) || defined(OS2009)
#include "weather-sensor.h"
#endif
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef RELEASE
#undef DEBUGFUNCTIONCALL
#endif
/*******************************************************************************/
#define CLOCK_FILE	"/usr/share/omweather/city_in_clock.txt"
#define	DEFAULT_FONT	"Sans 12"
/*******************************************************************************/
/* Change the weather cache directory and update dependent variables. */
gboolean config_set_weather_dir_name(gchar * new_weather_dir_name) {
    GnomeVFSURI *weather_dir_uri;
    gboolean retval = FALSE;
    GnomeVFSURI *parent, *curr_uri;
    GList *list = NULL;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    /* Rest of the function devoted to making sure the directory exists. */
    weather_dir_uri = gnome_vfs_uri_new(new_weather_dir_name);
    if (!gnome_vfs_uri_exists(weather_dir_uri)) {
        list = g_list_prepend(list, curr_uri = weather_dir_uri);
        while (GNOME_VFS_ERROR_NOT_FOUND ==
               gnome_vfs_make_directory_for_uri(parent =
                                                gnome_vfs_uri_get_parent
                                                (curr_uri), 0755))
            list = g_list_prepend(list, curr_uri = parent);
        while (list != NULL) {
            if (GNOME_VFS_OK !=
                gnome_vfs_make_directory_for_uri((GnomeVFSURI *)
                                                 list->data, 0755)) {
                retval = FALSE;
                break;
            }
            list = g_list_remove(list, list->data);
        }
        retval = TRUE;
    } else
        retval = TRUE;
    if (retval) {
        if (app->config->cache_dir_name)
            g_free(app->config->cache_dir_name);
        app->config->cache_dir_name = new_weather_dir_name;
    }
    gnome_vfs_uri_unref(weather_dir_uri);
    return retval;
}

/*******************************************************************************/
void fill_user_stations_list_from_clock(GtkListStore ** list) {
    FILE *clock_file;
    gchar *home_city = NULL,
        *remote_city = NULL,
        *tmp = NULL, *station_name = NULL, *station_code =
        NULL, buffer[1024];
    GtkTreeIter iter;
    GConfClient *gconf_client = gconf_client_get_default();
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    /* City name from config file of clock */
    home_city = gconf_client_get_string(gconf_client,
                                        GCONF_KEY_CLOCK_HOME_LOCATION,
                                        NULL);
    if (!home_city)
        return;
    remote_city = gconf_client_get_string(gconf_client,
                                          GCONF_KEY_CLOCK_REMOTE_LOCATION,
                                          NULL);
    if (!remote_city)
        return;
    /* This is code for the first start during installation of deb packet */
    if ((clock_file = fopen(CLOCK_FILE, "r")) == NULL)
        sleep(25);
    else
        fclose(clock_file);

    if ((clock_file = fopen(CLOCK_FILE, "r"))) {
        while (!feof(clock_file)) {
            memset(buffer, 0, sizeof(buffer));
            fgets(buffer, sizeof(buffer) - 1, clock_file);
            tmp = strchr(buffer, ';');  /* Separator changed from | to ; in 0.19 version */
            if (!tmp)
                tmp = strchr(buffer, '|');      /* Finding old separator */
            if (!tmp)
                continue;
            if (!strncmp(buffer, home_city, tmp - buffer) ||
                !strncmp(buffer, remote_city, tmp - buffer)) {
                /* Prepare struct */
                tmp[strlen(tmp) - 1] = 0;
                station_code = g_strdup(tmp + 1);
                tmp[0] = 0;
                station_name = g_strdup(buffer);
                /* Add station to the user stations list */
                gtk_list_store_append(*list, &iter);
                gtk_list_store_set(*list, &iter,
                                   NAME_COLUMN, station_name,
                                   ID0_COLUMN, station_code,
#if defined(ENABLE_GPS)
                                   2, FALSE,
#endif
                                   3, g_strdup("weather.com"),
                                   -1);
                /* A current station */
                if (!strncmp(buffer, home_city, tmp - buffer)) {
                    app->config->current_station_id = station_code;
                    app->config->current_station_name = station_name;
                    app->config->current_station_source = g_strdup("weather.com");
                }
            }
        }
        fclose(clock_file);
    }
    if (home_city)
        g_free(home_city);
    if (remote_city)
        g_free(remote_city);
}

/*******************************************************************************/
void fill_user_stations_list(GSList * source_list, GtkListStore ** list) {
    GtkTreeIter	iter;
    gchar   *temp1 = NULL,
            *temp2 = NULL,
            *temp3 = NULL,
#ifdef ENABLE_GPS
            *temp4 = NULL,
#endif
            *station_name = NULL,
            *station_code = NULL,
            *station_source = NULL;
#ifdef ENABLE_GPS
    gboolean    is_gps = FALSE;
#endif
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    while (source_list) {
        temp1 = strdup((gchar *) source_list->data);
        /* Delimit Id and name */
        if (strlen(temp1) > 0) {
            /* station name */
            temp2 = strtok(temp1, "@"); /* Delimiter - @ */
            if (temp2)          /* Check random error */
                if (strlen(temp2) > 0){
                    station_code = g_strdup(temp2);
                }
            /* station code */
            temp2 = strtok(NULL, "@");  /* Delimiter - @ */
            if (temp2)
                station_name = g_strdup(temp2);
            /* station source */
            temp3 = strtok(NULL, "@");
            if (temp3)
                station_source = g_strdup(temp3);
            /* weather.com by default */
            if (!station_source || (!strcmp(station_source,"(null)"))){
                if (station_source)
                   g_free(station_source);
                station_source = g_strdup("weather.com");
            }

           fprintf(stderr,"%s : %s : %s \n", station_code, station_name, station_source );
#ifdef ENABLE_GPS
            /* gps or manual station */
            temp4 = strtok(NULL, "@");
            if (temp4 && !strcmp("G",temp4))
                is_gps = TRUE;
            else
                is_gps = FALSE;
#endif
            if (station_name && station_code) {
                /* Add station to stations list */
                gtk_list_store_append(*list, &iter);
                gtk_list_store_set(*list, &iter,
#if defined(ENABLE_GPS)
                                   NAME_COLUMN, station_name,
                                   ID0_COLUMN, station_code,
                                   2, is_gps,
                                   3, station_source,
#else
                                   NAME_COLUMN, station_name,
                                   ID0_COLUMN, station_code,
                                   3, station_source,
#endif
                                   -1);
            }
            if (app->config->current_station_name && app->config->current_station_id
                && (!strcmp(station_name, app->config->current_station_name)
                && !strcmp(station_code, app->config->current_station_id)
                && !app->config->current_station_source)) {
                app->config->current_station_source = g_strdup(station_source);
            }
            if (!app->config->current_station_id) {
                app->config->current_station_id = g_strdup(station_code);
                app->config->current_station_name = g_strdup(station_name);
                app->config->current_station_source = g_strdup("weather.com");
            }

            if(station_name){
                g_free(station_name);
                station_name = NULL;
            }
            if(station_code){
                g_free(station_code);
                station_code = NULL;
            }
            if(station_source){
                g_free(station_source);
                station_source = NULL;
            }
        }
        g_free(temp1);
        source_list = g_slist_next(source_list);
    }
}
/*******************************************************************************/
GSList *create_stations_string_list(void) {
    GSList      *stlist = NULL;
    GtkTreeIter iter;
    gboolean	valid;
    gboolean    is_gps;
    gchar       *station_name = NULL,
                *station_code = NULL,
                *station_source = NULL,
                *str = NULL;
    gchar       type_of_station = 'M';
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    valid =
        gtk_tree_model_get_iter_first(GTK_TREE_MODEL
                                      (app->user_stations_list), &iter);
    while (valid) {
        gtk_tree_model_get(GTK_TREE_MODEL(app->user_stations_list),
                           &iter,
                           NAME_COLUMN, &station_name,
                           ID0_COLUMN, &station_code,
                           2, &is_gps,
                           3, &station_source, -1);
        if (is_gps)
            type_of_station = 'G';
        else
            type_of_station = 'M';

        if (!station_source || (!strcmp(station_source,"(null)"))){
                if (station_source)
                   g_free(station_source);
                station_source = g_strdup("weather.com");
        }
        if (!station_name || (!strcmp(station_name,"(null)"))){
                if (station_name)
                   g_free(station_name);
                station_name = g_strdup(" ");
        }
        if (!station_code || (!strcmp(station_code,"(null)"))){
                if (station_code)
                   g_free(station_code);
                station_code = g_strdup(" ");
        }

        str = g_strdup_printf("%s@%s@%s@%c", station_code, station_name,
                              station_source, type_of_station);
        stlist = g_slist_append(stlist, str);
        g_free(station_name);
        g_free(station_code);
        g_free(station_source);
        valid =
            gtk_tree_model_iter_next(GTK_TREE_MODEL
                                     (app->user_stations_list), &iter);
    }
    return stlist;
}

/*******************************************************************************/
GtkListStore *create_time_update_list(void) {
    GtkListStore *list = NULL;
    GtkTreeIter iter;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    list = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);

    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("Never"), 1, 0, -1);
    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("every 5 minutes"), 1, 5, -1);
    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("every 30 minutes"), 1, 30, -1);
    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("every hour"), 1, 60, -1);
    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("every 2 hours"), 1, 120, -1);
    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("every 4 hours"), 1, 240, -1);
    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("every 8 hours"), 1, 480, -1);
    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("every 24 hours"), 1, 1440, -1);
    gtk_list_store_append(list, &iter);
    gtk_list_store_set(list, &iter, 0, _("every minute (Debug)"), 1, 1, -1);

    return list;
}
/*******************************************************************************/
gint read_config(AppletConfig * config) {
    GConfValue *value = NULL;
    GConfClient *gconf_client = NULL;
    GSList *stlist = NULL;
    GError *gerror = NULL;
    GdkColor DEFAULT_FONT_COLOR = { 0, 0xFF00, 0xFF00, 0x0000 };
    GdkColor DEFAULT_BACKGROUND_FONT_COLOR = { 0, 0x0000, 0x0000, 0x0000 };
    gchar tmp_buff[1024], *home_dir, *tmp = NULL;
    gboolean first_start = FALSE;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    gconf_client = gconf_client_get_default();

    if (!gconf_client) {
        fprintf(stderr, _("Failed to initialize GConf. Quitting.\n"));
        return 1;
    }
    /* Check first start then  */
    tmp = NULL;
    tmp = gconf_client_get_string(gconf_client,
                                  GCONF_KEY_WEATHER_PROGRAM_VERSION, NULL);
    if (!tmp) {
        first_start = TRUE;
    } else {
        g_free(tmp);
        tmp = NULL;
        first_start = FALSE;
    }

    /* Get Weather Cache Directory.  Default is "~/apps/omweather". */
    tmp = gconf_client_get_string(gconf_client,
                                  GCONF_KEY_WEATHER_DIR_NAME, NULL);
    if (!tmp) {
        home_dir = getenv("HOME");
        if (!home_dir) {
            fprintf(stderr,
                    _
                    ("Cannot get path to the HOME directory. Quitting.\n"));
            return 1;
        }
        memset(tmp_buff, 0, sizeof(tmp_buff));
        snprintf(tmp_buff, sizeof(tmp_buff) - 1, "%s%s",
                 home_dir, "/apps/omweather");
        home_dir = NULL;
        tmp = tmp_buff;
    } else {
        snprintf(tmp_buff, sizeof(tmp_buff) - 1, "%s", tmp);
        if (tmp)
            g_free(tmp);
        tmp = NULL;
    }
    config->cache_directory = g_strdup(tmp_buff);

    if (!config_set_weather_dir_name
        (gnome_vfs_expand_initial_tilde(tmp_buff)))
        fprintf(stderr, _("Could not create weather cache directory.\n"));

    config->mode = gconf_client_get_int(gconf_client,
                                                GCONF_KEY_MODE,
                                                NULL);

    if (config->mode < SIMPLE_MODE
        || config->mode > EXTENDED_MODE)
	/* Get MODE  Default SIMPLE FOR OS2009  EXTENDED FOR OS2008 */
	#ifdef OS2008
	    config->mode = EXTENDED_MODE;
	#else
	    config->mode = SIMPLE_MODE;
	#endif

    /* Get Weather source name. */
    config->current_source = gconf_client_get_string(gconf_client,
                                                      GCONF_KEY_WEATHER_CURRENT_SOURCE_NAME,
                                                      NULL);
    /* Get Weather country name. */
    config->current_country = gconf_client_get_string(gconf_client,
                                                      GCONF_KEY_WEATHER_CURRENT_COUNTRY_NAME,
                                                      NULL);
    /* Get current station id */
    config->current_station_id = NULL;
    config->current_station_id = gconf_client_get_string(gconf_client,
                                                         GCONF_KEY_WEATHER_CURRENT_STATION_ID,
                                                         NULL);
    if (config->current_station_id
        && strlen(config->current_station_id) == 0){
        g_free(config->current_station_id);
        config->current_station_id = NULL;
    }
    /* Get Weather current station name. */
    config->current_station_name = NULL;
    config->current_station_name = gconf_client_get_string(gconf_client,
                                                           GCONF_KEY_WEATHER_CURRENT_STATION_NAME,
                                                           NULL);
    if (config->current_station_name
        && strlen(config->current_station_name) == 0){
        g_free(config->current_station_name);
        config->current_station_name = NULL;
    }
    /* Get weather current station source */
    config->current_station_source = NULL;
    config->current_station_source = gconf_client_get_string(gconf_client,
                                                  GCONF_KEY_CURRENT_STATION_SOURCE,
                                                  NULL);
    if (config->current_station_source
        && strlen(config->current_station_source) == 0){
        g_free(config->current_station_source);
        config->current_station_source = NULL;
    }
    /* Get GPS station name and id */
#ifdef ENABLE_GPS
    app->gps_station.name[0] = 0;
    tmp = NULL;
    tmp = gconf_client_get_string(gconf_client,
                                  GCONF_KEY_GPS_STATION_NAME, NULL);
    if (tmp) {
        snprintf(app->gps_station.name,
                 sizeof(app->gps_station.name) - 1, "%s", tmp);
        g_free(tmp);
    }
    app->gps_station.id0[0] = 0;
    tmp = NULL;
    tmp = gconf_client_get_string(gconf_client,
                                  GCONF_KEY_GPS_STATION_ID, NULL);
    if (tmp) {
        snprintf(app->gps_station.id0, sizeof(app->gps_station.id0) - 1,
                 "%s", tmp);
        g_free(tmp);
        tmp = NULL;
    }
#endif
    /* Get user Weather Stations list */
    stlist = gconf_client_get_list(gconf_client,
                                   GCONF_KEY_WEATHER_STATIONS_LIST,
                                   GCONF_VALUE_STRING, NULL);
    if (stlist) {
        gtk_list_store_clear(app->user_stations_list);
        fill_user_stations_list(stlist, &app->user_stations_list);
        g_slist_foreach(stlist, (GFunc)g_free, NULL);
        g_slist_free(stlist);
    }

    /* Get user alerts list */
    stlist = gconf_client_get_list(gconf_client,
                                   GCONF_KEY_WEATHER_ALERTS_LIST,
                                   GCONF_VALUE_STRING, NULL);
    if (stlist) {
        fill_user_alerts_list(stlist, &app->user_alerts_list);
        g_slist_free(stlist);
    }
    /* Get Enable Animation flag. Default is TRUE. */
    value =
        gconf_client_get(gconf_client, GCONF_KEY_ENABLE_ANIMATION,
                         NULL);
    if (value) {
        config->animation = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->animation = TRUE;

    /* Get icon set name */
    config->icon_set = gconf_client_get_string(gconf_client,
                                               GCONF_KEY_WEATHER_ICON_SET,
                                               NULL);
    update_icons_set_base(config->icon_set);

    /* Get Weather Icon Size  */
    config->icons_size = gconf_client_get_int(gconf_client,
                                              GCONF_KEY_WEATHER_ICONS_SIZE,
                                              NULL);
    if (config->icons_size < TINY || config->icons_size > SUPER_GIANT)
        config->icons_size = LARGE;
    /* Get setting tab number  */
    config->current_settings_page = gconf_client_get_int(gconf_client,
                                                         GCONF_KEY_WEATHER_SETTING_TAB_NUMBER,
                                                         NULL);
    if (config->current_settings_page < 0 ||
        config->current_settings_page > MAX_SETTINGS_PAGE_NUMBER)
        config->current_settings_page = 0;

    /* Get Weather periodic update time. */
    config->update_interval = gconf_client_get_int(gconf_client,
                                                   GCONF_KEY_WEATHER_UPDATE_INTERVAL,
                                                   NULL);
    if (config->update_interval < 0 || config->update_interval > 24 * 60)       /* No more than 24 hours */
        config->update_interval = 60;
    remove_periodic_event();    /* delete event from list */
    add_periodic_event(time(NULL));     /* add new event */

#ifdef ENABLE_GPS
    /* Get gps_station. Default is FALSE */
    value =
        gconf_client_get(gconf_client, GCONF_KEY_USE_GPS_STATION, NULL);
    if (value) {
        config->gps_station = gconf_value_get_bool(value);
        gconf_value_free(value);
        add_gps_event(1);
    } else{
        config->gps_station = FALSE;
#if defined OS2009
        config->gps_station = TRUE;
        add_gps_event(1);
#endif
    }
#endif

    /* Get Weather font color. */
    tmp = NULL;
    tmp = gconf_client_get_string(gconf_client,
                                  GCONF_KEY_WEATHER_FONT_COLOR, NULL);
    if (!tmp || !gdk_color_parse(tmp, &(config->font_color)))
        config->font_color = DEFAULT_FONT_COLOR;
    g_free(tmp);

    /* Get font description. */
    tmp = NULL;
    tmp =
        gconf_client_get_string(gconf_client, GCONF_KEY_WEATHER_FONT,
                                NULL);
    if (!tmp)
        config->font = g_strdup(DEFAULT_FONT);
    else {
        config->font = g_strdup(tmp);
        g_free(tmp);
    }
    tmp = NULL;
    tmp = gconf_client_get_string(gconf_client,
                                  GCONF_KEY_WEATHER_BACKGROUND_COLOR,
                                  NULL);
    if (!tmp || !gdk_color_parse(tmp, &(config->background_color)))
        config->background_color = DEFAULT_BACKGROUND_FONT_COLOR;
    g_free(tmp);
    tmp = NULL;
#if !defined OS2008 || !defined OS2009
    /* Get Enable Transparency flag. Default is TRUE. */
    value =
        gconf_client_get(gconf_client, GCONF_KEY_ENABLE_TRANSPARENCY,
                         NULL);
    if (value) {
        config->transparency = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->transparency = TRUE;
#endif
    /* Get Enable Weather For Two Hours. Default is TRUE. */
    value =
        gconf_client_get(gconf_client,
                         GCONF_KEY_SHOW_WEATHER_FOR_TWO_HOURS, NULL);
    if (value) {
        config->show_weather_for_two_hours = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->show_weather_for_two_hours = TRUE;


    /* Hack This parameter is default for SIMPLE mode */
    if (config->mode == SIMPLE_MODE)
        config->show_weather_for_two_hours = TRUE;

    /* Get Split Button State. Default is FALSE */
    value = gconf_client_get(gconf_client, GCONF_KEY_SEPARATE_DATA, NULL);
    if (value) {
        config->separate = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->separate = FALSE;
#ifdef OS2008
    /* Get Use sensor Button State. Default is FALSE */
    value = gconf_client_get(gconf_client, GCONF_KEY_USE_SENSOR, NULL);
    if (value) {
        config->use_sensor = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->use_sensor = FALSE;
    /* Get display sensor at  */
    config->display_at = gconf_client_get_int(gconf_client,
                                              GCONF_KEY_DISPLAY_SENSOR_AT,
                                              NULL);
    if (config->display_at < ICON || config->display_at > STATION_NAME)
        config->display_at = STATION_NAME;
    /* Get sensor update time  */
    config->sensor_update_time = gconf_client_get_int(gconf_client,
                                                      GCONF_KEY_SENSOR_UPDATE_TIME,
                                                      NULL);
    if (config->sensor_update_time < 0 || config->sensor_update_time > 30)
        config->sensor_update_time = 0;
    /* start timer for read data from device temperature sensor */
    if (config->use_sensor) {
        read_sensor(FALSE);
        if (config->sensor_update_time)
            app->sensor_timer =
                g_timeout_add(config->sensor_update_time * 1000 * 60,
                              (GtkFunction) read_sensor,
                              GINT_TO_POINTER(1));
        else
            app->sensor_timer = 0;
    } else
        app->sensor_timer = 0;
#endif
#if defined OS2008 || defined OS2009
    /* Get value of transparence default 0 */
    config->alpha_comp = gconf_client_get_int(gconf_client,
                                              GCONF_KEY_ALPHA_COMPONENT,
                                              &gerror);
    if (gerror || config->alpha_comp <= 0) {
        config->alpha_comp = 0;
        if (gerror)
            g_error_free(gerror);
    }
    /* Get value of corner radius. Default is 10 */
    config->corner_radius = gconf_client_get_int(gconf_client,
                                                 GCONF_KEY_CORNER_RADIUS,
                                                 &gerror);
    if (gerror || config->corner_radius <= 0) {
        config->corner_radius = 10;
        if (gerror)
            g_error_free(gerror);
    }
#endif

    /* Get auto_downloading_after_connecting. Default is FALSE */
    value =
        gconf_client_get(gconf_client,
                         GCONF_KEY_DOWNLOADING_AFTER_CONNECTING, NULL);
    if (value) {
        config->downloading_after_connecting = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->downloading_after_connecting = FALSE;

    /* Get Swap Temperature Button State. Default is TRUE */
    value =
        gconf_client_get(gconf_client, GCONF_KEY_SWAP_TEMPERATURE, NULL);
    if (value) {
        config->swap_hi_low_temperature = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->swap_hi_low_temperature = TRUE;

    /* Get Show wind Button State. Default is FALSE */
    value = gconf_client_get(gconf_client, GCONF_KEY_SHOW_WIND, NULL);
    if (value) {
        config->show_wind = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->show_wind = FALSE;

    /* Get Show Station Name. Default is TRUE */
    value =
        gconf_client_get(gconf_client, GCONF_KEY_SHOW_STATION_NAME, NULL);
    if (value) {
        config->show_station_name = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->show_station_name = TRUE;
    /* Get Show Arrows State. Default is TRUE */
    value = gconf_client_get(gconf_client, GCONF_KEY_SHOW_ARROWS, NULL);
    if (value) {
        config->show_arrows = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->show_arrows = TRUE;

    /* Get Temperature Unit  Default Celsius */
    config->temperature_units = gconf_client_get_int(gconf_client,
                                                     GCONF_KEY_WEATHER_TEMPERATURE_UNIT,
                                                     &gerror);
    (config->temperature_units) ? (config->temperature_units = FAHRENHEIT)
        : (config->temperature_units = CELSIUS);
    if (gerror) {
        g_error_free(gerror);
        gerror = NULL;
    }

    /* Get Layout Default Preset ONE */
    config->icons_layout = gconf_client_get_int(gconf_client,
                                                GCONF_KEY_ICONS_LAYOUT,
                                                NULL);
    if (config->icons_layout < ONE_ROW
        || config->icons_layout > PRESET_NOW_PLUS_SEVEN || first_start)
        config->icons_layout = PRESET_NOW;

    /*!!!!!!!!!!!!!!!! Tempory hack !!!!!!!!!!!!!!
    config->icons_layout = PRESET_NOW;

    */


    /* Get Text Position  Default Right */
    config->text_position = RIGHT;
    config->text_position = gconf_client_get_int(gconf_client,
                                                GCONF_KEY_TEXT_POSITION,
                                                NULL);
    if (config->text_position < RIGHT
        || config->text_position > NOTHING)
        config->text_position = RIGHT;

    /* Get number days to show default 5 */
    config->days_to_show = gconf_client_get_int(gconf_client,
                                                GCONF_KEY_WEATHER_DAYS,
                                                &gerror);
    if (gerror || config->days_to_show <= 0
        || config->days_to_show > Max_count_weather_day) {
        config->days_to_show = 5;
        if(gerror){
            g_error_free(gerror);
            gerror = NULL;
        }
    }
    config->previous_days_to_show = config->days_to_show;

    /* Get distance units */
    config->distance_units = gconf_client_get_int(gconf_client,
                                                  GCONF_KEY_WEATHER_DISTANCE_UNITS,
                                                  &gerror);
    if (gerror || config->distance_units > SEA_MILES) {
        config->distance_units = METERS;
        if(gerror){
            g_error_free(gerror);
            gerror = NULL;
        }
    }
    /* Get pressure units */
    config->pressure_units = gconf_client_get_int(gconf_client,
                                                  GCONF_KEY_WEATHER_PRESSURE_UNITS,
                                                  &gerror);
    if (gerror || config->pressure_units < MB
        || config->pressure_units > MM) {
        config->pressure_units = MB;
        if(gerror){
            g_error_free(gerror);
            gerror = NULL;
        }
        g_error_free(gerror);
    }
    /* Get wind units */
    config->wind_units = gconf_client_get_int(gconf_client,
                                              GCONF_KEY_WEATHER_WIND_UNITS,
                                              &gerror);
    if (gerror || config->wind_units > BEAUFORT_SCALE) {
        config->wind_units = METERS_S;
        if(gerror){
            g_error_free(gerror);
            gerror = NULL;
        }
    }
    /* Get the  type of clicking default SHORT click */
    config->clicking_type = gconf_client_get_int(gconf_client,
                                                 GCONF_KEY_CLICK_FOR_LAUNCH_POPUP,
                                                 &gerror);
    if(gerror || config->clicking_type > LONG_CLICK){
        config->clicking_type = SHORT_CLICK;
        if (gerror){
            g_error_free(gerror);
            gerror = NULL;
        }
    }
    /* Get switch time */
    config->switch_time = gconf_client_get_int(gconf_client,
                                               GCONF_KEY_WEATHER_SWITCH_TIME,
                                               &gerror);
    if (gerror || (config->switch_time != 0 &&
                   config->switch_time != 10 &&
                   config->switch_time != 20 &&
                   config->switch_time != 30 &&
                   config->switch_time != 40 &&
                   config->switch_time != 50
                   && config->switch_time != 60)) {
        config->switch_time = 0;        /* Default value - Never */
        if(gerror){
            g_error_free(gerror);
            gerror = NULL;
        }
    } else {
        if (config->switch_time > 0) {
            g_source_remove(app->switch_timer);
            app->switch_timer = g_timeout_add(config->switch_time * 1000,
                                              (GtkFunction)
                                              switch_timer_handler,
                                              app->main_window);
        }
    }
    /* Get valid data time for current weather */
    config->data_valid_interval =
        gconf_client_get_int(gconf_client,
                             GCONF_KEY_WEATHER_VALID_DATA_TIME, &gerror);
    if (gerror || (config->data_valid_interval != 1 &&
                   config->data_valid_interval != 2 &&
                   config->data_valid_interval != 4 &&
                   config->data_valid_interval != 8)) {
        config->data_valid_interval = 4 * 3600; /* Default value - 4 hours */
        if (gerror)
            g_error_free(gerror);
    } else
        config->data_valid_interval *= 3600;

    /* If this first start then fill default station from clock config */
    if (first_start) {
        if (!app->config->current_station_id) {
            fill_user_stations_list_from_clock(&app->user_stations_list);
            if (app->iap_connected)
                update_weather(TRUE);
        }
    }

    /*Get GSM and WLAN Default only WLAN*/
    value = gconf_client_get(gconf_client, GCONF_KEY_UPDATE_GSM, NULL);
    if (value) {
        config->update_gsm = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->update_gsm = FALSE;

    value = gconf_client_get(gconf_client, GCONF_KEY_UPDATE_WLAN, NULL);
    if (value) {
        config->update_wlan = gconf_value_get_bool(value);
        gconf_value_free(value);
    } else
        config->update_wlan = TRUE;

    /* Get scaling in popup window default 1 */
    config->scale_in_popup = gconf_client_get_int(gconf_client,
                                      GCONF_KEY_SCALE_IN_POPUP,
                                                     NULL);
    if (config->scale_in_popup < 1 || config->scale_in_popup >6)
        config->scale_in_popup = 1;
    /* Get VIEW_MODE Default COLLAPSED */
    config->view_mode = COLLAPSED_VIEW_MODE;
    config->view_mode = gconf_client_get_int(gconf_client,
                                       GCONF_KEY_VIEW_MODE,
                                                     NULL);
    if (config->view_mode < COLLAPSED_VIEW_MODE
                || config->mode > EXPANDED_VIEW_MODE)
                        config->view_mode = COLLAPSED_VIEW_MODE;

    /* Get mod Default Digia */
    tmp = NULL;
    tmp = gconf_client_get_string(gconf_client,
                                              GCONF_KEY_MOD,
                                              &gerror);
    if (!tmp ||gerror) {
        config->mod = g_strdup("Digia");
        if(gerror){
            g_error_free(gerror);
            gerror = NULL;
        }
    }else
        config->mod = g_strdup(tmp);
    if (tmp)
        g_free(tmp);

    check_current_station_id();
    gconf_client_clear_cache(gconf_client);
    g_object_unref(gconf_client);
    return 0;
}

/*******************************************************************************/
void config_update_proxy(void) {
    GConfClient *gconf_client = gconf_client_get_default();
    gchar *proxy_mode = NULL;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    proxy_mode = gconf_client_get_string(gconf_client,
                                    GCONF_KEY_PROXY_MODE, NULL);
    if (proxy_mode && strcmp(proxy_mode, "none") != 0){
        if (app->config->iap_http_proxy_host)
            g_free(app->config->iap_http_proxy_host);
        /* Get proxy data */
        if (gconf_client_get_bool(gconf_client, GCONF_KEY_HTTP_PROXY_ON, NULL)) {
            /* HTTP Proxy is on. */
            app->config->iap_http_proxy_host =
                gconf_client_get_string(gconf_client,
                                    GCONF_KEY_HTTP_PROXY_HOST, NULL);
            app->config->iap_http_proxy_port =
                gconf_client_get_int(gconf_client, GCONF_KEY_HTTP_PROXY_PORT,
                                 NULL);
        } else {                    /* HTTP Proxy is off. */
            app->config->iap_http_proxy_host = NULL;
            app->config->iap_http_proxy_port = 0;
        }
    }
    g_object_unref(gconf_client);
    if (proxy_mode)
        g_free(proxy_mode);
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
}

/*******************************************************************************/
void config_save(AppletConfig * config) {
    gchar temp_buffer[16];
    GConfClient *gconf_client;
    GSList *stlist = NULL;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    gconf_client = gconf_client_get_default();
    if (!gconf_client) {
        fprintf(stderr,
                _
                ("Failed to initialize GConf. Settings were not saved.\n"));
        return;
    }
    /* Save program version */
    if (VERSION)
        gconf_client_set_string(gconf_client,
                                GCONF_KEY_WEATHER_PROGRAM_VERSION,
                                VERSION, NULL);
    /* Save Weather Cache Directory. */
    if (config->cache_dir_name)
        gconf_client_set_string(gconf_client,
                                GCONF_KEY_WEATHER_DIR_NAME,
                                config->cache_dir_name, NULL);
    /* Save Weather source name. */
    if (config->current_source)
        gconf_client_set_string(gconf_client,
                                GCONF_KEY_WEATHER_CURRENT_SOURCE_NAME,
                                config->current_source, NULL);
    /* Save Weather country name. */
    if (config->current_country)
        gconf_client_set_string(gconf_client,
                                GCONF_KEY_WEATHER_CURRENT_COUNTRY_NAME,
                                config->current_country, NULL);
    /* Save Weather station name. */
    if (config->current_station_name)
        gconf_client_set_string(gconf_client,
                                GCONF_KEY_WEATHER_CURRENT_STATION_NAME,
                                config->current_station_name, NULL);
    else
        gconf_client_set_string(gconf_client,
                                GCONF_KEY_WEATHER_CURRENT_STATION_NAME,
                                "", NULL);
    /* Save Weather station id. */
    if (config->current_station_id)
        gconf_client_set_string(gconf_client,
                                GCONF_KEY_WEATHER_CURRENT_STATION_ID,
                                config->current_station_id, NULL);
    else
        gconf_client_set_string(gconf_client,
                                GCONF_KEY_WEATHER_CURRENT_STATION_ID, "",
                                NULL);
    /* Save Weather Data Source  */
    if(config->current_station_source)
        gconf_client_set_string(gconf_client,
                            GCONF_KEY_CURRENT_STATION_SOURCE,
                            config->current_station_source, NULL);
    else
        gconf_client_set_string(gconf_client,
                            GCONF_KEY_CURRENT_STATION_SOURCE, "",
                            NULL);
    /* Save icon set name */
    if (config->icon_set)
        gconf_client_set_string(gconf_client,
                                GCONF_KEY_WEATHER_ICON_SET,
                                config->icon_set, NULL);
    /* Save Weather Icon Size  */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_WEATHER_ICONS_SIZE,
                         config->icons_size, NULL);

    /* Save current setting tab number  */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_WEATHER_SETTING_TAB_NUMBER,
                         config->current_settings_page, NULL);
#ifdef OS2008
    /* Save Use sensor Button State */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_USE_SENSOR, config->use_sensor, NULL);
    /* Save display sensor at  */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_DISPLAY_SENSOR_AT,
                         config->display_at, NULL);
    /* Save sensor update time */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_SENSOR_UPDATE_TIME,
                         config->sensor_update_time, NULL);
#endif
    /* Save scaling in popup window */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_SCALE_IN_POPUP,
                         config->scale_in_popup, NULL);
    /* Save Weather Font Color */
    sprintf(temp_buffer, "#%02x%02x%02x",
            config->font_color.red >> 8,
            config->font_color.green >> 8, config->font_color.blue >> 8);
    gconf_client_set_string(gconf_client,
                            GCONF_KEY_WEATHER_FONT_COLOR, temp_buffer,
                            NULL);
    /* Save Font Description */
    gconf_client_set_string(gconf_client,
                            GCONF_KEY_WEATHER_FONT, config->font, NULL);
    /* Save Background Color */
    sprintf(temp_buffer, "#%02x%02x%02x",
            config->background_color.red >> 8,
            config->background_color.green >> 8,
            config->background_color.blue >> 8);
    gconf_client_set_string(gconf_client,
                            GCONF_KEY_WEATHER_BACKGROUND_COLOR,
                            temp_buffer, NULL);
    /* Save Weather Update setting  */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_WEATHER_UPDATE_INTERVAL,
                         config->update_interval, NULL);
    /* Save Enable Transparency flag. */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_ENABLE_TRANSPARENCY,
                          config->transparency, NULL);
    /* Save Enable Animation flag. */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_ENABLE_ANIMATION,
                          config->animation, NULL);
    /*Save Enable Weather For Two Hours */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_SHOW_WEATHER_FOR_TWO_HOURS,
                          config->show_weather_for_two_hours, NULL);
    /* Save Split Button State */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_SEPARATE_DATA, config->separate, NULL);
  /* Save Background Color */
    sprintf(temp_buffer, "#%02x%02x%02x",
                         config->background_color.red >> 8,
                         config->background_color.green >> 8,
                         config->background_color.blue >> 8);
    gconf_client_set_string(gconf_client,
                         GCONF_KEY_WEATHER_BACKGROUND_COLOR,
                         temp_buffer, NULL);
    /* Save Downloading after connecting State */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_DOWNLOADING_AFTER_CONNECTING,
                          config->downloading_after_connecting, NULL);
#if defined OS2008 || OS2009
#ifdef ENABLE_GPS
    /* Save Use GPS station */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_USE_GPS_STATION,
                          config->gps_station, NULL);
#endif
    /* Save Alpha component */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_ALPHA_COMPONENT, config->alpha_comp,
                         NULL);
    /* Save Corner Radius */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_CORNER_RADIUS, config->corner_radius,
                         NULL);
#endif
    /* Save Swap Temperature Button State */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_SWAP_TEMPERATURE,
                          config->swap_hi_low_temperature, NULL);
    /* Save Show Wind Button State */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_SHOW_WIND, config->show_wind, NULL);
    /* Save Show Wind gust Button State */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_SHOW_WIND_GUST,
                          config->show_wind_gust, NULL);
    /* Save Type of clicking for showing window popup */
    gconf_client_set_int(gconf_client,
                          GCONF_KEY_CLICK_FOR_LAUNCH_POPUP,
                          config->clicking_type, NULL);
    /* Show Station Name */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_SHOW_STATION_NAME,
                          config->show_station_name, NULL);
    /* Show Arrows */
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_SHOW_ARROWS, config->show_arrows,
                          NULL);
    /* Save Weather Temperature Unit  */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_WEATHER_TEMPERATURE_UNIT,
                         config->temperature_units, NULL);
    /* Save Days to show. */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_WEATHER_DAYS, config->days_to_show,
                         NULL);
    /* Save distance units. */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_WEATHER_DISTANCE_UNITS,
                         config->distance_units, NULL);
    /* Save pressure units. */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_WEATHER_PRESSURE_UNITS,
                         config->pressure_units, NULL);
    /* Save wind units. */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_WEATHER_WIND_UNITS,
                         config->wind_units, NULL);
    /* Save switch time */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_WEATHER_SWITCH_TIME,
                         config->switch_time, NULL);
    /* Save valid data time */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_WEATHER_VALID_DATA_TIME,
                         config->data_valid_interval / 3600, NULL);
    /* Save Layout type. */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_ICONS_LAYOUT, config->icons_layout,
                         NULL);
    /* Save Mode. */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_MODE, config->mode,
                         NULL);
    /* Save position of text. */
    gconf_client_set_int(gconf_client,
                         GCONF_KEY_TEXT_POSITION, config->text_position,
                         NULL);
    /* Save station list */
    stlist = create_stations_string_list();
    gconf_client_set_list(gconf_client,
                          GCONF_KEY_WEATHER_STATIONS_LIST,
                          GCONF_VALUE_STRING, stlist, NULL);
    /* Free stlist */
    g_slist_foreach(stlist, (GFunc) g_free, NULL);
    g_slist_free(stlist);
    /* Save alerts list */
    stlist = create_list_of_user_alerts(app->user_alerts_list);
    gconf_client_set_list(gconf_client,
                          GCONF_KEY_WEATHER_ALERTS_LIST,
                          GCONF_VALUE_STRING, stlist, NULL);
    /* Free stlist */
    if (stlist){
        g_slist_foreach(stlist, (GFunc) g_free, NULL);
        g_slist_free(stlist);
    }
#ifdef ENABLE_GPS
    /* Save current GPS station */
    if (config->gps_station) {
        if (app->gps_station.name)
            gconf_client_set_string(gconf_client,
                                    GCONF_KEY_GPS_STATION_NAME,
                                    app->gps_station.name, NULL);
        if (app->gps_station.id0)
            gconf_client_set_string(gconf_client,
                                    GCONF_KEY_GPS_STATION_ID,
                                    app->gps_station.id0, NULL);
    }
#endif
    /*Save GSM update setting*/
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_UPDATE_GSM,
                          config->update_gsm, NULL);
    /*Save WLAN update setting*/
    gconf_client_set_bool(gconf_client,
                          GCONF_KEY_UPDATE_WLAN,
                          config->update_wlan, NULL);
    /*Save View mode*/
    gconf_client_set_int(gconf_client,
                             GCONF_KEY_VIEW_MODE, config->view_mode,
                                                      NULL);
    /* Save mod */
    if (config->mod)
        gconf_client_set_string(gconf_client,
                                GCONF_KEY_MOD,
                                config->mod, NULL);

    g_object_unref(gconf_client);
#ifdef DEBUGFUNCTIONCALL
    END_FUNCTION;
#endif
}
/***********************************************************************************/
void check_current_station_id(void) {
    GtkListStore    *all_information_list = NULL;
    GtkTreeIter     iter,
                    iter2;
    gboolean        valid,
                    valid_all_information,
                    equals = FALSE;
    gchar           *station_name = NULL,
                    *station_code = NULL,
                    *station_source = NULL,
                    *station_country = NULL,
                    *station_region = NULL,
                    *is_gps = NULL;

    valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(app->user_stations_list), 
                                                                                    &iter);
    while(valid){
       gtk_tree_model_get(GTK_TREE_MODEL(app->user_stations_list), &iter,
                                        1, &station_code,
                                        -1);
        if(!strcmp(station_code, app->config->current_station_id)){
            valid = FALSE;
            equals = TRUE;
        }
        else{
            valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(app->user_stations_list), &iter);
            equals = FALSE;
        }
        if(station_code)
            g_free(station_code);
    }

    if(!equals){
        valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(app->user_stations_list), &iter);
        if(valid){
            gtk_tree_model_get(GTK_TREE_MODEL(app->user_stations_list), &iter,
                                                0, &station_name,
                                                1, &station_code,
                                                2, &is_gps,
                                                3, &station_source, -1);
            app->config->current_station_id = station_code;
            app->config->current_station_name = station_name;
            app->config->current_station_source = station_source;
            all_information_list = get_all_information_about_station(station_source,
                                                                              station_code);
            valid_all_information = gtk_tree_model_get_iter_first(GTK_TREE_MODEL
                                                            (all_information_list), &iter2);
            if(valid_all_information){
                gtk_tree_model_get(GTK_TREE_MODEL(all_information_list), &iter2,
                                                    0, &station_country,
                                                    1, &station_region, -1);
                app->config->current_country = station_country;
            }
        }
    }
        

  /*  while (valid){
        gtk_tree_model_get(GTK_TREE_MODEL(app->user_stations_list), &iter,
                                          0, &station_name,
                                          1, &station_code,
                                          2, &is_gps,
                                          3, &station_source, -1);
        if(strcmp(station_code, app->config->current_station_id)){
            app->config->current_station_id = station_code;
            app->config->current_station_name = station_name;
            app->config->current_station_source = station_source;

            all_information_list = get_all_information_about_station(station_source,
                                                                         station_code);
            valid_all_information = gtk_tree_model_get_iter_first(GTK_TREE_MODEL
                                                          (all_information_list), &iter2);
            if(valid_all_information){
                gtk_tree_model_get(GTK_TREE_MODEL(all_information_list), &iter2,
                                                  0, &station_country,
                                                  1, &station_region,
         //                                         2, &station_country_id,
           //                                       3, &station_region_id,
                                                  -1);
                app->config->current_country = station_country;
            }
           fprintf(stderr, "\ncurrent_station_id %s\n", app->config->current_station_id); 
        }
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(app->user_stations_list), &iter);
    }*/
}
/*******************************************************************************/
