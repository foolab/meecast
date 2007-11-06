#include <glib.h>
#include "weather-home_hash.h"
/*******************************************************************************/
GHashTable* hash_table_create(void){
    GHashTable *hash;
    
    hash = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(hash, "NO STATION", _("NO STATION"));
    g_hash_table_insert(hash, "m/s", _("m/s"));
    g_hash_table_insert(hash, "km/s", _("km/s"));
    g_hash_table_insert(hash, "mi/s", _("mi/s"));
    g_hash_table_insert(hash, "m/h", _("m/h"));
    g_hash_table_insert(hash, "km/h", _("km/h"));
    g_hash_table_insert(hash, "mi/h", _("mi/h"));
    g_hash_table_insert(hash, "AM Clouds / PM Sun", _("AM Clouds / PM Sun"));
    g_hash_table_insert(hash, "AM Drizzle", _("AM Drizzle"));    
    g_hash_table_insert(hash, "AM Drizzle / Wind", _("AM Drizzle / Wind"));
    g_hash_table_insert(hash, "AM Fog / PM Clouds", _("AM Fog / PM Clouds"));
    g_hash_table_insert(hash, "AM Fog / PM Sun", _("AM Fog / PM Sun"));    
    g_hash_table_insert(hash, "AM Light Rain", _("AM Light Rain"));
    g_hash_table_insert(hash, "AM Light Rain / Wind", _("AM Light Rain / Wind"));
    g_hash_table_insert(hash, "AM Light Snow", _("AM Light Snow"));
    g_hash_table_insert(hash, "AM Light Wintry Mix", _("AM Light Wintry Mix"));
    g_hash_table_insert(hash, "AM Rain", _("AM Rain"));
    g_hash_table_insert(hash, "AM Rain / Snow", _("AM Rain / Snow"));
    g_hash_table_insert(hash, "AM Rain / Snow Showers", _("AM Rain / Snow Showers"));
    g_hash_table_insert(hash, "AM Rain / Snow Showers / Wind", _("AM Rain / Snow Showers / Wind"));    
    g_hash_table_insert(hash, "AM Showers", _("AM Showers"));
    g_hash_table_insert(hash, "AM Snow", _("AM Snow"));
    g_hash_table_insert(hash, "AM Snow Showers", _("AM Snow Showers"));
    g_hash_table_insert(hash, "CALM", _("CALM"));
    g_hash_table_insert(hash, "Clear", _("Clear"));
    g_hash_table_insert(hash, "Clear / Wind", _("Clear / Wind"));
    g_hash_table_insert(hash, "Clouds Early / Clearing Late", _("Clouds Early / Clearing Late"));
    g_hash_table_insert(hash, "Cloudy and Windy", _("Cloudy and Windy"));
    g_hash_table_insert(hash, "Cloudy", _("Cloudy"));
    g_hash_table_insert(hash, "Drizzle", _("Drizzle"));
    g_hash_table_insert(hash, "Drizzle / Fog", _("Drizzle / Fog"));
    g_hash_table_insert(hash, "Drizzle Late", _("Drizzle Late"));    
    g_hash_table_insert(hash, "E", _("E"));
    g_hash_table_insert(hash, "ENE", _("ENE"));
    g_hash_table_insert(hash, "ESE", _("ESE"));
    g_hash_table_insert(hash, "Fair", _("Fair"));
    g_hash_table_insert(hash, "falling", _("falling"));
    g_hash_table_insert(hash, "Few Showers", _("Few Showers"));
    g_hash_table_insert(hash, "Few Showers / Wind", _("Few Showers / Wind"));
    g_hash_table_insert(hash, "Few Snow", _("Few Snow"));
    g_hash_table_insert(hash, "Few Snow Showers", _("Few Snow Showers"));
    g_hash_table_insert(hash, "First Quarter", _("First Quarter"));
    g_hash_table_insert(hash, "Fog", _("Fog"));
    g_hash_table_insert(hash, "Foggy", _("Foggy"));    
    g_hash_table_insert(hash, "Fog Early / Clouds Late", _("Fog Early / Clouds Late"));
    g_hash_table_insert(hash, "Fog Late", _("Fog Late"));    
    g_hash_table_insert(hash, "Full", _("Full"));
    g_hash_table_insert(hash, "Haze", _("Haze"));
    g_hash_table_insert(hash, "Heavy Rain", _("Heavy Rain"));
    g_hash_table_insert(hash, "Heavy Rain / Wind", _("Heavy Rain / Wind"));
    g_hash_table_insert(hash, "Heavy Snow", _("Heavy Snow"));
    g_hash_table_insert(hash, "High", _("High"));
    g_hash_table_insert(hash, "Isolated T-Storms", _("Isolated T-Storms"));
    g_hash_table_insert(hash, "Isolated T-Storms / Wind", _("Isolated T-Storms / Wind"));
    g_hash_table_insert(hash, "Light Drizzle", _("Light Drizzle"));
    g_hash_table_insert(hash, "Light Rain and Windy", _("Light Rain and Windy"));
    g_hash_table_insert(hash, "Light Rain and Shower", _("Light Rain and Shower"));
    g_hash_table_insert(hash, "Light Rain", _("Light Rain"));
    g_hash_table_insert(hash, "Light Rain / Fog", _("Light Rain / Fog"));    
    g_hash_table_insert(hash, "Light Rain / Wind", _("Light Rain / Wind"));
    g_hash_table_insert(hash, "Light Rain / Wind Late", _("Light Rain / Wind Late"));    
    g_hash_table_insert(hash, "Light Rain Early", _("Light Rain Early"));
    g_hash_table_insert(hash, "Light Rain Late", _("Light Rain Late"));
    g_hash_table_insert(hash, "Light Rain Shower", _("Light Rain Shower"));    
    g_hash_table_insert(hash, "Light Snow", _("Light Snow"));
    g_hash_table_insert(hash, "Light Snow Early", _("Light Snow Early"));
    g_hash_table_insert(hash, "Light Snow Late", _("Light Snow Late"));
    g_hash_table_insert(hash, "Light Snow Shower", _("Light Snow Shower"));
    g_hash_table_insert(hash, "Light Snow / Fog", _("Light Snow / Fog"));        
    g_hash_table_insert(hash, "Low", _("Low"));
    g_hash_table_insert(hash, "Mist", _("Mist"));
    g_hash_table_insert(hash, "Mostly Clear", _("Mostly Clear"));
    g_hash_table_insert(hash, "Mostly Cloudy", _("Mostly Cloudy"));
    g_hash_table_insert(hash, "Mostly Cloudy and Windy", _("Mostly Cloudy and Windy"));
    g_hash_table_insert(hash, "Mostly Cloudy / Wind", _("Mostly Cloudy / Wind"));
    g_hash_table_insert(hash, "Mostly Sunny", _("Mostly Sunny"));
    g_hash_table_insert(hash, "N/A", _("N/A"));
    g_hash_table_insert(hash, "NE", _("NE"));
    g_hash_table_insert(hash, "New", _("New"));
    g_hash_table_insert(hash, "N", _("N"));
    g_hash_table_insert(hash, "NNE", _("NNE"));
    g_hash_table_insert(hash, "NNW", _("NNW"));
    g_hash_table_insert(hash, "NW", _("NW"));
    g_hash_table_insert(hash, "Overcast", _("Overcast"));
    g_hash_table_insert(hash, "Partly Cloudy", _("Partly Cloudy"));
    g_hash_table_insert(hash, "Partly Cloudy / Wind", _("Partly Cloudy / Wind"));
    g_hash_table_insert(hash, "Partly Cloudy and Windy", _("Partly Cloudy and Windy"));
    g_hash_table_insert(hash, "PM Fog", _("PM Fog"));
    g_hash_table_insert(hash, "PM Light Rain", _("PM Light Rain"));
    g_hash_table_insert(hash, "PM Light Snow", _("PM Light Snow"));    
    g_hash_table_insert(hash, "PM Rain", _("PM Rain"));
    g_hash_table_insert(hash, "PM Rain / Wind", _("PM Rain / Wind"));
    g_hash_table_insert(hash, "PM Showers", _("PM Showers"));
    g_hash_table_insert(hash, "PM Showers / Wind", _("PM Showers / Wind"));
    g_hash_table_insert(hash, "PM Snow Showers", _("PM Snow Showers"));
    g_hash_table_insert(hash, "Rain and Snow", _("Rain and Snow"));
    g_hash_table_insert(hash, "Rain Early", _("Rain Early"));
    g_hash_table_insert(hash, "Rain", _("Rain"));
    g_hash_table_insert(hash, "Rain Shower and Windy", _("Rain Shower and Windy"));
    g_hash_table_insert(hash, "Rain Shower", _("Rain Shower"));
    g_hash_table_insert(hash, "Rain / Snow", _("Rain / Snow"));
    g_hash_table_insert(hash, "Rain / Snow Early", _("Rain / Snow Early"));
    g_hash_table_insert(hash, "Rain / Snow Late", _("Rain / Snow Late"));
    g_hash_table_insert(hash, "Rain / Snow / Wind", _("Rain / Snow / Wind"));
    g_hash_table_insert(hash, "Rain / Snow Showers Late", _("Rain / Snow Showers Late"));
    g_hash_table_insert(hash, "Rain / Snow Showers", _("Rain / Snow Showers"));
    g_hash_table_insert(hash, "Rain / Snow Showers / Wind", _("Rain / Snow Showers / Wind"));
    g_hash_table_insert(hash, "Rain / Thunder", _("Rain / Thunder"));    
    g_hash_table_insert(hash, "Rain to Wintry Mix", _("Rain to Wintry Mix"));
    g_hash_table_insert(hash, "Rain to Wintry Mix / Wind", _("Rain to Wintry Mix / Wind"));
    g_hash_table_insert(hash, "Rain / Wind Early", _("Rain / Wind Early"));
    g_hash_table_insert(hash, "Rain / Wind", _("Rain / Wind"));
    g_hash_table_insert(hash, "rising", _("rising"));
    g_hash_table_insert(hash, "Scattered Showers", _("Scattered Showers"));
    g_hash_table_insert(hash, "Scattered Snow", _("Scattered Snow"));
    g_hash_table_insert(hash, "Scattered Snow Showers", _("Scattered Snow Showers"));
    g_hash_table_insert(hash, "Scattered T-Storms", _("Scattered T-Storms"));
    g_hash_table_insert(hash, "SE", _("SE"));
    g_hash_table_insert(hash, "Showers Early", _("Showers Early"));
    g_hash_table_insert(hash, "Showers in the Vicinity", _("Showers in the Vicinity"));
    g_hash_table_insert(hash, "Showers Late", _("Showers Late"));
    g_hash_table_insert(hash, "Showers", _("Showers"));
    g_hash_table_insert(hash, "Showers / Wind", _("Showers / Wind"));
    g_hash_table_insert(hash, "Smoke", _("Smoke"));
    g_hash_table_insert(hash, "Snow Showers Late", _("Snow Showers Late"));
    g_hash_table_insert(hash, "Snow Shower", _("Snow Shower"));
    g_hash_table_insert(hash, "Snow Shower / Wind", _("Snow Shower / Wind"));
    g_hash_table_insert(hash, "Snow Showers / Wind Early", _("Snows Showers / Wind Early"));
    g_hash_table_insert(hash, "Snow Showers Early", _("Snows Showers Early"));
    g_hash_table_insert(hash, "Snow", _("Snow"));
    g_hash_table_insert(hash, "Snow / Wind", _("Snow / Wind"));
    g_hash_table_insert(hash, "Snow to Rain", _("Snow to Rain"));
    g_hash_table_insert(hash, "Snow to Wintry Mix", _("Snow to Wintry Mix"));
    g_hash_table_insert(hash, "Snow to Wintry Mix / Wind", _("Snow to Wintry Mix / Wind"));
    g_hash_table_insert(hash, "S", _("S"));
    g_hash_table_insert(hash, "SSE", _("SSE"));
    g_hash_table_insert(hash, "SSW", _("SSW"));
    g_hash_table_insert(hash, "steady", _("steady"));
    g_hash_table_insert(hash, "Storm", _("Storm"));
    g_hash_table_insert(hash, "Sunny", _("Sunny"));
    g_hash_table_insert(hash, "Sunny / Wind", _("Sunny / Wind"));
    g_hash_table_insert(hash, "SW", _("SW"));
    g_hash_table_insert(hash, "T-Storms", _("T-Storms"));
    g_hash_table_insert(hash, "T-Storms Early", _("T-Storms Early"));    
    g_hash_table_insert(hash, "Thunder", _("Thunder"));    
    g_hash_table_insert(hash, "Unlimited", _("Unlimited"));
    g_hash_table_insert(hash, "Waning Crescent", _("Waning Crescent"));
    g_hash_table_insert(hash, "Waning Gibbous", _("Waning Gibbous"));
    g_hash_table_insert(hash, "Waxing Crescent", _("Waxing Crescent"));
    g_hash_table_insert(hash, "Waxing Gibbous", _("Waxing Gibbous"));
    g_hash_table_insert(hash, "Wind", _("Wind"));
    g_hash_table_insert(hash, "Wintry Mix to Snow", _("Wintry Mix to Snow"));
    g_hash_table_insert(hash, "Wintry Mix / Wind Early", _("Wintry Mix / Wind Early"));
    g_hash_table_insert(hash, "Wintry Mix / Wind", _("Wintry Mix / Wind"));
    g_hash_table_insert(hash, "Wintry Mix", _("Wintry Mix"));
    g_hash_table_insert(hash, "WNW", _("WNW"));
    g_hash_table_insert(hash, "WSW", _("WSW"));
    g_hash_table_insert(hash, "W", _("W"));
    g_hash_table_insert(hash, "VAR", _("VAR"));
    return  hash;
}
/*******************************************************************************/
gpointer hash_table_find(gpointer key){
    gpointer	orig_key,
		value,
		result;
    
    if(g_hash_table_lookup_extended(app->hash,
				    key,
				    &orig_key,
				    &value))
	result = value;
    else
	result = key;
    return result;
}
/*******************************************************************************/
