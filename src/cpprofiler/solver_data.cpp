#include "solver_data.hh"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace cpprofiler
{

    /// convert json array to std::vector
    static std::vector<int> parse_reasons_json(QJsonValue &json_arr)
    {
        std::vector<int> res;

        auto arr = json_arr.toArray();

        for (auto el : arr)
        {

            if (el.isDouble())
            {
                res.push_back(el.toInt());
            }
        }

        return res;
    }

    /// convert json array to std::vector
    static std::vector<SolverID> parse_nogoods_json(QJsonValue &json_arr)
    {
        std::vector<SolverID> res;

        auto arr = json_arr.toArray();

        for (auto el : arr)
        {

            if (!el.isArray())
            {
                const auto obj = el.toObject();
                /// object should be of the form {"nid":<>, "rid":<>, "tid":<>}

                auto nid_obj = obj.value("nid");
                auto rid_obj = obj.value("rid");
                auto tid_obj = obj.value("tid");

                if (nid_obj.isDouble() && rid_obj.isDouble() && tid_obj.isDouble()) {

                    const auto nid = nid_obj.toInt();
                    const auto rid = rid_obj.toInt();
                    const auto tid = tid_obj.toInt();

                    res.push_back({nid, rid, tid});
                }
            }
        }

        return res;
    }

    void SolverData::processInfo(NodeID nid, const std::string& info_str) {

        auto info_bytes = QByteArray::fromStdString(info_str);
        auto json_doc = QJsonDocument::fromJson(info_bytes);

        if (json_doc.isNull() || json_doc.isArray())
        {
            print("info not a json object");
        }
        else
        {
            QJsonObject json_obj = json_doc.object();
            auto reasons = json_obj.value("reasons");

            if (!reasons.isArray())
            {
                print("node {} has info: |{}|", nid, info_str);
                print("error: reasons is not an array!");
            }
            else
            {
                auto constraints = parse_reasons_json(reasons);
                contrib_cs_.insert({nid, std::move(constraints)});
            }

            auto nogoods_json = json_obj.value("nogoods");

            if (!nogoods_json.isArray())
            {
                print("error: nogoods is not an array!");
            }
            else
            {
                auto nogoods = parse_nogoods_json(nogoods_json);

                print("node {} has info: |{}|", nid, info_str);

                for (const auto sid : nogoods) {

                    print("sid: [{}, {}, {}]", sid.nid, sid.rid, sid.tid);
                    // auto nid = getNodeId(sid);
                    // print("nid: {}", nid);
                }

            }
        }

    }

    void IdMap::addPair(SolverID sid, tree::NodeID nid)
    {
        QWriteLocker locker(&m_lock);

        print("sid: [{}, {}, {}] nid: {}", sid.nid, sid.rid, sid.tid, nid);

        uid2id_.insert({sid, nid});
        nid2uid_.insert({nid, sid});
    }

    tree::NodeID IdMap::get(SolverID sid) const
    {
        QReadLocker locker(&m_lock);
        return uid2id_.at(sid);
    }

} // namespace cpprofiler