#ifndef _es_script_object_data_buffer_h_
#define _es_script_object_data_buffer_h_

class ESSCRIPT_CLASS EsScriptObjectDataBuffer : public EsBinBuffer
{
public:
  typedef std::shared_ptr<EsScriptObjectDataBuffer> Ptr;

private:
  struct private_access {};

public:
  // prohibited functionality
  explicit EsScriptObjectDataBuffer(EsScriptObjectDataBuffer::private_access);
  
  static EsScriptObjectDataBuffer::Ptr create();

  static EsScriptObjectDataBuffer::Ptr& null();
  
protected:  
  // internal services
  //
  // directly assign buffer from other data
  void assignDirect(EsBinBuffer::const_pointer src, size_t offs, size_t count);
  // reset accumulated fixups
  void fixupsReset();
  // handle memory layout notification fixup
  void onDataLayoutFixup(long oldOffs, size_t newOffs, size_t objSize);
  // apply accumulated fixups
  void fixupsApply(size_t newSize);
  // data blocks manipulation
  void copyUp(size_t oldStart, size_t newStart, size_t size);
  void copyDown(size_t oldStart, size_t newStart, size_t size);
  void zeroOut(size_t start, size_t size);
  
protected:
  // fixup entry
  struct Fixup
  {
    inline Fixup(long oldOffs = 0, size_t newOffs = 0, size_t objSize = 0) :
    m_oldOffs(oldOffs),
    m_newOffs(newOffs),
    m_objSize(objSize)
    {}
    
    long m_oldOffs;
    size_t m_newOffs;
    size_t m_objSize;
  };
  typedef std::map<size_t, Fixup> Fixups;
    
protected:
  // data members  
  //
  // fixups accumulator, fixups get auto-sorted by newOffset in ascending order
  Fixups m_fixupsDown;
  Fixups m_fixupsUp;
  Fixups m_fixupsNew;
  
  friend class EsScriptObject;
  friend class EsScriptArrayObject;  

private:
  EsScriptObjectDataBuffer() ES_REMOVEDECL;
  EsScriptObjectDataBuffer(const EsScriptObjectDataBuffer&) ES_REMOVEDECL;
  const EsScriptObjectDataBuffer& operator=(const EsScriptObjectDataBuffer&) ES_REMOVEDECL;
};
typedef EsScriptObjectDataBuffer::Ptr EsScriptObjectDataBufferPtr;

#ifdef ESSCRIPT_OBJECT_BUFFER_USE_TRACE
#  define ESSCRIPT_OBJECT_BUFFER_TRACE1(a)                    ES_DEBUG_TRACE(a);
#  define ESSCRIPT_OBJECT_BUFFER_TRACE2(a, b)                ES_DEBUG_TRACE((a), (b));
#  define ESSCRIPT_OBJECT_BUFFER_TRACE3(a, b, c)              ES_DEBUG_TRACE((a), (b), (c));
#  define ESSCRIPT_OBJECT_BUFFER_TRACE4(a, b, c, d)          ES_DEBUG_TRACE((a), (b), (c), (d));
#  define ESSCRIPT_OBJECT_BUFFER_TRACE5(a, b, c, d, e)        ES_DEBUG_TRACE((a), (b), (c), (d), (e));
#  define ESSCRIPT_OBJECT_BUFFER_TRACE6(a, b, c, d, e, f)    ES_DEBUG_TRACE((a), (b), (c), (d), (e), (f));
#  define ESSCRIPT_OBJECT_BUFFER_TRACE7(a, b, c, d, e, f, j)  ES_DEBUG_TRACE((a), (b), (c), (d), (e), (f), (j));
#else
#  define ESSCRIPT_OBJECT_BUFFER_TRACE1(a)
#  define ESSCRIPT_OBJECT_BUFFER_TRACE2(a, b)
#  define ESSCRIPT_OBJECT_BUFFER_TRACE3(a, b, c)
#  define ESSCRIPT_OBJECT_BUFFER_TRACE4(a, b, c, d)
#  define ESSCRIPT_OBJECT_BUFFER_TRACE5(a, b, c, d, e)
#  define ESSCRIPT_OBJECT_BUFFER_TRACE6(a, b, c, d, e, f)
#  define ESSCRIPT_OBJECT_BUFFER_TRACE7(a, b, c, d, e, f, j)
#endif

#endif // _es_script_object_data_buffer_h_
