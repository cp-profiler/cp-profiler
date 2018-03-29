#include "tree_merger.hh"
#include "../execution.hh"
#include "../tree/structure.hh"
#include "../core.hh"

#include "../utils/utils.hh"

#include <QStack>

namespace cpprofiler { namespace analysis {

    using namespace tree;

TreeMerger::TreeMerger(const Execution& ex_l_, const Execution& ex_r_, tree::NodeTree& nt)
    : ex_l(ex_l_), ex_r(ex_r_),
      tree_l(ex_l.tree()),
      tree_r(ex_r.tree()),
      res_tree(nt) {


    connect(this, &QThread::finished, this, &QObject::deleteLater);

}

TreeMerger::~TreeMerger() {

    qDebug() << "Merging: done";
}

static void find_and_replace_all(std::string& str, std::string substr_old, std::string substr_new) {
    auto pos = str.find(substr_old);
    while (pos != std::string::npos) {
        str.replace(pos, std::string(substr_old).length(), substr_new);
        pos = str.find(substr_old);
    }
}

static bool labelsEqual(std::string lhs, std::string rhs) {
    /// NOTE(maxim): removes whitespaces before comparing;
    /// this will be necessary as long as Chuffed and Gecode don't agree
    /// on whether to put whitespaces around operators (Gecode uses ' '
    /// for parsing logbrancher while Chuffed uses them as a delimiter
    /// between literals)

    // if (lhs.substr(0,3) == "[i]" || lhs.substr(0,3) == "[f]") {
    //   lhs = lhs.substr(3);
    // }

    // if (rhs.substr(0,3) == "[i]" || rhs.substr(0,3) == "[f]") {
    //   rhs = rhs.substr(3);
    // }

    lhs.erase(remove_if(lhs.begin(), lhs.end(), isspace), lhs.end());
    rhs.erase(remove_if(rhs.begin(), rhs.end(), isspace), rhs.end());

    find_and_replace_all(lhs, "==", "=");
    find_and_replace_all(rhs, "==", "=");

    // qDebug() << "comparing: " << lhs.c_str() << " " << rhs.c_str();

    if (lhs.compare(rhs) != 0) {
      return false;
    }

    return true;
}

static bool compareNodes(NodeID n1, const NodeTree& nt1,
                         NodeID n2, const NodeTree& nt2,
                         bool with_labels) {

  if (n1 == NodeID::NoNode || n2 == NodeID::NoNode) return false;

  if (nt1.childrenCount(n1) != nt2.childrenCount(n2)) return false;

  if (nt1.getStatus(n1) != nt2.getStatus(n2)) return false;

  if (with_labels) {
      const auto& label1 = nt1.getLabel(n1);
      const auto& label2 = nt2.getLabel(n2);
      if (!labelsEqual(label1, label2)) return false;
  }

  return true;
}


/// Copy the subtree rooted at nid_s of nt_s as a subtree rooted at nid in nt
static void copy_tree_into(NodeTree& nt, NodeID nid, const NodeTree& nt_s, NodeID nid_s) {

    QStack<NodeID> stack; QStack<NodeID> stack_s;
    stack.push(nid); stack_s.push(nid_s);

    while(stack_s.size() > 0) {
        auto node_s = stack_s.pop();
        auto node = stack.pop();

        auto kids = nt_s.childrenCount(node_s);
        auto status = nt_s.getStatus(node_s);

        /// TODO: make sure only the reference to a label is stored
        auto label = nt_s.getLabel(node_s);

        nt.transformNode(node, kids, status, label);

        for (auto alt = 0; alt < kids; ++alt) {
            stack.push( nt.getChild(node, alt) );
            stack_s.push( nt_s.getChild(node_s, alt) );
        }
    }


}

static void create_pentagon(NodeTree& nt, NodeID nid,
                const NodeTree& nt_l, NodeID nid_l,
                const NodeTree& nt_r, NodeID nid_r) {

    nt.transformNode(nid, 2, NodeStatus::MERGED);

    /// copy the subtree of nt_l into target_l
    auto target_l = nt.getChild(nid, 0);

    copy_tree_into(nt, target_l, nt_l, nid_l);

    /// copy the subtree of nt_r into target_r
    auto target_r = nt.getChild(nid, 1);

    copy_tree_into(nt, target_r, nt_r, nid_r);

}



void TreeMerger::run() {

    qDebug() << "Merging: running...";

    /// Must hold mutexes of both node trees
    auto& mutex_l = tree_l.treeMutex();
    auto& mutex_r = tree_r.treeMutex();

    /// Can this dead-lock?
    utils::MutexLocker locker_l(&mutex_l);
    utils::MutexLocker locker_r(&mutex_r);

    QStack<NodeID> stack_l, stack_r, stack;

    auto root_l = tree_l.getRoot();
    auto root_r = tree_r.getRoot();

    stack_l.push(root_l); stack_r.push(root_r);

    auto root = res_tree.createRootNew(0);

    stack.push(root);

    while(stack_l.size() > 0) {

        // utils::sleep_for_ms(300);

        auto node_l = stack_l.pop();
        auto node_r = stack_r.pop();
        auto target = stack.pop();

        bool equal = compareNodes(node_l, tree_l, node_r, tree_r, false);

        if (equal) {

            auto kids = tree_l.childrenCount(node_l);

            // dupNode(node_l, tree_l, target, res_tree);

            // res_tree.resetNumberOfChildren(target, kids);

            auto status = tree_l.getStatus(node_l);
            auto label = tree_l.getLabel(node_l);
            res_tree.transformNode(target, kids, status, label);

            for (auto i = kids - 1; i >= 0; --i) {
                stack_l.push( tree_l.getChild(node_l, i) );
                stack_r.push( tree_r.getChild(node_r, i) );
                stack.push( res_tree.getChild(target, i) );
            }

            qDebug() << "nodes equal";


        } else {
            qDebug() << "nodes not equal";


            create_pentagon(res_tree, target, tree_l, node_l, tree_r, node_r);



        }

    }



}


}}