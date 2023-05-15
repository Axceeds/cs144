#pragma once

#include "byte_stream.hh"

#include <string>
#include <set>
class Reassembler
{
private:
    struct block_node{
        uint64_t begin = 0;
        uint64_t length = 0;
        std::string data = "";
        bool operator<(const block_node t) const {return begin <t.begin; }
    };
    std::set<block_node> _blocks = {};
    uint64_t _unassembled_byte = 0;
    uint64_t _head_index = 0;
    bool is_EOF = false;

    uint64_t sum_set();
    long merge_block(block_node &elm1,const block_node &elm2);

public:
  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring, Writer& output );

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;
};
