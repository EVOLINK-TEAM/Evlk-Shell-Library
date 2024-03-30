#pragma once

#include "WString.h"

namespace _EVLK_DICTMAN_
{
    struct dictman
    {
    private:
        struct pos
        {
            const char *b, *n, *l;
            pos() { b = n = l = nullptr; }
            pos(const char *b, const char *n, const char *l) : b(b), n(n), l(l){};
            operator bool() { return b && *b != '\0' && n && l; }
        };

        String pool = "";
        size_t Lenth = 0;
        const char *__begin__() const { return pool.c_str(); }
        const char *__end__() const { return pool.c_str() + pool.length(); }
        size_t __lenth__(const char *);
        void _found(pos &p) const;
        void found(const String &key, pos &p) const;
        void found(size_t index, pos &p) const;
        bool __replace(pos &p, const char *b, size_t n);
        bool _set(pos &p, const char *v, size_t n);
        bool _insert(const char *k, size_t kn, const char *v, size_t vn);
        bool _erase(pos &p);

    public:
        const char *getpool() const;
        size_t lenth();
        bool set(String key, String value, bool overwrite = true);
        bool set(size_t index, String value);
        bool get(String key, String &value) const;
        bool get(size_t index, String &value) const;
        bool erase(String key);
        bool erase(size_t index);
    };
}