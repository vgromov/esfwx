#ifndef _es_data_stack_h_
#define _es_data_stack_h_

// Uncomment following, to use vector container for data stack internal implementation
#ifndef ES_USE_VECTOR_FOR_STACK
# define ES_USE_VECTOR_FOR_STACK 1
#else
# include <deque>
#endif

/// Data stack class
///
class EsScriptDataStack
{
public:
  typedef std::shared_ptr<EsScriptDataStack> Ptr;

#if 1 == ES_USE_VECTOR_FOR_STACK
public:
  enum {depthDefault = 32};
#endif

public:
  EsScriptDataStack() ES_NOTHROW;

  inline bool empty() const ES_NOTHROW { return m_data.empty(); }
  inline size_t size() const ES_NOTHROW { return m_data.size(); }

  void clear() ES_NOTHROW;
  void truncate(size_t newSize);

  // data stack manipulation
  void push(const EsScriptValAccessorIntf::Ptr& acc) ES_NOTHROW;
  EsScriptValAccessorIntf::Ptr pop();

  EsString::Array trace() const ES_NOTHROW;

#if defined(DEBUG) && defined(ESSCRIPT_DATA_STACK_USE_TRACE)
  void trace() const;
#endif

protected:
#if 1 == ES_USE_VECTOR_FOR_STACK
  typedef std::vector<EsScriptValAccessorIntf::Ptr> Container;
#else
  typedef std::deque<EsScriptValAccessorIntf::Ptr> Container;
#endif
  Container m_data;
};

#endif // _es_data_stack_h_
