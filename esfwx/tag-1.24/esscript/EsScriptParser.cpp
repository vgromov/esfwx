#include "esscriptpch.h"
#pragma hdrstop

#include "EsScriptParser.h"
//--------------------------------------------------------------------------------

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

EsScriptParser::Node::Node(const EsScriptParser& parser, ulong id, ulong start, ulong end, const EsScriptParser::Node* parent /*= nullptr*/, size_t expectedChildrenCnt /*= 0*/) :
  m_parser(&parser),
  m_parent(parent),
  m_id(id),
  m_start(start),
  m_end(end)
{
  if(expectedChildrenCnt)
    m_children.reserve(expectedChildrenCnt);
}
//--------------------------------------------------------------------------------

void EsScriptParser::Node::rangeCheckToInclude(ulong start, ulong end) ES_NOTHROW
{
  if(m_start > start)
    m_start = start;

  if(m_end < end)
    m_end = end;

  // Propagate to entire parent chain
  EsScriptParser::Node* parent = const_cast<EsScriptParser::Node*>(m_parent);
  if(parent)
    parent->rangeCheckToInclude(
      start, 
      end
    );
}
//--------------------------------------------------------------------------------

EsScriptParser::Node::PtrT EsScriptParser::Node::childAdd(ulong id, ulong start, ulong end, size_t expectedChildrenCnt)
{
  PtrT node(
    new Node(
      *m_parser,
      id,
      start,
      end,
      this,
      expectedChildrenCnt
    )
  );
  ES_ASSERT(node);

  // widen our range to be child-inclusive
  rangeCheckToInclude(
    start,
    end
  );

  m_children.push_back(node);

  return node;
}
//--------------------------------------------------------------------------------

bool EsScriptParser::Node::isIdOneOf(const EsScriptParser::IdsT& ids) const ES_NOTHROW
{
  return ids.end() != ids.find(m_id);
}
//--------------------------------------------------------------------------------

const EsScriptParser::Node* EsScriptParser::Node::firstChildGetById(int id, bool doNested /*= true*/) const ES_NOTHROW
{
  if(!haveChildren())
    return nullptr;

  for(size_t idx = 0; idx < m_children.size(); ++idx)
  {
    PtrT node = m_children[idx];

    if(id == node->idGet())
      return node.get();

    if(doNested && node->haveChildren())
    {
      const EsScriptParser::Node* nested = node->firstChildGetById(
        id, 
        doNested
      );

      if(nested)
        return nested;
    }
  }

  return nullptr;
}
//--------------------------------------------------------------------------------

EsString EsScriptParser::Node::stringExtract(const EsString& in) const ES_NOTHROW
{
  if(in.size() <= m_end)
    return EsString::null();

  if(EsScriptParser::strValId == m_id)
    return stringValExtract(
      in.substr(
        m_start, 
        (m_end - m_start)
      )
    );
  else
    return in.substr(
      m_start, 
      (m_end - m_start)
    );
}
//--------------------------------------------------------------------------------

EsString EsScriptParser::Node::stringValExtract(const EsString& str) const ES_NOTHROW
{
  ulong len = m_end - m_start;
  if(str.size() < len)
    return EsString::null();

  if(EsScriptParser::strValId != m_id)
    return str;

  EsString::value_type pfx = str[0];

  return EsString::fromString(
    (esT('"') != pfx) ?
      str.substr(1) :
      str,
    EsString::StrQuote,
    EsLocale::locale()
  );
}
//--------------------------------------------------------------------------------

const EsScriptParser::Node* EsScriptParser::Node::deepestNodeFindByPos(ulong pos) const ES_NOTHROW
{
  ES_DEBUG_TRACE(
    esT("Node::deepestNodeFindByPos(pos=%d)=> id=%d, m_start=%d, m_end=%d"),
    pos,
    m_id,
    m_start,
    m_end
  );

  if(m_start > pos || m_end < pos)
    return nullptr;

  for(size_t idx = 0; idx < m_children.size(); ++idx)
  {
    const Node& node = childGet(idx);
    const Node* found = node.deepestNodeFindByPos(pos);

    if(found)
      return found;
  }

  return this;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

EsScriptParser::EsScriptParser() :
  m_hashPrev(0),
  m_isParsed(false),
  m_stop(0)
{}
//--------------------------------------------------------------------------------

const EsScriptParser::Node* EsScriptParser::deepestNodeFindByPos(ulong pos) const ES_NOTHROW
{
  if(!m_isParsed)
    return nullptr;

  for(size_t idx = 0; idx < m_roots.size(); ++idx)
  {
    Node::PtrT root = m_roots[idx];
    ES_ASSERT(root);
    ES_ASSERT(root->isOk());

    ulong start, end;
    root->rangeGet(
      start,
      end
    );

    if(start <= pos && end >= pos)
    {
      if(root->haveChildren())
      {
        const EsScriptParser::Node* innerNode = root->deepestNodeFindByPos(pos);
        if(innerNode)
          return innerNode;
      }

      return root.get();
    }
  }

  return nullptr;
}
//--------------------------------------------------------------------------------

void EsScriptParser::reset()
{
  m_roots.clear();
  m_stop = 0;
  m_isParsed = false;
}
//--------------------------------------------------------------------------------