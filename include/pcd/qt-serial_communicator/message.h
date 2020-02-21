/// \file message.h
/// \brief Defines the serial_communicator::message class.
#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>

namespace serial_communicator {
///
/// \brief Represents a message that can be sent or recieved through the Serial Communicator.
///
class message
{
public:
    // CONSTRUCTORS
    ///
    /// \brief message Creates a new message that has no data fields.
    /// \param id The ID of the message.
    ///
    message(uint16_t id);
    ///
    /// \brief message Creates a new message with data fields.
    /// \param id The ID of the message.
    /// \param data_length The size of the data fields in bytes.
    ///
    message(uint16_t id, uint16_t data_length);
    ///
    /// \brief message Creates a message from a serialized byte array.
    /// \param byte_array The byte array to copy and create the message from.
    ///
    message(const uint8_t* byte_array);
    ~message();

    // METHODS
    template <typename T>
    ///
    /// \brief set_field Sets a data field in the message.
    /// \param address The address of the field to write to.
    /// \param data The data to write to the field at the specified address.
    ///
    void set_field(uint16_t address, T data);
    template <typename T>
    ///
    /// \brief get_field Gets a data field from the message.
    /// \param address The address of the field to read from.
    /// \return The data read from the field.
    ///
    T get_field(uint16_t address) const;
    ///
    /// \brief serialize Serializes the message into the given byte array.
    /// \param byte_array The byte array to serialize the message into.
    ///
    void serialize(uint8_t* byte_array) const;

    // PROPERTIES
    ///
    /// \brief p_id Gets the ID of the message.
    /// \return The ID of the message.
    ///
    uint16_t p_id() const;
    ///
    /// \brief p_priority Gets the priority of the message.
    /// \return The priority of the message.
    ///
    uint8_t p_priority() const;
    ///
    /// \brief p_data_length Gets the data length of the message in bytes.
    /// \return The data length of the message in bytes.
    ///
    uint16_t p_data_length() const;
    ///
    /// \brief p_message_length Gets the total length of the message in bytes.
    /// \return The total length of the message in bytes.
    ///
    uint32_t p_message_length() const;

private:
    // VARIABLES
    ///
    /// \brief m_id The message's ID.
    ///
    uint16_t m_id;
    ///
    /// \brief m_priority The message's priority.
    ///
    uint8_t m_priority;
    ///
    /// \brief m_data_length The message's data length, in bytes.
    ///
    uint16_t m_data_length;
    ///
    /// \brief m_data The message's data.
    ///
    uint8_t* m_data;

    // METHODS
    ///
    /// \brief set_field Sets a data field in the message.
    /// \param address The address of the field to write to.
    /// \param size The size of the data in bytes.
    /// \param data A void pointer to the data to write.
    ///
    void set_field(uint16_t address, uint32_t size, void* data);
    ///
    /// \brief get_field Gets a data field from the message.
    /// \param address The address of the field to read from.
    /// \param size The size of the data in bytes.
    /// \param data A void pointer to the output variable to read the data into.
    ///
    void get_field(uint16_t address, uint32_t size, void* data) const;
};
}

#endif // MESSAGE_H
