/* vim: set sw=4 ts=4 et: */
/*
 * This file is part of omweather-weather-com-stations-db
 *
 * Copyright (C) 2006-2009 Vlad Vasiliev
 * Copyright (C) 2006-2009 Pavel Fialko
 * 	for the code
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU  General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 * You should have received a copy of the GNU  General Public
 * License along with this software; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
*/
/*******************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "main.h"
#include <unistd.h>
#include <string.h>
#include <time.h>
/*******************************************************************************/
static GHashTable *data = NULL;
/*******************************************************************************/
gboolean
source_init(void){
    data = g_hash_table_new(g_str_hash, g_str_equal);
    if(!data)
        return FALSE;
    return TRUE;
}
/*******************************************************************************/
void
source_destroy(void){
    GHashTable  *hashtable = NULL;
    GSList  *tmp = NULL;
    /* free station location data */
    hashtable = g_hash_table_lookup(data, "location");
    if(hashtable){
        g_hash_table_foreach(hashtable, free_fields, NULL);
        g_hash_table_remove_all(hashtable);
        g_hash_table_unref(hashtable);
    }
    /* free station current data */
    hashtable = g_hash_table_lookup(data, "current");
    if(hashtable){
        g_hash_table_foreach(hashtable, free_fields, NULL);
        g_hash_table_remove_all(hashtable);
        g_hash_table_unref(hashtable);
    }
    /* free station days data */
    tmp = g_hash_table_lookup(data, "forecast");
    while(tmp){
        hashtable = (GHashTable*)tmp->data;
        g_hash_table_foreach(hashtable, free_fields, NULL);
        g_hash_table_remove_all(hashtable);
        g_hash_table_unref(hashtable);
        tmp = g_slist_next(tmp);
    }
    tmp = g_hash_table_lookup(data, "forecast");
    if(tmp)
        g_slist_free(tmp);
    if(data){
        g_hash_table_remove_all(data);
        g_hash_table_destroy(data);
    }
}
/*******************************************************************************/
void
free_fields(gpointer key, gpointer val, gpointer user_data){
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    if(val){
        g_free(val);
        val = NULL;
    }
}
/*******************************************************************************/
gint
get_station_weather_data(const gchar *station_id_with_path, GHashTable *data,
                                                       gboolean get_detail_data){
    xmlDoc  *doc = NULL;
    xmlNode *root_node = NULL;
    gint    days_number = -1;
    gchar   buffer[1024],
            *delimiter = NULL;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    if(!station_id_with_path || !data)
        return -1;
/* check for new file, if it exist, than rename it */
    *buffer = 0;
    snprintf(buffer, sizeof(buffer) - 1, "%s.new", station_id_with_path);
    if(!access(buffer, R_OK))
        rename(buffer, station_id_with_path);
    /* check file accessability */
    if(!access(station_id_with_path, R_OK)){
        /* check that the file containe valid data */
        doc = xmlReadFile(station_id_with_path, NULL, 0);
        if(!doc)
            return -1;
        root_node = xmlDocGetRootElement(doc);
        if(root_node->type == XML_ELEMENT_NODE &&
                strstr((char*)root_node->name, "err")){
            xmlFreeDoc(doc);
            xmlCleanupParser();
            return -2;
        }
        else{
            /* prepare station id */
            *buffer = 0;
            delimiter = strrchr(station_id_with_path, '/');
            if(delimiter){
                delimiter++; /* delete '/' */
                snprintf(buffer, sizeof(buffer) - 1, "%s", delimiter);
                delimiter = strrchr(buffer, '.');
                if(!delimiter){
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    return -1;
                }
                *delimiter = 0;
                if(get_detail_data)
                    days_number = parse_xml_detail_data(buffer, root_node, data);
                else
                    days_number = parse_xml_data(buffer, root_node, data);
            }
            xmlFreeDoc(doc);
            xmlCleanupParser();
        }
    }
    else
        return -1;/* file isn't accessability */
    return days_number;
}
/*******************************************************************************/
gint
parse_xml_data(const gchar *station_id, xmlNode *root_node, GHashTable *data){
    xmlNode     *cur_node = NULL,
                *child_node = NULL,
                *child_node2 = NULL,
                *child_node3 = NULL,
                *child_node4 = NULL;
    xmlChar     *temp_xml_string = NULL;
    xmlChar     *part_of_day = NULL;
    gint        store2day = 0,
                count_day = 0;
    gchar       id_station[10],
                buff[256];
    struct tm   tmp_tm = {0};
    GSList      *forecast = NULL;
    GHashTable  *location = NULL,
                *current = NULL,
                *day = NULL;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    for(cur_node = root_node->children; cur_node; cur_node = cur_node->next){
        if( cur_node->type == XML_ELEMENT_NODE ){
            /* get weather station data */
            if(!xmlStrcmp(cur_node->name, (const xmlChar *) "loc" ) ){
                temp_xml_string = xmlGetProp(cur_node, (const xmlChar*)"id");
                snprintf(id_station, sizeof(id_station) - 1,
                            "%s", temp_xml_string);
                xmlFree(temp_xml_string);
                /* If station in xml not station in config file exit */
                if( strcmp(id_station, station_id) )
                    return -1;
                location = g_hash_table_new(g_str_hash, g_str_equal);
                for(child_node = cur_node->children; child_node; child_node = child_node->next){
                    if( child_node->type == XML_ELEMENT_NODE ){
                        /* station name */
                        if( !xmlStrcmp(child_node->name, (const xmlChar *)"dnam") ){
                            temp_xml_string = xmlNodeGetContent(child_node);
                            g_hash_table_insert(location, "station_name",
                                                g_strdup((char*)temp_xml_string));
                            xmlFree(temp_xml_string);
                            continue;
                        }
                        /* station lattitude */
                        if( !xmlStrcmp(child_node->name, (const xmlChar *)"lat") ){
                            temp_xml_string = xmlNodeGetContent(child_node);
                            g_hash_table_insert(location, "station_latitude",
                                                g_strdup((char*)temp_xml_string));
                            xmlFree(temp_xml_string);
                            continue;
                        }
                        /* station longitude */
                        if( !xmlStrcmp(child_node->name, (const xmlChar *)"lon") ){
                            temp_xml_string = xmlNodeGetContent(child_node);
                            g_hash_table_insert(location, "station_longitude",
                                                g_strdup((char*)temp_xml_string));
                            xmlFree(temp_xml_string);
                            continue;
                        }
                        /* station time zone */
                        if( !xmlStrcmp(child_node->name, (const xmlChar *)"zone") ){
                            temp_xml_string = xmlNodeGetContent(child_node);
                            g_hash_table_insert(location, "station_time_zone",
                                                g_strdup((char*)temp_xml_string));
                            xmlFree(temp_xml_string);
                            continue;
                        }
                    }
                }
                g_hash_table_insert(data, "location", (gpointer)location);
            }
            /* Fill current day */
            if(!xmlStrcmp(cur_node->name, (const xmlChar *) "cc" ) ){
                current = g_hash_table_new(g_str_hash, g_str_equal);
                for(child_node = cur_node->children; child_node != NULL; child_node = child_node->next){
                    /* last update */
                    if( child_node->type == XML_ELEMENT_NODE  &&
                            ( !xmlStrcmp(child_node->name, (const xmlChar *)"lsup") ) ){
                        temp_xml_string = xmlNodeGetContent(child_node);
                        g_hash_table_insert(current, "last_update", g_strdup((char*)temp_xml_string));
                        xmlFree(temp_xml_string);
                        continue;
                    }
                    /* temperature */
                    if( child_node->type == XML_ELEMENT_NODE  &&
                            ( !xmlStrcmp(child_node->name, (const xmlChar *)"tmp") ) ){
                        temp_xml_string = xmlNodeGetContent(child_node);
                        g_hash_table_insert(current, "day_hi_temperature",
                                            g_strdup((char*)temp_xml_string));
                        xmlFree(temp_xml_string);
                        continue;
                    }
                    /* feel like */
                    if( child_node->type == XML_ELEMENT_NODE  &&
                            ( !xmlStrcmp(child_node->name, (const xmlChar *)"flik") ) ){
                        temp_xml_string = xmlNodeGetContent(child_node);
                        g_hash_table_insert(current, "feel_like",
                                            g_strdup((char*)temp_xml_string));
                        xmlFree(temp_xml_string);
                        continue;
                    }
                    /* title */
                    if(!xmlStrcmp(child_node->name, (const xmlChar *)"t") ){
                        temp_xml_string = xmlNodeGetContent(child_node);
                        g_hash_table_insert(current, "title", g_strdup((char*)temp_xml_string));
                        xmlFree(temp_xml_string);
                        continue;
                    }
                    /* icon */
                    if(!xmlStrcmp(child_node->name, (const xmlChar *)"icon") ){
                        temp_xml_string = xmlNodeGetContent(child_node);
                        g_hash_table_insert(current, "icon",
                                            g_strdup((char*)temp_xml_string));
                        xmlFree(temp_xml_string);
                        continue;
                    }
                    /* pressure data */
                    if(!xmlStrcmp(child_node->name, (const xmlChar *)"bar") ){
                        for(child_node2 = child_node->children; child_node2 != NULL; child_node2 = child_node2->next){
                            if( child_node2->type == XML_ELEMENT_NODE ){
                                /* pressure */
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"r") ){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(current, "pressure",
                                                        g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                }
                                /* direction */
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"d") ){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(current, "pressure_direction",
                                                        g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                }
                            }
                        }
                        continue;
                    }
                    /* humidity */
                    if(!xmlStrcmp(child_node->name, (const xmlChar *)"hmid") ){
                        temp_xml_string = xmlNodeGetContent(child_node);
                        g_hash_table_insert(current, "humidity", g_strdup((char*)temp_xml_string));
                        xmlFree(temp_xml_string);
                        continue;
                    }
                    /* visible */
                    if(!xmlStrcmp(child_node->name, (const xmlChar *)"vis") ){
                        temp_xml_string = xmlNodeGetContent(child_node);
                        g_hash_table_insert(current, "visible", g_strdup((char*)temp_xml_string));
                        xmlFree(temp_xml_string);
                        continue;
                    }
                    /* wind data */
                    if(!xmlStrcmp(child_node->name, (const xmlChar *)"wind") ){
                        for(child_node2 = child_node->children; child_node2 != NULL; child_node2 = child_node2->next){
                            if( child_node2->type == XML_ELEMENT_NODE ){
                                /* speed */
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"s") ){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(current, "wind_speed", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                }
                                /* gust */
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"gust") ){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(current, "wind_gust", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                }
                                /* direction */
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"t") ){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(current, "wind_direction", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                }
                            }
                        }
                        continue;
                    }
                    /* Moon data */
                    if(!xmlStrcmp(child_node->name, (const xmlChar *)"moon") ){
                        for(child_node2 = child_node->children; child_node2 != NULL; child_node2 = child_node2->next){
                            if( child_node2->type == XML_ELEMENT_NODE ){
                                /* icon */
/* not used now
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"icon") ){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(current, "moon_icon", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                }
*/
                                /* phase */
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"t") ){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(current, "moon_phase", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                }
                            }
                        }
                        continue;
                    }
                }/* end for */
                g_hash_table_insert(data, "current", (gpointer)current);
            }
            /* Fill other days */
            if(!xmlStrcmp(cur_node->name, (const xmlChar *) "dayf" ) ){
                for(child_node = cur_node->children; child_node; child_node = child_node->next){
                    if(child_node->type == XML_ELEMENT_NODE  &&
                            ( !xmlStrcmp(child_node->name, (const xmlChar *)"day") ) ){
                        day = g_hash_table_new(g_str_hash, g_str_equal);
                        /* get 24h name */
                        temp_xml_string = xmlGetProp(child_node, (const xmlChar*)"t");
                        /* prepare locale value for day name */
                        memset(buff, 0, sizeof(buff));
                        memcpy(buff, temp_xml_string, (strlen((char*)temp_xml_string) > sizeof(buff)) ?
                                            (sizeof(buff) - 1) :
                                            (strlen((char*)temp_xml_string)));
                        strptime(buff, "%A", &tmp_tm);
                        memset(buff, 0, sizeof(buff));
                        strftime(buff, sizeof(buff) - 1, "%a", &tmp_tm);
                        g_hash_table_insert(day, "day_name", g_strdup(buff));
                        xmlFree(temp_xml_string);
                        /* get 24h date */
                        temp_xml_string = xmlGetProp(child_node, (const xmlChar*)"dt");
                        g_hash_table_insert(day, "day_date", g_strdup((char*)temp_xml_string));
                        xmlFree(temp_xml_string);
                        for(child_node2 = child_node->children; child_node2; child_node2 = child_node2->next){
                            if( child_node2->type == XML_ELEMENT_NODE){
                                /* 24h hi temperature */
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"hi")){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(day, "day_hi_temperature", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                    continue;
                                }
                                /* 24h low temperature */
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"low")){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(day, "day_low_temperature", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                    continue;
                                }
                                /* 24h sunrise */
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"sunr")){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(day, "day_sunrise", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                    continue;
                                }
                                /* 24h sunset */
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"suns")){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(day, "day_sunset", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                    continue;
                                }
                                /* 24h part */
                                if(!xmlStrcmp(child_node2->name, (const xmlChar *)"part")){
                                    part_of_day = xmlGetProp(child_node2, (const xmlChar*)"p");
                                    if( !xmlStrcmp(part_of_day, (const xmlChar *)"d") )
                                        store2day = 1;
                                    else
                                        store2day = 0;
                                    xmlFree(part_of_day);
                                    for(child_node3 = child_node2->children; child_node3; child_node3 = child_node3->next){
                                        if( child_node3->type == XML_ELEMENT_NODE){
                                            /* humidity */
                                            if(!xmlStrcmp(child_node3->name, (const xmlChar *)"hmid") ){
                                                temp_xml_string = xmlNodeGetContent(child_node3);
                                                if(!store2day)
                                                    g_hash_table_insert(day, "night_humidity", g_strdup((char*)temp_xml_string));
                                                else
                                                    g_hash_table_insert(day, "day_humidity", g_strdup((char*)temp_xml_string));
                                                xmlFree(temp_xml_string);
                                                continue;
                                            }
                                            /* ppcp */
                                            if(!xmlStrcmp(child_node3->name, (const xmlChar *)"ppcp") ){
                                                temp_xml_string = xmlNodeGetContent(child_node3);
                                                if(!store2day)
                                                    g_hash_table_insert(day, "night_ppcp", g_strdup((char*)temp_xml_string));
                                                else
                                                    g_hash_table_insert(day, "day_ppcp", g_strdup((char*)temp_xml_string));
                                                xmlFree(temp_xml_string);
                                                continue;
                                            }
                                            /* title */
                                            if(!xmlStrcmp(child_node3->name, (const xmlChar *)"t") ){
                                                temp_xml_string = xmlNodeGetContent(child_node3);
                                                if(!store2day)
                                                    g_hash_table_insert(day, "night_title", g_strdup((char*)temp_xml_string));
                                                else
                                                    g_hash_table_insert(day, "day_title", g_strdup((char*)temp_xml_string));
                                                xmlFree(temp_xml_string);
                                                continue;
                                            }
                                            /* icon */
                                            if(!xmlStrcmp(child_node3->name, (const xmlChar *)"icon") ){
                                                temp_xml_string = xmlNodeGetContent(child_node3);
                                                if(!store2day)
                                                    g_hash_table_insert(day, "night_icon", g_strdup((char*)temp_xml_string));
                                                else
                                                    g_hash_table_insert(day, "day_icon", g_strdup((char*)temp_xml_string));
                                                xmlFree(temp_xml_string);
                                                continue;
                                            }
                                            /* wind data */
                                            if(!xmlStrcmp(child_node3->name, (const xmlChar *)"wind") ){
                                                for(child_node4 = child_node3->children; child_node4; child_node4 = child_node4->next){
                                                    if( child_node4->type == XML_ELEMENT_NODE){
                                                        /* speed */
                                                        if(!xmlStrcmp(child_node4->name, (const xmlChar *)"s") ){
                                                            temp_xml_string = xmlNodeGetContent(child_node4);
                                                            if(!store2day)
                                                                g_hash_table_insert(day, "night_wind_speed", g_strdup((char*)temp_xml_string));
                                                            else
                                                                g_hash_table_insert(day, "day_wind_speed", g_strdup((char*)temp_xml_string));
                                                            xmlFree(temp_xml_string);
                                                            continue;
                                                        }
                                                        /* title */
                                                        if(!xmlStrcmp(child_node4->name, (const xmlChar *)"t") ){
                                                            temp_xml_string = xmlNodeGetContent(child_node4);
                                                            if(!store2day)
                                                                g_hash_table_insert(day, "night_wind_title",
                                                                                    g_strdup((char*)temp_xml_string));
                                                            else
                                                                g_hash_table_insert(day, "day_wind_title",
                                                                                    g_strdup((char*)temp_xml_string));
                                                            xmlFree(temp_xml_string);
                                                            continue;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        /* add day to the days list */
                        if(day){
                            forecast = g_slist_append(forecast, (gpointer)day);
                            day = NULL;
                            count_day++;
                        }
                    }
                }
                g_hash_table_insert(data, "forecast", (gpointer)forecast);
            }
        }
    }
    return count_day;
}
/*******************************************************************************/
gint
parse_xml_detail_data(const gchar *station_id, xmlNode *root_node, GHashTable *data){
    xmlNode     *cur_node = NULL,
                *child_node = NULL,
                *child_node2 = NULL,
                *child_node3 = NULL;
    GHashTable  *detail = NULL,
                *hours_data = NULL;
    xmlChar     *temp_xml_string;
    gint        count_hour = 0;
    gchar       buff[256];
    struct tm   tmp_tm = {0};
    GSList      *hour_weather = NULL;
#ifdef DEBUGFUNCTIONCALL
    START_FUNCTION;
#endif
    for(cur_node = root_node->children; cur_node; cur_node = cur_node->next){
        if( cur_node->type == XML_ELEMENT_NODE ){
            if( !xmlStrcmp(cur_node->name, (const xmlChar *) "hbhf") ){
                hours_data = g_hash_table_new(g_str_hash, g_str_equal);
                for(child_node = cur_node->children; child_node; child_node = child_node->next){
                    /* last update */
                    if( child_node->type == XML_ELEMENT_NODE  &&
                            ( !xmlStrcmp(child_node->name, (const xmlChar *)"lsup") ) ){
                        temp_xml_string = xmlNodeGetContent(child_node);
                        g_hash_table_insert(hours_data, "last_update", g_strdup((char*)temp_xml_string));
                        xmlFree(temp_xml_string);
                        continue;
                    }
                    if( child_node->type == XML_ELEMENT_NODE  &&
                            (!xmlStrcmp(child_node->name, (const xmlChar *)"hour") )){
                        detail = g_hash_table_new(g_str_hash, g_str_equal);
                        /* Get an hour */
                        temp_xml_string = xmlGetProp(child_node, (const xmlChar *)"c");
                        /* prepare locale value for hour */
                        memset(buff, 0, sizeof(buff));
                        memcpy(buff, temp_xml_string, (strlen((char*)temp_xml_string)                                                                    > sizeof(buff)) ?
                                                                 (sizeof(buff) - 1) :
                                                   (strlen((char*)temp_xml_string)));
                        strptime(buff, "%H", &tmp_tm);
                        memset(buff, 0, sizeof(buff));
                        strftime(buff, sizeof(buff) - 1, "%H", &tmp_tm);
                        g_hash_table_insert(detail, "hours", g_strdup(buff));
                        xmlFree(temp_xml_string);
                        for(child_node2 = child_node->children; child_node2; child_node2 = child_node2->next){
                            if( child_node2->type == XML_ELEMENT_NODE){
                                /* hour temperature */
                               if(!xmlStrcmp(child_node2->name, (const xmlChar *)"tmp"))    {
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(detail, "hour_temperature", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                    continue;
                               }
                               /* feels like for hour */
                               if(!xmlStrcmp(child_node2->name, (const xmlChar *)"flik")){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(detail, "hour_feels_like", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                    continue;
                               }
                               /* icon for hour */
                               if(!xmlStrcmp(child_node2->name, (const xmlChar *)"icon")){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(detail, "hour_icon", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                    continue;
                               }
                               /* title for hour */
                               if(!xmlStrcmp(child_node2->name, (const xmlChar *)"t")){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(detail, "hour_title", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                    continue;
                               }
                               /* wind data for hour */
                               if(!xmlStrcmp(child_node2->name, (const xmlChar *)"wind")){
                                    for(child_node3 = child_node2->children; child_node3 != NULL; child_node3 = child_node3->next){
                                        if( child_node3->type == XML_ELEMENT_NODE ){
                                            /* speed */
                                            if(!xmlStrcmp(child_node3->name, (const xmlChar *)"s")){
                                                temp_xml_string = xmlNodeGetContent(child_node3);
                                                g_hash_table_insert(detail, "hour_wind_speed", g_strdup((char*)temp_xml_string));
                                                xmlFree(temp_xml_string);
                                            }
                                            /* gust */
                                            if(!xmlStrcmp(child_node3->name, (const xmlChar *)"gust") ){
                                                temp_xml_string = xmlNodeGetContent(child_node3);
                                                g_hash_table_insert(detail, "hour_wind_gust", g_strdup((char*)temp_xml_string));
                                                xmlFree(temp_xml_string);
                                            }
                                            /* direction */
                                            if(!xmlStrcmp(child_node3->name, (const xmlChar *)"t")){
                                                temp_xml_string = xmlNodeGetContent(child_node3);
                                                g_hash_table_insert(detail, "hour_wind_direction", g_strdup((char*)temp_xml_string));
                                                xmlFree(temp_xml_string);
                                            }
                                        }
                                    }
                               }
                               /* hour humidity */
                               if(!xmlStrcmp(child_node2->name, (const xmlChar *)"hmid") ){
                                    temp_xml_string = xmlNodeGetContent(child_node2);
                                    g_hash_table_insert(detail, "hour_humidity", g_strdup((char*)temp_xml_string));
                                    xmlFree(temp_xml_string);
                                    continue;
                               }
                            }
                        }
                        if(detail){
                            hour_weather = g_slist_append(hour_weather, (gpointer)detail);
                            detail = NULL;
                            count_hour++;
                        }
                    }
                }
            }
        }
    }
    g_hash_table_insert(hours_data, "hours_data", (gpointer)hour_weather);
    g_hash_table_insert(data, "detail", (gpointer)hours_data);
    return count_hour;
}
/*******************************************************************************/
