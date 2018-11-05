#ifndef _es_script_parser_h_
#define _es_script_parser_h_

/// Lightweight Grammar parser and AST traversal adapter class
///
class ESSCRIPT_CLASS EsScriptParser
{
public:
  enum {
    noneId, //< To comply with compiler grammar IDs
    skipId, //< To comply with compiler grammar IDs
#define GRAMMAR_RULE(Name) Name ## Id,
#include "EsScriptCompilerRuleNames.hxx"
    idsCount
  };
  typedef std::set<int> IdsT;

  class ESSCRIPT_CLASS Node
  {
  public:
    typedef std::shared_ptr<Node> PtrT;

  protected:
    Node(const EsScriptParser& parser, ulong id, ulong start, ulong end, const Node* parent = nullptr, size_t expectedChildrenCnt = 0);

  public:
    inline bool isOk() const ES_NOTHROW { return nullptr != m_parser && noneId != m_id; }
    inline bool isRoot() const ES_NOTHROW { return nullptr == m_parent; }

    inline const EsScriptParser& parserGet() const ES_NOTHROW { return *m_parser; }

    inline int idGet() const ES_NOTHROW { return m_id; }

    inline const Node& parentGet() const ES_NOTHROW { return *m_parent; }

    inline bool haveChildren() const ES_NOTHROW { return !m_children.empty(); }
    inline size_t childrenCountGet() const ES_NOTHROW { return m_children.size(); }
    inline const Node& childGet(size_t idx) const { return *m_children[idx].get(); }

    bool isIdOneOf(const EsScriptParser::IdsT& ids) const ES_NOTHROW;

    inline void rangeGet(ulong& start, ulong& end) const ES_NOTHROW { start = m_start; end = m_end; }

    const Node* firstChildGetById(int id, bool doNested = true) const ES_NOTHROW;

    EsString stringExtract(const EsString& in) const ES_NOTHROW;
    EsString stringValExtract(const EsString& str) const ES_NOTHROW;

  protected:
    const Node* deepestNodeFindByPos(ulong pos) const ES_NOTHROW;
    PtrT childAdd(ulong id, ulong start, ulong end, size_t expectedChildrenCnt);
    void rangeCheckToInclude(ulong start, ulong end) ES_NOTHROW;

    template <typename ChildrenT>
    static void childrenNodesAdd(
      EsString::const_pointer inStart,
      const EsScriptParser::Node::PtrT& node,
      const ChildrenT& astChildren
    );

  protected:
    std::vector<PtrT> m_children;
    const EsScriptParser* m_parser;
    const Node* m_parent;
    int m_id;
    ulong m_start;
    ulong m_end;

  private:
    Node() ES_REMOVEDECL;
    Node(const Node&) ES_REMOVEDECL;
    Node& operator=(const Node&) ES_REMOVEDECL;

    friend class EsScriptParser;
  };

public:
  EsScriptParser();

  inline bool isParsed() const ES_NOTHROW { return m_isParsed; }

  inline ulong stopGet() const ES_NOTHROW { return m_stop; }

  inline size_t rootsCountGet() const ES_NOTHROW { return m_roots.size(); }
  inline const Node& rootNodeGet(size_t idx) const { return *m_roots[idx].get(); }

  bool parse(const EsString& in);

  const Node* deepestNodeFindByPos(ulong pos) const ES_NOTHROW;

protected:
  void reset();

  template <typename AstTreesT>
  void rootsInit(
    EsString::const_pointer inStart,
    AstTreesT& trees
  );

  template <typename AstNodeT>
  void rootInit(
    EsString::const_pointer inStart,
    const AstNodeT& astRoot
  );

protected:
  EsString::HashT m_hashPrev;
  std::vector<Node::PtrT> m_roots;
  ulong m_stop;
  bool m_isParsed;

private:
  EsScriptParser(const EsScriptParser&) ES_REMOVEDECL;
  EsScriptParser& operator=(const EsScriptParser&) ES_REMOVEDECL;
};

#endif // _es_script_parser_h_
