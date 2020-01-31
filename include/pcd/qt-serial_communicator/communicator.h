/// \file communicator.h
/// \brief Defines the serial_communicator::communicator class.
#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include "message.h"
#include "message_status.h"
#include "utility/outbound.h"
#include "utility/inbound.h"

#include <QtSerialPort/QSerialPort>

///
/// \brief Includes all software for implementing the serial_communicator.
///
namespace serial_communicator {
///
/// \brief A communicator for transmitting and receiving messages via serial.
///
class communicator
{
public:
    // CONSTRUCTORS
    ///
    /// \brief communicator Creates a new communicator instance.
    /// \param port The serial port to communicate over.
    /// \param baud The baud rate of the serial connection.
    ///
    communicator(QSerialPort* serial_port);
    ~communicator();

    // METHODS
    ///
    /// \brief send Sends a message by adding it to the communicator's transmit queue.
    /// \param message The message to send. The communicator takes ownership of the pointer.
    /// \param receipt_required OPTIONAL Indicates that the message should be retransmitted until a receipt is
    /// received from the receiver, or the maximum amount of transmissions has been reached.
    /// \param tracker OPTIONAL A pointer that allows external code to monitor the status of a message in real time.
    /// \return Returns TRUE if the message was successfully placed in the transmit queue, otherwise FALSE.
    /// \details This places a message into the TX queue for sending.  The communicator sends messages from the queue
    /// based on highest priority, followed by oldest.  The calling code can keep track of the message's status
    /// using the Tracker parameter.  The Communicator will update the Tracker pointer as the message's status
    /// changes.  Once placed in the queue, the message's status is set to QUEUED.
    ///
    bool send(message* message, bool receipt_required = false, message_status* tracker = nullptr);
    ///
    /// \brief messages_available Gets the total number of messages available to read from the receive queue.
    /// \return The number of available messages to read.
    ///
    unsigned short messages_available() const;
    ///
    /// \brief receive Grabs a message from the receive queue.
    /// \param id OPTIONAL The ID of the message to read. Defaults to 0xFFFF, which will grab the next available message.
    /// \return A pointer to the received message. The calling code takes ownership of the message pointer.
    /// \details Messages are always returned by highest priority, followed by oldest in age.
    ///
    message* receive(unsigned short id = 0xFFFF);
    ///
    /// \brief spin Performs a single spin of the communicator's internal duties.
    /// \note This should be called at a constant rate within the main loop of external code.
    /// \details A single spin operation will only attempt to send and received one message. This is to prevent the spin
    /// method from severely blocking the main loop of the external code.
    ///
    void spin();

    // PROPERTIES
    ///
    /// \brief p_queue_size Gets the size of the transmit and receive buffers, in number of messages.
    /// \return The size of the transmit and receive buffers, in number of messages.
    /// \note The default size is 10 messages for each buffer.
    /// \details If the internal transmit or receive queues become full, they will not allow any more
    /// messages to be enqueued until space opens up from a spin() call.  These queue sizes ensure that
    /// the system's memory does not fill up.
    ///
    unsigned short p_queue_size();
    ///
    /// \brief p_queue_size Sets the size of the transmit and receive buffers, in number of messages.
    /// \param value The size of the transmit and receive buffers, in number of messages.
    /// \note The default size is 10 messages for each buffer.
    /// \details If the internal transmit or receive queues become full, they will not allow any more
    /// messages to be enqueued until space opens up from a spin() call.  These queue sizes ensure that
    /// the system's memory does not fill up.
    ///
    void p_queue_size(unsigned short value);
    ///
    /// \brief p_receipt_timeout Gets the receipt timeout in milliseconds.
    /// \return The receipt timeout in milliseconds.
    /// \details When a message is sent with receipt required, the transmittnig communicator will
    /// wait for the specified timeout to receive a receipt message from the receiving
    /// communicator.  If the timeout elapses without getting a receipt, the communicator will
    /// then attempt to retransmit the message and repeat this process until either a receipt is
    /// received or the maximum number of transmissions has been reached.
    /// \note The default value is 100ms.
    ///
    unsigned int p_receipt_timeout();
    ///
    /// \brief p_receipt_timeout Sets the receipt timeout in milliseconds.
    /// \param value The receipt timeout in milliseconds.
    /// \details When a message is sent with receipt required, the transmittnig communicator will
    /// wait for the specified timeout to receive a receipt message from the receiving
    /// communicator.  If the timeout elapses without getting a receipt, the communicator will
    /// then attempt to retransmit the message and repeat this process until either a receipt is
    /// received or the maximum number of transmissions has been reached.
    /// \note The default value is 100ms.
    ///
    void p_receipt_timeout(unsigned int value);
    ///
    /// \brief p_max_transmissions Gets the maximum number of times a message may be transmitted.
    /// \return The maximum number of times a message may be transmitted.
    /// \details When a message is sent with a receipt required, the communicator will wait for
    /// a receipt from the message's endpoint.  If no receipt is received, the communicator will
    /// retransmit the message until the maximum transmissions is reached before giving up.  If the
    /// maximum number of retries is reached without getting a receipt, the message's tracker
    /// status will be set to NOTRECEIVED.
    /// \note The default value is 5 transmissions.
    ///
    unsigned char p_max_transmissions();
    ///
    /// \brief p_max_transmissions Gets the maximum number of times a message may be transmitted.
    /// \param value The maximum number of times a message may be transmitted.
    /// \details When a message is sent with a receipt required, the communicator will wait for
    /// a receipt from the message's endpoint.  If no receipt is received, the communicator will
    /// retransmit the message until the maximum transmissions is reached before giving up.  If the
    /// maximum number of retries is reached without getting a receipt, the message's tracker
    /// status will be set to NOTRECEIVED.
    /// \note The default value is 5 transmissions.
    ///
    void p_max_transmissions(unsigned char value);

private:
    // ENUMERATIONS
    ///
    /// \brief Enumerates the types of the message's receipt field.
    ///
    enum class receipt_type
    {
        NOT_REQUIRED = 0,       ///< In a transmitted message, indicates that no receipt is required from the receiver.
        REQUIRED = 1,           ///< In a transmitted message, indicates that a receipt is required from the receiver.
        RECEIVED = 2,           ///< In a receipt message, indicates that the message was properly received.
        CHECKSUM_MISMATCH = 3   ///< In a receipt message, indicates that the message was received, but the checksum did not match.
    };

    // CONSTANTS
    ///
    /// \brief m_header_byte Stores the message header byte.
    ///
    const unsigned char m_header_byte = 0xAA;
    ///
    /// \brief m_escape_byte Stores the message escape byte.
    ///
    const unsigned char m_escape_byte = 0x1B;

    // PARAMETERS
    ///
    /// \brief m_queue_size Stores the size of the transmit/receive queues, in messages.
    ///
    unsigned short m_queue_size;
    ///
    /// \brief m_receipt_timeout Stores the receipt timeout in milliseconds.
    ///
    unsigned int m_receipt_timeout;
    ///
    /// \brief m_max_transmissions Stores the maximum amount of transmissions for one message.
    ///
    unsigned char m_max_transmissions;

    // VARIABLES
    ///
    /// \brief m_serial_port The communicator's serial port.
    ///
    QSerialPort* m_serial_port;
    ///
    /// \brief m_sequence_counter Stores the current sequence number for assigning unique and monotonic sequence IDs to messages.
    ///
    unsigned int m_sequence_counter;

    // QUEUES
    ///
    /// \brief m_tx_queue The internal transmit queue.
    ///
    utility::outbound** m_tx_queue;
    ///
    /// \brief m_rx_queue The internal receive queue.
    ///
    utility::inbound** m_rx_queue;

    // METHODS
    ///
    /// \brief spin_tx Conducts the transmit duties during a spin cycle.
    ///
    void spin_tx();
    ///
    /// \brief spin_rx Conducts the receive duties during a spin cycle.
    ///
    void spin_rx();
    ///
    /// \brief tx Serializes a message and writes it to the serial buffer.
    /// \param message The message to write.
    ///
    void tx(utility::outbound* message);
    ///
    /// \brief tx Writes data to a serial buffer with proper escapement.
    /// \param buffer The buffer of unescaped packet bytes to escape and send.
    /// \param length The length of the unescaped packet buffer.
    ///
    void tx(unsigned char* buffer, unsigned int length);
    ///
    /// \brief rx Reads a specified amount of bytes from the serial buffer.
    /// \param buffer The pre-allocated buffer to store read bytes in.
    /// \param length The number of bytes to read.
    /// \return TRUE if length bytes was read successfully, otherwise FALSE (a.k.a. timeout)
    /// \details This method will read bytes from the buffer and correct for escape bytes.
    ///
    bool rx(unsigned char* buffer, unsigned int length);
    ///
    /// \brief checksum Calculates the XOR checksum of the provided data array.
    /// \param data The data to calculate the checksum for.
    /// \param length The length of the data array.
    /// \return The XOR checksum of the data.
    ///
    unsigned char checksum(unsigned char* data, unsigned int length);
    ///
    /// \brief serial_read Conducts a read operation on the serial port.
    /// \param buffer The buffer to read the data into.
    /// \param length The length of bytes to read.
    /// \param timeout_ms The timeout for the read operation, in milliseconds.
    /// \return The number of bytes actually read into the buffer.
    ///
    unsigned long serial_read(unsigned char* buffer, unsigned int length, unsigned int timeout_ms = 30);
};
}

#endif // COMMUNICATOR_H
