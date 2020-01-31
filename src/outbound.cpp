#include "pcd/qt-serial_communicator/utility/outbound.h"

using namespace serial_communicator;
using namespace serial_communicator::utility;

outbound::outbound(message* message, unsigned int sequence_number, bool receipt_required, message_status* tracker)
{
    // Store locals.
    outbound::m_message = message;
    outbound::m_sequence_number = sequence_number;
    outbound::m_receipt_required = receipt_required;
    outbound::m_tracker = tracker;

    // Initialize counters.
    outbound::m_transmit_timestamp = std::chrono::high_resolution_clock::now();
    outbound::m_n_transmissions = 0;

    // Set status to queued.
    outbound::update_status(message_status::QUEUED);
}
outbound::~outbound()
{
    delete outbound::m_message;
}

// METHODS
void outbound::mark_transmitted()
{
    // Update transmission timestamp.
    outbound::m_transmit_timestamp = std::chrono::high_resolution_clock::now();
    // Increment transmission counter.
    outbound::m_n_transmissions++;
}
void outbound::update_status(message_status status)
{
    // Update internal status.
    outbound::m_status = status;
    // Update tracker if available.
    if(outbound::m_tracker)
    {
        *outbound::m_tracker = status;
    }
}
bool outbound::timeout_elapsed(unsigned int timeout) const
{
    // Check if the given timeout has been elapsed.
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - outbound::m_transmit_timestamp).count() > timeout;
}
bool outbound::can_retransmit(unsigned char transmit_limit) const
{
    return outbound::m_n_transmissions < transmit_limit;
}

// PROPERTIES
const message* outbound::p_message() const
{
    return outbound::m_message;
}
unsigned int outbound::p_sequence_number() const
{
    return outbound::m_sequence_number;
}
bool outbound::p_receipt_required() const
{
    return outbound::m_receipt_required;
}
unsigned char outbound::p_n_transmissions() const
{
    return outbound::m_n_transmissions;
}
message_status outbound::p_status() const
{
    return outbound::m_status;
}
