#ifndef FLEX_OBJ_DEBUG_H
#define FLEX_OBJ_DEBUG_H

namespace flex
{
  struct obj_debug
  {
    static const int DEFAULT_VAL = 1;
    static const int INIT_KEY = 858599509;

    obj_debug() :
        val(DEFAULT_VAL), init(INIT_KEY), move_only(false), was_copied(false)
    {
    }

    obj_debug(int i) :
        val(i), init(INIT_KEY), move_only(false), was_copied(false)
    {
    }

    obj_debug(const obj_debug& o) :
        val(o.val), init(INIT_KEY), move_only(o.move_only), was_copied(true)
    {

    }

    obj_debug& operator=(const obj_debug& o)
    {
      val = o.val;
      move_only = o.move_only;
      was_copied = true;
      return *this;
    }

#ifdef FLEX_HAS_CXX11
    obj_debug(const obj_debug&& o) :
    val(o.val), init(INIT_KEY), move_only(o.move_only), was_copied(o.was_copied)
    {

    }

    obj_debug& operator=(const obj_debug&& o)
    {
      val = o.val;
      move_only = o.move_only;
      was_copied = o.was_copied;
      return *this;
    }
#endif

    operator int() const
    {
      return val;
    }

    ~obj_debug()
    {
      init = 0;
    }

    int val;
    int init;
    bool move_only;
    bool was_copied;
  };

} //flex namespace
#endif /* FLEX_OBJ_DEBUG_H */
