#ifndef WRITEBUFFER_HPP
#define WRITEBUFFER_HPP

#include <stdio.h>
#include <sys/types.h> /* ssize_t, needed by xbt/str.h, included by msg/msg.h */
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <memory>
#include <cstring>

/**
 * Copy from Batsim writebuffer class https://batsim.readthedocs.io/en/latest/
 * @brief Buffered-write output file
 */
class WriteBuffer
{
public:
    /**
     * @brief Builds a WriteBuffer
     * @param[in] filename The file that will be written
     * @param[in] buffer_size The size of the buffer (in bytes).
     */
    explicit WriteBuffer(const std::string & filename,
                         size_t buffer_size = 64*1024);

    /**
     * @brief WriteBuffers cannot be copied.
     * @param[in] other Another instance
     */
    WriteBuffer(const WriteBuffer & other) = delete;

    /**
     * @brief Destructor
     * @details This method flushes the buffer if it is not empty, destroys the buffer and closes the file.
     */
    ~WriteBuffer();

    /**
     * @brief Appends a text at the end of the buffer. If the buffer is full, it is automatically flushed into the disk.
     * @param[in] text The text to append
     */
    void append_text(const char * text);

    /**
     * @brief Write the current content of the buffer into the file
     */
    void flush_buffer();

private:
    std::ofstream f;            //!< The file stream on which the buffer is outputted
    const size_t buffer_size;   //!< The buffer maximum size
    char * buffer = nullptr;    //!< The buffer
    size_t buffer_pos = 0;         //!< The current position of the buffer (previous positions are already written)
};

#endif