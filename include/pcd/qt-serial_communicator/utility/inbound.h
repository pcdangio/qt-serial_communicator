/// \file inbound.h
/// \brief Defines the serial_communicator::utility::inbound class.
#ifndef INBOUND_H
#define INBOUND_H

#include "pcd/qt-serial_communicator/message.h"

namespace serial_communicator {
///
/// \brief Includes utility software for the SerialCommunicator.
///
namespace utility {
///
/// \brief Provides management of inbound messages.
///
class inbound
{
public:
    // CONSTRUCTORS
    ///
    /// \brief inbound Creates a new inbound instance.
    /// \param message A pointer to the received message.
    /// \param sequence_number The originating sequence number of the received message.
    /// \details This instance takes ownership of the message pointer.
    ///
    inbound(message* message, unsigned int sequence_number);

    // PROPERTIES
    ///
    /// \brief p_message Gets a pointer to the received message.
    /// \return A pointer to the received message.
    ///
    message* p_message() const;
    ///
    /// \brief p_sequence_number Gets the originiating sequence number of the received message.
    /// \return The originating sequence number of the received message.
    ///
    unsigned int p_sequence_number() const;

private:
    ///
    /// \brief m_message Stores a pointer to the recieved message.
    ///
    message* m_message;
    ///
    /// \brief m_sequence_number Stores the originating sequence number of the received message.
    ///
    unsigned int m_sequence_number;
};

}}

#endif // INBOUND_H
