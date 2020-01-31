#include "pcd/qt-serial_communicator/utility/inbound.h"

using namespace serial_communicator;
using namespace serial_communicator::utility;

// CONSTRUCTORS
inbound::inbound(message* message, unsigned int sequence_number)
{
    inbound::m_message = message;
    inbound::m_sequence_number = sequence_number;
}

// PROPERTIES
message* inbound::p_message() const
{
    return inbound::m_message;
}
unsigned int inbound::p_sequence_number() const
{
    return inbound::m_sequence_number;
}
