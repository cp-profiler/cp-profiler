#include "receiver_worker.hh"
#include "conductor.hh"
#include <iostream>
#include <string>
#include <thread>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

#include "execution.hh"

namespace cpprofiler {

    ReceiverWorker::ReceiverWorker(QTcpSocket& socket)
    : m_socket(socket)
    {

    }

    static int32_t ArrayToInt(const QByteArray& ba) {
        const char* p = ba.data();
        return *(reinterpret_cast<const quint32*>(p));
    }

    void ReceiverWorker::doRead() {

        // are there enough bytes to read something
        bool can_read_more = true;

        while (m_socket.bytesAvailable() > 0 || can_read_more) {

            if (m_socket.bytesAvailable() > 0) can_read_more = true;

            m_buffer.append(m_socket.readAll());

            // read the size of the next field if haven't already
            if (!m_state.size_read) {

                if (m_buffer.size() < m_state.bytes_read + FIELD_SIZE_NBYTES) {
                    /// can't read, need to wait for more bytes
                    can_read_more = false;
                    continue;
                }

                /// enough bytes to read size
                m_state.msg_size = ArrayToInt(m_buffer.mid(m_state.bytes_read, 4));
                m_state.bytes_read += 4;
                m_state.size_read = true;

            } else {

                if (m_buffer.size() < m_state.bytes_read + m_state.msg_size) {
                    /// can't read, need to wait for more bytes
                    can_read_more = false;
                    continue;
                }

                marshalling.deserialize(m_buffer.data() + m_state.bytes_read, m_state.msg_size);

                auto msg = marshalling.get_msg();
                handleMessage(msg);

                m_state.bytes_read += m_state.msg_size;
                m_state.size_read = false;

                m_state.msg_processed++;

                /// reset the buffer every MSG_PER_BUFFER messages
                if (m_state.msg_processed == MSG_PER_BUFFER) {
                    m_state.msg_processed = 0;
                    m_buffer.remove(0, m_state.bytes_read);
                    m_state.bytes_read = 0;
                }

            }
        }



    }

    void ReceiverWorker::handleStart(const Message& msg) {

        /// create new execution

        std::string execution_name = "<no name>";

        if (msg.has_info()) {

            auto info_bytes = QByteArray::fromStdString(msg.info());
            auto json_doc = QJsonDocument::fromJson(info_bytes);

            if (json_doc.isNull() || json_doc.isArray()) {
                std::cerr << "Can't parse json in info\n";
            } else {
                QJsonObject json_obj = json_doc.object();

                auto name_val = json_obj.value("name");

                if (name_val.isString()) {
                    execution_name = name_val.toString().toStdString();
                }
            }


        }

        std::cerr << "execution name: " << execution_name << std::endl;

        execution.reset(new Execution{});

        /// This uses a blocking connection
        emit newExecution(execution.get());



    }

    void ReceiverWorker::handleMessage(const Message& msg) {

        switch (msg.type()) {
            case cpprofiler::MsgType::NODE:
                std::cerr << "N";
                // execution->handleNewNode(msg);
            break;
            case cpprofiler::MsgType::START:
                std::cerr << "START\n";
                handleStart(msg);
            break;
            case cpprofiler::MsgType::DONE:
                // emit doneReceiving();
                std::cerr << "DONE\n";
            break;
            case cpprofiler::MsgType::RESTART:
                std::cerr << "RESTART\n";
            break;
            default:
                std::cerr << "unknown type\n";
        }

    }

}