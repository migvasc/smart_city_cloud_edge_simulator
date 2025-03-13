#include "WriteBuffer.hpp"
#include <iosfwd>
#include <algorithm>
#include <fstream>
#include <random>
#include <boost/algorithm/string/join.hpp>
#include <stdlib.h>
#include <xbt.h>
#include <math.h>
#include <float.h>


WriteBuffer::WriteBuffer(const std::string & filename, size_t buffer_size)
    : buffer_size(buffer_size)
{
    xbt_assert(buffer_size > 0, "Invalid buffer size (%zu)", buffer_size);
    buffer = new char[buffer_size];
    file_path = filename;
    f.open(filename, std::ios_base::trunc);    
    xbt_assert(f.is_open(), "Cannot write file '%s'", filename.c_str());
}

WriteBuffer::~WriteBuffer()
{
    if (buffer_pos > 0)
    {
        flush_buffer();
    }

    if (buffer != nullptr)
    {
        delete[] buffer;
        buffer = nullptr;

        f.close();
    }
}


void WriteBuffer::append_text(const char * text)
{
    // Buffer is already closed
    if(buffer == nullptr) return;
    const size_t text_length = strlen(text);

    // Is the buffer big enough?
    if (buffer_pos + text_length < buffer_size)
    {
        // Append the text into the buffer
        memcpy(buffer + buffer_pos, text, text_length * sizeof(char));
        buffer_pos += text_length;
    }
    else
    {
        // Write the current buffer content in the file
        flush_buffer();

        // Does the text fit in the (now empty) buffer?
        if (text_length < buffer_size)
        {
            // Copy the text into the buffer
            memcpy(buffer, text, text_length * sizeof(char));
            buffer_pos = text_length;
        }
        else
        {
            // Directly write the text into the file
            f.write(text, static_cast<std::streamsize>(text_length));
        }
    }
}

void WriteBuffer::flush_buffer()
{
    f.write(buffer, static_cast<std::streamsize>(buffer_pos));
    f.flush(); // Força gravação imediata no disco
    buffer_pos = 0;
    // Force writing the text content to the file
    f.flush();
}

void WriteBuffer::close_buffer()
{

    delete buffer;
    buffer = nullptr;
}


