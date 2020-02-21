#include "pcd/qt-serial_communicator/message.h"

#include <QtEndian>
#include <cstring>

using namespace serial_communicator;

// CONSTRUCTORS
message::message(uint16_t id)
{
    message::m_id = id;
    message::m_priority = 0;
    message::m_data_length = 0;
    message::m_data = nullptr;
}
message::message(uint16_t id, uint16_t data_length)
{
    message::m_id = id;
    message::m_priority = 0;
    message::m_data_length = data_length;
    message::m_data = new uint8_t[data_length];
}
message::message(const uint8_t* byte_array)
{
    // Deserialize the message from the byte array.
    // Read the ID.
    message::m_id = qFromBigEndian(*reinterpret_cast<const uint16_t*>(&byte_array[0]));
    // Read the priority.
    message::m_priority = byte_array[2];
    // Read the data length.
    message::m_data_length = qFromBigEndian(*reinterpret_cast<const uint16_t*>(&byte_array[3]));
    // Read the data.
    message::m_data = new uint8_t[message::m_data_length];
    std::memcpy(message::m_data, &byte_array[5], message::m_data_length);
}
message::~message()
{
    // Clean up data array.
    if(message::m_data)
    {
        delete [] message::m_data;
    }
}

// METHODS
template <typename T>
void message::set_field(uint16_t address, T data)
{
    message::set_field(address, sizeof(data), &data);
}
template void message::set_field<uint8_t>(uint16_t address, uint8_t data);
template void message::set_field<int8_t>(uint16_t address, int8_t data);
template void message::set_field<uint16_t>(uint16_t address, uint16_t data);
template void message::set_field<int16_t>(uint16_t address, int16_t data);
template void message::set_field<uint32_t>(uint16_t address, uint32_t data);
template void message::set_field<int32_t>(uint16_t address, int32_t data);
template void message::set_field<uint64_t>(uint16_t address, uint64_t data);
template void message::set_field<int64_t>(uint16_t address, int64_t data);
template void message::set_field<float>(uint16_t address, float data);
template void message::set_field<double>(uint16_t address, double data);

void message::set_field(uint16_t address, uint32_t size, void *data)
{
    switch(size)
    {
    case 1:
    {
        message::m_data[address] = *static_cast<uint8_t*>(data);
        break;
    }
    case 2:
    {
        uint16_t value = qToBigEndian(*static_cast<uint16_t*>(data));
        std::memcpy(&message::m_data[address], & value, 2);
        break;
    }
    case 4:
    {
        uint32_t value = qToBigEndian(*static_cast<uint32_t*>(data));
        std::memcpy(&message::m_data[address], & value, 4);
        break;
    }
    case 8:
    {
        uint64_t value = qToBigEndian(*static_cast<quint64*>(data));
        std::memcpy(&message::m_data[address], & value, 8);
        break;
    }
    }
}

template <typename T>
T message::get_field(uint16_t address) const
{
    T output;
    message::get_field(address, sizeof(output), &output);
    return output;
}
template uint8_t message::get_field<uint8_t>(uint16_t address) const;
template int8_t message::get_field<int8_t>(uint16_t address) const;
template uint16_t message::get_field<uint16_t>(uint16_t address) const;
template int16_t message::get_field<int16_t>(uint16_t address) const;
template uint32_t message::get_field<uint32_t>(uint16_t address) const;
template int32_t message::get_field<int32_t>(uint16_t address) const;
template uint64_t message::get_field<uint64_t>(uint16_t address) const;
template int64_t message::get_field<int64_t>(uint16_t address) const;
template float message::get_field<float>(uint16_t address) const;
template double message::get_field<double>(uint16_t address) const;

void message::get_field(uint16_t address, uint32_t size, void *data) const
{
    switch(size)
    {
    case 1:
    {
        *static_cast<uint8_t*>(data) = static_cast<uint8_t>(message::m_data[address]);
        break;
    }
    case 2:
    {
        uint16_t value = qFromBigEndian(*reinterpret_cast<uint16_t*>(&message::m_data[address]));
        std::memcpy(data, &value, 2);
        break;
    }
    case 4:
    {
        uint32_t value = qFromBigEndian(*reinterpret_cast<uint32_t*>(&message::m_data[address]));
        std::memcpy(data, &value, 4);
        break;
    }
    case 8:
    {
        uint64_t value = qFromBigEndian(*reinterpret_cast<quint64*>(&message::m_data[address]));
        std::memcpy(data, &value, 8);
        break;
    }
    }
}

void message::serialize(uint8_t *byte_array) const
{
    // Serialize the ID first.
    uint16_t be_id = qToBigEndian(message::m_id);
    std::memcpy(&byte_array[0], &be_id, 2);
    // Serialize priority.
    byte_array[2] = message::m_priority;
    // Serialize data length.
    uint16_t be_data_length = qToBigEndian(message::m_data_length);
    std::memcpy(&byte_array[3], &be_data_length, 2);
    // Copy in data, as it is already guaranteed big endian.
    std::memcpy(&byte_array[5], message::m_data, message::m_data_length);
}


// PROPERTIES
uint16_t message::p_id() const
{
    return message::m_id;
}
uint8_t message::p_priority() const
{
    return message::m_priority;
}
uint16_t message::p_data_length() const
{
    return message::m_data_length;
}
uint32_t message::p_message_length() const
{
    return message::m_data_length + 5;
}
