/// \file message_status.h
/// \brief Defines the serial_communicator::message_status enumeration.
#ifndef MESSAGE_STATUS_H
#define MESSAGE_STATUS_H

namespace serial_communicator {
///
/// \brief Enumerates the various message states.
///
enum class message_status
{
  QUEUED = 0,       ///< The message has been queued, but has not yet been sent.
  SENT = 1,         ///< The message has been sent, and no receipt was required.
  VERIFYING = 2,    ///< The message has been sent, and the communicator is verifying that the message was received.
  RECEIVED = 3,     ///< The message was sent, and was verified as received from the receiving communicator.
  NOTRECEIVED = 4   ///< The message was sent, but no verification was received.
};
}

#endif // MESSAGE_STATUS_H
