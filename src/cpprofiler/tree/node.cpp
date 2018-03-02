#include "node.hh"

#include <cassert>
#include <iostream>
#include <exception>
#include <QDebug>

namespace cpprofiler { namespace tree {

    Node::Tag Node::getTag() const {
        // obtain the right-most bit
        return static_cast<Tag>(reinterpret_cast<ptrdiff_t>(m_childrenOrFirstChild) & 3);
    }


    void Node::setTag(Tag tag) {
        auto itag = static_cast<int>(tag);
        m_childrenOrFirstChild = reinterpret_cast<void*>(
            (reinterpret_cast<ptrdiff_t>(m_childrenOrFirstChild) & ~(3)) | itag
        );
    }

    void Node::setParent(NodeID pid) {
        m_parent = pid;
    }

    int Node::getNumberOfChildren() const {

        auto tag = getTag();

        switch (tag) {
            case Tag::LEAF: return 0;
            case Tag::ONE_CHILD: return 1;
            case Tag::TWO_CHILDREN: return 2;
            default: return m_noOfChildren;
        }
    }

    /// TODO(maxim): make sure this is only called once...
    void Node::setNumberOfChildren(int n) {
        if (n < 0) throw std::exception();

        switch (n) {
            case 0: setTag(Tag::LEAF); break;
            case 1: setTag(Tag::ONE_CHILD); break;
            case 2: setTag(Tag::TWO_CHILDREN); break;
            default: {
                setTag(Tag::MORE_CHILDREN);
                m_noOfChildren = n;
                m_children = new NodeID[n];
            } break;
        }

    }

    void Node::resetNumberOfChildren(int n) {
        // can only change from 0 to 2 for now
        if (getNumberOfChildren() != 0) throw std::exception();

        if (n == 2) {
            setTag(Tag::TWO_CHILDREN);
        }

        if (n > 2) {
            setTag(Tag::MORE_CHILDREN);
            m_noOfChildren = n;
            m_children = new NodeID[n];
        }

    }

    Node::Node(NodeID parent_nid, int kids) : m_parent(parent_nid) {
        setNumberOfChildren(kids);
    }

    void Node::setChild(NodeID nid, int alt) {

        auto kids = getNumberOfChildren();

        if (kids <= 0) {
            throw no_child();
        } else if (kids <= 2) {
            if (alt == 0) {
                m_childrenOrFirstChild = 
                    reinterpret_cast<void*>(static_cast<int>(nid) << 2);
                    setNumberOfChildren(kids);
            } else {
                if (alt != 1) { throw std::exception(); }
                m_noOfChildren = -static_cast<int>(nid);
            }
        } else {
            m_children[alt] = nid;
        }

        auto kids2 = getNumberOfChildren();

        if (kids != kids2) throw;


    }

    NodeID Node::getChild(int alt) {
        auto kids = getNumberOfChildren();

        if (kids <= 0) {
            throw std::exception();
        } else if (kids <= 2) {
            if (alt == 0) {
                return static_cast<NodeID>(static_cast<int>
                    (reinterpret_cast<ptrdiff_t>(m_childrenOrFirstChild) >> 2));
            } else {
                return NodeID{-m_noOfChildren};
            }
        } else {
            return m_children[alt];
        }

    }

    NodeID Node::getParent() const {
        return m_parent;
    }

    Node::~Node() {
        if (getTag() == Tag::MORE_CHILDREN) {
            delete[] m_children;
        }
    }


}}