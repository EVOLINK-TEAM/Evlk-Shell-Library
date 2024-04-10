#include "evlk_dictman.h"
#include <cstring>

namespace _EVLK_DICTMAN_
{
    size_t dictman::__lenth__(const char *b)
    {
        size_t N = 0;
        pos p(b, NULL, NULL);
        _found(p);
        while (p)
        {
            N++;
            p.b = p.l + 1;
            _found(p);
        }
        return N;
    }
    void dictman::_found(pos &p) const
    {
        p.l = p.n = NULL;
        if (!p.b || *p.b == '\0')
        {
            p = pos();
            return;
        }
        p.n = strchr(p.b, '=');
        p.l = strchr(p.n, '\n');
    }
    void dictman::found(const String &key, pos &p) const
    {
        p = pos(__begin__(), NULL, NULL);
        _found(p);
        while (p)
        {
            String name(p.b, p.n - p.b);
            if (key == name)
                return;
            p.b = p.l + 1;
            _found(p);
        }
        p = pos();
    }
    void dictman::found(size_t index, pos &p) const
    {
        p = pos();
        if (!Lenth)
            return;
        p.b = this->__begin__();
        _found(p);
        for (size_t i = 0; p && i < index; i++)
        {
            p.b = p.l + 1;
            _found(p);
        }
        return;
    }

    bool dictman::__replace(pos &p, const char *b, size_t n)
    {
        if (!p)
            return false;
        pool = String(__begin__(), p.b - __begin__()) + String(b, n) + String(p.l + 1, __end__() - p.l - 1);
        return true;
    }
    bool dictman::_set(pos &p, const char *b, size_t n)
    {
        if (!p)
            return false;
        String s = String(p.b, p.n - p.b) + '=' + String(b, n) + '\n';
        return __replace(p, s.c_str(), s.length());
    }
    bool dictman::_insert(const char *k, size_t kn, const char *n, size_t nn)
    {
        String name(k, kn);
        String value(n, nn);
        pool += name + '=' + value + '\n';
        Lenth++;
        return true;
    }
    bool dictman::_erase(pos &p)
    {
        if (!__replace(p, "", 0))
            return false;
        Lenth--;
        return true;
    };
    const char *dictman::getpool() const
    {
        return pool.c_str();
    };
    size_t dictman::lenth()
    {
        return Lenth;
    }
    bool dictman::set(String key, String value, bool overwrite)
    {
        pos p;
        found(key, p);
        if (overwrite && _set(p, value.c_str(), value.length()))
            return true;
        return _insert(key.c_str(), key.length(), value.c_str(), value.length());
    }
    bool dictman::set(size_t index, String value)
    {
        pos p;
        found(index, p);
        return _set(p, value.c_str(), value.length());
    }
    bool dictman::get(String key, String &value) const
    {
        value = "";
        pos p;
        found(key, p);
        if (!p)
            return false;
        value = String(p.n + 1, p.l - p.n - 1);
        return true;
    }
    bool dictman::get(size_t index, String &value) const
    {
        value = "";
        pos p;
        found(index, p);
        if (!p)
            return false;
        value = String(p.n + 1, p.l - p.n - 1);
        return true;
    }
    bool dictman::erase(String key)
    {
        pos p;
        found(key, p);
        return _erase(p);
    }
    bool dictman::erase(size_t index)
    {
        pos p;
        found(index, p);
        return _erase(p);
    }
}