#ifndef UTILS_H_
#define UTILS_H_

#include <glib.h>

#define CMP_KEY(res,str,key) (res) |= !strcmp((str), (key))
#define DATA_NTH(nth, list) g_list_nth(list, nth)->data
#define CMPS(cmps, str1, str2) g_string_equal((str1), g_string_overwrite(cmps, 0, (str2) ))

size_t max_variadic(int count, ...);
gboolean cmpstr(GString *str1, const gchar *str2);


#endif
