#ifndef Y_STRING_H
#define Y_STRING_H

#include <cstddef>
#include <cassert>

#include "alloc.h"

namespace y 
{
template <typename CharT> class base_string {
public:
  base_string()
      : m_str(m_default_str), m_str_len(0), m_reserve_len(STRING_DEFAULT_LEN),
        m_allocater(Allocator::get_default_allocator()) {
    m_str[0] = '\0';
  }

public:
  base_string &operator+(const base_string<CharT> &str) {}

public:
  base_string &append(const base_string<CharT> &str) {
    if (m_reserve_len < m_str_len + str.m_str_len) {
      assert(m_reserve_len >= STRING_DEFAULT_LEN);
      if (m_reserve_len > STRING_DEFAULT_LEN) {
        m_str = m_allocater->realloc(m_str, m_str_len + str.m_str_len + 1);
        
      }
    }
  }

private:
  static const unsigned STRING_DEFAULT_LEN = 8;
  char m_default_str[STRING_DEFAULT_LEN];
  char *m_str;
  size_t m_str_len, m_reserve_len;
  Allocator *m_allocater;
};
}

#endif //  Y_STRING_H