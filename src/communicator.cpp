#include "pcd/qt-serial_communicator/communicator.h"

#include <QDateTime>
#include <QThread>
#include <QCoreApplication>
#include <QtEndian>
#include <cstring>

using namespace serial_communicator;

// CONSTRUCTORS
communicator::communicator(QSerialPort *serial_port)
{
    // Set up the serial port.
    communicator::m_serial_port = serial_port;
    communicator::m_serial_port->flush();
    communicator::connect(communicator::m_serial_port, &QSerialPort::readyRead, this, &communicator::data_ready);
    communicator::m_escape_next = false;

    // Set up the spin timer.
    communicator::m_timer = new QTimer();
    communicator::connect(communicator::m_timer, &QTimer::timeout, this, &communicator::timer);
    communicator::m_timer->setInterval(20);
    communicator::m_timer->start();

    // Initialize parameters to default values.
    communicator::m_queue_size = 10;
    communicator::m_receipt_timeout = 100;
    communicator::m_max_transmissions = 5;

    // Initialize sequence counter.
    communicator::m_sequence_counter = 0;

    // Initialize queues.
    communicator::m_tx_queue = new utility::outbound*[communicator::m_queue_size];
    communicator::m_rx_queue = new utility::inbound*[communicator::m_queue_size];
    for(uint16_t i = 0; i < communicator::m_queue_size; i++)
    {
        communicator::m_tx_queue[i] = nullptr;
        communicator::m_rx_queue[i] = nullptr;
    }
}
communicator::~communicator()
{
    // Stop tx spin timer.
    communicator::m_timer->stop();
    delete communicator::m_timer;

    // Clean up queues.
    for(uint16_t i = 0; i < communicator::m_queue_size; i++)
    {
        if(communicator::m_tx_queue[i] != nullptr)
        {
            delete communicator::m_tx_queue[i];
        }
        if(communicator::m_rx_queue[i] != nullptr)
        {
            delete communicator::m_rx_queue[i];
        }
    }
    delete [] communicator::m_tx_queue;
    delete [] communicator::m_rx_queue;
}

// PUBLIC METHODS
bool communicator::send(message* message, bool receipt_required, message_status* tracker)
{
    // Find an open spot in the transmit queue.
    for(uint16_t i = 0; i < communicator::m_queue_size; i++)
    {
        if(communicator::m_tx_queue[i] == nullptr)
        {
            // Open space found. Add outbound message and increment sequence counter.
            communicator::m_tx_queue[i] = new utility::outbound(message, communicator::m_sequence_counter++, receipt_required, tracker);
            // Quit here.
            return true;
        }
    }

    // If this point reached, a spot was not found.
    delete message;
    return false;
}
uint16_t communicator::messages_available() const
{
    // Count and return total number of messages in receive queue.
    uint16_t n_messages = 0;
    for(uint16_t i = 0; i < communicator::m_queue_size; i++)
    {
        if(communicator::m_rx_queue[i] != nullptr)
        {
            n_messages++;
        }
    }
    return n_messages;
}
message* communicator::receive(uint16_t id)
{
    // Find a message with the matching ID that has the highest priority, followed by oldest age.
    utility::inbound* to_read = nullptr;
    uint16_t location = 0;

    for(uint16_t i = 0; i < communicator::m_queue_size; i++)
    {
        // Check if there is a valid message at this location.
        if(communicator::m_rx_queue[i] != nullptr)
        {
            // Store local reference to this message.
            utility::inbound* current = communicator::m_rx_queue[i];

            // Check if the message has a matching id.
            if(id == 0xFFFF || current->p_message()->p_id() == id)
            {
                // If to_read is currently empty, initialize it.
                if(to_read == nullptr)
                {
                    to_read = current;
                    location = i;
                }
                else
                {
                    // Check to see if the current message beats the to_read message in priority.
                    if(current->p_message()->p_priority() > to_read->p_message()->p_priority())
                    {
                        // Replace the to_read message with the current message.
                        to_read = current;
                        location = i;
                    }
                    // Otherwise, check if priorities are equal.
                    else if(current->p_message()->p_priority() == to_read->p_message()->p_priority())
                    {
                        // Compare age.
                        if(current->p_sequence_number() < to_read->p_sequence_number())
                        {
                            // Replace to_read message with current message.
                            to_read = current;
                            location = i;
                        }
                    }
                }

            }
        }
    }

    // Check if a message was actually found.
    if(to_read == nullptr)
    {
        return nullptr;
    }

    // If this point has been reached, a valid message has been found to read.

    // Extract the message from the inbound instance before it is deleted.
    message* output = to_read->p_message();

    // Remove the inbound entry from the receive queue.
    delete communicator::m_rx_queue[location];
    communicator::m_rx_queue[location] = nullptr;

    // Return the read message.
    return output;
}

// PUBLIC PROPERTIES
uint16_t communicator::p_queue_size()
{
    return communicator::m_queue_size;
}
void communicator::p_queue_size(uint16_t value)
{
    // Check if a resize is necessary.
    if(value != communicator::m_queue_size)
    {
        // Resize the queues.
        // Create new queues.
        utility::outbound** new_tx = new utility::outbound*[value];
        utility::inbound** new_rx = new utility::inbound*[value];

        // Copy current queues into new queues.
        for(uint16_t i = 0; i < communicator::m_queue_size; i++)
        {
            new_tx[i] = communicator::m_tx_queue[i];
            new_rx[i] = communicator::m_rx_queue[i];
        }

        // Fill any remaining space with nullptrs.
        for(uint16_t i = communicator::m_queue_size; i < value; i++)
        {
            new_tx[i] = nullptr;
            new_rx[i] = nullptr;
        }

        // Delete old queues and replace them.
        delete [] communicator::m_tx_queue;
        delete [] communicator::m_rx_queue;
        communicator::m_tx_queue = new_tx;
        communicator::m_rx_queue = new_rx;

        // Update the queue size variable.
        communicator::m_queue_size = value;
    }
}
uint32_t communicator::p_receipt_timeout()
{
    return communicator::m_receipt_timeout;
}
void communicator::p_receipt_timeout(uint32_t value)
{
    communicator::m_receipt_timeout = value;
}
uint8_t communicator::p_max_transmissions()
{
    return communicator::m_max_transmissions;
}
void communicator::p_max_transmissions(uint8_t value)
{
    communicator::m_max_transmissions = value;
}

// PRIVATE METHODS
void communicator::spin_tx()
{
    // Send the message with the highest priority or age.

    // First, find the message with the highest priority or age.
    utility::outbound* to_send = nullptr;
    uint16_t location = 0;
    for(uint16_t i = 0; i < communicator::m_queue_size; i++)
    {
        // Check if this address has a valid outbound message in it.
        if(communicator::m_tx_queue[i] != nullptr)
        {
            // Get local reference to the current message.
            utility::outbound* current = communicator::m_tx_queue[i];

            // Check if this outbound is actively awaiting for receipt (e.g. hasn't yet timed out)
            if(current->p_status() == message_status::VERIFYING && current->timeout_elapsed(communicator::m_receipt_timeout) == false)
            {
                // Message timeout hasn't expired for receipt yet, so no need to resend at this time.
                // Skip this message.
                continue;
            }

            // Check if to_send has a value yet.
            if(to_send == nullptr)
            {
                // Update to_send with current value.
                to_send = current;
                location = i;
            }
            else
            {
                // Check if current message has greater priority.
                if(current->p_message()->p_priority() > to_send->p_message()->p_priority())
                {
                    // Update to_send with current value.
                    to_send = current;
                    location = i;
                }
                // Check if current message has equal priority.
                else if(current->p_message()->p_priority() == to_send->p_message()->p_priority())
                {
                    // Check if current message has older age.
                    if(current->p_sequence_number() < to_send->p_sequence_number())
                    {
                        // Update to_send with current value.
                        to_send = current;
                        location = i;
                    }
                }
            }
        }
    }

    // Check that a message was actually found to send.
    if(to_send == nullptr)
    {
        return;
    }

    // At this point, to_send contains the appropriate message to send.
    // Check if this is the first time the message is being sent.
    if(to_send->p_n_transmissions() == 0)
    {
        // Message has not been sent yet.
        // Send the message.
        communicator::tx(to_send);
        // Check if receipt is required.
        if(to_send->p_receipt_required())
        {
            // Receipt is required.
            // Leave in the tx queue and update status.
            to_send->update_status(message_status::VERIFYING);
        }
        else
        {
            // Receipt is not required.
            // Update status to sent and delete from queue.
            to_send->update_status(message_status::SENT);
            delete communicator::m_tx_queue[location];
            communicator::m_tx_queue[location] = nullptr;
        }
    }
    else
    {
        // Message has been sent at least once and has timed out waiting for a receipt.
        // Check if message can be resent.
        if(to_send->can_retransmit(communicator::m_max_transmissions))
        {
            // Message can be resent.
            communicator::tx(to_send);
        }
        else
        {
            // Message has already been sent the maximum number of times.
            // Update status and delete.
            to_send->update_status(message_status::NOTRECEIVED);
            delete communicator::m_tx_queue[location];
            communicator::m_tx_queue[location] = nullptr;
        }
    }
}
void communicator::spin_rx()
{
    // Pop bytes until the header byte is found.
    bool header_found = false;
    while(!communicator::m_serial_buffer.empty() && header_found == false)
    {
        if(communicator::m_serial_buffer.front() == communicator::m_header_byte)
        {
            header_found = true;
        }
        else
        {
            communicator::m_serial_buffer.pop_front();
        }
    }

    // Check if header was found.
    if(!header_found)
    {
        // No valid header found, quit.
        return;
    }

    // Start packet size tracking.
    // Initialize with 1 header, 4 sequence, 1 receipt, 2 message id, 1 priority, 2 data length.
    uint32_t packet_length = 11;

    // If this point reached, a valid header has been found.
    // Message data length is needed.
    if(communicator::m_serial_buffer.size() < packet_length)
    {
        return;
    }
    // Read bytes 9 and 10 to get the data length.
    uint8_t data_length_bytes[2];
    data_length_bytes[0] = communicator::m_serial_buffer[9];
    data_length_bytes[1] = communicator::m_serial_buffer[10];

    // Extract the data length from the end of packet_front.
    uint16_t data_length = qFromBigEndian(*reinterpret_cast<uint16_t*>(&data_length_bytes));

    // Finalize packet size with data length and checksum.
    packet_length += data_length + 1;

    // Check if packet length exists in the buffer.
    if(communicator::m_serial_buffer.size() < packet_length)
    {
        return;
    }

    // Create packet array.
    uint8_t* packet = new uint8_t[packet_length];
    // Read packet from buffer.
    for(uint32_t i = 0; i < packet_length; ++i)
    {
        packet[i] = communicator::m_serial_buffer.front();
        communicator::m_serial_buffer.pop_front();
    }

    // If this point is reached, a full packet has been read.

    // Validate the checksum.
    bool checksum_ok = packet[packet_length-1] == communicator::checksum(packet, packet_length-1);
    // Extract sequence number from the packet.
    uint32_t sequence_number = qFromBigEndian(*reinterpret_cast<uint32_t*>(&packet[1]));

    // Handle receipts
    switch(static_cast<communicator::receipt_type>(packet[5]))
    {
    case communicator::receipt_type::NOT_REQUIRED:
    {
        // Do nothing
        break;
    }
    case communicator::receipt_type::REQUIRED:
    {
        // Draft and send a receipt message outside of the typical outbound/tx_queue.
        // Receipt messages do not need to be tracked.
        uint8_t receipt[12];
        // Copy header(1), sequence(4), receipt(1), id(2), and priority(1) back into receipt.  Then add zero data length (2) and checksum (1).
        std::memcpy(receipt, packet, 9);
        // Update the receipt field.
        if(checksum_ok)
        {
            receipt[5] = static_cast<uint8_t>(communicator::receipt_type::RECEIVED);
        }
        else
        {
            receipt[5] = static_cast<uint8_t>(communicator::receipt_type::CHECKSUM_MISMATCH);
        }
        // No data fields.
        receipt[9] = 0;
        receipt[10] = 0;
        // Set checksum.
        receipt[11] = communicator::checksum(receipt, 11);
        // Write message.
        communicator::tx(receipt, 12);
        break;
    }
    case communicator::receipt_type::RECEIVED:
    {
        // If checksum is ok, remove the associated message from the TXQ if it is still in there.
        if(checksum_ok)
        {
            for(uint16_t i = 0; i < communicator::m_queue_size; i++)
            {
                if(communicator::m_tx_queue[i] != nullptr)
                {
                    utility::outbound* current = communicator::m_tx_queue[i];
                    if(current->p_sequence_number() == sequence_number)
                    {
                        // Update the message's status.
                        current->update_status(message_status::RECEIVED);
                        // Remove it from the queue.
                        delete communicator::m_tx_queue[i];
                        communicator::m_tx_queue[i] = nullptr;
                        // Quit the for loop.
                        break;
                    }
                }
            }
        }
        break;
    }
    case communicator::receipt_type::CHECKSUM_MISMATCH:
    {
        // Find the associated message based on sequence number and immediately resend it.
        if(checksum_ok)
        {
            for(uint16_t i = 0; i < communicator::m_queue_size; i++)
            {
                if(communicator::m_tx_queue[i] != nullptr)
                {
                    utility::outbound* current = communicator::m_tx_queue[i];
                    if(current->p_sequence_number() == sequence_number)
                    {
                        // Check if message can be resent.
                        if(current->can_retransmit(communicator::m_max_transmissions))
                        {
                            // Message can be resent.
                            communicator::tx(current);
                        }
                        else
                        {
                            // Message has already been sent the maximum number of times.
                            // Update status and delete.
                            current->update_status(message_status::NOTRECEIVED);
                            delete communicator::m_tx_queue[i];
                            communicator::m_tx_queue[i] = nullptr;
                        }
                        // Quit the for loop.
                        break;
                    }
                }
            }
        }
        break;
    }
    }

    // Lastly, put packet into inbound message in the rx_queue.
    if(checksum_ok)
    {
        // Find an open position in the RXQ.
        for(uint16_t i = 0; i < communicator::m_queue_size; i++)
        {
            if(communicator::m_rx_queue[i] == nullptr)
            {
                // Extract the message from the packet.
                message* msg = new message(&packet[6]);
                // Add new inbound to the rx_queue.
                communicator::m_rx_queue[i] = new utility::inbound(msg, sequence_number);

                // Exit for loop.
                break;
            }
        }
    }

    // Delete the packet.
    delete [] packet;
}
void communicator::tx(utility::outbound* message)
{
    // Serialize the packet without escapes.
    // First, get total packet length = message length + 7 (1 header, 4 sequence, 1 receipt, 1 checksum)
    uint32_t packet_size = message->p_message()->p_message_length() + 7;
    // Create packet.
    uint8_t* packet = new uint8_t[packet_size];
    // Write the header, sequence, and receipt.
    packet[0] = communicator::m_header_byte;
    uint32_t be_sequence = qToBigEndian(message->p_sequence_number());
    std::memcpy(&packet[1], &be_sequence, 4);
    packet[5] = message->p_receipt_required();
    // Write the message bytes.
    message->p_message()->serialize(&packet[6]);
    // Calculate and add CRC.
    packet[packet_size-1] = communicator::checksum(packet, packet_size - 1);

    // Write to the serial port.
    communicator::tx(packet, packet_size);

    // Mark that the message has been sent.
    message->mark_transmitted();

    // Delete the packet.
    delete [] packet;
}
void communicator::tx(uint8_t* buffer, uint32_t length)
{
    // Check if escapes are needed.
    uint32_t n_escapes = 0;
    // Only check after the header.
    for(uint32_t i = 1; i < length; i++)
    {
        if(buffer[i] == communicator::m_header_byte || buffer[i] == communicator::m_escape_byte)
        {
            n_escapes++;
        }
    }

    // Handle escapes.
    if(n_escapes > 0)
    {
        // Escapes needed.
        uint8_t* esc_buffer = new uint8_t[length + n_escapes];
        uint32_t esc_write_position = 0;
        // Copy the header byte first since it should not be escaped.
        esc_buffer[esc_write_position++] = buffer[0];
        // Only check after the header.
        for(uint32_t i = 1; i < length; i++)
        {
            if(buffer[i] == communicator::m_header_byte || buffer[i] == communicator::m_escape_byte)
            {
                // Insert escape.
                esc_buffer[esc_write_position++] = communicator::m_escape_byte;
                // Copy in byte decremented by one.
                esc_buffer[esc_write_position++] = buffer[i] - 1;
            }
            else
            {
                // Copy buffer byte in.
                esc_buffer[esc_write_position++] = buffer[i];
            }
        }

        // Write the escaped buffer.
        communicator::m_serial_port->write((char*)esc_buffer, length + n_escapes);
        // Delete the escaped buffer.
        delete [] esc_buffer;
    }
    else
    {
        // Escapes not needed.  Write buffer as is.
        communicator::m_serial_port->write((char*)buffer, length);
    }
    // Since writing is asynchronous, wait for the bytes to begin writing.
    communicator::m_serial_port->waitForBytesWritten(-1);
}
uint8_t communicator::checksum(uint8_t* data, uint32_t length)
{
    uint8_t checksum = 0;
    for(uint32_t i = 0 ; i < length; i++)
    {
        checksum ^= data[i];
    }
    return checksum;
}
uint64_t communicator::serial_read(uint8_t *buffer, uint32_t length, uint32_t timeout_ms)
{
    // Wait until number of bytes requested is available or timeout occurs.
    // Grab starting timestamp.
    uint64_t start_time = QDateTime::currentMSecsSinceEpoch();
    // Initialize bytes_available.
    uint64_t bytes_available = communicator::m_serial_port->bytesAvailable();
    // Wait while checking timeout.
    while(bytes_available < length)
    {
        // Check if timeout has occured.
        if(QDateTime::currentMSecsSinceEpoch() - start_time > timeout_ms)
        {
            // Quite the while loop.
            break;
        }

        // Sleep to wait for more bytes.
        QThread::usleep(100);

        // Update bytes available.
        bytes_available = communicator::m_serial_port->bytesAvailable();
    }

    // If this point is reached, either enough bytes are available or timeout has occured.
    // Read the smaller of length or bytes_available and return bytes read.
    return communicator::m_serial_port->read((char*)(buffer), qMin(static_cast<uint64_t>(length), bytes_available));
}

// PRIVATE SLOTS
void communicator::timer()
{
    communicator::spin_tx();
    communicator::spin_rx();
}
void communicator::data_ready()
{
    // Read the new data from the port.
    QByteArray new_data = communicator::m_serial_port->readAll();

    // Add to the internal buffer, handling escapes.
    for(auto current_byte = new_data.cbegin(); current_byte != new_data.cend(); ++current_byte)
    {
        // Check for escape byte.
        if(*current_byte == communicator::m_escape_byte)
        {
            // Mark next byte as escaped.
            communicator::m_escape_next = true;
        }
        else
        {
            // Add byte to buffer with escape.
            communicator::m_serial_buffer.push_back(*current_byte + static_cast<uint8_t>(communicator::m_escape_next));
            // Mark next byte as not escaped.
            communicator::m_escape_next = false;
        }
    }
}
