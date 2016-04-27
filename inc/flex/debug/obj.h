#ifndef FLEX_DEBUG_OBJ_H
#define FLEX_DEBUG_OBJ_H

namespace flex
{
  namespace debug
  {
    struct obj
    {
      static const int DEFAULT_VAL = 1;
      static const int INIT_KEY = 858599509;

      obj() :
          val(DEFAULT_VAL), init(INIT_KEY), move_only(false), was_copied(false)
      {
      }

      obj(int i) :
          val(i), init(INIT_KEY), move_only(false), was_copied(false)
      {
      }

      obj(int i, bool) :
          val(i), init(INIT_KEY), move_only(false), was_copied(false)
      {
      }

      obj(const obj& o) :
          val(o.val), init(INIT_KEY), move_only(o.move_only), was_copied(true)
      {

      }

      obj& operator=(const obj& o)
      {
        val = o.val;
        move_only = o.move_only;
        was_copied = true;
        return *this;
      }

#ifdef FLEX_HAS_CXX11
      obj(const obj&& o) :
      val(o.val), init(INIT_KEY), move_only(o.move_only), was_copied(o.was_copied)
      {

      }

      obj& operator=(const obj&& o)
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

      ~obj()
      {
        init = 0;
      }

      int val;
      int init;
      bool move_only;
      bool was_copied;
    };

  } //debug namespace
} //flex namespace
#endif /* FLEX_DEBUG_OBJ_H */
