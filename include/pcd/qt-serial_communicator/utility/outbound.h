/// \file outbound.h
/// \brief Defines the serial_communicator::utility::outbound class.
#ifndef OUTBOUND_H
#define OUTBOUND_H

#include "pcd/qt-serial_communicator/message.h"
#include "pcd/qt-serial_communicator/message_status.h"

#include <chrono>

namespace serial_communicator {
namespace utility {
///
/// \brief Provides management of outbound messages.
///
class outbound
{
public:
    // CONSTRUCTORS
    ///
    /// \brief outbound Initializes a new outbound instance.
    /// \param message The outbound message.
    /// \param sequence_number The originating sequence number of the outbound message.
    /// \param receipt_required A flag indicating if receipt is required for the outbound message.
    /// \param tracker A tracker for external observation of an outgoing message's status.
    ///
    outbound(message* message, uint32_t sequence_number, bool receipt_required, message_status* tracker);
    ~outbound();

    // METHODS
    ///
    /// \brief mark_transmitted Instrucst the outgoing message that it has been transmitted.
    /// \details Call this method any time the message is transmitted.  It informs the instance
    /// to update counters and timestamps related to retransmission.
    ///
    void mark_transmitted();
    ///
    /// \brief update_status Updates the internal status and tracker to a new message status.
    /// \param status The new status to set.
    ///
    void update_status(message_status status);
    ///
    /// \brief timeout_elapsed Checks if a specified timeout has elapsed since the message was last transmitted.
    /// \param timeout The length of the timeout period in milliseconds.
    /// \return TRUE if the timeout has elapsed, otherwise FALSE.
    ///
    bool timeout_elapsed(uint32_t timeout) const;
    ///
    /// \brief can_retransmit Checks if the message can be retransmitted, or if it has reached its max transmissions.
    /// \param transmit_limit The maximum allowed transmissions of the message.
    /// \return TRUE if the message may be retransmitted, otherwise FALSE.
    ///
    bool can_retransmit(uint8_t transmit_limit) const;

    // PROPERTIES
    ///
    /// \brief p_message Gets a reference to the current outbound message.
    /// \return A const reference to the current outbound message.
    ///
    const message* p_message() const;
    ///
    /// \brief p_sequence_number Gets the originating sequence number of the outbound message.
    /// \return The originating sequence number of the outbound message.
    ///
    uint32_t p_sequence_number() const;
    ///
    /// \brief p_receipt_required Gets if the message is requiring a receipt from the receiver or not.
    /// \return TRUE if receipt is required, otherwise FALSE.
    ///
    bool p_receipt_required() const;
    ///
    /// \brief p_n_transmissions Gets the total number of transmissions of the message.
    /// \return The total number of transmissions of the message.
    ///
    unsigned char p_n_transmissions() const;
    ///
    /// \brief p_status Gets the current status of the message.
    /// \return The current status of the message.
    ///
    message_status p_status() const;

private:
    // VARIABLES
    ///
    /// \brief m_message Stores the outgoing message.
    ///
    message* m_message;
    ///
    /// \brief m_sequence_number Stores the originating sequence number of the outgoing message.
    ///
    uint32_t m_sequence_number;
    ///
    /// \brief m_receipt_required Stores the flag indicating if receipt is required for the outgoing message.
    ///
    bool m_receipt_required;
    ///
    /// \brief m_status Stores the current status of the message.
    ///
    message_status m_status;
    ///
    /// \brief m_tracker Stores a pointer to the tracker for external observation of the outgoing message's status.
    ///
    message_status* m_tracker;
    ///
    /// \brief m_transmit_timestamp Stores the last time in which the message was transmitted.
    ///
    std::chrono::high_resolution_clock::time_point m_transmit_timestamp;
    ///
    /// \brief m_n_transmissions Stores the total number of times the message has been transmitted.
    ///
    uint8_t m_n_transmissions;
};
}}

#endif // OUTBOUND_H
