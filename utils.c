#include "utils.h"
#include <glib.h>

size_t max_variadic(int count, ...) {
    if (count <= 0) return INT_MIN; // Manejo de caso vacío

    va_list args;
    va_start(args, count);

    // Inicializar con el primer argumento
    size_t maximum = va_arg(args, size_t);

    // Comparar los restantes
    for (int i = 1; i < count; i++) {
        size_t num = va_arg(args, size_t);
        if (num > maximum) {
            maximum = num;
        }
    }

    va_end(args);
    return maximum;
}

gboolean cmpstr(GString *str1, const gchar *str2) {
    GString *cmps = g_string_new(str2);
    return g_string_equal(str1, cmps);
}
