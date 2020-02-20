#include "pcd/qt-serial_communicator/utility/inbound.h"

using namespace serial_communicator;
using namespace serial_communicator::utility;

// CONSTRUCTORS
inbound::inbound(message* message, uint32_t sequence_number)
{
    inbound::m_message = message;
    inbound::m_sequence_number = sequence_number;
}

// PROPERTIES
message* inbound::p_message() const
{
    return inbound::m_message;
}
uint32_t inbound::p_sequence_number() const
{
    return inbound::m_sequence_number;
}
